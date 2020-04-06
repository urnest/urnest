// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/stringToUInt.hh>

// send all responses with the http version that this webapp supports
auto const SUPPORTED_HTTP_VERSION(xju::http::HTTP_1_0);

struct Sessions
{
public:
  
private:
  xju::Mutex guard_;
  std::map<SessionId,std::unique_ptr<Session> > sessions_;
};

struct ClosedConns
{
  ClosedConns() noexcept:
      changed_(guard_),
      stop_(false)
  {
  }
  void stop() noexcept
  {
    Lock l(guard_);
    stop=true;
    changed_.signal(l);
  }
  // returns 0 if stopped
  xju::ip::Socket* getNext() noexcept{
    Lock l(guard_);
    while(!stop_ && !closed_.size()){
      changed_.wait(l);
    }
    if(stop_){
      return 0;
    }
    return closed_.pop_front();
  }
  
  void closed(xju::ip::TCPSocket const* s) noexcept{
    xju::Lock l(guard_);
    closed_.insert(s);
    changed_.signal(l);
  }
  
private:
  xju::Mutex guard_;
  xju::Condition changed_;
  bool stop_;
  std::deque<xju::ip::TCPSocket const*> closed_;
};

// handle request from out-of-session client
xju::http::Response login(xju::http::Request const& request,
                          Sessions& sessions) throw(
                            Shutdown,
                            xju::Exception)
{
  try{
    if (request.requestLine.m_=="POST"&&
        request.requestLine.t_.path_==xju::uri::Path(
          {xju::uri::Segment(""),
           xju::uri::Segment("login")})){
      auto const vars(xju::http::decodeMimeRequest(request.headers_,
                                                   request.body_));
      Sessions::Ref session(sessions.newSession(
                              vars.get("user"),
                              vars.get("password")));
      return xju::http::redirect(
        {xju::uri::Segment("")},
        xju::http::setCookie("WEBAPPSESSION",session.id_));
    }
    return loginResources.get(request.requestLine.t_.path);
  }
  catch(AuthenticationFailed const& e){
    logInformation(readableRepr(e),XJU_TRACED);
    xju::sleepFor(std::chrono::seconds(5));
    return xju::http::Response(
      xju::http::StatusLine(
        SUPPORTED_HTTP_VERSION,
        xju::http::StatusCode(403), //Forbidden
        xju::http::ReasonPhrase("Authentication failed for user "+e.user_)),
      {},
      {});
  }
}

struct Conn
{
  Conn(std::unique_ptr<xju::ip::TCPSocket>&& s,
       xju::io::Input const& shutdownServer,
       ClosedConns& closedConns,
       Resources const& loginResources,
       Sessions const& sessions):
      s_(s),
      shutdownServer_(shutdownServer),
      closedConns_(closedConns),
      t_([this&](){
           this->run();
         })
  {
  }
private:
  std::unique_ptr<xju::ip::TCPSocket> > const s_;
  xju::io::Input const& shutdownServer_;
  ClosedConns& closedConns_;
  Resources const& loginResources_;
  Session& sessions_;
  xju::Thread const t_;

  void run() noexcept{
    std::ostringstream s;
    s << "run connection from " << s_->peerAddress().first << ":"
      << s_->peerAddress.second();
    try{
      TLSSocket s(s_,crypto_,xju::steadyNow()+handshakeTimeout_);
      xju::io::istream i(s);
      xju::io::ostream o(s);
      while(true){
        //REVISIT: use select here to implement connection idle timeout
        //         but only if i is empty
        auto const deadline(xju::steadyNow()+std::chrono::minutes(1));
        i.setDeadline(deadline);
        o.setDeadline(deadline);
        xju::http::Request const request(xju::http::getRequest(i,4000U));
        //versioning from rfc7230:
        //  1.1 sender must send message such that it will be
        //      correctly interpreted by 1.0 receiver unless sender
        //      knows receiver is 1.1 capable
        //  1.1 receiver can send 1.1 as version in response to 1.0
        //      (but message must be 1.0 content only) to indicate
        //      to sender that receiver supports 1.1 features
        //  1.1 receiver can treat 1.0 request same as 1.1
        //  1.0 supports Content-Length but Transfer-Encoding is 1.1
        //      only; so simplest to avoid sending Transfer-Encoding
        //  1.* precludes Content-Length in:
        //      - 1xx status responses info
        //      - 204 status responses no content
        //      - successful responses to CONNECT
        //      REVISIT: can we statically encapsulate that somehow? Or do
        //      we apply it on-the-fly? Former is impossible so do latter.
        //  - use 400 Bad Request where Content-Length is invalid
        if (request.requestLine.v_.major()!=xju::http::HTTPVersion::Major(1)){
          logError("only HTTP 1.x supported, not "+
                   xju::format::str(request.requestLine.v_),XJU_TRACED);
          xju::http::encodeResponse(
            o,
            xju::http::Response(
              xju::http::StatusLine(
                SUPPORTED_HTTP_VERSION,
                xju::http::StatusCode(505),
                xju::http::ReasonPhrase("HTTP Version "+
                                        xju::format::str(
                                          request.requestLine.v_)+
                                        " Not supported")),
              {},
              {}));
          throw Shutdown(XJU_TRACED);
        }
        Cookies const cookies(request.headers_);
        auto sessionId(cookies.get("WEBAPPSESSION",""));
        xju::http::encodeResponse(
          o,
          (!sessionId.size()?
           login(request,loginResources_,sessions_):
           sessions.get(sessionId).handle(request)));
        o.flush();
      }
    }
    catch(Shutdown& e){
      e.addContext(s.str(),XJU_TRACED);
      logInformation("close connection",e,XJU_TRACED);
    }
    catch(xju::Exception& e){
      e.addContext(s.str(),XJU_TRACED);
      logError("close connection",e,XJU_TRACED);
    }
  }
};

int main(int argc, char* argv[])
{
  try{
    int i=0;
    xju::ip::Port const port(
      xju::stringToUInt(argv[++i])); //listen for https on
    std::string const group(argv[++i]); //app user group

    xju::ip::tls::Crypto crypto();
    xju::Thread cr([[&]](){crypto.run();});
    
    xju::ip::TCPService listener(port,3,true);

    Sessions sessions;
    auto shutdownServer(xju::pipe(true,true));

    xju::Mutex connsGuard;
    std::map<xju::ip::Socket const*, std::unique_ptr<Conn> > conns;
    xju::Condition connsChanged;

    ClosedConns closedConns;

    xju::Thread reaper([&](){
        auto s(closedConns.getNext());
        if (!s){
          return;
        }
        Lock l(connsGuard_);
        conns_.erase(conns_.find(s));
      },
      [&](){
        closedConns.stop();
      });
          
    while(!xju::io::select(
            {shutdownServer.first.get()},{},xju::steadyNow()).size()){
      std::set<xju::io::Input const*> r{&listener,shutdownServer.first.get()};
      auto const r(xju::io::select(
                     r,{},
                     xju::steadyEternity).first.size());
      if(r.find(&listener)!=r.end())
      {
        std::unique_ptr<xju::ip::TCPSocket> s(new xju::ip::TCPSocket(
                                                listener,
                                                xju::steadyNow()));
        xju::ip:TCPSocket const* sp(s.get());
        xju::Lock l(connsGuard);
        conns.insert(std::make_pair(sp,std::unique_ptr<Conn> >(
                                      std::move(s),
                                      *shutdownServer.first.get(),
                                      closedConns,
                                      loginResources,
                                      sessions)));
                                       
      }    
    }
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv,argv+argc," ");
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}

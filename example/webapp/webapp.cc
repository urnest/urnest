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

struct Conn
{
  Conn(std::unique_ptr<xju::ip::TCPSocket>&& s,
       xju::io::Input const& shutdownServer,
       ClosedConns& closedConns):
      s_(s),
      shutdownServer_(shutdownServer),
      closedConns_(closedConns),
      t_([&]{
           this->run();
         }
  {
  }
private:
  std::unique_ptr<xju::ip::TCPSocket> s_;
  xju::io::Input const& shutdownServer_;
  xju::Thread t_;

  void run() noexcept{
    try{
      TLSSocket s(s_,crypto_,xju::steadyNow()+handshakeTimeout_);
      while(true){
        SessionRef session(login());
        try{
          while(true){
            Request nextRequest();
            if (request is logout){
              respondWith(loggedOut);
              break;
            }
            respondWith(session.handle(request));
          }
        }
        catch(SessionExpired const&)
        {
        }
      }
    }
    catch(Shutdown const&){
      return;
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "run connection from " << s_->peerAddress().first << ":"
        << s_->peerAddress.second();
      e.addContext(s.str(),XJU_TRACED);
      std::cerr << "ERROR: aborting connection because "
                << readableRepr(e);
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

    std::map<SessionId,std::unique_ptr<Session> > sessions;
    auto shutdownServer(xju::pipe(true,true));

    xju::Mutex connsGuard_;
    std::map<xju::ip::Socket const*, std::unique_ptr<Conn> > conns_;
    xju::Condition connsChanged_;

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
        xju::Lock l(connsGuard_);
        conns_.insert(std::make_pair(sp,std::unique_ptr<Conn> >(
                                       std::move(s)),
                                     *shutdownServer.first.get(),
                                     closedConns));
                                       
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

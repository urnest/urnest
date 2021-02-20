// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <string>
#include <algorithm>
#include <iostream>
#include <xju/stringToUInt.hh>
#include <xju/ip/Port.hh>
#include <xju/pipe.hh>
#include <chrono>
#include <xju/Subprocess.hh>
#include <xju/unistd.hh>
#include <xju/signal.hh>
#include <signal.h>
#include <xju/io/select.hh>
#include <xju/Exception.hh>
#include <sstream>
#include <xju/format.hh>
#include <xju/steadyNow.hh>

bool hasStarted(std::string const& output) noexcept
{
  static std::string const a("Starting ChromeDriver ");
  static std::string const b(" on port ");
  return
    std::search(output.begin(),output.end(),
                a.begin(),a.end())!=output.end() &&
    std::search(output.begin(),output.end(),
                b.begin(),b.end())!=output.end();
}

int main(int argc, char* argv[])
{
  if (argc < 3){
    std::cerr << "usage: " << argv[0] 
              << " <iterations> <chromedriver-exe> <args...>" << std::endl;
    std::cerr << "e.g. " << argv[0]
              << " /usr/bin/chromedriver --port=3737 --allowed-ips=127.0.0.1\n";
    
    return 2;
  }
  using std::chrono::milliseconds;
  using std::chrono::seconds;
  using std::chrono::duration_cast;
  
  try{
    auto const iterations(xju::stringToUInt(argv[1]));
    std::string exe(argv[2]);
    std::vector<std::string> args(&argv[2],&argv[argc]);
    for(unsigned int i=0; i<iterations; ++i){
      try{
        auto stdoutPipe(xju::pipe(true,false));
        xju::io::IStream& stdout(*stdoutPipe.first);
        auto const deadline(xju::steadyNow()+seconds(10));
        int exitStatus(0);
        std::string output;
        auto const t1(xju::steadyNow());
        try{
          xju::Subprocess p(
            exitStatus,
            [&](){
              stdoutPipe.second->useAsStdout();
              xju::exec(exe,args);
              return 0;
            },
            [&](pid_t pid){
              xju::syscall(xju::kill,XJU_TRACED)(pid,SIGINT);
            });
          stdoutPipe.second.reset();
          while(xju::steadyNow()<deadline && !hasStarted(output)){
            xju::io::select({&stdout},deadline);
            auto const n(output.size());
            output.resize(n+512);
            try{
              output.resize(n+stdout.read(&output[n],512,xju::steadyNow()));
            }
            catch(xju::io::Input::Closed const&){
              output.resize(n);
              std::ostringstream s;
              s << exe << " closed its stdout before signalling startup\n";
              throw xju::Exception(s.str(),XJU_TRACED);
            }
          }
          if (!hasStarted(output)){
            p.kill(SIGABRT);
            std::ostringstream s;
            s << "deadline reached before started";
            throw xju::Exception(s.str(),XJU_TRACED);
          }
        }
        catch(xju::Exception& e){
          if (exitStatus!=0){
            std::ostringstream s;
            s << "verify startup, noting chromedriver exit status "
              << exitStatus << " and output "
              << xju::format::quote(xju::format::cEscapeString(output));
            e.addContext(s.str(),XJU_TRACED);
            throw;
          }
        }
        try{
          while(true){
            auto const n(output.size());
            output.resize(n+512);
              output.resize(n+stdout.read(&output[n],512,xju::steadyNow()));
          }
        }
        catch(xju::io::Input::Closed const&){
        }
        auto const duration(duration_cast<milliseconds>(xju::steadyNow()-t1));
        std::cout << "iteration " << i << " took " << duration.count() << "ms, output was:\n";
        std::cout << output << "\n";
      }
      catch(xju::Exception& e){
        std::ostringstream s;
        s << "iteration " << i;
        e.addContext(s.str(),XJU_TRACED);
        throw;
      }
    }
  }
  catch(xju::Exception& e){
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}

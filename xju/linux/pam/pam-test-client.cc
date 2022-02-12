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
#include <xju/assert.hh>
#include <sstream>
#include <xju/format.hh>
#include <xju/UserName.hh>
#include <optional>
#include <xju/linux/pam/Transaction.hh>
#include <xju/linux/pam/Authentic.hh>
#include <xju/linux/pam/Account.hh>
#include <iostream>
#include <xju/linux/pam/Session.hh>

// get argument i from argv representing described function e.g. user name
// 
std::string getArg(std::string const& description,
                   char* argv[],
                   int argc,
                   int i)
{
  xju::assert_less_equal(i, argc);
  try{
    if (i < argc){
      return argv[i];
    }
    std::ostringstream s;
    s << "missing " << description;
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "get next command line argument as " << description << ", having consumed "
      << xju::format::join(&argv[0], &argv[i], std::string(", "));
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  try{
    int i(0);
    auto const serviceName(getArg("service name", argv, argc, ++i));
    xju::UserName const userName(getArg("user name", argv, argc, ++i));
    std::optional<std::string> password;
    if (++i<argc){
      password = getArg("user name", argv, argc, i);
    }

    xju::linux::pam::Transaction transaction(serviceName, userName, password);
    std::cout << "pam_start OK\n";
    
    xju::linux::pam::Authentic authentic(transaction);
    std::cout << "pam_authenticate OK\n";

    xju::linux::pam::Account account(authentic);
    std::cout << "pam_acct_mgmt OK\n";
    
    //Credentials credentials(account);
    //Session s(credentials);
    xju::linux::pam::Session s(account);
    std::cout << "pam_open_session and pam_close_session OK" << std::endl;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}

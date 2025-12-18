// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <hcp/parser.hh>
#include <hcp/ast.hh>
#include <iostream>
#include <xju/file/read.hh>
#include <sstream>
#include <xju/file/write.hh>

using namespace hcp_parser;
using namespace hcp_ast;

PR wspc() throw()
{
  static PR wspc(
    anon("optional whitespace",
         !whitespaceChar()|
         named<hcp_ast::WhiteSpace>(
              "whitespace",
              zeroOrMore()*(whitespaceChar()))));
  return wspc;
}


class DirInTag{};
typedef TaggedItem<DirInTag> DirIn;

PR dirIn()  noexcept
{
  static PR result(named<DirIn>(
                     "/*in*/",
                     parseLiteral("/*in*/"))+
                   wspc());
  return result;
}

class DirOutTag{};
typedef TaggedItem<DirOutTag> DirOut;

PR dirOut()  noexcept
{
  static PR result(named<DirOut>(
                     "/*out*/",
                     parseLiteral("/*out*/"))+
                   wspc());
  return result;
}

class ParamTypeTag{};
typedef TaggedItem<ParamTypeTag> ParamType;

PR paramType()  noexcept
{
  static PR result("/*"+
                   named<ParamType>(
                     "paramType",
                     parseUntil("*/"))+
                   "*/"+
                   wspc());
  return result;
}

class ImplTag{};
typedef TaggedItem<ImplTag> Impl;

PR impl()  noexcept
{
  static PR result("{"+wspc()+
                   named<Impl>(
                     "impl",
                     balanced(parseLiteral("}")))+
                   "}"+
                   wspc());
  return result;
}

class ArgVarDeclTag{};
typedef TaggedItem<ArgVarDeclTag> ArgVarDecl;

PR argVarDecl()  noexcept
{
  static PR result(named<ArgVarDecl>(
                     "argVarDecl",
                     var_non_fp())+
                   wspc());
  return result;
}

class InArgDeclTag{};
typedef TaggedItem<InArgDeclTag> InArgDecl;

PR inArgDecl()  noexcept
{
  static PR result(named<InArgDecl>(
                     "in arg decl",
                     dirIn()+wspc()+
                     paramType()+wspc()+
                     argVarDecl())+
                   wspc());
  return result;
}

class OutArgDeclTag{};
typedef TaggedItem<OutArgDeclTag> OutArgDecl;

PR outArgDecl()  noexcept
{
  static PR result(named<OutArgDecl>(
                     "out arg decl",
                     dirOut()+wspc()+
                     paramType()+wspc()+
                     argVarDecl())+
                   wspc());
  return result;
}

class ArgDeclTag{};
typedef TaggedItem<ArgDeclTag> ArgDecl;

PR argDecl()  noexcept
{
  static PR result(named<ArgDecl>(
                     "arg decl",
                     inArgDecl()|outArgDecl())+
                   wspc());
  return result;
}

class ProcNameTag{};
typedef TaggedItem<ProcNameTag> ProcName;

PR procName()  noexcept
{
  static PR result(named<ProcName>(
                     "proc name",
                     identifier()+wspc()));
  return result;
}

class ProcDefTag{};
typedef TaggedItem<ProcDefTag> ProcDef;

PR procDef()  noexcept
{
  static PR result(named<ProcDef>(
                     "proc def",
                     procName() + wspc() +
                     listOf("(" + wspc(),
                            identifier() + wspc(), //arg
                            ","+wspc(),
                            ")" + wspc()) +
                     parseUntil(argDecl()+";" + wspc(),
                                parseLiteral("{")) +
                     impl() + wspc()));
  return result;
}


class RequestTag{};
typedef TaggedItem<RequestTag> Request;

PR request()  noexcept
{
  static PR result(named<Request>(
                     "/*request*/",
                     parseLiteral("/*request*/"))+
                   wspc());
  return result;
}

class SplitRequestTag{};
typedef TaggedItem<SplitRequestTag> SplitRequest;

PR splitRequest()  noexcept
{
  static PR result(named<SplitRequest>(
                     "/*split-request*/",
                     parseLiteral("/*split-request*/"))+
                   wspc());
  return result;
}

class NoticeTag{};
typedef TaggedItem<NoticeTag> Notice;

PR notice()  noexcept
{
  static PR result(named<Notice>(
                     "/*notice*/",
                     parseLiteral("/*notice*/"))+
                   wspc());
  return result;
}

class ServerMessageTag{};
typedef TaggedItem<ServerMessageTag> ServerMessage;

PR serverMessage() noexcept
{
  static PR result(named<ServerMessage>(
                     "serverMessage",
                     (request() | splitRequest() | notice()) +
                     procDef()) + wspc());
  return result;
}

class ClientMessageTag{};
typedef TaggedItem<ClientMessageTag> ClientMessage;

PR clientMessage() noexcept
{
  static PR result(named<ClientMessage>(
                     "clientMessage",
                     notice() +
                     procDef()) + wspc());
  return result;
}

class ClientStubTag{};
typedef TaggedItem<ClientStubTag> ClientStub;

PR clientStub() noexcept
{
  static PR result(named<ClientStub>(
                     "clientStub",
                     parseLiteral("/*client*/") + wspc() +
                     clientMessage()));
  return result;
}

class ServerStubTag{};
typedef TaggedItem<ServerStubTag> ServerStub;

PR serverStub() noexcept
{
  static PR result(named<ServerStub>(
                     "serverStub",
                     parseLiteral("/*server*/") + wspc() +
                     serverMessage()));
  return result;
}

class StubTag{};
typedef TaggedItem<StubTag> Stub;

PR stub() noexcept
{
  static PR result(named<Stub>(
                     "stub",
                     serverStub() | clientStub()));
  return result;
}

std::string mapParamType(std::string const& x)
{
  if (x=="str"){
    return "tps_Str";
  }
  else if (x=="int"){
    return "int";
  }
  std::ostringstream s;
  s << "unknown param type " << xju::format::quote(x)
    << " (expected \"str\" or \"int)\"";
  throw xju::Exception(s.str(),XJU_TRACED);
}

std::string stripParamType(std::string const& x)
{
  auto i=x.end();
  while (i!=x.begin() && (*(i-1)==' ' || *(i-1)=='*')){
    --i;
  }
  return std::string(x.begin(),i);
}

int main(int argc, char* argv[])
{
  if (argc != 4){
    std::cerr << "usage: " << argv[0] << " <input-file> <in-file-name> <out-file-name>"
              << std::endl;
    return 1;
  }
  
  PR parseStubs = parseUntil(stub(), endOfFile());

  auto x = xju::file::read(xju::path::split(argv[1]));
  
  auto const root{parseString(x.begin(),x.end(), parseStubs)};

  auto const stubs{hcp_ast::findChildrenOfType<Stub>(root)};
  std::ostringstream inStubs;
  inStubs << "#include <gmc/gmc.h>" << "\n";
  inStubs << "#include <odin/inc/Type.hh>" << "\n";
  inStubs << "#include <odin/inc/Func.hh>" << "\n";
  inStubs << "#include <odin/inc/Var.hh>" << "\n";
  inStubs << "#include <string.h>" << "\n";
    
  inStubs << "extern bool IPC_Do_Return;" << "\n";
  inStubs << "extern int *IPC_IArg1, *IPC_IArg2, *IPC_IArg3;" << "\n";
  inStubs << "extern tp_Str IPC_SArg1, IPC_SArg2, IPC_SArg3;" << "\n";
  inStubs << "" << "\n\n";



  for(auto const& stub:stubs){
    const std::string procName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ProcName>(stub)));
    for(auto const& clientStub: hcp_ast::findChildrenOfType<ClientStub>(stub)){
      inStubs << "#ifndef SERVER_ONLY" << "\n";
      inStubs << "static void" << "\n";
      inStubs << procName << "_Msg(" << "\n";
      inStubs << "   bool* IPC_AbortPtr)" << "\n";
      inStubs << "{" << "\n";
      for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(clientStub)){
        auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
        auto const paramName(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
        inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
      }
      inStubs << "" << "\n";
      inStubs << "   *IPC_AbortPtr = false;" << "\n";
      for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(clientStub)){
        std::string const paramType = hcp_ast::reconstruct(
          hcp_ast::findOnlyChildOfType<ParamType>(arg));
        std::string const paramName = hcp_ast::reconstruct(
          hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg));
        if (paramType == "str"){
          inStubs << "   IPC_Read_Str(IPC_AbortPtr, " << paramName << ");" << "\n";
        }
        else if (paramType == "int"){
          inStubs << "   IPC_Read_Int(IPC_AbortPtr, &" << paramName << ");" << "\n";
        }
        else{
          xju::assert_never_reached();
        }
        inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
      }
      inStubs << "   " << hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<Impl>(stub)) << "\n";
      inStubs << "}" << "\n";
      inStubs << "#endif" << "\n\n";
    }
  }
  
  for(auto const& stub:stubs){
    const std::string procName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ProcName>(stub)));
    for(auto const& serverStub: hcp_ast::findChildrenOfType<ServerStub>(stub)){
      inStubs << "\n";
      if (hcp_ast::findChildrenOfType<Request>(serverStub).size()){
        inStubs << "#ifndef CLIENT_ONLY" << "\n";
        inStubs << "static void" << "\n";
        inStubs << procName << "_Msg(" << "\n";
        inStubs << "   bool* IPC_AbortPtr)" << "\n";
        inStubs << "{" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
        }
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
        }
        inStubs << "" << "\n";
        inStubs << "   *IPC_AbortPtr = false;" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType == "int"){
            inStubs << "   IPC_Read_Int(IPC_AbortPtr, &" << paramName << ");\n";
          }
          else if (paramType == "str"){
            inStubs << "   IPC_Read_Str(IPC_AbortPtr, " << paramName << ");\n";
          }
          else{
            xju::assert_never_reached();
          }
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
        }
        
        inStubs << "   Local_" << procName << "(";
        std::vector<std::string> localParams;
        auto const args(hcp_ast::findChildrenOfType<ArgDecl>(serverStub));
        for(auto& inOrOutArg: args){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(inOrOutArg)));
          std::string const paramName = hcp_ast::reconstruct(
            hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(inOrOutArg));
          for(auto& arg: hcp_ast::findChildrenOfType<OutArgDecl>(inOrOutArg)){
            if (paramType == "int"){
              localParams.push_back(std::string("&"+paramName));
            }
            else if (paramType == "str"){
              localParams.push_back(paramName);
            }
            else {
              xju::assert_never_reached();
            }
          }
          for(auto& arg: hcp_ast::findChildrenOfType<InArgDecl>(inOrOutArg)){
            if (paramType == "int"){
              localParams.push_back(std::string(paramName));
            }
            else if (paramType == "str"){
              localParams.push_back(paramName);
            }
            else {
              xju::assert_never_reached();
            }
          }
        }
        inStubs << xju::format::join(localParams.begin(),
                                     localParams.end(),
                                     ", ") << ");\n";
        if (hcp_ast::findChildrenOfType<Request>(serverStub).size()){
          inStubs << "   IPC_Write_Int(IPC_AbortPtr, 1);\n";
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
            auto const paramType(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
            auto const paramName(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
            if (paramType == "int"){
              inStubs << "   IPC_Write_Int(IPC_AbortPtr, " << paramName << ");\n";
            }
            else if (paramType == "str"){
              inStubs << "   IPC_Write_Str(IPC_AbortPtr, " << paramName << ");\n";
            }
            else{
              xju::assert_never_reached();
            }
            inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          }
        }
        inStubs << "}" << "\n";
        inStubs << "#endif" << "\n";
      }
      else if (hcp_ast::findChildrenOfType<Notice>(serverStub).size()){
        inStubs << "#ifndef CLIENT_ONLY" << "\n";
        inStubs << "static void" << "\n";
        inStubs << procName << "_Msg(" << "\n";
        inStubs << "   bool* IPC_AbortPtr)" << "\n";
        inStubs << "{" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
        }
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
        }
        inStubs << "" << "\n";
        inStubs << "   *IPC_AbortPtr = false;" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType == "int"){
            inStubs << "   IPC_Read_Int(IPC_AbortPtr, &" << paramName << ");\n";
          }
          else if (paramType == "str"){
            inStubs << "   IPC_Read_Str(IPC_AbortPtr, " << paramName << ");\n";
          }
          else{
            xju::assert_never_reached();
          }
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
        }
        
        inStubs << "   Local_" << procName << "(";
        std::vector<std::string> localParams;
        auto const args(hcp_ast::findChildrenOfType<ArgDecl>(serverStub));
        for(auto& inOrOutArg: args){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(inOrOutArg)));
          std::string const paramName = hcp_ast::reconstruct(
            hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(inOrOutArg));
          for(auto& arg: hcp_ast::findChildrenOfType<OutArgDecl>(inOrOutArg)){
            if (paramType == "int"){
              localParams.push_back(std::string("&"+paramName));
            }
            else if (paramType == "str"){
              localParams.push_back(paramName);
            }
            else {
              xju::assert_never_reached();
            }
          }
          for(auto& arg: hcp_ast::findChildrenOfType<InArgDecl>(inOrOutArg)){
            if (paramType == "int"){
              localParams.push_back(std::string(paramName));
            }
            else if (paramType == "str"){
              localParams.push_back(paramName);
            }
            else {
              xju::assert_never_reached();
            }
          }
        }
        inStubs << xju::format::join(localParams.begin(),
                                     localParams.end(),
                                     ", ") << ");\n";
        if (hcp_ast::findChildrenOfType<Request>(serverStub).size()){
          inStubs << "   IPC_Write_Int(IPC_AbortPtr, 1);\n";
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
            auto const paramType(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
            auto const paramName(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
            if (paramType == "int"){
              inStubs << "   IPC_Write_Int(IPC_AbortPtr, " << paramName << ");\n";
            }
            else if (paramType == "str"){
              inStubs << "   IPC_Write_Str(IPC_AbortPtr, " << paramName << ");\n";
            }
            else{
              xju::assert_never_reached();
            }
            inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          }
        }
        inStubs << "}" << "\n";
        inStubs << "#endif" << "\n";
      }
      else if (hcp_ast::findChildrenOfType<SplitRequest>(serverStub).size()){
        inStubs << "#ifndef CLIENT_ONLY" << "\n";
        inStubs << "static void" << "\n";
        inStubs << procName << "_Msg(" << "\n";
        inStubs << "   bool* IPC_AbortPtr)" << "\n";
        inStubs << "{" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          inStubs << "   " << mapParamType(paramType) << " " << paramName << ";\n";
        }
        inStubs << "" << "\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType == "int"){
            inStubs << "   IPC_Read_Int(IPC_AbortPtr, &" << paramName << ");\n";
          }
          else if (paramType == "str"){
            inStubs << "   IPC_Read_Str(IPC_AbortPtr, " << paramName << ");\n";
          }
          else{
            xju::assert_never_reached();
          }
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
        }
        
        inStubs << "   Local_" << procName << "(";
        std::vector<std::string> localParams;
        auto const args(hcp_ast::findChildrenOfType<InArgDecl>(serverStub));
        for(auto& arg: args){
          std::string const paramName = hcp_ast::reconstruct(
            hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg));
          localParams.push_back(paramName);
        }
        inStubs << xju::format::join(localParams.begin(),
                                     localParams.end(),
                                     ", ") << ");\n";
        if (hcp_ast::findChildrenOfType<Request>(serverStub).size()){
          inStubs << "   IPC_Write_Int(IPC_AbortPtr, 1);\n";
          inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
            auto const paramType(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
            auto const paramName(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
            if (paramType == "int"){
              inStubs << "   IPC_Write_Int(IPC_AbortPtr, " << paramName << ");\n";
            }
            else if (paramType == "str"){
              inStubs << "   IPC_Write_Str(IPC_AbortPtr, " << paramName << ");\n";
            }
            else{
              xju::assert_never_reached();
            }
            inStubs << "   if (*IPC_AbortPtr) return;" << "\n";
          }
        }
        inStubs << "}" << "\n\n";
        inStubs << "void" << "\n";
        inStubs << "LocalEnd_" << procName << "(" << "\n";

        std::vector<std::string> x;
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          std::ostringstream s;
          s << "   " << stripParamType(paramType) << " " << paramName;
          x.push_back(s.str());
        }
        inStubs << xju::format::join(x.begin(), x.end(), ",\n") << "\n";
        inStubs << "   )\n";
        inStubs << "{\n";
        inStubs << "   bool IPC_Abort;\n\n";
        inStubs << "   if (IsServer && Is_LocalClient(CurrentClient)) {\n";
        int i=1;
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType=="str"){
            inStubs << "      (void)strcpy(IPC_SArg"<<i<<", " << paramName << ");\n";
          }
          else{
            inStubs << "      *IPC_IArg"<<i<<" = " << paramName << ";\n";
          }
          ++i;
        }

        inStubs << "      FORBIDDEN(IPC_Do_Return);\n";
        inStubs << "\n";
        inStubs << "      IPC_Do_Return = true;\n";
        inStubs << "\n";
        inStubs << "      return; };\n\n";
        inStubs << "   IPC_Write_Int(&IPC_Abort, 1);\n";
        inStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType=="str"){
            inStubs << "   IPC_Write_Str(&IPC_Abort, " << paramName << ");\n";
          }
          else{
            inStubs << "   IPC_Write_Int(&IPC_Abort, " << paramName << ");\n";
          }
          inStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
          ++i;
        }
        inStubs << "}\n";
        inStubs << "#endif" << "\n";
      }
    }
  }
  inStubs << "void\n";
  inStubs << "IPC_Do_Msg(\n";
  inStubs << "   bool* IPC_AbortPtr,\n";
  inStubs << "   int MsgType)\n";
  inStubs << "{\n";
  inStubs << "   switch (MsgType) {\n";
  inStubs << "      case 1: {\n";
  inStubs << "         IPC_Do_Return = true;\n";
  inStubs << "         *IPC_AbortPtr = false;\n";
  inStubs << "         break; }/*case*/;\n";
  int i=2;
  for(auto const& stub:stubs){
    const std::string procName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ProcName>(stub)));
    for(auto const& clientStub: hcp_ast::findChildrenOfType<ClientStub>(stub)){
      inStubs << "      case "<<i<<": {\n";
      inStubs << "#ifndef SERVER_ONLY\n";
      inStubs << "         "<<procName<<"_Msg(IPC_AbortPtr);\n";
      inStubs << "#else\n";
      inStubs << "         *IPC_AbortPtr = true;\n";
      inStubs << "#endif\n";
      inStubs << "         break; }/*case*/;\n";
    }
    for(auto const& clientStub: hcp_ast::findChildrenOfType<ServerStub>(stub)){
      inStubs << "      case "<<i<<": {\n";
      inStubs << "#ifndef CLIENT_ONLY\n";
      inStubs << "         "<<procName<<"_Msg(IPC_AbortPtr);\n";
      inStubs << "#else\n";
      inStubs << "         *IPC_AbortPtr = true;\n";
      inStubs << "#endif\n";
      inStubs << "         break; }/*case*/;\n";
    }
    ++i;
  }
  inStubs << "      default: {\n";
  inStubs << "         FATALERROR(\"Unexpected message type\"); };}/*switch*/;\n";
  inStubs << "   }/*IPC_Do_Msg*/\n";

  xju::file::write(
    xju::path::split(argv[2]),
    inStubs.str().c_str(),
    inStubs.str().size(),
    xju::file::Mode(0666));

  std::ostringstream outStubs;
  outStubs << "#include \"inc/GMC.h\"\n";
  outStubs << "\n";
  outStubs << "extern bool IPC_Do_Return;\n";
  outStubs << "extern int *IPC_IArg1, *IPC_IArg2, *IPC_IArg3;\n";
  outStubs << "extern tp_Str IPC_SArg1, IPC_SArg2, IPC_SArg3;\n";

  int procId=1;
  for(auto const& stub:stubs){
    const std::string procName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ProcName>(stub)));
    ++procId;
    outStubs << "\n";
    for(auto const& clientStub: hcp_ast::findChildrenOfType<ClientStub>(stub)){
      outStubs << "#ifndef CLIENT_ONLY" << "\n";
      outStubs << "void\n";
      outStubs << procName << "(\n";
      std::vector<std::string> x;
      std::vector<std::string> paramNames;
      for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(clientStub)){
        auto const paramType(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(arg)));
        auto const paramName(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
        std::ostringstream s;
        if (paramType=="char *"){
          s << "   char* " << paramName;
        }
        else{
          s << "   " << stripParamType(paramType) << " " << paramName;
        }
        x.push_back(s.str());
        paramNames.push_back(paramName);
      }
      outStubs << xju::format::join(x.begin(), x.end(), ",\n") << "\n";
      outStubs << "   )\n";
      for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(clientStub)){
        auto const paramType(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(arg)));
        auto const paramName(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
        if (paramType=="char *"){
          outStubs << "   char* " << paramName << "\n";
        }
        else{
          outStubs << "   " << stripParamType(paramType) << " " << paramName << "\n";
        }
      }
      outStubs << "{\n";
      outStubs << "   bool IPC_Abort;\n";
      outStubs << "\n";
      outStubs << "#ifndef SERVER_ONLY\n";
      outStubs << "   if (IsServer && Is_LocalClient(CurrentClient)) {\n";
      outStubs << "      Local_"<<procName<<"("<<xju::format::join(paramNames.begin(),paramNames.end(),", ")<<");\n";
      outStubs << "   }else{\n";
      outStubs << "#endif\n";
      outStubs << "   IPC_Write_Int(&IPC_Abort, "<<procId<<");\n";
      outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
      for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(clientStub)){
        auto const paramType(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
        auto const paramName(
          hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
        if (paramType=="str"){
          outStubs << "   IPC_Write_Str(&IPC_Abort, "<<paramName<<");\n";
        }
        else if (paramType=="int"){
          outStubs << "   IPC_Write_Int(&IPC_Abort, "<<paramName<<");\n";
        }
        else{
          xju::assert_never_reached();
        }
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
      }
      outStubs << "#ifndef SERVER_ONLY\n";
      outStubs << "   };\n";
      outStubs << "#endif\n";
      outStubs << "   }\n";
      outStubs << "#endif\n";

    }
    for(auto const& serverStub: hcp_ast::findChildrenOfType<ServerStub>(stub)){
      if (hcp_ast::findChildrenOfType<Request>(serverStub).size()){
        outStubs << "#ifndef SERVER_ONLY\n";
        outStubs << "void\n";
        std::vector<std::string> x;
        std::vector<std::string> y;
        std::vector<std::string> paramNames;
        for(auto const& inOutArg: hcp_ast::findChildrenOfType<ArgDecl>(serverStub)){
          bool const isInArg(hcp_ast::findChildrenOfType<InArgDecl>(inOutArg).size());
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(inOutArg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(inOutArg)));
          std::ostringstream s;
          std::ostringstream t;
          if (isInArg){
            s << "   " << stripParamType(paramType) << " " << paramName;
            t << "   " << stripParamType(paramType) << ", " << paramName << "\n";
          }
          else {
            if (paramType=="int *"){
              s << "   int* " << paramName << "";
              t << "   int* " << paramName << "\n";
            }
            else if (paramType=="tp_Status *"){
              s << "   tp_Status* " << paramName << "";
              t << "   tp_Status* " << paramName << "\n";
            }
            else{
              s << "   " << stripParamType(paramType) << " " << paramName << "";
              t << "   " << stripParamType(paramType) << " " << paramName << "\n";
            }
          }
          x.push_back(s.str());
          y.push_back(t.str());
          paramNames.push_back(paramName);
        }
        if (x.size()){
          outStubs << procName <<"(\n";
          outStubs << xju::format::join(x.begin(), x.end(), ",\n") << "\n";
          outStubs << "   )\n";
          outStubs << xju::format::join(y.begin(), y.end(), "");
        }
        else{
          outStubs << procName <<"()\n";
        }
        outStubs << "{\n";
        outStubs << "   bool IPC_Abort;\n";
        outStubs << "\n";
        outStubs << "   bool IPC_Cmd_Abort;\n";
        outStubs << "\n";
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   if (IsServer && Is_LocalClient(CurrentClient)) {\n";
        outStubs << "      Local_"<<procName<<"("<<xju::format::join(paramNames.begin(),paramNames.end(),", ")<<");\n";
        outStubs << "   }else{\n";
        outStubs << "#endif\n";
        outStubs << "   IPC_Write_Int(&IPC_Abort, "<<procId<<");\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType=="str"){
            outStubs << "   IPC_Write_Str(&IPC_Abort, "<<paramName<<");\n";
          }
          else if (paramType=="int"){
            outStubs << "   IPC_Write_Int(&IPC_Abort, "<<paramName<<");\n";
          }
          else{
            xju::assert_never_reached();
          }
          outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        }
        outStubs << "   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);\n";
        outStubs << "   FORBIDDEN(IPC_Cmd_Abort);\n";
        outStubs << "   FORBIDDEN(!IPC_Do_Return);\n";
        outStubs << "   IPC_Do_Return = false;\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<OutArgDecl>(serverStub)){
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType=="str"){
            outStubs << "   IPC_Read_Str(&IPC_Abort, "<<paramName<<");\n";
          }
          else if (paramType=="int"){
            outStubs << "   IPC_Read_Int(&IPC_Abort, "<<paramName<<");\n";
          }
          else{
            xju::assert_never_reached();
          }
          outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        }
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   };\n";
        outStubs << "#endif\n";
        outStubs << "   }\n";
        outStubs << "#endif\n";
      }
      if (hcp_ast::findChildrenOfType<Notice>(serverStub).size()){
        outStubs << "#ifndef SERVER_ONLY\n";
        outStubs << "void\n";
        std::vector<std::string> paramNames;
        if (hcp_ast::findChildrenOfType<InArgDecl>(serverStub).size()){
          outStubs << procName << "(\n";
          std::vector<std::string> x;
          for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
            auto const paramType(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(arg)));
            auto const paramName(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
            std::ostringstream s;
            if (paramType=="tp_Str "){
              s << "   tp_Str " << paramName << "";
            }
            else if (paramType=="tp_FileName "){
              s << "   tp_FileName " << paramName << "";
            }
            else if (paramType=="tp_JobID "){
              s << "   tp_JobID " << paramName << "";
            }
            else if (paramType=="bool "){
              s << "   bool " << paramName << "";
            }
            else{
              xju::assert_never_reached();
            }
            x.push_back(s.str());
            paramNames.push_back(paramName);
          }
          outStubs << xju::format::join(x.begin(), x.end(), ",\n");
          outStubs << "\n   )\n";
          for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub)){
            auto const paramType(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::TypeRef>(arg)));
            auto const paramName(
              hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
            if (paramType=="tp_Str "){
              outStubs << "   tp_Str " << paramName << "\n";
            }
            else if (paramType=="tp_JobID "){
              outStubs << "   tp_JobID " << paramName << "\n";
            }
            else if (paramType=="tp_FileName "){
              outStubs << "   tp_FileName " << paramName << "\n";
            }
            else if (paramType=="bool "){
              outStubs << "   bool " << paramName << "\n";
            }
            else{
              xju::assert_never_reached();
            }
          }
        }
        else{
          outStubs << procName << "()\n";
        }
        outStubs << "{\n";
        outStubs << "   bool IPC_Abort;\n";
        outStubs << "\n";
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   if (IsServer && Is_LocalClient(CurrentClient)) {\n";
        outStubs << "      Local_"<<procName<<"("<<xju::format::join(paramNames.begin(),paramNames.end(),", ")<<");\n";
        outStubs << "   }else{\n";
        outStubs << "#endif\n";
        outStubs << "   IPC_Write_Int(&IPC_Abort, "<<procId<<");\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        for(auto const& arg: hcp_ast::findChildrenOfType<InArgDecl>(serverStub))
        {
          auto const paramType(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ParamType>(arg)));
          auto const paramName(
            hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(arg)));
          if (paramType=="int"){
            outStubs << "   IPC_Write_Int(&IPC_Abort, "<<paramName<<");\n";
          }
          else if (paramType=="str"){
            outStubs << "   IPC_Write_Str(&IPC_Abort, "<<paramName<<");\n";
          }
          else {
            xju::assert_never_reached();
          }
          outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        }
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   };\n";
        outStubs << "#endif\n";
        outStubs << "   }\n";
        outStubs << "#endif\n";
      }
      if (hcp_ast::findChildrenOfType<SplitRequest>(serverStub).size()){
        // there's only one, so it is hard-coded
        xju::assert_equal(procName, std::string("Get_OdinFile"));
        outStubs << "#ifndef SERVER_ONLY\n";
        outStubs << "void\n";
        outStubs << procName<<"(\n";
        outStubs << "   tp_FileName FileName,\n";
        outStubs << "   tp_Status* StatusPtr,\n";
        outStubs << "   bool* ExecFlagPtr,\n";
        outStubs << "   tp_Str OdinExpr,\n";
        outStubs << "   bool NeedsData)\n";
        outStubs << "{\n";
        outStubs << "   bool IPC_Abort;\n";
        outStubs << "\n";
        outStubs << "   bool IPC_Cmd_Abort;\n";
        outStubs << "\n";
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   if (IsServer && Is_LocalClient(CurrentClient)) {\n";
        outStubs << "      FORBIDDEN(IPC_Do_Return);\n";
        outStubs << "      FORBIDDEN(IPC_SArg1 != NIL);\n";
        outStubs << "      IPC_SArg1 = FileName;\n";
        outStubs << "      FORBIDDEN(IPC_IArg2 != NIL);\n";
        outStubs << "      IPC_IArg2 = StatusPtr;\n";
        outStubs << "      FORBIDDEN(IPC_IArg3 != NIL);\n";
        outStubs << "      IPC_IArg3 = ExecFlagPtr;\n";
        outStubs << "      Local_Get_OdinFile(OdinExpr, NeedsData);\n";
        outStubs << "      if (!IPC_Do_Return) {\n";
        outStubs << "         IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);\n";
        outStubs << "         FORBIDDEN(IPC_Cmd_Abort);\n";
        outStubs << "         FORBIDDEN(!IPC_Do_Return); }/*if*/;\n";
        outStubs << "      IPC_Do_Return = false;\n";
        outStubs << "      IPC_SArg1 = NIL;\n";
        outStubs << "      IPC_IArg2 = NIL;\n";
        outStubs << "      IPC_IArg3 = NIL;\n";
        outStubs << "   }else{\n";
        outStubs << "#endif\n";
        outStubs << "   IPC_Write_Int(&IPC_Abort, "<<procId<<");\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "   IPC_Write_Str(&IPC_Abort, OdinExpr);\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "   IPC_Write_Int(&IPC_Abort, NeedsData);\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "   IPC_Get_Commands(&IPC_Cmd_Abort, (char *)NIL);\n";
        outStubs << "   FORBIDDEN(IPC_Cmd_Abort);\n";
        outStubs << "   FORBIDDEN(!IPC_Do_Return);\n";
        outStubs << "   IPC_Do_Return = false;\n";
        outStubs << "   IPC_Read_Str(&IPC_Abort, FileName);\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "   IPC_Read_Int(&IPC_Abort, StatusPtr);\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "   IPC_Read_Int(&IPC_Abort, ExecFlagPtr);\n";
        outStubs << "   if (IPC_Abort) IPC_Do_Abort();\n";
        outStubs << "#ifndef CLIENT_ONLY\n";
        outStubs << "   };\n";
        outStubs << "#endif\n";
        outStubs << "   }\n";
        outStubs << "#endif\n";
 
      }
    }
  }
  
  
  xju::file::write(
    xju::path::split(argv[3]),
    outStubs.str().c_str(),
    outStubs.str().size(),
    xju::file::Mode(0666));

}

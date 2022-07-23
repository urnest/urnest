/*
Copyright (C) 2016 Trevor Taylor

This file is part of the Odin system.

The Odin system is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation (see the file COPYING).

The Odin system is distributed WITHOUT ANY WARRANTY, without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*/

#include "inc/LookupPlainAbsPath.h"

#include <utility>
#include <string>
#include <map>
#include <algorithm>

extern "C"
{
  
#include "inc/GMC.h"
#include "inc/FileName.h"
#include "inc/FKind_.h"
#include "inc/Flag_.h"
#include "inc/LogLevel_.h"
#include "inc/NodTyp_.h"
#include "inc/Status_.h"
#include "inc/Str.h"

}

namespace
{
//pre: path starts with / and path != "/" and path does not end with /
std::pair<std::string,std::string> splitPath(std::string const& path){
  std::string::const_reverse_iterator i(std::find(path.rbegin(),path.rend(),'/'));
  return std::make_pair(std::string(path.begin(),
                                    path.end()-(i-path.rbegin()+1)),
                        std::string(path.end()-(i-path.rbegin()),
                                    path.end()));
}
static std::map<std::string, tp_FilHdr>* rootMap=0;

std::map<std::string, tp_FilHdr>& makeRootMap(){
  rootMap=new std::map<std::string, tp_FilHdr>;
  rootMap->insert(std::make_pair("",Copy_FilHdr(RootFilHdr)));
  return *rootMap;
}
tp_FilHdr LookupPlainAbsPath_(std::string const& p){
  static std::map<std::string, tp_FilHdr>& known(makeRootMap());
  std::map<std::string, tp_FilHdr>::iterator i(known.find(p));
  if (i!=known.end()){
    tp_FilHdr result(Copy_FilHdr((*i).second));
    if (p.size()) Do_Log("found known abspath", result, LOGLEVEL_IntGenerate);
    return result;
  }
  std::pair<std::string,std::string> db(splitPath(p));
  tp_FilHdr parent(LookupPlainAbsPath_(db.first));
  tp_FilHdr result=Do_Keys(parent,(char*)db.second.c_str());
  known.insert(std::make_pair(p,result));
  return Copy_FilHdr(result);
}
}

tp_FilHdr LookupPlainAbsPath(char const*const path){
  return LookupPlainAbsPath_(std::string(path));
}

void RetAbsPaths()
{
  if (rootMap) {
    for(std::map<std::string, tp_FilHdr>::iterator i=rootMap->begin();
        i!=rootMap->end();
        ++i) {
      Ret_FilHdr((*i).second);
    }
    delete rootMap;
  }
}


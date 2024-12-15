extern "C"
{
#include "inc/GMC.h"
#include "inc/System.hh"
#include "inc/Type.hh"
#include "inc/Func.hh"
#include "inc/Var.hh"
}

#include <sys/inotify.h>
int Create_Inotify_Fd(){
  abort();
}

void Inotify_Watch_Dir(tp_FileName FileName){
}

void Inotify_Watch_File(tp_FileName FileName){
}
char const* Inotify_Get_Next_Change() {
  return 0;
}

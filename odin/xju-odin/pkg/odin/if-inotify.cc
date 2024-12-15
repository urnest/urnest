extern "C"
{
#include "inc/GMC.h"
#include "inc/System.hh"
#include "inc/Type.hh"
#include "inc/Func.hh"
#include "inc/Var.hh"
}

#include <sys/inotify.h>
#include <set>
#include <map>
#include <string>
#include <algorithm>

static int inotifyFd = -1;
std::set<std::string>* watching = 0;
std::map<int, std::string>* watches = 0;

int Create_Inotify_Fd(){
  if (inotifyFd != -1){
    fprintf(stderr,"Create_Inotify_Fd already called\n");
    abort();
  }
  /* fprintf(stderr,"Create_Inotify_Fd\n"); */
  inotifyFd = inotify_init();
  if (inotifyFd == -1){
    abort();
  }
  watching = new std::set<std::string>;
  watches = new std::map<int, std::string>;
  return inotifyFd;
}

bool Is_Inside_Cache(std::string const& d){
  static bool x=false;
  static std::string cache_dir;
  if (!x){
    cache_dir=std::string(OdinDirName);
    x=true;
  }
  if (d.size() >= cache_dir.size()){
    if (std::equal(cache_dir.begin(), cache_dir.end(), d.begin()) &&
        (d.size()==cache_dir.size() ||
         d[cache_dir.size()]=='/')){
      return true;
    }
  }
  return false;
}

void Inotify_Watch_Dir(tp_FileName FileName){
  if (inotifyFd==-1){
    return;
  }
  /* fprintf(stderr, "Inotify_Watch_Dir %s\n", FileName); */
  std::string d(FileName);
  if (d==""){
    d="/";
  }
  if (Is_Inside_Cache(d)){
    /* fprintf(stderr, "(ignore - cache file)\n"); */
    return;
  }
  if (std::find(watching->begin(), watching->end(), d)==watching->end()){
    int w=inotify_add_watch(inotifyFd, d.c_str(),(
                              IN_ATTRIB|IN_CLOSE_WRITE|IN_CREATE|IN_MOVED_TO|
                              IN_DELETE|IN_MOVED_FROM|IN_ONLYDIR));
    if (w==-1){
      /* fprintf(stderr, "failed to watch %s\n",d.c_str()); */
      perror(" because");
      abort();
    }
    else{
      watching->insert(d);
      watches->insert({w,d});
    }
  }
}

void Inotify_Watch_File(tp_FileName FileName){
  if (inotifyFd==-1){
    return;
  }
  /* fprintf(stderr, "Inotify_Watch_File %s\n", FileName); */
  char const* lastSlash=0;
  for(char const* p=FileName; *p != 0; ++p){
    if (*p == '/'){
      lastSlash=p;
    }
  }
  if (lastSlash){
    std::string d((char const*)FileName, lastSlash);
    Inotify_Watch_Dir((char*)d.c_str());
  }
  else{
    /* fprintf(stderr, "%s not absolute?\n", FileName); */
  }
}
char const* Inotify_Get_Next_Change() {
  static std::string result;
  static uint8_t* const buffer = (uint8_t*)malloc(32000);
  static uint8_t* end=buffer;
  static size_t thisRead=0;
  static uint8_t* p=end;
  if (inotifyFd==-1){
    fprintf(stderr, "Inotify_Get_Next_Change called before Create_Inotify_Fd\n");
    abort();;
  }
  if(thisRead==0){
    fd_set _readfds, *readfds = &_readfds;
    FD_ZERO(readfds);
    FD_SET(inotifyFd, readfds);
    struct timeval t = { 0, 0 };
    const int nfds = select(inotifyFd+1, readfds, 0, 0, &t);
    if (!FD_ISSET(inotifyFd, readfds)){
      return 0;
    }
    thisRead=read(inotifyFd,buffer,32000);
    p=buffer;
    end=p+thisRead;
  }
  if((end-p)>=sizeof(inotify_event)){
    auto event((inotify_event const*)p);
    p+=sizeof(inotify_event)+event->len;
    auto const i(watches->find(event->wd));
    if (i==watches->end()){
      fprintf(stderr,"unknown watch descriptor %d for changed file %s\n", event->wd, event->name);
    }
    else{
      if(event->len){ // len is zero if watched directory changes
        result=(*i).second+"/"+event->name;
      }
      else{
        result=(*i).second;
      }
      /* fprintf(stderr, "Inotify_Get_Next_Change reporting %s\n", result.c_str()); */
      return result.c_str();
    }
  }
  thisRead=0;
  end=buffer;
  p=buffer;
  return 0;
}

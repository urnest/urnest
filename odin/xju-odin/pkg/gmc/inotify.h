#ifndef GMC_INOTIFY_H
#define GMC_INOTIFY_H

#include <gmc/gmc.h>

extern int Create_Inotify_Fd();
extern void Inotify_Watch_Dir(tp_FileName FileName);
extern void Inotify_Watch_File(tp_FileName FileName);
extern char const* Inotify_Get_Next_Change();

#endif

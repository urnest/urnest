#include "file.h"

void Get_FileInfo(tp_SKind* SKindPtr,int* SysModTimePtr,tp_FileName FileName)
{
   struct stat buf;
   int status;

   FORBIDDEN(FileName == ERROR);

   status = lstat(FileName, &buf);
   if (status != 0) {
      *SKindPtr = SK_NoFile;
      *SysModTimePtr = 0;
      return; }/*if*/;
   *SysModTimePtr = buf.st_mtime;
   /*select*/{
      if ((buf.st_mode & S_IFLNK) == S_IFLNK) {
         Inotify_Watch_File(FileName);
	 *SKindPtr = SK_SymLink;
	 status = stat(FileName, &buf); /*give automounter a kick*/
      }else if ((buf.st_mode & S_IFDIR) == S_IFDIR) {
         Inotify_Watch_Dir(FileName);
	 *SKindPtr = SK_Dir;
      }else if ((buf.st_mode & S_IEXEC) == S_IEXEC) {
         Inotify_Watch_File(FileName);
	 *SKindPtr = SK_Exec;
      }else if ((buf.st_mode & S_IFREG) == S_IFREG) {
         Inotify_Watch_File(FileName);
	 *SKindPtr = SK_Reg;
      }else{
         Inotify_Watch_File(FileName);
	 *SKindPtr = SK_Special; };}/*select*/;
   }/*Get_FileInfo*/



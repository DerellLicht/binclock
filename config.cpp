//****************************************************************************
//  Copyright (c) 2009-2017  Daniel D Miller
//  
//  Written by:  Dan Miller
//****************************************************************************
//  Filename will be same as executable, but will have .ini extensions.
//  Config file will be stored in same location as executable file.
//  Comments will begin with '#'
//  First line:
//  device_count=%u
//  Subsequent file will have a section for each device.
//****************************************************************************
#include <windows.h>
#include <stdio.h>   //  fopen, etc
#include <stdlib.h>  //  atoi()
#include <limits.h>  //  PATH_MAX

#include "common.h"
#include "binclock.h"

static char ini_name[PATH_MAX+1] = "" ;

//****************************************************************************
static void strip_comments(char * const bfr)
{
   char * const hd = strchr(bfr, '#') ;
   if (hd != 0)
      *hd = 0 ;
}  //lint !e818  Pointer parameter 'bfr' (line 38) could be declared as pointing to const

//****************************************************************************
// bitmap_idx=2
// bit_menu=2
// attr_on=65280
// attr_off=16512
// layout=1
static LRESULT save_default_ini_file(void)
{
   FILE *fd = fopen(ini_name, "wt") ;
   if (fd == 0) {
      LRESULT result = (LRESULT) GetLastError() ;
      syslog("%s open: %s\n", ini_name, get_system_message((DWORD) result)) ;
      return result;
   }
   //  save any global vars
   fprintf(fd, "bitmap_idx=%u\n", bitmap_idx) ;
   fprintf(fd, "bit_menu=%u\n", bit_menu) ;
   fprintf(fd, "attr_on=%u\n", attr_on) ;
   fprintf(fd, "attr_off=%u\n", attr_off) ;
   fprintf(fd, "layout=%u\n", layout_method) ;
   fclose(fd) ;
   return ERROR_SUCCESS;
}

//****************************************************************************
LRESULT save_cfg_file(void)
{
   return save_default_ini_file() ;
}

//****************************************************************************
//  - derive ini filename from exe filename
//  - attempt to open file.
//  - if file does not exist, create it, with device_count=0
//    no other data.
//  - if file *does* exist, open/read it, create initial configuration
//****************************************************************************
LRESULT read_config_file(void)
{
   char inpstr[128] ;
   LRESULT result = derive_filename_from_exec(ini_name, (char *) ".ini") ; //lint !e1773
   if (result != 0)
      return result;

   FILE *fd = fopen(ini_name, "rt") ;
   if (fd == 0)
      return save_default_ini_file() ;

   while (fgets(inpstr, sizeof(inpstr), fd) != 0) {
      strip_comments(inpstr) ;
      strip_newlines(inpstr) ;
      if (strlen(inpstr) == 0) {
         continue;
      }
      char *iptr = strchr(inpstr, '=');
      if (iptr == NULL) {
         continue;
      }
      *iptr++ = 0 ;

      if (strcmp(inpstr, "bitmap_idx") == 0) {
         // syslog("enabling factory mode\n") ;
         bitmap_idx = (uint) atoi(iptr) ;
      } else
      if (strcmp(inpstr, "bit_menu") == 0) {
         // syslog("enabling factory mode\n") ;
         bit_menu = (uint) atoi(iptr) ;
      } else
      if (strcmp(inpstr, "attr_on") == 0) {
         // syslog("enabling factory mode\n") ;
         attr_on = (uint) atoi(iptr) ;
      } else
      if (strcmp(inpstr, "attr_off") == 0) {
         // syslog("enabling factory mode\n") ;
         attr_off = (uint) atoi(iptr) ;
      } else
      if (strcmp(inpstr, "layout") == 0) {
         // syslog("enabling factory mode\n") ;
         layout_method = (uint) atoi(iptr) ;
      } else
      {
         syslog("unknown: [%s]\n", inpstr) ;
      }
   }
   return 0;
}


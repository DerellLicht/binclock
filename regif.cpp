//**********************************************************************
//  regif.cpp/hpp
//  This mis-named class is actually an INI-file manager.
//  It started out being an encapsulation of Windows Registry functions,
//  but after having various problems with registry management,
//  I've switched back to using ini files for configuration.
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  09/27/06 07:44
//  
//**********************************************************************
//  09/27/06 revision 
//  
//  Well, I'm having to replace the Windows PrivateProfileString
//  functions, since the Get function neither updates the ini file
//  as required (when the target field does not yet exist),
//  nor does it provide any indication that this update is required.
//  
//  Since I'm having to re-write this to deal with ini-file
//  updates myself, I'm going to just disregard the group name
//  for now.  This is reasonable since I typically use a different
//  ini file for each application anyway.  Group names are needed when
//  one uses a common INI for multiple applications, to avoid
//  namespace collisions.
//  
//  Anyway, because I want to get this done with, I'm going to
//  do it the simple way until I see some requirement for a complex
//  solution (which will probably occur on my very next application).
//**********************************************************************
#include <windows.h>
#include <stdlib.h>  //  _MAX_PATH
#include <errno.h>
#include <sys/stat.h>

#include "common.h"
#include "regif.hpp"

//*****************************************************************
#include <stdio.h>

int registry_iface::find_exe_location(void)
{
   char *strptr ;
   struct stat st ;

   //  see if we can find and show parent filename
   // if (GetModuleFileName(g_hInst, filename, sizeof(filename)) == 0) {
   if (GetModuleFileName(NULL, ininame, sizeof(ininame)) != 0) {
      strptr = strrchr(ininame, '.') ;
      //  if no extention on existing path, just concat the name
      if (strptr == 0) {
         strcat(ininame, ".ini") ;
      } 
      //  if extension *is* present, overwrite with .ini
      else {
         strcpy(strptr, ".ini") ;
      }
   } else {
      wsprintf(errstr, "GetModuleFileName: %s", get_system_message()) ;
      OutputDebugString(errstr) ;
      strcpy(ininame, "c:\\dummy_ini.ini") ;
   }

   //  lastly, see if file already exists
   int result = stat(ininame, &st) ;
   ini_exists = (result == 0) ? 1 : 0 ;
   return 0;
}

//************************************************************
//  class constructor
//************************************************************
registry_iface::registry_iface(char *group_name)
{
   // hwndStatus = 0 ;

   //  derive INI filename, and see if it exists
   status = find_exe_location() ;

   //  select a group name
   if (group_name == 0  ||  *group_name == 0)
      grp_name[0] = 0 ;
   else
      strcpy(grp_name, group_name) ;

   if (strlen(grp_name) >= REG_FIELD_LEN) 
      status = EINVAL ;

   //  if file doesn't exist, create it and add group entry.
   if (status == 0  &&  !ini_exists) {
      FILE *fd = fopen(ininame, "wt") ;
      if (fd == 0) {
         status = errno ;
         return ;
      }
      fprintf(fd, "[%s]\n", grp_name) ;
      fclose(fd) ;
   }
}

//************************************************************
//  class destructor
//************************************************************
registry_iface::~registry_iface()
{
   //  nothing to do so far
}

//*****************************************************************
int registry_iface::read_ini_file_into_buffer(void)
{
   // char ini_read_bfr[REG_MAX_LINES][REG_FIELD_LEN+1] ;
   int result = 0 ;
   FILE *fd = fopen(ininame, "rt") ;
   if (fd == 0) 
      return -errno ;
   unsigned idx = 0 ;
   while (fgets(ini_read_bfr[idx], REG_FIELD_LEN, fd) != 0) {
      strip_newlines(ini_read_bfr[idx]) ;
      if (++idx == REG_MAX_LINES) {
         result = -EFBIG ;
         break;
      }
   }
   fclose(fd) ;
   if (result == 0)
      result = idx ;
   read_bfr_valid = 1 ;
   rbfr_count = idx ;
   return result ;
}

//*****************************************************************
int registry_iface::WriteRegifProfileString(char *field, char *value)
{
   int j, idx ;
   if (!read_bfr_valid) {
      idx = read_ini_file_into_buffer() ;
      if (idx < 0)
         return idx;
   }
   //  now, search the buffer for our target, and write the new value.
   //  If field not found, add entry to end of buffer.
   //  When done, write the updated ini file.
   //  see what we read.
   idx = rbfr_count ;
   char *eqptr ;
   int found = 0 ;
   int flen = strlen(field) ;
   for (j=0; j<idx; j++) {
      if (strncmp(field, ini_read_bfr[j], flen) == 0) {
         //  if I found the target string, return what we found
         eqptr = strchr(ini_read_bfr[j], '=') ;
         if (eqptr == 0) 
            continue;
         eqptr++ ;
         wsprintf(errstr, "%s=%s", field, value) ;
         strncpy(ini_read_bfr[j], errstr, sizeof(ini_read_bfr[j])) ;
         found = 1 ;
         break;
      }
   }
   if (!found) {
      wsprintf(errstr, "%s=%s", field, value) ;
      strncpy(ini_read_bfr[idx], errstr, sizeof(ini_read_bfr[idx])) ;
      idx++ ;
   }
   //  write the output file
   FILE *fd = fopen(ininame, "wt") ;
   if (fd == 0) 
      return -errno ;
   for (j=0; j<idx; j++) {
      fprintf(fd, "%s\n", ini_read_bfr[j]);
   }
   fclose(fd) ;
   
   read_bfr_valid = 0 ;
   return 0;
}

//*****************************************************************
//  Note: this will ONLY return non-zero if file accesses fail
//*****************************************************************
int registry_iface::GetRegifProfileString(char *field, char *dflt, char *bfr, int bfrsize)
{
   char *eqptr ;
   int flen ;
   int idx = read_ini_file_into_buffer() ;
   if (idx < 0)
      return idx;

   if (idx == 0) 
      goto return_default;

   //  see what we read.
   flen = strlen(field) ;
   for (int j=0; j<idx; j++) {
      // wsprintf(errstr, "seek [%s]", field) ;
      // OutputDebugString(errstr) ;
      if (strncmp(field, ini_read_bfr[j], flen) == 0) {
         // wsprintf(errstr, "   %u:[%s]", j, ini_read_bfr[j]) ;
         // OutputDebugString(errstr) ;
         
         //  if I found the target string, return what we found
         eqptr = strchr(ini_read_bfr[j], '=') ;
         if (eqptr == 0) {
            OutputDebugString("returning default") ;
            goto return_default;
         }
         eqptr++ ;
         // wsprintf(errstr, "   returning [%s]", eqptr) ;
         // OutputDebugString(errstr) ;
         strncpy(bfr, eqptr, bfrsize) ;
         return 0;
      }
   }
   //  if not found, update registry
   // goto return_default;
   
return_default:
   strncpy(bfr, dflt, bfrsize) ; //  return default string
   //  then update existing registry
   return WriteRegifProfileString(field, dflt);
}

//*****************************************************************
//    assumptions: Global variables:
//       char ininame[] ;  //  defined in wcommon.cpp
//       HWND hwndStatus ;
//*****************************************************************
void registry_iface::ini_write_tag(char *grp, char *name, char *lstr)
{
   // char errstr[81] ;
   // sprintf(errstr, "grp=%s, name=%s, lstr=%s, ininame=%s", 
   //    grp, name, lstr, ininame) ;
   // show_message(NULL, errstr) ;

   if (WriteRegifProfileString(name, lstr) == 0) {
      // if (hwndStatus != 0) {
      //    wsprintf(pbfr, "INI write %s %s: %s", grp, name, get_system_message()) ;
      //    SetWindowText(hwndStatus, pbfr) ;
      // }
   }
}  //lint !e715

void registry_iface::ini_write_tag(char *grp, char *name, unsigned inval)
{
   char bfr[12] ;
   wsprintf(bfr, "%u", inval) ;
   ini_write_tag(grp, name, bfr) ;
}

void registry_iface::ini_write_tag(char *grp, char *name, double inval)
{
   char bfr[12] ;
   sprintf(bfr, "%f", inval) ;
   ini_write_tag(grp, name, bfr) ;
}

//*****************************************************************
void registry_iface::ini_read_tag(char *grp, char *name, char *rval, int rval_len, char *dflt)
{
   int rlen = GetRegifProfileString(name, dflt, rval, rval_len) ;
   if (rlen == 0) {
      // char tempstr[80] ;
      // sprintf(tempstr, "ini_read_tag: %s", get_system_message()) ;
      // show_message(NULL, tempstr) ;
      // if (hwndStatus != 0) {
      //    wsprintf(pbfr, "INI read %s: %s not found", grp, name) ;
      //    SetWindowText(hwndStatus, pbfr) ;
      // }
   } else {
      // char tempstr[80] ;
      // sprintf(tempstr, "%s, %s, %s\n", grp, name, rval) ;
      // OutputDebugString(tempstr) ;
      // sprintf(tempstr, "GetRegifProfileString returned %u\n", (unsigned) rlen) ;
      // OutputDebugString(tempstr) ;
   }
}  //lint !e715

void registry_iface::ini_read_tag(char *grp, char *name, unsigned *rval, unsigned dflt_val)
{
   char bfr[16], dflt[16] ;

   wsprintf(dflt, "%u", dflt_val) ;
   wsprintf(bfr,  "%u", *rval) ;
   if (GetRegifProfileString(name, dflt, bfr, (int) sizeof(bfr)) != 0) {
      *rval = dflt_val ;
      // if (hwndStatus != 0) {
      //    wsprintf(pbfr, "INI read %s: %s not found", grp, name) ;
      //    SetWindowText(hwndStatus, pbfr) ;
      // }
   } else {
      *rval = (unsigned) atoi(bfr) ;
   }
}  //lint !e715

void registry_iface::ini_read_tag(char *grp, char *name, double *rval, double dflt_val)
{
   char bfr[16], dflt[16] ;

   sprintf(dflt, "%f", dflt_val) ;
   sprintf(bfr,  "%f", *rval) ;
   if (GetRegifProfileString(name, dflt, bfr, (int) sizeof(bfr)) != 0) {
      *rval = dflt_val ;
      // if (hwndStatus != 0) {
      //    wsprintf(pbfr, "INI read %s: %s not found", grp, name) ;
      //    SetWindowText(hwndStatus, pbfr) ;
      // }
   } else {
      *rval = (double) strtod(bfr, 0) ;
   }
}  //lint !e715

//**************************************************************
//  NOTE that change_group does not actually verify
//  the existance of a group by that name!!
//  It just stores a copy of the name in this client
//  class, to expedite later registry accesses.
//**************************************************************
int registry_iface::change_group(char *group_name)
{
   if (strlen(group_name) >= REG_FIELD_LEN) {
      status = EINVAL ;
   } else {
      status = 0 ;
      strcpy(grp_name, group_name) ;
   }
   return status ;
}

//**************************************************************
int registry_iface::get_param(char *var_name, unsigned *value)
{
   int dflt_val = *value;
   ini_read_tag(grp_name, var_name, value, dflt_val) ;
   status = 0 ;
   return 0 ;
}

//**************************************************************
int registry_iface::get_param(char *var_name, double *value)
{
   double dflt_val = *value;
   ini_read_tag(grp_name, var_name, value, dflt_val) ;
   status = 0 ;
   return 0 ;
}

//**************************************************************
int registry_iface::get_param(char *var_name, char *value)
{
   char pbfr[REG_FIELD_LEN] ;

   if (strlen(value) >= REG_FIELD_LEN) {
      // if (hwndStatus != 0) 
      //    SetWindowText(hwndStatus, "get_param: default too long") ;
      wsprintf(errstr, "get_param_str, too long\n") ;
      OutputDebugString(errstr) ;
      status = EINVAL ;
   } else {
      // char tempstr[260] ;
      // sprintf(tempstr, "ini name: %s\n", ininame) ;
      // OutputDebugString(tempstr) ;
      strcpy(pbfr, value) ;
      ini_read_tag(grp_name, var_name, value, REG_FIELD_LEN, pbfr) ;
      status = 0 ;
   }
   return status ;
}

//**************************************************************
int registry_iface::set_param(char *var_name, unsigned value)
{
   ini_write_tag(grp_name, var_name, value) ;
   status = 0 ;
   return status ;
}

//**************************************************************
int registry_iface::set_param(char *var_name, double value)
{
   ini_write_tag(grp_name, var_name, value) ;
   status = 0 ;
   return status ;
}

//**************************************************************
int registry_iface::set_param(char *var_name, char *value)
{
   ini_write_tag(grp_name, var_name, value) ;
   status = 0 ;
   return status ;
}


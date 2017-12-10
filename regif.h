//**********************************************************************
//  regif.cpp/hpp
//  These files represent a class which encapsulates the interface 
//  between the registry server (grimalkin) and clients.  
//  Two specific mechanisms are encapsulated by this class:
//  
//  - The message queues
//    These message queues are the mechanisms which are used to
//    provide communication between clients and server.
//  
//  - The registry command structure
//    These classes translate the internal registry data structures
//    into a more-intuitive named command api.
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  06/09/03 14:07
//  
//**********************************************************************
#define  REG_FIELD_LEN     128
#define  REG_MAX_LINES     100

//************************************************************
//  this is the client-side interface to the registry
//************************************************************
class registry_iface {
private:
   char ininame[_MAX_PATH] ;
   char grp_name[REG_FIELD_LEN+1] ;
   char ini_read_bfr[REG_MAX_LINES][REG_FIELD_LEN+1] ;
   int  rbfr_count ;
   int  read_bfr_valid ;
   int  status ;
   int  ini_exists ;
   char errstr[260] ;

   int  find_exe_location(void) ;
   void ini_write_tag(char *grp, char *name, char *lstr);
   void ini_write_tag(char *grp, char *name, unsigned inval);
   void ini_write_tag(char *grp, char *name, double inval);
   void ini_read_tag(char *grp, char *name, char *rval, int rval_len, char *dflt);
   void ini_read_tag(char *grp, char *name, unsigned *rval, unsigned dflt_val);
   void ini_read_tag(char *grp, char *name, double *rval, double dflt_val);
   int  read_ini_file_into_buffer(void);
   int  WriteRegifProfileString(char *field, char *value) ;
   int  GetRegifProfileString(char *field, char *dflt, char *bfr, int bfrsize) ;
   
public:
//lint -esym(1712, registry_iface)  default constructor not defined for class 'registry_iface'
    registry_iface(char *group_name) ;
   ~registry_iface(void) ;
   int  change_group(char *group_name) ;
   int  get_status(void) { return status ; } ;
   int  ini_file_exists(void) { return ini_exists ; } ;

   // void set_status_window(HWND hwnd) { hwndStatus = hwnd ; } ;
   int  get_param(char *var_name, unsigned *value) ;
   int  get_param(char *var_name, char *value) ;
   int  get_param(char *var_name, double *value) ;

   int  set_param(char *var_name, unsigned value);
   int  set_param(char *var_name, char *value);
   int  set_param(char *var_name, double value) ;
} ;


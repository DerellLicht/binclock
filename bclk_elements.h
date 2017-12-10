//***********************************************************************
//  adding new graphics sets to binclock
//  - add sprite name
//  - LoadImage / DestroyObject
//  - define WIDTH/HEIGHT/OFFSET/MASK
//  - add masking function
//  - add drawing function
//  - build SystemTray menu dynamically
//***********************************************************************
//  TODO:
//  
//  - led_color still needs to be managed by the class; 
//    This is actually an offset into the bitmap
//  
//***********************************************************************

typedef unsigned char   u8 ;

#define  BE_LINEAR   0
#define  BE_PAIRS    1
#define  BE_DRAWN    2

//  box-drawing parameters
#define  BX_SOLID          0
#define  BX_SHADOW_OUTER   1
#define  BX_SHADOW_INNER   2

#define  IMG_FNAME_LEN  1024

//***********************************************************************
class bclock_element {
private:
   char bm_name[IMG_FNAME_LEN+1] ;
   unsigned el_width ;
   unsigned el_height ;
   unsigned flags ;
   int mask_idx ; //  negative means no mask is used
   unsigned off_idx ;
   int x_offset ;
   int y_offset ;
   u8 *skip_elements ;
   unsigned num_elements ;
   unsigned curr_element ;
   HBITMAP hSpriteBitmap;
   HMENU menu_hdl ;
   unsigned menu_code ;
   unsigned object_code ;
   char **color_menu_str ;
   char menu_str[30] ;
   char errstr[81] ;

   //  attributes for draw_element
   COLORREF attr_lhigh ;
   COLORREF attr_llow  ;
   // In the meantime, I should let user select attr_high/low.
   COLORREF attr_high ;
   COLORREF attr_low  ;

   //  internal functions
   COLORREF select_color(COLORREF init_attr);
   void Box(HDC hdc, int x0, int y0, int x1, int y1, unsigned style, COLORREF fgattr, COLORREF bgattr);
   void Solid_Rect(HDC hdc, int xl, int yu, int xr, int yl, COLORREF Color);
   void draw_frame(HDC hdc, unsigned x, unsigned y, unsigned on_noff);
   
public:
   bclock_element(HINSTANCE g_hInst, char *name, unsigned width, unsigned be_flags, 
            int mask_index, unsigned off_index, unsigned start_element);
   ~bclock_element() ;
   unsigned add_menu_data(unsigned umenu_code, char *mstr) ;
   int get_menu_id(unsigned menu_idx) ;
   char *get_menu_str(void) ;
   void set_image_offsets(int dx, int dy) ;
   void mask_the_source(HDC hdc) ;
   void draw_sprite(HDC hdc, unsigned on_noff, unsigned xidest, unsigned yidest);
   unsigned get_on_color(void) { return curr_element ; } ;
   unsigned get_off_color(void) { return off_idx ; } ;
   unsigned next_led_color(void) ;
   void add_color_menu_str(unsigned menu_idx, char *mstr) ;
   void add_skip_element(unsigned idx) {
      if (idx < num_elements) 
         skip_elements[idx] = 1 ;
   } ;
   HMENU get_menu_handle(void) { return menu_hdl; } ;
   HMENU build_options_menu(void);
   void set_element_attr(COLORREF fgnd, COLORREF bgnd);
} ;


//***********************************************************************
//  adding new graphics sets to binclock
//  - add sprite name
//  - LoadImage / DestroyObject
//  - define WIDTH/HEIGHT/OFFSET/MASK
//  - add masking function
//  - add drawing function
//  - build SystemTray menu dynamically
//***********************************************************************

#include <windows.h>
// #include <string.h>

#include "common.h"
#include "binclock.h"
#include "bclk_elements.h"

static unsigned be_object_num = 0 ;

//lint -esym(1762, bclock_element::Box, bclock_element::Solid_Rect)

//***********************************************************************
unsigned bclock_element::next_led_color(void)
{
   if (flags & BE_DRAWN) {
      
   } else {
      curr_element++ ;
      while (1) {
         if (curr_element == num_elements) {
            curr_element = 0 ;
            continue;
         }
         if (skip_elements[curr_element] != 0) {
            curr_element++ ;
            continue;
         }
         break;
      }
      if (flags & BE_PAIRS) 
         off_idx = curr_element - 1 ;
   }

   return curr_element ;
}

//***********************************************************************
bclock_element::bclock_element(HINSTANCE g_hInst, char *name, unsigned width, 
   unsigned be_flags, int mask_index, unsigned off_index, unsigned start_element) :
   bm_name(0),
   el_width(width),
   el_height(0),
   flags(be_flags),
   mask_idx(mask_index),
   off_idx(off_index),
   x_offset(0),
   y_offset(0),
   skip_elements(0),
   num_elements(0),
   curr_element(start_element),
   hSpriteBitmap(NULL),
   hdlMenu(0),
   menu_code(0),
   object_code(0),
   color_menu_str(NULL),
   menu_str(NULL),
   attr_lhigh(0),
   attr_llow (0),
   attr_high(0),
   attr_low (0)
{
   BITMAP bm;
   object_code = be_object_num++ ;

   bm_name  = (char *) new char[IMG_FNAME_LEN+1] ;
   menu_str = (char *) new char[MENU_STR_LEN+1] ;

   //  if no filename provided, assume BE_DRAWN format
   //  i.e., the "led" will be a drawn image, not an image-file element
   if (name == 0) {
      bm_name[0] = 0 ;
      hSpriteBitmap = 0 ;

      el_height = width ;
      //  for drawn types, num_elements (which is an overloaded variable
      //  which represents *both* number of drawable elements in the bitmap,
      //  and number of string elements in the menu-string list
      num_elements = 4 ;   //  hard-coded for now

      //  BE_DRAWN attributes
      attr_lhigh = GetSysColor(COLOR_BTNHIGHLIGHT) ;
      attr_llow  = GetSysColor(COLOR_BTNSHADOW) ;
      attr_high  = RGB(0, 255, 0) ;
      attr_low   = RGB(63, 63, 63) ;
   } 
   //  open the bitmap file, read sprite images into memory
   else {
      strncpy(bm_name, name, IMG_FNAME_LEN) ;
      bm_name[IMG_FNAME_LEN] = 0 ;
      hSpriteBitmap = (HBITMAP) LoadImage (g_hInst, bm_name, 
         IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
      if (hSpriteBitmap == NULL) {
         syslog("%s: LoadImage: %s\n", bm_name, get_system_message()) ;
      }

      if (GetObject ((HGDIOBJ) hSpriteBitmap, sizeof (BITMAP), &bm) == 0) {
         syslog("%s: GetObject: %s\n", bm_name, get_system_message()) ;
      }
      el_height = bm.bmHeight ;
      if (el_width == 0) {
         el_width = bm.bmHeight ;
      }
      num_elements = (unsigned) bm.bmWidth / el_width ;
   }

   // syslog("%s: loaded, width=%u, elements=%u\n", bm_name, el_width, num_elements) ;
   color_menu_str = new char *[num_elements] ;
   unsigned j ;
   for (j=0; j<num_elements; j++) {
      char colorstr[20];   //  temp buffer
      wsprintf(colorstr, "color %u", j) ;
      color_menu_str[j] = new char[strlen(colorstr)+1] ;
      strcpy(color_menu_str[j], colorstr) ;
   }

   skip_elements = new u8[num_elements] ;
   ZeroMemory(skip_elements, num_elements) ;
   if (off_idx >= num_elements  ||  (mask_idx > 0  &&  (unsigned) mask_idx >= num_elements)) {
      syslog("ERROR: invalid elements: num_elements=%u, off_idx=%u, mask_idx=%d\n", 
         num_elements, off_idx, mask_idx);
   }
   else {
      skip_elements[off_idx] = 1 ;
      if (mask_idx >= 0) {
         skip_elements[mask_idx] = 1 ; //lint !e661 !e662
      }
   }
}

//***********************************************************************
bclock_element::~bclock_element()
{
   DeleteObject ((HGDIOBJ) hSpriteBitmap);
   hSpriteBitmap = NULL ;
   delete[] bm_name ;
   delete[] menu_str ;
   delete[] skip_elements ;
   hdlMenu = NULL ;
   //  how do I free color_menu_str[] ???
//lint -esym(1740, bclock_element::color_menu_str)

}

//******************************************************************
//  these are used only for BE_DRAWN option
//******************************************************************
void bclock_element::set_element_attr(COLORREF fgnd, COLORREF bgnd)
{
   attr_high = fgnd ;
   attr_low  = bgnd ;
}

//******************************************************************
void bclock_element::set_image_offsets(int dx, int dy)
{
   x_offset = dx ;
   y_offset = dy ;
}

//******************************************************************
//  this function sets aside menuID codes for all of its 
//  sub-menu elements.  So sub-menu item I = umenu_code + I
//  It then returns the next valid menuID
//******************************************************************
unsigned bclock_element::add_menu_data(unsigned umenu_code, char *mstr)
{
   menu_code = umenu_code ;
   strncpy(menu_str, mstr, MENU_STR_LEN) ;
   menu_str[MENU_STR_LEN] = 0 ; //  make sure line is NULL-term
   umenu_code += num_elements ;  //  reserve menu/message numbers for all colors
   return umenu_code ;
}

//******************************************************************
void bclock_element::add_color_menu_str(unsigned menu_idx, char *mstr)
{
   //  for BE_DRAWN, num_elements is currently hard-coded in constructor...
   //  There's gotta be a better way to handle that...
   if (menu_idx >= num_elements)
      return ;

   if (color_menu_str[menu_idx] != 0)
      delete[] color_menu_str[menu_idx] ;

   color_menu_str[menu_idx] = new char[strlen(mstr)+1] ;
   strcpy(color_menu_str[menu_idx], mstr) ;
}

//****************************************************************
COLORREF bclock_element::select_color(COLORREF init_attr)
{
   static CHOOSECOLOR cc ;
   static COLORREF    crCustColors[16] ;

   ZeroMemory(&cc, sizeof(cc));
   cc.lStructSize    = sizeof (CHOOSECOLOR) ;
   // cc.rgbResult      = RGB (0x80, 0x80, 0x80) ; // set initial color in dialog
   cc.rgbResult      = init_attr ; // set initial color in dialog
   cc.lpCustColors   = crCustColors ;
   cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;

   if (ChooseColor(&cc) == TRUE) {
      return cc.rgbResult ;
   } else {
      return 0 ;
   }
}

//******************************************************************
//  Okay, this is going a little awry...
//  For BE_DRAWN, which has SetColor functions in the
//  color-selection menu, this function needs to act differently;
//  hmmm....
//  I guess for SetColor functions, this should call select_color(), 
//  set the appropriate attribute, then return -1 so caller doesn't do anything else...
//  
//  Not a very clean solution; the user of the class will have
//  a HARD time anticipating how this function works.
//******************************************************************
int bclock_element::get_menu_id(unsigned menu_idx) 
{
   if (menu_idx < menu_code) 
      return -1 ;
   if (menu_idx >= (menu_code+num_elements)) 
      return -1 ;
   //  otherwise, it's to us
   curr_element = menu_idx - menu_code ;
   if ((flags & BE_DRAWN)  &&  curr_element > 1) {
      if (curr_element == 2) {   //  set foreground color
         attr_high = select_color(attr_high) ;
         attr_on = attr_high ;
         // inireg.set_param("attr_on", (unsigned) attr_high) ;
      } else
      if (curr_element == 3) {   //  set background color
         attr_low = select_color(attr_low) ;
         attr_off = attr_low ;
         // inireg.set_param("attr_off", (unsigned) attr_low) ;
      } 
//       syslog("curr_element=%u (mi=%u, mc=%u)\n", curr_element, menu_idx, menu_code) ;
//       inireg.set_param("bit_menu", (unsigned) curr_element) ;
      save_cfg_file();
      return -1;
   } 
   if (flags & BE_PAIRS) 
      off_idx = curr_element - 1 ;
   //  this needs to store the actual index into the array...
   // inireg.set_param("bit_menu", (unsigned) curr_element) ;
   bit_menu = curr_element ;
   save_cfg_file();
   return object_code ;
}

//******************************************************************
char *bclock_element::get_menu_str(void) 
{
   return menu_str;
}

//******************************************************************
//  mask the target array (do this ONCE)
//******************************************************************
void bclock_element::mask_the_source(HDC hdc)
{
   if (mask_idx < 0)
      return ;
   unsigned xmask  = (unsigned) mask_idx * el_width  ;

   HDC hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hSpriteBitmap);

   unsigned j ;
   for (j=0; j<num_elements; j++) {
      //  don't mask the mask image!!
      if (mask_idx >= 0  &&  j == (unsigned) mask_idx)
         continue;

      unsigned xsrc = j * el_width  ;
      if (!BitBlt (hdcMem, xsrc, 0, el_width, el_height, hdcMem, xmask, 0, SRCINVERT)) {
         syslog("BitBlt (source mask): %s", get_system_message()) ;
      }
   }

   DeleteDC (hdcMem);
}

//******************************************************************
HMENU bclock_element::build_options_menu(void)
{
   unsigned j ;
   // char mmsg[40] ;
   HMENU hMenuOptions = CreateMenu() ;
      // AppendMenu(hMenuOptions, MF_STRING, IDM_UNDO,  "&Undo") ;
      // AppendMenu(hMenuOptions, MF_SEPARATOR, 0, NULL) ;
      // AppendMenu(hMenuOptions, MF_STRING, IDM_CUT,   "Cu&t") ;
      // AppendMenu(hMenuOptions, MF_STRING, IDM_COPY,  "&Copy") ;
      // AppendMenu(hMenuOptions, MF_STRING, IDM_PASTE, "&Paste") ;
      // AppendMenu(hMenuOptions, MF_STRING, IDM_DEL,   "De&lete") ;

   // AppendMenu(hMenuOptions, MF_STRING, menu_code, "switch to ME!") ;
   for (j=0; j<num_elements; j++) {
      //  don't mask the mask image!!
      if (skip_elements[j])
         continue;

      // wsprintf(mmsg, "color %u", j) ;
      // AppendMenu(hMenuOptions, MF_STRING, IDC_STATIC, mmsg) ;
      AppendMenu(hMenuOptions, MF_STRING, menu_code+j, color_menu_str[j]) ;
   }
   hdlMenu = hMenuOptions ;
   return hMenuOptions;
}

//************************************************************************
void bclock_element::Box(HDC hdc, int x0, int y0, int x1, int y1, unsigned style, COLORREF fgattr, COLORREF bgattr)
{
   HPEN hPen = 0 ;

   switch (style) {
   case BX_SOLID:
      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y0, NULL) ;
      LineTo  (hdc, x1, y0) ;
      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      LineTo  (hdc, x0, y0) ;
      break;

   case BX_SHADOW_OUTER:
      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y1, NULL) ;
      LineTo  (hdc, x0, y0) ;
      LineTo  (hdc, x1, y0) ;

      hPen = CreatePen(PS_SOLID, 1, bgattr) ;
      SelectObject(hdc, hPen) ;

      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      SetPixel(hdc, x0, y1, bgattr) ;
      break;

   case BX_SHADOW_INNER:
      hPen = CreatePen(PS_SOLID, 1, bgattr) ;
      SelectObject(hdc, hPen) ;

      MoveToEx(hdc, x0, y1, NULL) ;
      LineTo  (hdc, x0, y0) ;
      LineTo  (hdc, x1, y0) ;

      hPen = CreatePen(PS_SOLID, 1, fgattr) ;
      SelectObject(hdc, hPen) ;

      LineTo  (hdc, x1, y1) ;
      LineTo  (hdc, x0, y1) ;
      SetPixel(hdc, x0, y1, fgattr) ;
      break;

   }  //lint !e744

   if (hPen != 0) {
      SelectObject(hdc, GetStockObject(BLACK_PEN)) ;  //  deselect my pen
      DeleteObject (hPen) ;
   }
}

//************************************************************************
void bclock_element::Solid_Rect(HDC hdc, int xl, int yu, int xr, int yl, COLORREF Color)
{
   HBRUSH hBrush ;
   RECT   rect ;

   SetRect (&rect, xl, yu, xr, yl) ;
   hBrush = CreateSolidBrush (Color) ;
   FillRect (hdc, &rect, hBrush) ;
   DeleteObject (hBrush) ;
}

//*********************************************************************
//  this draws the clock frame
//*********************************************************************
void bclock_element::draw_frame(HDC hdc, unsigned x, unsigned y, unsigned on_noff)
{
   unsigned xl = x ;
   unsigned yt = y ;

   // syslog("x=%u, y=%u, th=Tu, pad=%u, fw=%u\n",
   //    x, y, fthickness, fpadding) ;
   // unsigned xr = x + get_frame_width() ;
   // unsigned yb = y + get_frame_height() ;
   // unsigned frame_edge = fthickness + fpadding ;
   unsigned xr = x + el_width ;
   unsigned yb = y + el_height ;

   Solid_Rect(hdc, xl, yt, xr, yb, (on_noff) ? attr_high : attr_low) ;

   // Box(hdc, xl, yt, xr, yb, BX_SOLID, attr_lhigh, 0) ;
   // xl++ ;  yt++ ;  xr-- ;  yb-- ;
   // Box(hdc, xl, yt, xr, yb, BX_SOLID, attr_lhigh, 0) ;
   // xl-- ;  yt-- ;  
   // Box(hdc, xl, yt, xr, yb, BX_SOLID, attr_lhigh, 0) ;

   Box(hdc, xl, yt, xr, yb, BX_SHADOW_OUTER, attr_lhigh, attr_llow) ;
   // xl++ ;  yt++ ;  xr-- ;  yb-- ;
   // Box(hdc, xl, yt, xr, yb, BX_SHADOW_OUTER, attr_lhigh, attr_llow) ;
   xl++ ;  yt++ ;  xr-- ;  yb-- ;
   Box(hdc, xl, yt, xr, yb, BX_SHADOW_INNER, attr_lhigh, attr_llow) ;
   // xl++ ;  yt++ ;  xr-- ;  yb-- ;
   // Box(hdc, xl, yt, xr, yb, BX_SHADOW_INNER, attr_lhigh, attr_llow) ;
}

//******************************************************************
// void bclock_element::draw_sprite(HDC hdc, unsigned scol, unsigned srow, unsigned xidest, unsigned yidest)
void bclock_element::draw_sprite(HDC hdc, unsigned on_noff, unsigned xidest, unsigned yidest)
{
   unsigned xmask, xsrc ;
   int xdest, ydest ;
   HDC hdcMem ;

   unsigned scol = (on_noff == 0) ? off_idx : curr_element ;
   xsrc  = scol * el_width  ;
   // ysrc  = srow * el_height ;
   xdest = (int) xidest + x_offset ;
   ydest = (int) yidest + y_offset ;

   hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hSpriteBitmap);

   if (flags & BE_DRAWN) {
      draw_frame(hdc, xidest, yidest, on_noff) ;
   } else
   if (mask_idx < 0) {
       if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xsrc, 0, SRCCOPY)) {
          // Statusbar_ShowMessage (get_system_message());
          syslog("BitBlt (copy): %s", get_system_message()) ;
       }
   }
   else {
      xmask  = (uint) mask_idx * el_width  ;
      if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xmask, 0, SRCAND)) {
         // Statusbar_ShowMessage (get_system_message());
         syslog("BitBlt (mask): %s", get_system_message()) ;
      }
      if (!BitBlt (hdc, xdest, ydest, el_width, el_height, hdcMem, xsrc, 0, SRCPAINT)) {
         // Statusbar_ShowMessage (get_system_message());
         syslog("BitBlt (image): %s", get_system_message()) ;
      }
   }
   DeleteDC (hdcMem);
}


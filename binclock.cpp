//*********************************************************************
//  cdtimer.cpp - Countdown timer using slider for setting
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  03/07/03 12:20
//  
//  compile with makefile
//*********************************************************************

#include <windows.h>
#include <commctrl.h>			  //  link to comctl32.lib
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <tchar.h>

#include "wcommon.h"
#include "binclock.h"
#include "bclk_elements.h"
#include "regif.hpp"

static char szClassName[] = "derelict's binary clock";

#define  ID_TRAYMENU       2000
#define  ID_TRAYOPEN       2001
#define  ID_MINIMIZE       2002
#define  ID_TOGGLE_LAYOUT  2003

#define  ID_NEXT_COLOR     2010
#define  ID_TRAYEXIT       2020
#define  ID_UNUSED         2021

#define  ID_LAMPS0         3000

#define  IDC_STATIC        1020

#define  IDT_TIMER   201
UINT  timerID = 0 ;

//*********************************************************************
// variables

// HWND hwnd;
HINSTANCE g_hInst;
static NOTIFYICONDATA NotifyIconData;

//*************************************
#define  STD_SPACE      20

// #define  FIELD_LEN      400
#define  SPRITE_WIDTH   22

#define  FIELD_HEIGHT   20
#define  TEXT_HEIGHT    20
// #define  BUTTON_HEIGHT  (FIELD_HEIGHT + 10)
#define  NEXT_FIELD     (FIELD_HEIGHT + 4)

#define  START_ROW      10
#define  BLANK_ROW      (START_ROW)
#define  HOURS_ROW      (BLANK_ROW + NEXT_FIELD)
#define  MINS_ROW       (HOURS_ROW + NEXT_FIELD)
#define  SECS_ROW       (MINS_ROW + NEXT_FIELD)
#define  BTN_ROW        (SECS_ROW + NEXT_FIELD + 10)

#define  LABEL_COL      STD_SPACE
#define  LABEL_LEN      60

#define  NUM_COL       (LABEL_COL + LABEL_LEN + STD_SPACE)
#define  NUMBER_LEN     30
#define  LED_COL       (NUM_COL + NUMBER_LEN + STD_SPACE)

#define  DIALOG_DX     (NUM_COL + NUMBER_LEN + STD_SPACE + 190)
#define  DIALOG_DY     (BTN_ROW + NEXT_FIELD + 10)
#define  DIALOG_X      (200)
#define  DIALOG_Y      (200)

//*************************************
// static char tempstr[260] ;

static HWND hwndHours;
static HWND hwndMins ;
static HWND hwndSecs ;

// function prototypes

static int cxClient = 0 ;
static int cyClient = 0 ;

static unsigned layout_method = 0 ;

//***********************************************************************
static unsigned bitmap_idx = 2 ;
static unsigned bit_menu = 2 ;

#define  NUM_ELEMENTS   9
static bclock_element *element_list[NUM_ELEMENTS] ;
// static HMENU menu_handles[NUM_ELEMENTS] ;

//*******************************************************************************
//  well, for some reason, INI files don't work at all in this situation.
//  GetPrivateProfileString() doesn't return the strings in the file
//  at all, even though the file is there and corrent.
//  Okay, I see what the issue is.  
//  First, the section (AppName) entry has to exist in the file.  
//  Second, the file is *not* created if it does not already exist!!
//  I thought it was... 
//*******************************************************************************
// - current object and color
// - for BE_DRAWN, rememeber fgnd/bgnd colors
registry_iface inireg("binclock") ;

static unsigned crfg = RGB(128, 255, 0) ;
static unsigned crbg = RGB(128, 64, 0) ;

static void read_config_data(void)
{
   // - current object and sub-item
   // - for BE_DRAWN, rememeber fgnd/bgnd colors
   inireg.get_param("bitmap_idx", &bitmap_idx) ;
   inireg.get_param("bit_menu", &bit_menu) ;
   inireg.get_param("attr_on", &crfg) ;
   inireg.get_param("attr_off", &crbg) ;
   inireg.get_param("layout", &layout_method) ;

   // wsprintf(tempstr, "bitmap_idx=%u", bitmap_idx) ;
   // OutputDebugString(tempstr) ;
}

//*********************************************************************
void load_bitmap_files(HWND hwnd)
{
   HDC hdc = GetDC(hwnd) ;
   unsigned menu_code = ID_LAMPS0 ;
   bclock_element *be_temp ;
   unsigned idx = 0 ;
   unsigned j ;
   char msg[81] ;
   unsigned start_element ;

   // bclock_element(HINSTANCE g_hInst, char *name, 
   //          unsigned width, unsigned flags,
   //          int mask_index, unsigned off_index, unsigned start_element);
   start_element = (bitmap_idx == idx) ? bit_menu : 10 ;
   be_temp = new bclock_element(g_hInst, "ledarray.bmp", 22, BE_LINEAR, 3, 4, start_element);
   be_temp->add_skip_element(0) ;
   be_temp->add_skip_element(1) ;
   be_temp->add_color_menu_str(2, "green") ;
   be_temp->add_skip_element(5) ;
   be_temp->add_skip_element(6) ;
   be_temp->add_color_menu_str(7, "orange") ;
   be_temp->add_skip_element(8) ;
   be_temp->add_skip_element(9) ;
   be_temp->add_color_menu_str(10, "red") ;
   be_temp->add_skip_element(11) ;
   be_temp->add_skip_element(12) ;
   be_temp->add_color_menu_str(13, "yellow") ;
   be_temp->mask_the_source(hdc) ;
   // be_temp->add_menu_data(menu_code++, "Large lamps") ;
   menu_code = be_temp->add_menu_data(menu_code, "Large lamps") ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 3 ;
   be_temp = new bclock_element(g_hInst, "square2.bmp", 0, BE_LINEAR, -1, 0, start_element);
   be_temp->set_image_offsets(0, 2) ;
   menu_code = be_temp->add_menu_data(menu_code, "Square lamps") ;
   static char *square_colors[10] = {
      "grey", "dark purple", "cyan", "green", "blue", "orange", "magenta", "purple", "red", "yellow" } ;
   for (j=0; j<10; j++)
      be_temp->add_color_menu_str(j, square_colors[j]) ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 3 ;
   be_temp = new bclock_element(g_hInst, "balls.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Small lamps") ;
   static char *balls_colors[7] = {
      " ", " ", "blue", "green", "orange", "yellow", "red" } ;
   for (j=0; j<7; j++)
      be_temp->add_color_menu_str(j, balls_colors[j]) ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 5 ;
   be_temp = new bclock_element(g_hInst, "ceramics.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->set_image_offsets(-5, -5) ;
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Ceramic lamps") ;
   static char *ceramics_colors[] = {
      " ", " ", "purple", "ecru", "verdant", "green", "tan", "fuchsia", "purple2", "red", "cyan", "yellow", 0 } ;
   for (j=0; ceramics_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, ceramics_colors[j]) ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "accent.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Accent balls") ;
   static char *accent_colors[] = {
      " ", " ", "green", "cyan", "blue", "pink", "red", "brown", "orange", "yellow", 0 } ;
   for (j=0; accent_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, accent_colors[j]) ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "marbles.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Glass marbles") ;
   static char *marble_colors[] = {
      " ", " ", "grey", "blue", "cyan", "yellow", "green", "purple", "red", 0 } ;
   for (j=0; marble_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, marble_colors[j]) ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 2 ;
   be_temp = new bclock_element(g_hInst, "leds.bmp", 0, BE_PAIRS, 0, start_element-1, start_element);
   be_temp->set_image_offsets(-2, -5) ;
   be_temp->add_skip_element(1) ;
   be_temp->add_color_menu_str(2, "green") ;
   be_temp->add_skip_element(3) ;
   be_temp->add_color_menu_str(4, "red") ;
   be_temp->add_skip_element(5) ;
   be_temp->add_color_menu_str(6, "yellow") ;
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "LEDs") ;
   element_list[idx++] = be_temp ;

   start_element = (bitmap_idx == idx) ? bit_menu : 6 ;
   be_temp = new bclock_element(g_hInst, "lights.bmp", 0, BE_LINEAR, 0, 1, start_element);
   be_temp->mask_the_source(hdc) ;
   menu_code = be_temp->add_menu_data(menu_code, "Glass lights") ;
   static char *light_colors[] = {
      " ", "blue", "red", "brown", "orange", "yellow", "green", "cyan", "blue", "fuchsia", 0 } ;
   for (j=0; light_colors[j] != 0; j++)
      be_temp->add_color_menu_str(j, light_colors[j]) ;
   element_list[idx++] = be_temp ;

                           //                 width         mask offset start_el
   be_temp = new bclock_element(g_hInst, NULL, 14, BE_DRAWN, 0,   0,     0);
   menu_code = be_temp->add_menu_data(menu_code, "Bound Boxes") ;
   static char *drawn_colors[] = { " ", "select this element", "change ON color", "change OFF color", 0 } ; 
   for (j=0; drawn_colors[j] != 0; j++) {
      // OutputDebugString(drawn_colors[j]) ;
      be_temp->add_color_menu_str(j, drawn_colors[j]) ;
   }
   //***********************************************************************
   //  store data from registry...
   //  
   //  This is *also* flawed... there needs to be one 
   //  color entry (in registry) for each created BE_DRAWN object...
   //  Yet the registry is a part of the application, 
   //  NOT the bclk_elements class...
   //  
   //  This is all wrong...
   //***********************************************************************
   //  what's specifically flawed about this is that, if the user later
   //  selects "change fg/bg attribute" from the taskbar menu,
   //  that calls a function in the bclk_elements class interface,
   //  so the class has to update our application registry with the
   //  new value - but again, the registry is an application construct,
   //  not a class construct.  How do I resolve this dilemma??
   //  For now, I've exported the inireg struct into the class code,
   //  but that mucks up re-usability because that bclk_elements class
   //  is now dependent upon definitions in the application.
   //  What's more, the class now imposes ini-file field requirements
   //  into the application, which is VERY hard to maintain...
   //***********************************************************************
   be_temp->set_element_attr(crfg, crbg) ;
   element_list[idx++] = be_temp ;

   //  this is an insufficient test; the program may abort
   //  before we can get to this test, if overrun occurs...
   if (idx > NUM_ELEMENTS) {
      wsprintf(msg, "too many elements created (%u vs %u)\n", idx, NUM_ELEMENTS) ;
      OutputDebugString(msg) ;
      MessageBox(NULL, msg, "DANGER!!", MB_OK) ;
   }

   ReleaseDC (hwnd, hdc) ;
}

//*********************************************************************
static void draw_horiz_binary_time(HDC hdc, unsigned row, unsigned tvalue)
{
   unsigned mask = 0x20 ;
   unsigned idx = LED_COL ;
   // for (unsigned j=0; j<8; j++) {
   for (unsigned j=0; j<6; j++) {
      element_list[bitmap_idx]->draw_sprite(hdc, (tvalue & mask), idx, row);
      idx += SPRITE_WIDTH + 4 ;
      mask >>= 1 ;
   }
}

//*********************************************************************
static void draw_bcd_time(HDC hdc, unsigned row, unsigned time_seg, unsigned draw_flags)
{
   if (draw_flags & 8)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 8), row, BLANK_ROW);
   if (draw_flags & 4)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 4), row, HOURS_ROW);
   if (draw_flags & 2)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 2), row, MINS_ROW);
   if (draw_flags & 1)
      element_list[bitmap_idx]->draw_sprite(hdc, (time_seg & 1), row, SECS_ROW);
}

//*********************************************************************
void update_timer_count(HWND hwnd)
{
   time_t ttm ;
   struct tm *gtm ;
   char tstr[30] ;

   time(&ttm) ;
   gtm = localtime(&ttm) ;

   wsprintf(tstr, " %02d", gtm->tm_hour) ;
   SetWindowText(hwndHours, tstr) ;
   wsprintf(tstr, " %02d", gtm->tm_min) ;
   SetWindowText(hwndMins, tstr) ;
   wsprintf(tstr, " %02d", gtm->tm_sec) ;
   SetWindowText(hwndSecs, tstr) ;

   HDC hdc = GetDC(hwnd) ;
   //  method 0 shows hours, mins, secs as 8-bit binary values, horizontally
   if (layout_method == 0) {
      draw_horiz_binary_time(hdc, HOURS_ROW, gtm->tm_hour) ;
      draw_horiz_binary_time(hdc, MINS_ROW,  gtm->tm_min) ;
      draw_horiz_binary_time(hdc, SECS_ROW,  gtm->tm_sec) ;
   }
   //  method 1 shows hours, mins, secs in BCD format, vertically
   else {
      unsigned hoursh = gtm->tm_hour / 10 ;
      unsigned hoursl = gtm->tm_hour % 10 ;
      unsigned minsh  = gtm->tm_min  / 10 ;
      unsigned minsl  = gtm->tm_min  % 10 ;
      unsigned secsh  = gtm->tm_sec  / 10 ;
      unsigned secsl  = gtm->tm_sec  % 10 ;
      unsigned idx = LED_COL ;

      draw_bcd_time(hdc, idx, hoursh, 0x3) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, hoursl, 0xF) ;

      idx += SPRITE_WIDTH + 16 ;
      draw_bcd_time(hdc, idx, minsh, 0x7) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, minsl, 0xF) ;

      idx += SPRITE_WIDTH + 16 ;
      draw_bcd_time(hdc, idx, secsh, 0x7) ;
      idx += SPRITE_WIDTH + 4 ;
      draw_bcd_time(hdc, idx, secsl, 0xF) ;
   }

   ReleaseDC (hwnd, hdc) ;
}

//*********************************************************************
// Main Process CallBack Function
//*********************************************************************
// HMENU hMenu;
// HMENU hsMenu;
HMENU hPopMenu = 0 ;
// #define IDM_SEP   32783

static LRESULT APIENTRY MainProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   POINT lpClickPoint;
   unsigned j ;

	switch (msg) {
   case WM_CREATE:
      read_config_data() ;
      load_bitmap_files(hwnd) ;

      CreateStatic ("hours: ", LABEL_COL, HOURS_ROW, LABEL_LEN,  FIELD_HEIGHT, IDC_STATIC, hwnd, g_hInst);
      hwndHours  = CreateText("0", NUM_COL,   HOURS_ROW, NUMBER_LEN, TEXT_HEIGHT, IDC_STATIC, hwnd, g_hInst);

      CreateStatic ("minutes: ", LABEL_COL, MINS_ROW, LABEL_LEN,  FIELD_HEIGHT, IDC_STATIC, hwnd, g_hInst);
      hwndMins   = CreateText("0",   NUM_COL,   MINS_ROW, NUMBER_LEN, TEXT_HEIGHT, IDC_STATIC, hwnd, g_hInst);

      CreateStatic ("seconds: ", LABEL_COL, SECS_ROW, LABEL_LEN,  FIELD_HEIGHT, IDC_STATIC, hwnd, g_hInst);
      hwndSecs   = CreateText("0",   NUM_COL,   SECS_ROW, NUMBER_LEN, TEXT_HEIGHT, IDC_STATIC, hwnd, g_hInst);

      timerID = SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC) NULL) ;

      //***********************************************
      // create tray menu
      //***********************************************
      // hMenu = LoadMenu (g_hInst, MAKEINTRESOURCE (ID_TRAYMENU));
      //  for some reason, I cannot simply create a menu here,
      //  and then attach it to the system tray...
      //  Niether AppendMenu nor InsertMenu actually do anything...

      // put the icon into a system tray
      NotifyIconData.cbSize = sizeof (NOTIFYICONDATA);
      NotifyIconData.hWnd = hwnd;
      NotifyIconData.uID = 0;
      NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
      NotifyIconData.uCallbackMessage = WM_USER; // tray events will generate WM_USER event
      // NotifyIconData.hIcon = (HICON) LoadImage (g_hInst, MAKEINTRESOURCE (IDAPPLICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); // load 16 x 16 pixels icon
      NotifyIconData.hIcon = (HICON) LoadIcon (g_hInst, MAKEINTRESOURCE (BCICON));
      lstrcpy (NotifyIconData.szTip, szClassName); // max 64 characters

      Shell_NotifyIcon (NIM_ADD, &NotifyIconData);

      break;

   case WM_SIZE:
      cxClient = LOWORD (lParam) ;
      cyClient = HIWORD (lParam) ;
      // maxx = cxClient - 1 ;
      // maxy = cyClient - 1 ;
   case WM_SETFOCUS:
      update_timer_count(hwnd) ;
      break;

   case WM_PAINT:
      BeginPaint (hwnd, &ps) ;
      update_timer_count(hwnd) ;
      EndPaint (hwnd, &ps) ;
      return 0 ;
    
   case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER:
         update_timer_count(hwnd) ;
         break;
      }
      break;

   case WM_USER:
      // event genereted by a system tray - the type of tray event that
      // generated the message can be found in lParam
      switch (lParam)   {
      case WM_RBUTTONUP:
      case WM_LBUTTONUP:
         GetCursorPos(&lpClickPoint);
         if (hPopMenu == 0) {
            hPopMenu = CreatePopupMenu();

            AppendMenu(hPopMenu, MF_STRING, ID_UNUSED, _T("derelict's binary clock")) ;
            AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL) ;

            for (j=0; j<NUM_ELEMENTS; j++) {
               // AppendMenu(hPopMenu, MF_STRING, 
               //       element_list[j]->get_menu_id(),
               //    _T(element_list[j]->get_menu_str())) ;
               HMENU hMenuTemp = (HMENU) element_list[j]->build_options_menu() ;
               // menu_handles[j] = hMenuTemp ;
               AppendMenu(hPopMenu, MF_POPUP, (UINT) hMenuTemp,  
                           _T(element_list[j]->get_menu_str())) ;
            }

            AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL) ;
            // AppendMenu(hPopMenu, MF_STRING, ID_NEXT_COLOR,    _T("Select next color"));
            AppendMenu(hPopMenu, MF_STRING, ID_TOGGLE_LAYOUT, _T("Toggle time format"));
            AppendMenu(hPopMenu, MF_STRING, ID_TRAYOPEN,      _T("Open clock window"));
            AppendMenu(hPopMenu, MF_STRING, ID_MINIMIZE,      _T("Minimize window"));
            AppendMenu(hPopMenu, MF_STRING, ID_TRAYEXIT,      _T("Exit from program"));
            // CheckMenuItem (hPopMenu, (UINT) menu_handles[bitmap_idx], MF_CHECKED);
            CheckMenuItem (hPopMenu, (UINT) element_list[bitmap_idx]->get_menu_handle(), MF_CHECKED);
         }

         SetForegroundWindow(hwnd);
         TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,
            lpClickPoint.x, lpClickPoint.y, 0, hwnd, NULL);
         break;

      // case WM_RBUTTONUP:
      //    // show window as response to right-clicking the tray icon
      //    ShowWindow (hwnd, SW_SHOWNORMAL);
      //    SetForegroundWindow (hwnd);
      //    break;
      }
      break;

   case WM_COMMAND:
      //  SysTray menu options
      if (HIWORD (wParam) == BN_CLICKED) {
         unsigned found = 0 ;
         //******************************************************
         //  first check for a hit among the block_elements
         //******************************************************
         for (j=0; j<NUM_ELEMENTS; j++) {
            // if (element_list[j]->get_menu_id() == LOWORD (wParam)) {
            int temp_idx = element_list[j]->get_menu_id(LOWORD (wParam)) ;
            if (temp_idx >= 0) {
               // bitmap_idx = LOWORD (wParam) - ID_LAMPS0;
               CheckMenuItem (hPopMenu, (UINT) element_list[bitmap_idx]->get_menu_handle(), MF_UNCHECKED);
               bitmap_idx = (unsigned) temp_idx ;
               CheckMenuItem (hPopMenu, (UINT) element_list[bitmap_idx]->get_menu_handle(), MF_CHECKED);
               inireg.set_param("bitmap_idx", bitmap_idx) ;
               found = 1 ;
               break;
            }
         }
         if (found) {
            InvalidateRect (hwnd, NULL, TRUE) ;
            break;
         }
         
         //******************************************************
         //  if the item code wasn't found in the
         //  block_elements array, check the normal codes
         //******************************************************
         switch (LOWORD (wParam)) {
         case ID_NEXT_COLOR: 
            element_list[bitmap_idx]->next_led_color() ;
            break;

         case ID_TOGGLE_LAYOUT:
            layout_method ^= 1 ;
            inireg.set_param("layout", layout_method) ;
            InvalidateRect (hwnd, NULL, TRUE) ;
            break;

         case ID_MINIMIZE:
            ShowWindow (hwnd, SW_HIDE);
            break;

         case ID_TRAYOPEN:
            ShowWindow (hwnd, SW_SHOW);   // open dialog
            break;

         case ID_TRAYEXIT:
            DestroyWindow (hwnd);
            return 1;

         }  // switch (LOWORD (wParam))
      }  //  if (HIWORD (wParam) == BN_CLICKED)
      break;

   case WM_DESTROY:
      for (j=0; j<NUM_ELEMENTS; j++) 
         delete element_list[j] ;

      if (timerID != 0) {
         KillTimer(hwnd, timerID) ;
         timerID = 0 ;
      }
      // remove the icon from a system tray and free .dll handle
      Shell_NotifyIcon (NIM_DELETE, &NotifyIconData);

      PostQuitMessage (0);
      break;

   default:
      return DefWindowProc (hwnd, msg, wParam, lParam);
	}
	return 0;
}

//*********************************************************************
// initialize application
//*********************************************************************
static void InitApp (HINSTANCE hInst)
{
   WNDCLASS wc;
   // wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.style = 0 ;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   // wc.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
   wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1) ;
   wc.hInstance = hInst;
   wc.hCursor = LoadCursor (NULL, IDC_ARROW);
   wc.hIcon = LoadIcon (hInst, MAKEINTRESOURCE(BCICON));
   wc.lpfnWndProc = (WNDPROC) MainProc;
   wc.lpszClassName = "binclock";
   wc.lpszMenuName = NULL;

   RegisterClass (&wc);

   HWND hwnd = CreateWindow ("binclock", szClassName, 
      // WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      // WS_BORDER | WS_SYSMENU,
      WS_BORDER,
      // WS_POPUP,
      DIALOG_X, DIALOG_Y, DIALOG_DX, DIALOG_DY, 0, 0, hInst, 0);

   ShowWindow (hwnd, SW_SHOW);
   UpdateWindow (hwnd);
}

//*********************************************************************
//  winmain entry point
//*********************************************************************
int APIENTRY WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR line, int CmdShow)
{
   MSG msg;
   g_hInst = hInst;

   //  initialize windows
   InitApp (hInst);

   //  run message loop
   while (GetMessage (&msg, 0, 0, 0)) {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
   }

   return msg.wParam;
}


//*****************************************************************
//  extract common code for standard derelict Windows operations.
//  
//  - Find INI filename
//  - Create addon objects for Windows applications
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  07/11/03 12:57
//*****************************************************************

#include <windows.h>

#include "wcommon.h"

//************************************************************
// A function to set a control's text to the default font
//************************************************************
//lint -esym(714, SetDefaultFont)
//lint -esym(759, SetDefaultFont)
//lint -esym(765, SetDefaultFont)
int SetDefaultFont (int identifier, HWND hwnd)
{
   SendDlgItemMessage (hwnd,
      identifier,
      WM_SETFONT,
      (WPARAM) GetStockObject (DEFAULT_GUI_FONT), MAKELPARAM (TRUE, 0));
   return 0;
}

//************************************************************
// A function to create static text
//lint -esym(714, CreateStatic)
//lint -esym(759, CreateStatic)
//lint -esym(765, CreateStatic)
HWND CreateStatic (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hStaticTemp;

   hStaticTemp = CreateWindowEx (0,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hStaticTemp;
}

//************************************************************
// A function to create a textarea
//lint -esym(714, CreateEdit)
//lint -esym(759, CreateEdit)
//lint -esym(765, CreateEdit)
HWND CreateEdit (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hEditTemp;

   hEditTemp = CreateWindowEx (WS_EX_CLIENTEDGE,
      "EDIT",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hEditTemp;
}

//************************************************************
// A function to create a textarea
//lint -esym(714, CreateText)
//lint -esym(759, CreateText)
//lint -esym(765, CreateText)
HWND CreateText(char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hEditTemp;

   hEditTemp = CreateWindowEx (
      WS_EX_STATICEDGE | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hEditTemp;
}
//************************************************************
// A function to create a button
//lint -esym(714, CreateButton)
//lint -esym(759, CreateButton)
//lint -esym(765, CreateButton)
HWND CreateButton (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hButtonTemp;

   hButtonTemp = CreateWindowEx (0,
      "BUTTON",
      tempText,
      WS_CHILD | WS_VISIBLE,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hButtonTemp;
}

//************************************************************
// A function to create a raised area
//lint -esym(714, CreateIsland)
//lint -esym(759, CreateIsland)
//lint -esym(765, CreateIsland)
HWND CreateIsland (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst) 
{
   HWND hStaticTemp;

   hStaticTemp = CreateWindowEx (WS_EX_WINDOWEDGE,
      "STATIC",
      tempText,
      WS_CHILD | WS_VISIBLE | WS_DLGFRAME,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hStaticTemp;
}

//************************************************************
// A function to create a checkbox
//lint -esym(714, CreateCheck)
//lint -esym(759, CreateCheck)
//lint -esym(765, CreateCheck)
HWND CreateCheck (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst)
{
   HWND hCheckTemp;

   hCheckTemp = CreateWindowEx (0,
      "BUTTON",
      tempText,
      WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
      x, y, width, height, hwnd, (HMENU) identifier, g_hInst, NULL);

   return hCheckTemp;
}


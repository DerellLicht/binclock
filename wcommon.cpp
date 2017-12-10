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

//********************************************************
// A function to set a control's text to
//     the default font
int SetDefaultFont (int identifier, HWND hwnd)
{
   SendDlgItemMessage (hwnd,
      identifier,
      WM_SETFONT,
      (WPARAM) GetStockObject (DEFAULT_GUI_FONT), MAKELPARAM (TRUE, 0));
   return 0;
}

// A function to create static text
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

// A function to create a textarea
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

// A function to create a textarea
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
// A function to create a button
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

// A function to create a raised area
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

// A function to create a checkbox
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


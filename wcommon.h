//*****************************************************************
//  extract common code for standard derelict Windows operations.
//  
//  - Find INI filename
//  - Create addon objects for Windows applications
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  07/11/03 16:29
//*****************************************************************

//  windows-object creation functions
int SetDefaultFont (int identifier, HWND hwnd);
HWND CreateStatic (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst);
HWND CreateEdit (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst);
HWND CreateText(char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst);
HWND CreateButton (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst);
HWND CreateCheck (char *tempText, int x, int y, int width, int height,
   int identifier, HWND hwnd, HINSTANCE g_hInst);


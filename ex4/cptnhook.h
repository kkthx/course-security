#pragma once

#include <cstdio>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "Psapi")

char _processName[MAX_PATH];
#define NOTEPAD_PROC_NAME "notepad.exe"
#define LOADER_PROC_NAME "explorer.exe"
#define DEBUG_DETECT_SLEEP_DURATION 10 // TODO: this should be 5
#define DEBUG_COUNT_MAX 2 // TODO: this should be 10


// TODO: reference additional headers your program requires here

void decodeStrings();

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

BOOL shouldReplace(char c);
char replaceChar(char c);
BOOL checkCheat(char c);
void sendKeystroke(char c);
LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam);

#pragma once

#include "WM_ExportMacro.h"

class WINDOWMANAGER_EXPORT WindowWIN32
{
public:
    WindowWIN32();
    HRESULT CreateMyWindow(
        const char* className,
        const char* wndName,
        HINSTANCE hInstance,
        int cmdShow,
        bool inFullScr
    );
    static LRESULT CALLBACK MyWndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
    HRESULT SwitchWindowSize();
    HINSTANCE GetWndInstance();
    HWND GetWndHandle();
    bool IsFullScreen();

private:
    HINSTANCE mInstance;
    HWND mWndHandle;
    bool mbFullScr;
};

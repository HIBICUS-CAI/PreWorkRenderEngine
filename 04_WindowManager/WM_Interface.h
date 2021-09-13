#pragma once

#include <Windows.h>
#include "WM_ExportMacro.h"
#include "WindowWIN32.h"

namespace WindowInterface
{
    bool WINDOWMANAGER_EXPORT StartUp();

    bool WINDOWMANAGER_EXPORT CreateInitWindow(
        const char* wndName,
        HINSTANCE hInstance,
        int cmdShow,
        UINT wndWidth = 1280,
        UINT wndHeight = 720,
        bool inFullScr = false
    );

    WindowWIN32 WINDOWMANAGER_EXPORT* GetWindowPtr();

    void WINDOWMANAGER_EXPORT CleanAndStop();
}

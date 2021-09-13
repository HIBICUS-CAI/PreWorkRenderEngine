#include "WM_Common.h"
#include "WM_Interface.h"

namespace WindowInterface
{
    WindowWIN32* g_Wnd = nullptr;

    bool StartUp()
    {
        g_Wnd = new WindowWIN32();

        return true;
    }

    bool CreateInitWindow(
        const char* wndName,
        HINSTANCE hInstance,
        int cmdShow,
        UINT wndWidth,
        UINT wndHeight,
        bool inFullScr
    )
    {
        if (!g_Wnd)
        {
            return false;
        }

        HRESULT hr = g_Wnd->CreateMyWindow(
            wndName, hInstance, cmdShow, 
            wndWidth, wndHeight, inFullScr);
        if (FAILED(hr))
        {
            delete g_Wnd;
            g_Wnd = nullptr;
            return false;
        }

        return true;
    }

    WindowWIN32* GetWindowPtr()
    {
        return g_Wnd;
    }

    void CleanAndStop()
    {
        if (g_Wnd)
        {
            delete g_Wnd;
            g_Wnd = nullptr;
        }
    }
}
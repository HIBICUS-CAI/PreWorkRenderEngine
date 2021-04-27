#include "WM_Common.h"
#include "WM_Interface.h"

namespace WindowInterface
{
    WindowWIN32* g_Wnd = nullptr;

    bool CreateInitWindow(
        const char* wndName,
        HINSTANCE hInstance,
        int cmdShow,
        bool inFullScr
    )
    {
        g_Wnd = new WindowWIN32();
        HRESULT hr = g_Wnd->CreateMyWindow(
            wndName, hInstance, cmdShow, inFullScr);
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
        delete g_Wnd;
        g_Wnd = nullptr;
    }
}
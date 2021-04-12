#include <Windows.h>
#include "WindowWIN32.h"
#include "tempD3d.h"
//---------------------------------------------
#include "InputManager.h"
#include "InputDeviceDirectInput.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    WindowWIN32* w = new WindowWIN32();
    w->CreateMyWindow("a test window",
        hInstance, iCmdShow, false);

    //---------------------------------------------
    InputManager im(w);
    im.CreateDirectInputMain();
    im.EnumAllInputDevices();

    if (FAILED(TEMP::InitD3D11Device(w->GetWndHandle())))
    {
        return -1;
    }
#ifdef SHOW_CUBE
    if (FAILED(TEMP::PrepareCube(w->GetWndHandle())))
    {
        return -2;
    }
#endif // SHOW_CUBE

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            im.PollAllInputDevices();
            TEMP::Render();
            //-------------------------
            if (im.IsThisKeyBeingPushedInSingle(KB_ESCAPE))
            {
                if (im.IsThisKeyBeingPushedInSingle(KB_ESCAPE))
                {
                    int a = 1 + 1;
                }
                PostQuitMessage(0);
            }
        }
    }

    TEMP::CleanupDevice();
    //--------------------------------------
    im.CloseDirectInputMain();
    //--------------------------------------
    delete w;

    return (int)msg.wParam;
}
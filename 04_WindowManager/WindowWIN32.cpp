#include "WM_Common.h"
#include "WindowWIN32.h"
#include <string.h>

WindowWIN32::WindowWIN32() :
    mInstance(nullptr), mWndHandle(nullptr), mbFullScr(false)
{

}

HRESULT WindowWIN32::CreateMyWindow(
    const char* wndName,
    HINSTANCE hInstance,
    int cmdShow,
    bool inFullScr)
{
    char className[128] = "";
    strcpy_s(className, sizeof(className), wndName);
    strcat_s(className, sizeof(className), " CLASS");

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MyWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    this->mInstance = hInstance;

    RECT rc = {
        0,0,1280,720
    };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    this->mWndHandle = CreateWindow(
        className,wndName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance,
        nullptr
    );
    if (!this->mWndHandle)
    {
        return E_FAIL;
    }

    ShowWindow(this->mWndHandle, cmdShow);
    HWND hDesk;
    hDesk = GetDesktopWindow();
    GetWindowRect(hDesk, &rc);
    UINT offsetX = rc.right / 2;
    UINT offsetY = rc.bottom / 2;
    UINT width = 1280;
    UINT height = 720;
    SetWindowLong(this->mWndHandle, GWL_STYLE,
        WS_OVERLAPPED);
    SetWindowPos(this->mWndHandle, HWND_NOTOPMOST, 
        offsetX - width / 2, offsetY - height / 2,
        width, height, SWP_SHOWWINDOW);

    if (inFullScr)
    {
        return SwitchWindowSize();
    }

    return S_OK;
}

LRESULT CALLBACK WindowWIN32::MyWndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT WindowWIN32::SwitchWindowSize()
{
    if (this->mbFullScr)
    {
        HWND hDesk;
        RECT rc;
        hDesk = GetDesktopWindow();
        GetWindowRect(hDesk, &rc);
        UINT offsetX = rc.right / 2;
        UINT offsetY = rc.bottom / 2;
        UINT width = 1280;
        UINT height = 720;
        SetWindowLong(this->mWndHandle, GWL_STYLE,
            WS_OVERLAPPED);
        SetWindowPos(this->mWndHandle, HWND_NOTOPMOST, 
            offsetX - width / 2, offsetY - height / 2,
            width, height, SWP_SHOWWINDOW);
    }
    else
    {
        HWND hDesk;
        RECT rc;
        hDesk = GetDesktopWindow();
        GetWindowRect(hDesk, &rc);
        SetWindowLong(this->mWndHandle, GWL_STYLE, WS_POPUP);
        SetWindowPos(this->mWndHandle, HWND_NOTOPMOST, 0, 0,
            rc.right, rc.bottom, SWP_SHOWWINDOW);
    }

    mbFullScr = !mbFullScr;

    return S_OK;
}

HINSTANCE WindowWIN32::GetWndInstance()
{
    return this->mInstance;
}

HWND WindowWIN32::GetWndHandle()
{
    return this->mWndHandle;
}

bool WindowWIN32::IsFullScreen()
{
    return this->mbFullScr;
}

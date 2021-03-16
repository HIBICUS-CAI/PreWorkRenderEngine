#include <iostream>
#include <Windows.h>

//#define CREATE_FULLSCREEN_ATINIT

HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
bool g_isFull = false;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    if (FAILED(InitWindow(hInstance, iCmdShow)))
    {
        return 0;
    }

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

        }
    }

    return (int)msg.wParam;
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = "WindowTest";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    g_hInst = hInstance;

    UINT width = GetSystemMetrics(SM_CXSCREEN);
    UINT height = GetSystemMetrics(SM_CYSCREEN);

#ifdef CREATE_FULLSCREEN_ATINIT
    RECT rc = {
        0, 0,
        width, height
    };
    AdjustWindowRect(&rc, WS_POPUP, FALSE);
    g_hWnd = CreateWindow("WindowTest",
        "This is a test about using window",
        WS_POPUP,
        0, 0,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance,
        nullptr);
#else
    RECT rc = {
        0, 0,
        1280, 720
    };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow("WindowTest",
        "This is a test about using window",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance,
        nullptr);
#endif // CREATE_FULLSCREEN_ATINIT
    if (!g_hWnd)
    {
        return E_FAIL;
    }

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

    case WM_KEYDOWN:
        if (wParam == VK_SPACE)
        {
            if (g_isFull)
            {
                HWND hDesk;
                RECT rc;
                hDesk = GetDesktopWindow();
                GetWindowRect(hDesk, &rc);
                int offsetX = rc.right / 2;
                int offsetY = rc.bottom / 2;
                int width = 1280;
                int height = 720;
                SetWindowLong(hWnd, GWL_STYLE,
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
                SetWindowPos(hWnd, HWND_NOTOPMOST, offsetX - width / 2, offsetY - height / 2,
                    width, height, SWP_SHOWWINDOW);
            }
            else
            {
                HWND hDesk;
                RECT rc;
                hDesk = GetDesktopWindow();
                GetWindowRect(hDesk, &rc);
                SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0,
                    rc.right, rc.bottom, SWP_SHOWWINDOW);
            }

            g_isFull = !g_isFull;
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

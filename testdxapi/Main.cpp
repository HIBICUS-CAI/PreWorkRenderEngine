#include <iostream>
#include <Windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

//#define CREATE_FULLSCREEN_ATINIT

// 管理win32方面的内容
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
bool g_isFull = false;

// 管理d3d11方面的内容
D3D_DRIVER_TYPE g_DriverType;
D3D_FEATURE_LEVEL g_FeatLevel;
ID3D11Device* gp_d3dDevice = nullptr;
ID3D11Device1* gp_d3dDevice1 = nullptr;
ID3D11DeviceContext* gp_ImmediateContext = nullptr;
ID3D11DeviceContext1* gp_ImmediateContext1 = nullptr;
IDXGISwapChain* gp_SwapChain = nullptr;
IDXGISwapChain1* gp_SwapChain1 = nullptr;
ID3D11RenderTargetView* gp_RenderTargetView = nullptr;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitD3D11Device();
void CleanupDevice();
void Render();
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
        return -1;
    }
    if (FAILED(InitD3D11Device()))
    {
        return -2;
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
            Render();
        }
    }

    CleanupDevice();

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

HRESULT InitD3D11Device()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    UINT deviceCreateFlag = 0;
#ifdef _DEBUG
    deviceCreateFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_WARP
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    UINT numFeatLevels = ARRAYSIZE(featureLevels);
    g_FeatLevel = featureLevels[0];

    for (UINT i = 0; i < numDriverTypes; i++)
    {
        g_DriverType = driverTypes[i];
        hr = D3D11CreateDevice(nullptr, g_DriverType,
            nullptr, deviceCreateFlag, featureLevels,
            numFeatLevels, D3D11_SDK_VERSION,
            &gp_d3dDevice, &g_FeatLevel, &gp_ImmediateContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, g_DriverType,
                nullptr, deviceCreateFlag,
                featureLevels + 1, numFeatLevels - 1,
                D3D11_SDK_VERSION, &gp_d3dDevice,
                &g_FeatLevel, &gp_ImmediateContext);
        }

        if (SUCCEEDED(hr))
        {
            break;
        }
    }
    if (FAILED(hr))
    {
        return hr;
    }

    IDXGIFactory1* dxgiFactory1 = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = gp_d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory1));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
    {
        return hr;
    }

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory1->QueryInterface(IID_PPV_ARGS(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // 11.1+
        hr = gp_d3dDevice->QueryInterface(IID_PPV_ARGS(&gp_d3dDevice1));
        if (SUCCEEDED(hr))
        {
            gp_ImmediateContext->QueryInterface(IID_PPV_ARGS(&gp_ImmediateContext1));
        }
        DXGI_SWAP_CHAIN_DESC1 dc = {};
        dc.Width = width;
        dc.Height = height;
        dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dc.SampleDesc.Count = 1;
        dc.SampleDesc.Quality = 0;
        dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dc.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(gp_d3dDevice, g_hWnd,
            &dc, nullptr, nullptr, &gp_SwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = gp_SwapChain1->QueryInterface(IID_PPV_ARGS(&gp_SwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // 11.0
        DXGI_SWAP_CHAIN_DESC dc = {};
        dc.BufferCount = 1;
        dc.BufferDesc.Width = width;
        dc.BufferDesc.Height = height;
        dc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dc.BufferDesc.RefreshRate.Numerator = 60;
        dc.BufferDesc.RefreshRate.Denominator = 1;
        dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dc.OutputWindow = g_hWnd;
        dc.SampleDesc.Count = 1;
        dc.SampleDesc.Quality = 0;
        dc.Windowed = TRUE;

        hr = dxgiFactory1->CreateSwapChain(gp_d3dDevice, &dc, &gp_SwapChain);
    }

    dxgiFactory1->Release();
    if (FAILED(hr))
    {
        return hr;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = gp_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr))
    {
        return hr;
    }

    hr = gp_d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gp_RenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
    {
        return hr;
    }

    gp_ImmediateContext->OMSetRenderTargets(1, &gp_RenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gp_ImmediateContext->RSSetViewports(1, &vp);

    return hr;
}

void CleanupDevice()
{
    if (gp_ImmediateContext)
    {
        gp_ImmediateContext->ClearState();
    }
    if (gp_RenderTargetView)
    {
        gp_RenderTargetView->Release();
    }
    if (gp_SwapChain1)
    {
        gp_SwapChain1->Release();
    }
    if (gp_SwapChain)
    {
        gp_SwapChain->Release();
    }
    if (gp_ImmediateContext1)
    {
        gp_ImmediateContext1->Release();
    }
    if (gp_ImmediateContext)
    {
        gp_ImmediateContext->Release();
    }
    if (gp_d3dDevice1)
    {
        gp_d3dDevice1->Release();
    }
    if (gp_d3dDevice)
    {
        gp_d3dDevice->Release();
    }
}

void Render()
{
    gp_ImmediateContext->ClearRenderTargetView(gp_RenderTargetView, DirectX::Colors::Azure);

    gp_SwapChain->Present(0, 0);
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
                UINT offsetX = rc.right / 2;
                UINT offsetY = rc.bottom / 2;
                UINT width = 1280;
                UINT height = 720;
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

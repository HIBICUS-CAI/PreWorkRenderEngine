#include <iostream>
#include <Windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

//#define CREATE_FULLSCREEN_ATINIT
//#define SHOW_CUBE
//#define SHOW_MESH

// 管理win32方面的内容
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
bool g_isFull = false;

// 管理d3d11方面的内容
struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

D3D_DRIVER_TYPE g_DriverType;
D3D_FEATURE_LEVEL g_FeatLevel;
ID3D11Device* gp_d3dDevice = nullptr;
ID3D11Device1* gp_d3dDevice1 = nullptr;
ID3D11DeviceContext* gp_ImmediateContext = nullptr;
ID3D11DeviceContext1* gp_ImmediateContext1 = nullptr;
IDXGISwapChain* gp_SwapChain = nullptr;
IDXGISwapChain1* gp_SwapChain1 = nullptr;
ID3D11RenderTargetView* gp_RenderTargetView = nullptr;
ID3D11VertexShader* gp_VertexShader = nullptr;
ID3D11PixelShader* gp_PixelShader = nullptr;
ID3D11InputLayout* gp_VertexLayout = nullptr;
ID3D11Buffer* gp_VertexBuffer = nullptr;
ID3D11Buffer* gp_IndexBuffer = nullptr;
ID3D11Buffer* gp_ConstantBuffer = nullptr;
DirectX::XMMATRIX g_World;
DirectX::XMMATRIX g_View;
DirectX::XMMATRIX g_Projection;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitD3D11Device();
void ChangeWindowSize();
void CleanupDevice();
void Render();
void RenderCube();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CompileShaderFromFile(const WCHAR* szFileName,
    LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
HRESULT PrepareCube();

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
    if (FAILED(PrepareCube()))
    {
        return -3;
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

#ifdef CREATE_FULLSCREEN_ATINIT
    UINT width = GetSystemMetrics(SM_CXSCREEN);
    UINT height = GetSystemMetrics(SM_CYSCREEN);

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
    HWND hDesk;
    hDesk = GetDesktopWindow();
    GetWindowRect(hDesk, &rc);
    UINT offsetX = rc.right / 2;
    UINT offsetY = rc.bottom / 2;
    UINT width = 1280;
    UINT height = 720;
    SetWindowLong(g_hWnd, GWL_STYLE,
        WS_OVERLAPPED);
    SetWindowPos(g_hWnd, HWND_NOTOPMOST, offsetX - width / 2, offsetY - height / 2,
        width, height, SWP_SHOWWINDOW);

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
        dc.BufferCount = 2;

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
        dc.BufferCount = 2;
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

void ChangeWindowSize()
{
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

    RECT rc = {};
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    IDXGIFactory1* dxgiFactory1 = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        gp_d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
        IDXGIAdapter* adapter = nullptr;
        dxgiDevice->GetAdapter(&adapter);
        adapter->GetParent(IID_PPV_ARGS(&dxgiFactory1));
        adapter->Release();
        dxgiDevice->Release();
    }
    IDXGIFactory2* dxgiFactory2 = nullptr;
    dxgiFactory1->QueryInterface(IID_PPV_ARGS(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // 11.1+
        gp_d3dDevice->QueryInterface(IID_PPV_ARGS(&gp_d3dDevice1));
        gp_ImmediateContext->QueryInterface(IID_PPV_ARGS(&gp_ImmediateContext1));
        DXGI_SWAP_CHAIN_DESC1 dc = {};
        dc.Width = width;
        dc.Height = height;
        dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dc.SampleDesc.Count = 1;
        dc.SampleDesc.Quality = 0;
        dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dc.BufferCount = 2;
        dxgiFactory2->CreateSwapChainForHwnd(gp_d3dDevice, g_hWnd,
            &dc, nullptr, nullptr, &gp_SwapChain1);
        gp_SwapChain1->QueryInterface(IID_PPV_ARGS(&gp_SwapChain));
        dxgiFactory2->Release();
    }
    else
    {
        // 11.0
        DXGI_SWAP_CHAIN_DESC dc = {};
        dc.BufferCount = 2;
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
        dxgiFactory1->CreateSwapChain(gp_d3dDevice, &dc, &gp_SwapChain);
    }
    dxgiFactory1->Release();
    ID3D11Texture2D* pBackBuffer = nullptr;
    gp_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer != nullptr)
    {
        gp_d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gp_RenderTargetView);
    }
    pBackBuffer->Release();
    gp_ImmediateContext->OMSetRenderTargets(1, &gp_RenderTargetView, nullptr);
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gp_ImmediateContext->RSSetViewports(1, &vp);
}

void CleanupDevice()
{
    if (gp_ImmediateContext)
    {
        gp_ImmediateContext->ClearState();
    }
    if (gp_ConstantBuffer)
    {
        gp_ConstantBuffer->Release();
    }
    if (gp_VertexBuffer)
    {
        gp_VertexBuffer->Release();
    }
    if (gp_IndexBuffer)
    {
        gp_IndexBuffer->Release();
    }
    if (gp_VertexLayout)
    {
        gp_VertexLayout->Release();
    }
    if (gp_VertexShader)
    {
        gp_VertexShader->Release();
    }
    if (gp_PixelShader)
    {
        gp_PixelShader->Release();
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

    RenderCube();

    gp_SwapChain->Present(0, 0);
}

void RenderCube()
{
    static float t = 0.0f;
    if (g_DriverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)DirectX::XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        t = (timeCur - timeStart) / 1000.0f;
    }

    g_World = DirectX::XMMatrixRotationY(t);
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(g_World);
    cb.mView = XMMatrixTranspose(g_View);
    cb.mProjection = XMMatrixTranspose(g_Projection);
    gp_ImmediateContext->UpdateSubresource(gp_ConstantBuffer, 0, nullptr, &cb, 0, 0);
    gp_ImmediateContext->VSSetShader(gp_VertexShader, nullptr, 0);
    gp_ImmediateContext->VSSetConstantBuffers(0, 1, &gp_ConstantBuffer);
    gp_ImmediateContext->PSSetShader(gp_PixelShader, nullptr, 0);
    gp_ImmediateContext->DrawIndexed(36, 0, 0);
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
        if (wParam == VK_F1)
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
                SetWindowLong(g_hWnd, GWL_STYLE,
                    WS_OVERLAPPED);
                SetWindowPos(g_hWnd, HWND_NOTOPMOST, offsetX - width / 2, offsetY - height / 2,
                    width, height, SWP_SHOWWINDOW);
                GetClientRect(g_hWnd, &rc);
                int a = 12;
            }
            else
            {
                HWND hDesk;
                RECT rc;
                hDesk = GetDesktopWindow();
                GetWindowRect(hDesk, &rc);
                SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP);
                SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0,
                    rc.right, rc.bottom, SWP_SHOWWINDOW);
            }

            g_isFull = !g_isFull;
            ChangeWindowSize();
        }
        else if (wParam==VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT CompileShaderFromFile(const WCHAR* szFileName,
    LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT PrepareCube()
{
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = gp_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), nullptr, &gp_VertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
    };
    UINT numInputLayouts = ARRAYSIZE(layout);
    hr = gp_d3dDevice->CreateInputLayout(layout, numInputLayouts,
        pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &gp_VertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
    {
        return hr;
    }
    gp_ImmediateContext->IASetInputLayout(gp_VertexLayout);

    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"PixelShader.hlsl", "main", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = gp_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(), nullptr, &gp_PixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
    {
        return hr;
    }

    SimpleVertex vertices[] =
    {
        {
            DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f),
            DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f),
            DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
            DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),
            DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f),
            DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f),
            DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f),
            DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
        },
        {
            DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f),
            DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
        },
    };
    D3D11_BUFFER_DESC bdc = {};
    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(SimpleVertex) * 8;
    bdc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = gp_d3dDevice->CreateBuffer(&bdc, &initData, &gp_VertexBuffer);
    if (FAILED(hr))
    {
        return hr;
    }
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    gp_ImmediateContext->IASetVertexBuffers(0, 1, &gp_VertexBuffer, &stride, &offset);
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(WORD) * 36;
    bdc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdc.CPUAccessFlags = 0;
    initData.pSysMem = indices;
    hr = gp_d3dDevice->CreateBuffer(&bdc, &initData, &gp_IndexBuffer);
    if (FAILED(hr))
    {
        return hr;
    }
    gp_ImmediateContext->IASetIndexBuffer(gp_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    gp_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bdc.Usage = D3D11_USAGE_DEFAULT;
    bdc.ByteWidth = sizeof(ConstantBuffer);
    bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdc.CPUAccessFlags = 0;
    hr = gp_d3dDevice->CreateBuffer(&bdc, nullptr, &gp_ConstantBuffer);
    if (FAILED(hr))
    {
        return hr;
    }
    g_World = DirectX::XMMatrixIdentity();
    DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.f, 0.f, -5.f, 0.f);
    DirectX::XMVECTOR lookat = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
    g_View = DirectX::XMMatrixLookAtLH(eye, lookat, up);
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    g_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2,
        width / (FLOAT)height, 0.01f, 100.f);

    return S_OK;
}

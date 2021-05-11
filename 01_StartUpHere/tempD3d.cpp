#include "tempD3d.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "tempDDSTextureLoader.h"

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexCoord;
};

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

struct Material
{
    DirectX::XMFLOAT4 mDiffuseAlbedo;
    DirectX::XMFLOAT3 mFresnelR0;
    FLOAT mShininess;
};

struct Light
{
    DirectX::XMFLOAT3 Strength = { 1.f,1.f,1.f };
    FLOAT FalloffStart = 1.f;
    DirectX::XMFLOAT3 Direction = { 0.f,-1.f,0.f };
    FLOAT FalloffEnd = 5.f;
    DirectX::XMFLOAT3 Position = { 0.f,0.f,0.f };
    FLOAT SpotPower = 64.f;
};

struct AmbientLight
{
    DirectX::XMFLOAT4 ALight = { 1.f,1.f,1.f,1.f };
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
ID3D11Texture2D* gp_DepthStencil = nullptr;
ID3D11DepthStencilView* gp_DepthStencilView = nullptr;
ID3D11VertexShader* gp_VertexShader = nullptr;
ID3D11PixelShader* gp_PixelShader = nullptr;
ID3D11InputLayout* gp_VertexLayout = nullptr;
ID3D11Buffer* gp_VertexBuffer = nullptr;
ID3D11Buffer* gp_IndexBuffer = nullptr;
ID3D11Buffer* gp_ConstantBuffer = nullptr;
ID3D11Buffer* gp_MatConstantBuffer = nullptr;
ID3D11Buffer* gp_LightConstantBuffer = nullptr;
ID3D11Buffer* gp_AmbientLightConstantBuffer = nullptr;
ID3D11ShaderResourceView* gp_TextureRV = nullptr;
ID3D11SamplerState* gp_SamplerLinear = nullptr;
DirectX::XMMATRIX g_World;
DirectX::XMMATRIX g_View;
DirectX::XMMATRIX g_Projection;
DirectX::XMFLOAT3 g_CameraPosition;
DirectX::XMFLOAT3 g_CameraLookAt;
DirectX::XMFLOAT3 g_CamearUpVec;
DirectX::XMFLOAT3 g_LightDirection;

namespace TEMP
{
    ID3D11Device* GetD3DDevicePointer()
    {
        return gp_d3dDevice;
    }
    ID3D11DeviceContext* GetD3DDevContPointer()
    {
        return gp_ImmediateContext;
    }
    D3D_DRIVER_TYPE GetDriverType()
    {
        return g_DriverType;
    }
    IDXGISwapChain* GetSwapChain()
    {
        return gp_SwapChain;
    }
    void TempRenderBegin()
    {
        gp_ImmediateContext->ClearRenderTargetView(
            gp_RenderTargetView, DirectX::Colors::Black);
        gp_ImmediateContext->ClearDepthStencilView(
            gp_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
    }
    void TempRenderEnd()
    {
        gp_SwapChain->Present(0, 0);
    }
    DirectX::XMFLOAT3 GetEyePos()
    {
        return g_CameraPosition;
    }
    DirectX::XMFLOAT3 GetEyeLookat()
    {
        return g_CameraLookAt;
    }
    DirectX::XMFLOAT3 GetEyeUp()
    {
        return g_CamearUpVec;
    }
    DirectX::XMMATRIX GetProjMat()
    {
        return g_Projection;
    }

    //-----------------------------------------------
    FLOAT gx = 0, gy = 0, gz = 0;
    void setLightOffsetX(FLOAT x)
    {
        gx = x;
    }
    void setLightOffsetY(FLOAT y)
    {
        gy = y;
    }
    void setLightOffsetZ(FLOAT z)
    {
        gz = z;
    }
    //-----------------------------------------------

    HRESULT InitD3D11Device(HWND wndHandle)
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(wndHandle, &rc);
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
                &gp_d3dDevice, &g_FeatLevel,
                &gp_ImmediateContext);

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
            hr = gp_d3dDevice->QueryInterface(
                IID_PPV_ARGS(&dxgiDevice));
            if (SUCCEEDED(hr))
            {
                IDXGIAdapter* adapter = nullptr;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(
                        IID_PPV_ARGS(&dxgiFactory1));
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
        hr = dxgiFactory1->QueryInterface(
            IID_PPV_ARGS(&dxgiFactory2));
        if (dxgiFactory2)
        {
            // 11.1+
            hr = gp_d3dDevice->QueryInterface(
                IID_PPV_ARGS(&gp_d3dDevice1));
            if (SUCCEEDED(hr))
            {
                gp_ImmediateContext->QueryInterface(
                    IID_PPV_ARGS(&gp_ImmediateContext1));
            }
            DXGI_SWAP_CHAIN_DESC1 dc = {};
            dc.Width = width;
            dc.Height = height;
            dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            dc.SampleDesc.Count = 1;
            dc.SampleDesc.Quality = 0;
            dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            dc.BufferCount = 2;

            hr = dxgiFactory2->CreateSwapChainForHwnd(
                gp_d3dDevice, wndHandle,
                &dc, nullptr, nullptr, &gp_SwapChain1);
            if (SUCCEEDED(hr))
            {
                hr = gp_SwapChain1->QueryInterface(
                    IID_PPV_ARGS(&gp_SwapChain));
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
            dc.OutputWindow = wndHandle;
            dc.SampleDesc.Count = 1;
            dc.SampleDesc.Quality = 0;
            dc.Windowed = TRUE;

            hr = dxgiFactory1->CreateSwapChain(
                gp_d3dDevice, &dc, &gp_SwapChain);
        }

        dxgiFactory1->Release();
        if (FAILED(hr))
        {
            return hr;
        }

        ID3D11Texture2D* pBackBuffer = nullptr;
        hr = gp_SwapChain->GetBuffer(
            0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr))
        {
            return hr;
        }

        hr = gp_d3dDevice->CreateRenderTargetView(
            pBackBuffer, nullptr, &gp_RenderTargetView);
        pBackBuffer->Release();
        if (FAILED(hr))
        {
            return hr;
        }

        D3D11_TEXTURE2D_DESC texDepSte = {};
        texDepSte.Width = width;
        texDepSte.Height = height;
        texDepSte.MipLevels = 1;
        texDepSte.ArraySize = 1;
        texDepSte.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        texDepSte.SampleDesc.Count = 1;
        texDepSte.SampleDesc.Quality = 0;
        texDepSte.Usage = D3D11_USAGE_DEFAULT;
        texDepSte.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texDepSte.CPUAccessFlags = 0;
        texDepSte.MiscFlags = 0;
        hr = gp_d3dDevice->CreateTexture2D(
            &texDepSte, nullptr, &gp_DepthStencil);
        if (FAILED(hr))
        {
            return hr;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC desDSV = {};
        desDSV.Format = texDepSte.Format;
        desDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        desDSV.Texture2D.MipSlice = 0;
        hr = gp_d3dDevice->CreateDepthStencilView(
            gp_DepthStencil, &desDSV, &gp_DepthStencilView);
        if (FAILED(hr))
        {
            return hr;
        }

        gp_ImmediateContext->OMSetRenderTargets(
            1, &gp_RenderTargetView, gp_DepthStencilView);

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

    HRESULT CompileShaderFromFile(const WCHAR* szFileName,
        LPCSTR szEntryPoint, LPCSTR szShaderModel,
        ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        ID3DBlob* pErrorBlob = nullptr;

        hr = D3DCompileFromFile(
            szFileName, nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            szEntryPoint, szShaderModel,
            dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char*>(
                    pErrorBlob->GetBufferPointer()));
                pErrorBlob->Release();
            }
            return hr;
        }
        if (pErrorBlob) pErrorBlob->Release();

        return S_OK;
    }

    HRESULT PrepareBasicRender(HWND wndHandle)
    {
        ID3DBlob* pVSBlob = nullptr;
        HRESULT hr = CompileShaderFromFile(
            L"tempVertexShader.hlsl", "main", "vs_5_0", &pVSBlob);
        if (FAILED(hr))
        {
            return hr;
        }
        hr = gp_d3dDevice->CreateVertexShader(
            pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), nullptr, &gp_VertexShader);
        if (FAILED(hr))
        {
            pVSBlob->Release();
            return hr;
        }

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            {
                "POSITION",0,
                DXGI_FORMAT_R32G32B32_FLOAT,0,0,
                D3D11_INPUT_PER_VERTEX_DATA,0
            },
            {
                "NORMAL",0,
                DXGI_FORMAT_R32G32B32_FLOAT,0,12,
                D3D11_INPUT_PER_VERTEX_DATA,0
            },
            {
                "TEXCOORD",0,
                DXGI_FORMAT_R32G32_FLOAT,0,24,
                D3D11_INPUT_PER_VERTEX_DATA,0
            }
        };
        UINT numInputLayouts = ARRAYSIZE(layout);
        hr = gp_d3dDevice->CreateInputLayout(
            layout, numInputLayouts,
            pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(),
            &gp_VertexLayout);
        pVSBlob->Release();
        if (FAILED(hr))
        {
            return hr;
        }
        gp_ImmediateContext->IASetInputLayout(gp_VertexLayout);

        ID3DBlob* pPSBlob = nullptr;
        hr = CompileShaderFromFile(
            L"tempPixelShader.hlsl", "main", "ps_5_0", &pPSBlob);
        if (FAILED(hr))
        {
            return hr;
        }
        hr = gp_d3dDevice->CreatePixelShader(
            pPSBlob->GetBufferPointer(),
            pPSBlob->GetBufferSize(), nullptr, &gp_PixelShader);
        pPSBlob->Release();
        if (FAILED(hr))
        {
            return hr;
        }
#ifdef SHOW_CUBE
        SimpleVertex vertices[] =
        {
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }
        };
        D3D11_BUFFER_DESC bdc = {};
        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(SimpleVertex) * 24;
        bdc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bdc.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, &initData, &gp_VertexBuffer);
        if (FAILED(hr))
        {
            return hr;
        }
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        gp_ImmediateContext->IASetVertexBuffers(
            0, 1, &gp_VertexBuffer, &stride, &offset);
        WORD indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };
        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(WORD) * 36;
        bdc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bdc.CPUAccessFlags = 0;
        initData.pSysMem = indices;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, &initData, &gp_IndexBuffer);
        if (FAILED(hr))
        {
            return hr;
        }
        gp_ImmediateContext->IASetIndexBuffer(
            gp_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        gp_ImmediateContext->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(ConstantBuffer);
        bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, nullptr, &gp_ConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(Material);
        bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, nullptr, &gp_MatConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(Light);
        bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, nullptr, &gp_LightConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        bdc.Usage = D3D11_USAGE_DEFAULT;
        bdc.ByteWidth = sizeof(AmbientLight);
        bdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &bdc, nullptr, &gp_AmbientLightConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        hr = DirectX::CreateDDSTextureFromFile(
            gp_d3dDevice, L"tempseafloor.dds",
            nullptr, &gp_TextureRV);
        if (FAILED(hr))
        {
            return hr;
        }

        D3D11_SAMPLER_DESC desSam = {};
        desSam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desSam.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desSam.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desSam.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desSam.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desSam.MinLOD = 0;
        desSam.MaxLOD = D3D11_FLOAT32_MAX;
        hr = gp_d3dDevice->CreateSamplerState(
            &desSam, &gp_SamplerLinear);
        if (FAILED(hr))
        {
            return hr;
        }
#endif // SHOW_CUBE

        g_World = DirectX::XMMatrixIdentity();
        g_CameraPosition = { 0.f,0.f,-15.f };
        g_CameraLookAt = { 0.f,0.f,1.f };
        g_CamearUpVec = { 0.f,1.f,0.f };
        g_LightDirection = g_CameraLookAt;
        DirectX::XMVECTOR eye = DirectX::XMVectorSet(
            g_CameraPosition.x,
            g_CameraPosition.y,
            g_CameraPosition.z,
            0.f);
        DirectX::XMVECTOR lookat = DirectX::XMVectorSet(
            g_CameraLookAt.x,
            g_CameraLookAt.y,
            g_CameraLookAt.z,
            0.f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(
            g_CamearUpVec.x,
            g_CamearUpVec.y,
            g_CamearUpVec.z,
            0.f);
        g_View = DirectX::XMMatrixLookAtLH(eye, lookat, up);
        RECT rc;
        GetClientRect(wndHandle, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;
        g_Projection = DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XM_PI / 6.f,
            width / (FLOAT)height, 0.01f, 100.f);

        D3D11_BUFFER_DESC lbdc = {};
        lbdc.Usage = D3D11_USAGE_DEFAULT;
        lbdc.ByteWidth = sizeof(ConstantBuffer);
        lbdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lbdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &lbdc, nullptr, &gp_ConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        lbdc.Usage = D3D11_USAGE_DEFAULT;
        lbdc.ByteWidth = sizeof(Material);
        lbdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lbdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &lbdc, nullptr, &gp_MatConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        lbdc.Usage = D3D11_USAGE_DEFAULT;
        lbdc.ByteWidth = sizeof(Light);
        lbdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lbdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &lbdc, nullptr, &gp_LightConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        lbdc.Usage = D3D11_USAGE_DEFAULT;
        lbdc.ByteWidth = sizeof(AmbientLight);
        lbdc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lbdc.CPUAccessFlags = 0;
        hr = gp_d3dDevice->CreateBuffer(
            &lbdc, nullptr, &gp_AmbientLightConstantBuffer);
        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }

    void ChangeWindowSize(HWND wndHandle)
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
        GetClientRect(wndHandle, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        IDXGIFactory1* dxgiFactory1 = nullptr;
        {
            IDXGIDevice* dxgiDevice = nullptr;
            gp_d3dDevice->QueryInterface(
                IID_PPV_ARGS(&dxgiDevice));
            IDXGIAdapter* adapter = nullptr;
            dxgiDevice->GetAdapter(&adapter);
            adapter->GetParent(IID_PPV_ARGS(&dxgiFactory1));
            adapter->Release();
            dxgiDevice->Release();
        }
        IDXGIFactory2* dxgiFactory2 = nullptr;
        dxgiFactory1->QueryInterface(
            IID_PPV_ARGS(&dxgiFactory2));
        if (dxgiFactory2)
        {
            // 11.1+
            gp_d3dDevice->QueryInterface(
                IID_PPV_ARGS(&gp_d3dDevice1));
            gp_ImmediateContext->QueryInterface(
                IID_PPV_ARGS(&gp_ImmediateContext1));
            DXGI_SWAP_CHAIN_DESC1 dc = {};
            dc.Width = width;
            dc.Height = height;
            dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            dc.SampleDesc.Count = 1;
            dc.SampleDesc.Quality = 0;
            dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            dc.BufferCount = 2;
            dxgiFactory2->CreateSwapChainForHwnd(
                gp_d3dDevice, wndHandle,
                &dc, nullptr, nullptr, &gp_SwapChain1);
            gp_SwapChain1->QueryInterface(
                IID_PPV_ARGS(&gp_SwapChain));
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
            dc.OutputWindow = wndHandle;
            dc.SampleDesc.Count = 1;
            dc.SampleDesc.Quality = 0;
            dc.Windowed = TRUE;
            dxgiFactory1->CreateSwapChain(
                gp_d3dDevice, &dc, &gp_SwapChain);
        }
        dxgiFactory1->Release();
        ID3D11Texture2D* pBackBuffer = nullptr;
        gp_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer != nullptr)
        {
            gp_d3dDevice->CreateRenderTargetView(
                pBackBuffer, nullptr, &gp_RenderTargetView);
        }
        pBackBuffer->Release();
        gp_ImmediateContext->OMSetRenderTargets(
            1, &gp_RenderTargetView, nullptr);
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
        if (gp_TextureRV)
        {
            gp_TextureRV->Release();
        }
        if (gp_SamplerLinear)
        {
            gp_SamplerLinear->Release();
        }
        if (gp_MatConstantBuffer)
        {
            gp_MatConstantBuffer->Release();
        }
        if (gp_LightConstantBuffer)
        {
            gp_LightConstantBuffer->Release();
        }
        if (gp_AmbientLightConstantBuffer)
        {
            gp_AmbientLightConstantBuffer->Release();
        }
        if (gp_DepthStencilView)
        {
            gp_DepthStencilView->Release();
        }
        if (gp_DepthStencil)
        {
            gp_DepthStencil->Release();
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

#ifdef _DEBUG
#ifdef CHECK_RELEASE
        ID3D11Debug* pDebug = nullptr;
        HRESULT hr = gp_d3dDevice->QueryInterface(
            IID_PPV_ARGS(&pDebug));
        if (SUCCEEDED(hr))
        {
            hr = pDebug->ReportLiveDeviceObjects(
                D3D11_RLDO_DETAIL);
            pDebug->Release();
        }
#endif // CHECK_RELEASE
#endif // _DEBUG

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
        //---------------------------
        DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(
            &g_LightDirection);
        DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationX(
            0.0001f * (FLOAT)gy);
        DirectX::XMVECTOR newLightDir =
            DirectX::XMVector3TransformNormal(
                lightDir, rotate
            );
        DirectX::XMStoreFloat3(&g_LightDirection, newLightDir);

        lightDir = DirectX::XMLoadFloat3(
            &g_LightDirection);
        rotate = DirectX::XMMatrixRotationY(0.0001f * (FLOAT)gx);
        newLightDir = DirectX::XMVector3TransformNormal(
            lightDir, rotate
        );
        DirectX::XMStoreFloat3(&g_LightDirection, newLightDir);
        //---------------------------

        gp_ImmediateContext->VSSetShader(
            gp_VertexShader, nullptr, 0);
        gp_ImmediateContext->PSSetShader(
            gp_PixelShader, nullptr, 0);
        Light lb;
        AmbientLight alb;
        Material mb;
        lb.Direction = g_LightDirection;
        lb.Position = { 0.f,0.f,-5.f };
        lb.Strength = { 1.f,1.f,1.f };
        lb.SpotPower = 64.f;
        lb.FalloffStart = 1.f;
        lb.FalloffEnd = 10.f;
        alb.ALight = { 1.f,1.f,1.f,1.f };
        mb.mDiffuseAlbedo = { 0.5f,0.5f,0.5f,1.f };
        mb.mFresnelR0 = { 0.95f,0.64f,0.54f };
        mb.mShininess = 0.875f;
        gp_ImmediateContext->UpdateSubresource(
            gp_LightConstantBuffer, 0,
            nullptr, &lb, 0, 0);
        gp_ImmediateContext->UpdateSubresource(
            gp_AmbientLightConstantBuffer, 0,
            nullptr, &alb, 0, 0);
        gp_ImmediateContext->UpdateSubresource(
            gp_MatConstantBuffer, 0, nullptr, &mb, 0, 0);

        gp_ImmediateContext->PSSetConstantBuffers(
            0, 1, &gp_LightConstantBuffer);
        gp_ImmediateContext->PSSetConstantBuffers(
            1, 1, &gp_AmbientLightConstantBuffer);
        gp_ImmediateContext->PSSetConstantBuffers(
            2, 1, &gp_MatConstantBuffer);
        gp_ImmediateContext->PSSetShaderResources(
            0, 1, &gp_TextureRV);
        gp_ImmediateContext->PSSetSamplers(
            0, 1, &gp_SamplerLinear);

#ifdef SHOW_CUBE
        RenderCube();
#endif // SHOW_CUBE
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
        DirectX::XMVECTOR eye = DirectX::XMLoadFloat4(
            &DirectX::XMFLOAT4(
                g_CameraPosition.x,
                g_CameraPosition.y,
                g_CameraPosition.z,
                0.f));
        DirectX::XMVECTOR lookat = DirectX::XMVectorSet(
            g_CameraPosition.x + g_CameraLookAt.x,
            g_CameraPosition.y + g_CameraLookAt.y,
            g_CameraPosition.z + g_CameraLookAt.z, 0.f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(
            g_CamearUpVec.x,
            g_CamearUpVec.y,
            g_CamearUpVec.z,
            0.f);
        g_View = DirectX::XMMatrixLookAtLH(eye, lookat, up);
        ConstantBuffer cb;
        cb.mWorld = XMMatrixTranspose(g_World);
        cb.mView = XMMatrixTranspose(g_View);
        cb.mProjection = XMMatrixTranspose(g_Projection);
        gp_ImmediateContext->UpdateSubresource(
            gp_ConstantBuffer, 0, nullptr, &cb, 0, 0);
        gp_ImmediateContext->IASetInputLayout(gp_VertexLayout);
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        gp_ImmediateContext->IASetVertexBuffers(
            0, 1, &gp_VertexBuffer, &stride, &offset);
        gp_ImmediateContext->IASetIndexBuffer(
            gp_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        gp_ImmediateContext->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        gp_ImmediateContext->VSSetConstantBuffers(
            0, 1, &gp_ConstantBuffer);
        
        gp_ImmediateContext->DrawIndexed(36, 0, 0);
    }
}

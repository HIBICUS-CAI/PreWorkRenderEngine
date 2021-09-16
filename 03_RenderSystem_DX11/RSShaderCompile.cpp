#include "RSShaderCompile.h"
#include <d3dcompiler.h>
#include <string>

HRESULT Tool::CompileShaderFromFile(const WCHAR* _fileName,
    LPCSTR _entryPoint, LPCSTR _shaderModel,
    ID3DBlob** _ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompileFromFile(
        _fileName, nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        _entryPoint, _shaderModel,
        dwShaderFlags, 0, _ppBlobOut, &pErrorBlob);
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
    if (pErrorBlob) { pErrorBlob->Release(); }

    return S_OK;
}

HRESULT Tool::CompileShaderFromFile(LPCSTR _fileName,
    LPCSTR _entryPoint, LPCSTR _shaderModel,
    ID3DBlob** _ppBlobOut)
{
    std::string path = std::string(_fileName);
    std::wstring wpath = std::wstring(path.begin(), path.end());

    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompileFromFile(
        wpath.c_str(), nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        _entryPoint, _shaderModel,
        dwShaderFlags, 0, _ppBlobOut, &pErrorBlob);
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
    if (pErrorBlob) { pErrorBlob->Release(); }

    return S_OK;
}

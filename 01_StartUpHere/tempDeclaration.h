#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>

struct MESH_VERTEX
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexCoord;
};
struct MESH_TEXTURE
{
    std::string Type;
    std::string Path;
    ID3D11ShaderResourceView* TexResView;
};

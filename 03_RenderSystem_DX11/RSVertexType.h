#pragma once

#include <DirectXMath.h>

namespace VertexType
{
    struct BasicVertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT2 TexCoord;
    };

    struct ColorVertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT4 Color;
    };

    struct TangentVertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT3 Tangent;
        DirectX::XMFLOAT2 TexCoord;
    };
}

//---------------------------------------------------------------
// File: RSVertexType.h
// Proj: RenderSystem_DX11
// Info: 声明RenderSystem_DX11相关所使用的顶点类型
// Date: 2021.9.19
// Mail: cai_genkan@outlook.com
// Comt: 目前考虑仅在RenderSystem_DX11中使用
//---------------------------------------------------------------

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

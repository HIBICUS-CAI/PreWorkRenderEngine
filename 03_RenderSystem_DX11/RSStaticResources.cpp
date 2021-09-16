//---------------------------------------------------------------
// File: RSStaticResources.cpp
// Proj: RenderSystem_DX11
// Info: 保存并提供所有常用的资源及其引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSStaticResources.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"
#include "RSTopic.h"
#include "RSShaderCompile.h"
#include "RSDevices.h"

RSStaticResources::RSStaticResources() :
    mRootPtr(nullptr),
    mVertexShaderMap({}), mGeometryShaderMap({}),
    mPixelShaderMap({}), mComputeShaderMap({}),
    mRasterizerStateMap({}), mDepthStencilStateMap({}),
    mBlendStateMap({}), mSamplerMap({}), mInputLayoutMap({}),
    mStaticPipelineMap({}), mStaticTopicMap({}), mMaterialMap({})
{

}

RSStaticResources::~RSStaticResources()
{

}

bool RSStaticResources::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    if (!CompileStaticShaders()) { return false; }
    if (!BuildStaticStates()) { return false; }
    if (!BuildStaticInputLayouts()) { return false; }
    if (!BuildStaticTopics()) { return false; }
    if (!BuildStaticPipelines()) { return false; }
    if (!BuildStaticMaterials()) { return false; }

    return true;
}

void RSStaticResources::CleanAndStop()
{
    for (auto& vShader : mVertexShaderMap)
    {
        SAFE_RELEASE(vShader.second);
    }
    mVertexShaderMap.clear();

    for (auto& gShader : mGeometryShaderMap)
    {
        SAFE_RELEASE(gShader.second);
    }
    mGeometryShaderMap.clear();

    for (auto& pShader : mPixelShaderMap)
    {
        SAFE_RELEASE(pShader.second);
    }
    mPixelShaderMap.clear();

    for (auto& cShader : mComputeShaderMap)
    {
        SAFE_RELEASE(cShader.second);
    }
    mComputeShaderMap.clear();

    for (auto& rState : mRasterizerStateMap)
    {
        SAFE_RELEASE(rState.second);
    }
    mRasterizerStateMap.clear();

    for (auto& dState : mDepthStencilStateMap)
    {
        SAFE_RELEASE(dState.second);
    }
    mDepthStencilStateMap.clear();

    for (auto& bState : mBlendStateMap)
    {
        SAFE_RELEASE(bState.second);
    }
    mBlendStateMap.clear();

    for (auto& sState : mSamplerMap)
    {
        SAFE_RELEASE(sState.second);
    }
    mSamplerMap.clear();

    for (auto& layout : mInputLayoutMap)
    {
        SAFE_RELEASE(layout.second);
    }
    mInputLayoutMap.clear();

    for (auto& pipeline : mStaticPipelineMap)
    {
        pipeline.second->ReleasePipeline();
        delete pipeline.second;
        pipeline.second = nullptr;
    }
    mStaticPipelineMap.clear();

    for (auto& topic : mStaticTopicMap)
    {
        topic.second->ReleaseTopic();
        delete topic.second;
        topic.second = nullptr;
    }
    mStaticTopicMap.clear();

    mMaterialMap.clear();
}

bool RSStaticResources::CompileStaticShaders()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

bool RSStaticResources::BuildStaticStates()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

bool RSStaticResources::BuildStaticInputLayouts()
{
    ID3DBlob* shaderBlob = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    HRESULT hr = S_OK;

    {
        hr = Tool::CompileShaderFromFile(
            L"..\\03_RenderSystem_DX11\\StaticResources\\InputLayoutForBasic.hlsl",
            "main", "vs_5_0", &shaderBlob);
        FAIL_HR_RETURN(hr);

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

        hr = mRootPtr->Devices()->GetDevice()->
            CreateInputLayout(
                layout, numInputLayouts,
                shaderBlob->GetBufferPointer(),
                shaderBlob->GetBufferSize(),
                &inputLayout);
        shaderBlob->Release();
        FAIL_HR_RETURN(hr);
        mInputLayoutMap.insert({ "BasicVertex",inputLayout });
        shaderBlob = nullptr;
        inputLayout = nullptr;
        hr = S_OK;
    }

    {
        hr = Tool::CompileShaderFromFile(
            L"..\\03_RenderSystem_DX11\\StaticResources\\InputLayoutForColor.hlsl",
            "main", "vs_5_0", &shaderBlob);
        FAIL_HR_RETURN(hr);

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
                "COLOR",0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,
                D3D11_INPUT_PER_VERTEX_DATA,0
            }
        };
        UINT numInputLayouts = ARRAYSIZE(layout);

        hr = mRootPtr->Devices()->GetDevice()->
            CreateInputLayout(
                layout, numInputLayouts,
                shaderBlob->GetBufferPointer(),
                shaderBlob->GetBufferSize(),
                &inputLayout);
        shaderBlob->Release();
        FAIL_HR_RETURN(hr);
        mInputLayoutMap.insert({ "ColorVertex",inputLayout });
        shaderBlob = nullptr;
        inputLayout = nullptr;
        hr = S_OK;
    }

    {
        hr = Tool::CompileShaderFromFile(
            L"..\\03_RenderSystem_DX11\\StaticResources\\InputLayoutForTangent.hlsl",
            "main", "vs_5_0", &shaderBlob);
        FAIL_HR_RETURN(hr);

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
                "TANGENT",0,
                DXGI_FORMAT_R32G32B32_FLOAT,0,24,
                D3D11_INPUT_PER_VERTEX_DATA,0
            },
            {
                "TEXCOORD",0,
                DXGI_FORMAT_R32G32_FLOAT,0,36,
                D3D11_INPUT_PER_VERTEX_DATA,0
            }
        };
        UINT numInputLayouts = ARRAYSIZE(layout);

        hr = mRootPtr->Devices()->GetDevice()->
            CreateInputLayout(
                layout, numInputLayouts,
                shaderBlob->GetBufferPointer(),
                shaderBlob->GetBufferSize(),
                &inputLayout);
        shaderBlob->Release();
        FAIL_HR_RETURN(hr);
        mInputLayoutMap.insert({ "TangentVertex",inputLayout });
        shaderBlob = nullptr;
        inputLayout = nullptr;
        hr = S_OK;
    }

    return true;
}

bool RSStaticResources::BuildStaticTopics()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

bool RSStaticResources::BuildStaticPipelines()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

bool RSStaticResources::BuildStaticMaterials()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

ID3D11VertexShader* RSStaticResources::GetStaticVertexShader(
    std::string& _shaderName)
{
    auto found = mVertexShaderMap.find(_shaderName);
    if (found != mVertexShaderMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11GeometryShader* RSStaticResources::GetStaticGeometryShader(
    std::string& _shaderName)
{
    auto found = mGeometryShaderMap.find(_shaderName);
    if (found != mGeometryShaderMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11PixelShader* RSStaticResources::GetStaticPixelShader(
    std::string& _shaderName)
{
    auto found = mPixelShaderMap.find(_shaderName);
    if (found != mPixelShaderMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11ComputeShader* RSStaticResources::GetStaticComputeShader(
    std::string& _shaderName)
{
    auto found = mComputeShaderMap.find(_shaderName);
    if (found != mComputeShaderMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11RasterizerState* RSStaticResources::GetStaticRasterizerState(
    std::string& _stateName)
{
    auto found = mRasterizerStateMap.find(_stateName);
    if (found != mRasterizerStateMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11DepthStencilState* RSStaticResources::GetStaticDepthStencilState(
    std::string& _stateName)
{
    auto found = mDepthStencilStateMap.find(_stateName);
    if (found != mDepthStencilStateMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11BlendState* RSStaticResources::GetStaticBlendState(
    std::string& _stateName)
{
    auto found = mBlendStateMap.find(_stateName);
    if (found != mBlendStateMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11SamplerState* RSStaticResources::GetStaticSampler(
    std::string& _samplerName)
{
    auto found = mSamplerMap.find(_samplerName);
    if (found != mSamplerMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

ID3D11InputLayout* RSStaticResources::GetStaticInputLayout(
    std::string& _layoutName)
{
    auto found = mInputLayoutMap.find(_layoutName);
    if (found != mInputLayoutMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

const RSPipeline* const RSStaticResources::GetStaticPipeline(
    std::string& _pipelineName)
{
    auto found = mStaticPipelineMap.find(_pipelineName);
    if (found != mStaticPipelineMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

const RSTopic* const RSStaticResources::GetStaticTopic(
    std::string& _topicName)
{
    auto found = mStaticTopicMap.find(_topicName);
    if (found != mStaticTopicMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

RS_MATERIAL_INFO* RSStaticResources::GetStaticMaterial(
    std::string& _materialName)
{
    auto found = mMaterialMap.find(_materialName);
    if (found != mMaterialMap.end())
    {
        return &(found->second);
    }
    else
    {
        return nullptr;
    }
}

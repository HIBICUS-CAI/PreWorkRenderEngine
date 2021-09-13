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
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSStaticResources::CleanAndStop()
{

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
    // TEMP----------------------
    return true;
    // TEMP----------------------
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
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11GeometryShader* RSStaticResources::GetStaticGeometryShader(
    std::string& _shaderName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11PixelShader* RSStaticResources::GetStaticPixelShader(
    std::string& _shaderName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11ComputeShader* RSStaticResources::GetStaticComputeShader(
    std::string& _shaderName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11RasterizerState* RSStaticResources::GetStaticRasterizerState(
    std::string& _stateName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11DepthStencilState* RSStaticResources::GetStaticDepthStencilState(
    std::string& _stateName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11BlendState* RSStaticResources::GetStaticBlendState(
    std::string& _stateName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11SamplerState* RSStaticResources::GetStaticSampler(
    std::string& _samplerName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

ID3D11InputLayout* RSStaticResources::GetStaticInputLayout(
    std::string& _layoutName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RSPipeline* RSStaticResources::GetStaticPipeline(
    std::string& _pipelineName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RSTopic* RSStaticResources::GetStaticTopic(
    std::string& _topicName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

RSMaterial* RSStaticResources::GetStaticMaterial(
    std::string& _materialName)
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

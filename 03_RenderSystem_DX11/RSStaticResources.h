//---------------------------------------------------------------
// File: RSStaticResources.h
// Proj: RenderSystem_DX11
// Info: 保存并提供所有常用的资源及其引用
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <unordered_map>

class RSStaticResources
{
public:
    RSStaticResources();
    ~RSStaticResources();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    ID3D11VertexShader* GetStaticVertexShader(
        std::string& _shaderName);
    ID3D11GeometryShader* GetStaticGeometryShader(
        std::string& _shaderName);
    ID3D11PixelShader* GetStaticPixelShader(
        std::string& _shaderName);
    ID3D11ComputeShader* GetStaticComputeShader(
        std::string& _shaderName);
    ID3D11RasterizerState* GetStaticRasterizerState(
        std::string& _stateName);
    ID3D11DepthStencilState* GetStaticDepthStencilState(
        std::string& _stateName);
    ID3D11BlendState* GetStaticBlendState(
        std::string& _stateName);
    ID3D11SamplerState* GetStaticSampler(
        std::string& _samplerName);
    ID3D11InputLayout* GetStaticInputLayout(
        std::string& _layoutName);
    const class RSPipeline* const GetStaticPipeline(
        std::string& _pipelineName);
    const class RSTopic* const GetStaticTopic(
        std::string& _topicName);
    RS_MATERIAL_INFO* GetStaticMaterial(
        std::string& _materialName);

private:
    bool CompileStaticShaders();
    bool BuildStaticStates();
    bool BuildStaticInputLayouts();
    bool BuildStaticTopics();
    bool BuildStaticPipelines();
    bool BuildStaticMaterials();

private:
    class RSRoot_DX11* mRootPtr;

    std::unordered_map<std::string, ID3D11VertexShader*>
        mVertexShaderMap;
    std::unordered_map<std::string, ID3D11GeometryShader*>
        mGeometryShaderMap;
    std::unordered_map<std::string, ID3D11PixelShader*>
        mPixelShaderMap;
    std::unordered_map<std::string, ID3D11ComputeShader*>
        mComputeShaderMap;

    std::unordered_map<std::string, ID3D11RasterizerState*>
        mRasterizerStateMap;
    std::unordered_map<std::string, ID3D11DepthStencilState*>
        mDepthStencilStateMap;
    std::unordered_map<std::string, ID3D11BlendState*>
        mBlendStateMap;

    std::unordered_map<std::string, ID3D11SamplerState*>
        mSamplerMap;

    std::unordered_map<std::string, ID3D11InputLayout*>
        mInputLayoutMap;

    std::unordered_map<std::string, class RSPipeline*>
        mStaticPipelineMap;
    std::unordered_map<std::string, class RSTopic*>
        mStaticTopicMap;

    std::unordered_map<std::string, RS_MATERIAL_INFO>
        mMaterialMap;
};

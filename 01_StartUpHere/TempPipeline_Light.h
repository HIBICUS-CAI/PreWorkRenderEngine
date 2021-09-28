#pragma once

#include <string>
#include <vector>
#include "RSPass_Base.h"

void PassRootToTempLightPipeline(class RSRoot_DX11* _root);

bool CreateTempLightPipeline();

class RSPass_Light :public RSPass_Base
{
public:
    RSPass_Light(std::string& _name, PASS_TYPE _type,
        class RSRoot_DX11* _root);
    RSPass_Light(const RSPass_Light& _source);
    virtual ~RSPass_Light();

public:
    virtual RSPass_Light* ClonePass() override;

    virtual bool InitPass();

    virtual void ReleasePass();

    virtual void ExecuatePass();

private:
    bool CreateShaders();
    bool CreateStates();
    bool CreateBuffers();
    bool CreateViews();
    bool CreateSamplers();

private:
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RasterizerState* mRasterizerState;
    ID3D11DepthStencilView* mDepthStencilView;
    ID3D11SamplerState* mMeshTexSampler;
    DRAWCALL_TYPE mDrawCallType;
    RSDrawCallsPipe* mDrawCallPipe;
    ID3D11Buffer* mViewProjStructedBuffer;
    ID3D11ShaderResourceView* mViewProjStructedBufferSrv;
    ID3D11Buffer* mInstanceStructedBuffer;
    ID3D11ShaderResourceView* mInstanceStructedBufferSrv;
    ID3D11Buffer* mLightStructedBuffer;
    ID3D11ShaderResourceView* mLightStructedBufferSrv;
    ID3D11Buffer* mAmbientStructedBuffer;
    ID3D11ShaderResourceView* mAmbientStructedBufferSrv;
};

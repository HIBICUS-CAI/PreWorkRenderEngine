//---------------------------------------------------------------
// File: RSTopic.h
// Proj: RenderSystem_DX11
// Info: 描述一个具体的主题并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: 创建主题时一定不能直接使用指针，一定要新构造一个对象
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSTopic
{
public:
    RSTopic(std::string& _name);
    RSTopic(const RSTopic& _source);
    ~RSTopic();

    const std::string& GetTopicName() const;
    void StartTopicAssembly();
    void FinishTopicAssembly();
    void SetExecuateOrder(UINT _order);
    UINT GetExecuateOrder() const;
    void SetMTContext(ID3D11DeviceContext* _mtContext);

    void InsertPass(class RSPass_Base* _pass);
    void ErasePass(class RSPass_Base* _pass);
    void ErasePass(std::string& _passName);
    bool HasPass(std::string& _passName);

    bool InitAllPasses();

    void ExecuateTopic();

    void ReleaseTopic();

private:
    const std::string mName;
    bool mAssemblyFinishFlag;
    UINT mExecuateOrderInPipeline;
    std::vector<class RSPass_Base*> mPassVector;
    ID3D11DeviceContext* mMTContext;
};

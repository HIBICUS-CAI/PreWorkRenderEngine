//---------------------------------------------------------------
// File: RSTopic.h
// Proj: RenderSystem_DX11
// Info: 描述一个具体的主题并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <Windows.h>

// TEMP------------------
constexpr UINT RS_INVALID_ORDER = 0;
// TEMP------------------

class RSTopic
{
public:
    RSTopic(std::string& _name);
    ~RSTopic();

    const std::string& GetTopicName() const;
    void StartTopicAssembly();
    void FinishTopicAssembly();
    void SetExecuateOrder(UINT _order);
    UINT GetExecuateOrder() const;

    void InsertPass(class RSPass_Base* _pass, UINT _execOrder);
    void ErasePass(class RSPass_Base* _pass);
    void ErasePass(std::string& _passName);
    bool HasPass(std::string& _passName);

    bool InitAllPasses();

    void ExecuateTopic();

private:
    const std::string mName;
    bool mAssemblyFinishFlag;
    UINT mExecuateOrderInPipeline;
    std::vector<class RSPass_Base*> mPassVector;
};

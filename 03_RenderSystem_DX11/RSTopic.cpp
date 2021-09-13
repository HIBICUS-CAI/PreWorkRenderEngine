//---------------------------------------------------------------
// File: RSTopic.cpp
// Proj: RenderSystem_DX11
// Info: 描述一个具体的主题并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSTopic.h"

RSTopic::RSTopic(std::string& _name) :
    mName(_name), mAssemblyFinishFlag(false),
    mExecuateOrderInPipeline(RS_INVALID_ORDER), mPassVector({})
{

}

RSTopic::~RSTopic()
{

}

const std::string& RSTopic::GetTopicName() const
{
    return mName;
}

void RSTopic::StartTopicAssembly()
{

}

void RSTopic::FinishTopicAssembly()
{

}

void RSTopic::SetExecuateOrder(UINT _order)
{

}

UINT RSTopic::GetExecuateOrder() const
{
    // TEMP----------------------
    return RS_INVALID_ORDER;
    // TEMP----------------------
}

void RSTopic::InsertPass(RSPass_Base* _pass, UINT _execOrder)
{

}

void RSTopic::ErasePass(RSPass_Base* _pass)
{

}

void RSTopic::ErasePass(std::string& _passName)
{

}

bool RSTopic::HasPass(std::string& _passName)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

bool RSTopic::InitAllPasses()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSTopic::ExecuateTopic()
{

}

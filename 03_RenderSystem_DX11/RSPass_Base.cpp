//---------------------------------------------------------------
// File: RSPass_Base.cpp
// Proj: RenderSystem_DX11
// Info: 描述一个抽象的渲染通道并提供基础信息的操作与获取方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSPass_Base.h"

RSPass_Base::RSPass_Base(std::string& _name, PASS_TYPE _type) :
    mName(_name), mPassType(_type),
    mExecuateOrderInTopic(RS_INVALID_ORDER)
{

}

RSPass_Base::~RSPass_Base()
{

}

const std::string& RSPass_Base::GetPassName() const
{
    return mName;
}

PASS_TYPE RSPass_Base::GetPassType() const
{
    return mPassType;
}

void RSPass_Base::SetExecuateOrder(UINT _order)
{

}

UINT RSPass_Base::GetExecuateOrder() const
{
    // TEMP----------------------
    return RS_INVALID_ORDER;
    // TEMP----------------------
}

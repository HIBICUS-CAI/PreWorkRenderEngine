//---------------------------------------------------------------
// File: RSPass_Base.cpp
// Proj: RenderSystem_DX11
// Info: 描述一个抽象的渲染通道并提供基础信息的操作与获取方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: 深拷贝的虚函数要加override并返回子类类型的动态分配指针
//---------------------------------------------------------------

#include "RSPass_Base.h"

RSPass_Base::RSPass_Base(std::string& _name, PASS_TYPE _type) :
    mName(_name), mPassType(_type),
    mExecuateOrderInTopic(RS_INVALID_ORDER)
{

}

RSPass_Base::RSPass_Base(const RSPass_Base& _source) :
    mName(_source.mName), mPassType(_source.mPassType),
    mExecuateOrderInTopic(_source.mExecuateOrderInTopic)
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
    mExecuateOrderInTopic = _order;
}

UINT RSPass_Base::GetExecuateOrder() const
{
    return mExecuateOrderInTopic;
}

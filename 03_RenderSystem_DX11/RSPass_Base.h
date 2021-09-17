//---------------------------------------------------------------
// File: RSPass_Base.h
// Proj: RenderSystem_DX11
// Info: 描述一个抽象的渲染通道并提供基础信息的操作与获取方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: 深拷贝的虚函数要加override并返回子类类型的动态分配指针
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSPass_Base
{
public:
    RSPass_Base(std::string& _name, PASS_TYPE _type);
    RSPass_Base(const RSPass_Base& _source);
    virtual ~RSPass_Base();

    const std::string& GetPassName() const;
    PASS_TYPE GetPassType() const;
    void SetExecuateOrder(UINT _order);
    UINT GetExecuateOrder() const;

public:
    virtual RSPass_Base* ClonePass() = 0;

    virtual bool InitPass() = 0;

    virtual void ReleasePass() = 0;

    virtual void ExecuatePass() = 0;

private:
    const std::string mName;
    const PASS_TYPE mPassType;
    UINT mExecuateOrderInTopic;
};

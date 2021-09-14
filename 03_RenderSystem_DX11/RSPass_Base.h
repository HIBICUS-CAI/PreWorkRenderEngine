//---------------------------------------------------------------
// File: RSPass_Base.h
// Proj: RenderSystem_DX11
// Info: ����һ���������Ⱦͨ�����ṩ������Ϣ�Ĳ������ȡ��ʽ
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <string>
#include <Windows.h>

class RSPass_Base
{
public:
    RSPass_Base(std::string& _name, PASS_TYPE _type);
    virtual ~RSPass_Base();

    const std::string& GetPassName() const;
    PASS_TYPE GetPassType() const;
    void SetExecuateOrder(UINT _order);
    UINT GetExecuateOrder() const;

public:
    virtual bool InitPass() = 0;

    virtual void ReleasePass() = 0;

    virtual void ExecuatePass() = 0;

private:
    const std::string mName;
    const PASS_TYPE mPassType;
    UINT mExecuateOrderInTopic;
};

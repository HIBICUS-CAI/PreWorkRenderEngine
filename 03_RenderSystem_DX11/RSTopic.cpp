//---------------------------------------------------------------
// File: RSTopic.cpp
// Proj: RenderSystem_DX11
// Info: ����һ����������Ⲣ�ṩִ���������ʽ
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: ��������ʱһ������ֱ��ʹ��ָ�룬һ��Ҫ�¹���һ������
//---------------------------------------------------------------

#include "RSTopic.h"
#include "RSPass_Base.h"
#include <algorithm>

RSTopic::RSTopic(std::string& _name) :
    mName(_name), mAssemblyFinishFlag(true),
    mExecuateOrderInPipeline(RS_INVALID_ORDER), mPassVector({})
{

}

RSTopic::RSTopic(const RSTopic& _source) :
    mName(_source.mName),
    mAssemblyFinishFlag(_source.mAssemblyFinishFlag),
    mExecuateOrderInPipeline(_source.mExecuateOrderInPipeline),
    mPassVector({})
{
    mPassVector.reserve(_source.mPassVector.size());
    for (auto& topic : _source.mPassVector)
    {
        RSPass_Base* onemore = topic->ClonePass();
        mPassVector.push_back(onemore);
    }
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
    mAssemblyFinishFlag = false;
}

void RSTopic::FinishTopicAssembly()
{
    mAssemblyFinishFlag = true;
}

void RSTopic::SetExecuateOrder(UINT _order)
{
    mExecuateOrderInPipeline = _order;
}

UINT RSTopic::GetExecuateOrder() const
{
    return mExecuateOrderInPipeline;
}

bool PassExecLessCompare(const RSPass_Base* a, const RSPass_Base* b)
{
    return a->GetExecuateOrder() < b->GetExecuateOrder();
}

void RSTopic::InsertPass(RSPass_Base* _pass)
{
    if (!mAssemblyFinishFlag)
    {
        mPassVector.push_back(_pass);
        std::sort(mPassVector.begin(), mPassVector.end(),
            PassExecLessCompare);
    }
}

void RSTopic::ErasePass(RSPass_Base* _pass)
{
    if (!mAssemblyFinishFlag)
    {
        for (auto i = mPassVector.begin();
            i != mPassVector.end(); i++)
        {
            if (*i == _pass)
            {
                (*i)->ReleasePass();
                delete (*i);
                mPassVector.erase(i);
                return;
            }
        }
    }
}

void RSTopic::ErasePass(std::string& _passName)
{
    if (!mAssemblyFinishFlag)
    {
        for (auto i = mPassVector.begin();
            i != mPassVector.end(); i++)
        {
            if ((*i)->GetPassName() == _passName)
            {
                (*i)->ReleasePass();
                delete (*i);
                mPassVector.erase(i);
                return;
            }
        }
    }
}

bool RSTopic::HasPass(std::string& _passName)
{
    for (auto& pass : mPassVector)
    {
        if (pass->GetPassName() == _passName)
        {
            return true;
        }
    }

    return false;
}

bool RSTopic::InitAllPasses()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& pass : mPassVector)
        {
            if (!pass->InitPass())
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

void RSTopic::ExecuateTopic()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& pass : mPassVector)
        {
            pass->ExecuatePass();
        }
    }
}

void RSTopic::ReleaseTopic()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& pass : mPassVector)
        {
            pass->ReleasePass();
            delete pass;
        }
        mPassVector.clear();
    }
}

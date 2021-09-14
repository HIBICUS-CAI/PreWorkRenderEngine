//---------------------------------------------------------------
// File: RSPipeline.cpp
// Proj: RenderSystem_DX11
// Info: 描述一个具体的流水线并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: 创建流水线时一定不能直接使用指针，一定要新构造一个对象
//---------------------------------------------------------------

#include "RSPipeline.h"
#include "RSTopic.h"
#include <algorithm>

RSPipeline::RSPipeline(std::string& _name) :
    mName(_name), mAssemblyFinishFlag(true), mTopicVector({})
{

}

RSPipeline::RSPipeline(const RSPipeline& _source) :
    mName(_source.mName),
    mAssemblyFinishFlag(_source.mAssemblyFinishFlag),
    mTopicVector({})
{
    mTopicVector.reserve(_source.mTopicVector.size());
    for (auto& topic : _source.mTopicVector)
    {
        RSTopic* onemore = new RSTopic(*topic);
        mTopicVector.push_back(onemore);
    }
}

RSPipeline::~RSPipeline()
{

}

const std::string& RSPipeline::GetPipelineName() const
{
    return mName;
}

void RSPipeline::StartPipelineAssembly()
{
    mAssemblyFinishFlag = false;
}

void RSPipeline::FinishPipelineAssembly()
{
    mAssemblyFinishFlag = true;
}

bool RSPipeline::HasTopic(std::string& _topicName)
{
    for (auto& topic : mTopicVector)
    {
        if (topic->GetTopicName() == _topicName)
        {
            return true;
        }
    }

    return false;
}

bool TopicExecLessCompare(const RSTopic* a, const RSTopic* b)
{
    return a->GetExecuateOrder() < b->GetExecuateOrder();
}

void RSPipeline::InsertTopic(RSTopic* _topic)
{
    if (!mAssemblyFinishFlag)
    {
        mTopicVector.push_back(_topic);
        std::sort(mTopicVector.begin(), mTopicVector.end(),
            TopicExecLessCompare);
    }
}

void RSPipeline::EraseTopic(RSTopic* _topic)
{
    if (!mAssemblyFinishFlag)
    {
        for (auto i = mTopicVector.begin();
            i != mTopicVector.end(); i++)
        {
            if (*i == _topic)
            {
                (*i)->ReleaseTopic();
                delete (*i);
                mTopicVector.erase(i);
                return;
            }
        }
    }
}

void RSPipeline::EraseTopic(std::string& _topicName)
{
    if (!mAssemblyFinishFlag)
    {
        for (auto i = mTopicVector.begin();
            i != mTopicVector.end(); i++)
        {
            if ((*i)->GetTopicName() == _topicName)
            {
                (*i)->ReleaseTopic();
                delete (*i);
                mTopicVector.erase(i);
                return;
            }
        }
    }
}

bool RSPipeline::InitAllTopics()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& topic : mTopicVector)
        {
            if (!topic->InitAllPasses())
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

void RSPipeline::ExecuatePipeline()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& topic : mTopicVector)
        {
            topic->ExecuateTopic();
        }
    }
}

void RSPipeline::ReleasePipeline()
{
    if (mAssemblyFinishFlag)
    {
        for (auto& topic : mTopicVector)
        {
            topic->ReleaseTopic();
            delete topic;
        }
        mTopicVector.clear();
    }
}

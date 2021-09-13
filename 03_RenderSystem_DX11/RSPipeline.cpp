//---------------------------------------------------------------
// File: RSPipeline.cpp
// Proj: RenderSystem_DX11
// Info: ����һ���������ˮ�߲��ṩִ���������ʽ
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSPipeline.h"

RSPipeline::RSPipeline(std::string& _name) :
    mName(_name), mAssemblyFinishFlag(false), mTopicVector({})
{

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

}

void RSPipeline::FinishPipelineAssembly()
{

}

bool RSPipeline::HasTopic(std::string& _topicName)
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSPipeline::InsertTopic(RSTopic* _topic, UINT _execOrder)
{

}

void RSPipeline::EraseTopic(RSTopic* _topic)
{

}

void RSPipeline::EraseTopic(std::string& _topicName)
{

}

bool RSPipeline::InitAllTopics()
{
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSPipeline::ExecuatePipeline()
{

}

//---------------------------------------------------------------
// File: RSPipeline.h
// Proj: RenderSystem_DX11
// Info: 描述一个具体的流水线并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <string>
#include <vector>
#include <Windows.h>

class RSPipeline
{
public:
    RSPipeline(std::string& _name);
    ~RSPipeline();

    const std::string& GetPipelineName() const;
    void StartPipelineAssembly();
    void FinishPipelineAssembly();

    bool HasTopic(std::string& _topicName);
    void InsertTopic(class RSTopic* _topic, UINT _execOrder);
    void EraseTopic(class RSTopic* _topic);
    void EraseTopic(std::string& _topicName);

    bool InitAllTopics();

    void ExecuatePipeline();

private:
    const std::string mName;
    bool mAssemblyFinishFlag;
    std::vector<class RSTopic*> mTopicVector;
};

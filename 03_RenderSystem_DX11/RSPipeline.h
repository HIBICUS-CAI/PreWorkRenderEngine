//---------------------------------------------------------------
// File: RSPipeline.h
// Proj: RenderSystem_DX11
// Info: ����һ���������ˮ�߲��ṩִ���������ʽ
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: ������ˮ��ʱһ������ֱ��ʹ��ָ�룬һ��Ҫ�¹���һ������
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

class RSPipeline
{
public:
    RSPipeline(std::string& _name);
    RSPipeline(const RSPipeline& _source);
    ~RSPipeline();

    const std::string& GetPipelineName() const;
    void StartPipelineAssembly();
    void FinishPipelineAssembly();

    bool HasTopic(std::string& _topicName);
    void InsertTopic(class RSTopic* _topic);
    void EraseTopic(class RSTopic* _topic);
    void EraseTopic(std::string& _topicName);

    bool InitAllTopics();

    void ExecuatePipeline();

    void ReleasePipeline();

private:
    const std::string mName;
    bool mAssemblyFinishFlag;
    std::vector<class RSTopic*> mTopicVector;
};

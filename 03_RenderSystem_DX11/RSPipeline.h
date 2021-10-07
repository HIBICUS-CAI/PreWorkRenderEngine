//---------------------------------------------------------------
// File: RSPipeline.h
// Proj: RenderSystem_DX11
// Info: 描述一个具体的流水线并提供执行与操作方式
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: 创建流水线时一定不能直接使用指针，一定要新构造一个对象
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"

struct TopicThread
{
    ID3D11DeviceContext* mDeferredContext = nullptr;
    ID3D11CommandList* mCommandList = nullptr;
    HANDLE mThreadHandle = nullptr;
    HANDLE mBeginEvent = nullptr;
    HANDLE mFinishEvent = nullptr;
    bool mExitFlag = false;
    struct ArgumentList
    {
        class RSTopic* mTopicPtr = nullptr;
        HANDLE mBeginEventPtr = nullptr;
        HANDLE mFinishEventPtr = nullptr;
        bool* mExitFlagPtr = nullptr;
        ID3D11DeviceContext* mDeferredContext = nullptr;
        ID3D11CommandList** mCommandListPtr = nullptr;
    } mArgumentList = {};
};

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

    bool InitAllTopics(class RSDevices* _devices);

    void ExecuatePipeline();

    void ReleasePipeline();

    void SuspendAllThread();
    void ResumeAllThread();

private:
    const std::string mName;
    bool mAssemblyFinishFlag;
    std::vector<class RSTopic*> mTopicVector;
    std::vector<TopicThread> mTopicThreads;
    std::vector<HANDLE> mFinishEvents;

    ID3D11DeviceContext* mImmediateContext;
    bool mMutipleThreadMode;
};

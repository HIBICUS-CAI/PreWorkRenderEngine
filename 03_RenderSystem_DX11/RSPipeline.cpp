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
#include "RSDevices.h"
#include <algorithm>
#include <thread>

unsigned __stdcall TopicThreadFunc(PVOID _argu);

RSPipeline::RSPipeline(std::string& _name) :
    mName(_name), mAssemblyFinishFlag(true), mTopicVector({}),
    mTopicThreads({}), mImmediateContext(nullptr),
    mMutipleThreadMode(false), mFinishEvents({})
{

}

RSPipeline::RSPipeline(const RSPipeline& _source) :
    mName(_source.mName),
    mAssemblyFinishFlag(_source.mAssemblyFinishFlag),
    mImmediateContext(_source.mImmediateContext),
    mTopicVector({}), mTopicThreads({}),
    mMutipleThreadMode(_source.mMutipleThreadMode),
    mFinishEvents({})
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

bool RSPipeline::InitAllTopics(RSDevices* _devices)
{
    if (!_devices) { return false; }
    mImmediateContext = _devices->GetSTContext();
    mMutipleThreadMode = _devices->GetCommandListSupport();

    if (mAssemblyFinishFlag)
    {
        for (auto& topic : mTopicVector)
        {
            if (!topic->InitAllPasses()) { return false; }
            if (mMutipleThreadMode)
            {
                ID3D11DeviceContext* deferred = nullptr;
                HRESULT hr = _devices->GetDevice()->
                    CreateDeferredContext(0, &deferred);
                FAIL_HR_RETURN(hr);
                topic->SetMTContext(deferred);

                TopicThread tt = {};
                tt.mDeferredContext = deferred;
                tt.mCommandList = nullptr;
                tt.mThreadHandle = NULL;
                tt.mExitFlag = false;
                tt.mArgumentList.mTopicPtr = topic;
                tt.mArgumentList.mDeferredContext = deferred;
                mTopicThreads.emplace_back(tt);
            }
        }

        UINT coreCount = std::thread::hardware_concurrency();
        DWORD_PTR affinity = 0;
        DWORD_PTR mask = 0;
        for (auto& t : mTopicThreads)
        {
            t.mArgumentList.mExitFlagPtr = &t.mExitFlag;
            t.mArgumentList.mCommandListPtr = &t.mCommandList;

            t.mBeginEvent = CreateEvent(nullptr, FALSE,
                FALSE, nullptr);
            t.mFinishEvent = CreateEvent(nullptr, FALSE,
                FALSE, nullptr);
            mFinishEvents.emplace_back(t.mFinishEvent);

            t.mArgumentList.mBeginEventPtr = t.mBeginEvent;
            t.mArgumentList.mFinishEventPtr = t.mFinishEvent;

            t.mThreadHandle = (HANDLE)_beginthreadex(
                nullptr, 0, TopicThreadFunc, &(t.mArgumentList),
                CREATE_SUSPENDED, nullptr);
            if (!t.mThreadHandle) { return false; }
            mask = SetThreadAffinityMask(t.mThreadHandle,
                static_cast<DWORD_PTR>(1) << affinity);
            affinity = (++affinity) % coreCount;
            //ResumeThread(t.mThreadHandle);
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
        if (mMutipleThreadMode)
        {
            for (auto& t : mTopicThreads)
            {
                SetEvent(t.mBeginEvent);
            }

            WaitForMultipleObjects((DWORD)mFinishEvents.size(),
                &mFinishEvents[0], TRUE, INFINITE);

            for (auto& t : mTopicThreads)
            {
                if (!t.mCommandList) { assert(false); return; }
                mImmediateContext->ExecuteCommandList(
                    t.mCommandList, TRUE);
                SAFE_RELEASE(t.mCommandList);
            }
        }
        else
        {
            for (auto& topic : mTopicVector)
            {
                topic->ExecuateTopic();
            }
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
        if (mMutipleThreadMode)
        {
            std::vector<HANDLE> handleVec = {};
            for (auto& thread : mTopicThreads)
            {
                handleVec.emplace_back(thread.mThreadHandle);
                thread.mExitFlag = true;
            }
        }
        for (auto& thread : mTopicThreads)
        {
            SAFE_RELEASE(thread.mDeferredContext);
            SAFE_RELEASE(thread.mCommandList);
            CloseHandle(thread.mBeginEvent);
            CloseHandle(thread.mFinishEvent);
        }
    }
}

unsigned __stdcall TopicThreadFunc(PVOID _argu)
{
    TopicThread::ArgumentList* argument = nullptr;
    argument = (TopicThread::ArgumentList*)_argu;
    if (!argument) { return -1; }

    auto topic = argument->mTopicPtr;
    auto deferred = argument->mDeferredContext;
    auto listptr = argument->mCommandListPtr;
    HANDLE begin = argument->mBeginEventPtr;
    HANDLE finish = argument->mFinishEventPtr;
    auto exit = argument->mExitFlagPtr;

    HRESULT hr = S_OK;
    while (true)
    {
        WaitForSingleObject(begin, INFINITE);
        if (*exit)
        {
            break;
        }
        topic->ExecuateTopic();
        hr = deferred->FinishCommandList(FALSE, listptr);
#ifdef _DEBUG
        if (FAILED(hr)) { return -2; }
#endif // _DEBUG
        SetEvent(finish);
        //SuspendThread(GetCurrentThread());
    }

    return 0;
}

void RSPipeline::SuspendAllThread()
{
    for (auto& t : mTopicThreads)
    {
        SuspendThread(t.mThreadHandle);
    }
}

void RSPipeline::ResumeAllThread()
{
    for (auto& t : mTopicThreads)
    {
        ResumeThread(t.mThreadHandle);
    }
}

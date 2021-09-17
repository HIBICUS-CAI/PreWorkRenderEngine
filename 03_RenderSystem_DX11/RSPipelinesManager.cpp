//---------------------------------------------------------------
// File: RSPipelinesManager.cpp
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的pipeline
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#include "RSPipelinesManager.h"
#include "RSRoot_DX11.h"
#include "RSPipeline.h"

RSPipelinesManager::RSPipelinesManager() :
    mRootPtr(nullptr), mCurrentPipeline(nullptr),
    mNextPipeline(nullptr), mPipelineMap({})
{

}

RSPipelinesManager::~RSPipelinesManager()
{

}

bool RSPipelinesManager::StartUp(RSRoot_DX11* _root)
{
    if (!_root) { return false; }

    mRootPtr = _root;

    return true;
}

void RSPipelinesManager::CleanAndStop()
{
    mNextPipeline = nullptr;
    mCurrentPipeline = nullptr;
    for (auto& pipeline : mPipelineMap)
    {
        pipeline.second->ReleasePipeline();
        delete pipeline.second;
    }
    mPipelineMap.clear();
}

void RSPipelinesManager::AddPipeline(
    std::string& _name, RSPipeline* _pipeline)
{
    if (mPipelineMap.find(_name) == mPipelineMap.end())
    {
        mPipelineMap.insert({ _name,_pipeline });
    }
}

RSPipeline* RSPipelinesManager::GetPipeline(
    std::string& _name) const
{
    auto found = mPipelineMap.find(_name);
    if (found != mPipelineMap.end())
    {
        return found->second;
    }
    else
    {
        return nullptr;
    }
}

void RSPipelinesManager::SetPipeline(std::string& _name)
{
    auto found = mPipelineMap.find(_name);
    if (found != mPipelineMap.end())
    {
        mNextPipeline = (*found).second;
    }
}

void RSPipelinesManager::SetPipeline(RSPipeline* _pipeline)
{
    mNextPipeline = _pipeline;
}

void RSPipelinesManager::ClearCurrentPipelineState()
{
    mCurrentPipeline = nullptr;
    mNextPipeline = nullptr;
}

void RSPipelinesManager::ExecuateCurrentPipeline()
{
    mCurrentPipeline->ExecuatePipeline();
}

void RSPipelinesManager::ProcessNextPipeline()
{
    if (mNextPipeline)
    {
        mCurrentPipeline = mNextPipeline;
        mNextPipeline = nullptr;
    }
}

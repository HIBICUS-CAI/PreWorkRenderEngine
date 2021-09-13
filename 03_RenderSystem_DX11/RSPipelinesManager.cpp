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
    // TEMP----------------------
    return true;
    // TEMP----------------------
}

void RSPipelinesManager::CleanAndStop()
{

}

void RSPipelinesManager::AddPipeline(
    std::string& _name, RSPipeline* _pipeline)
{

}

RSPipeline* RSPipelinesManager::GetPipeline(
    std::string& _name) const
{
    // TEMP----------------------
    return nullptr;
    // TEMP----------------------
}

void RSPipelinesManager::SetPipeline(std::string& _name)
{

}

void RSPipelinesManager::SetPipeline(RSPipeline* _pipeline)
{

}

void RSPipelinesManager::ClearCurrentPipeline()
{

}

void RSPipelinesManager::ExecuateCurrentPipeline()
{

}

void RSPipelinesManager::ProcessNextPipeline()
{

}

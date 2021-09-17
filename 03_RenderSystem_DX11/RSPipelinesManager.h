//---------------------------------------------------------------
// File: RSPipelinesManager.h
// Proj: RenderSystem_DX11
// Info: 保存并管理所有的pipeline
// Date: 2021.9.13
// Mail: cai_genkan@outlook.com
// Comt: NULL
//---------------------------------------------------------------

#pragma once

#include "RSCommon.h"
#include <unordered_map>

class RSPipelinesManager
{
public:
    RSPipelinesManager();
    ~RSPipelinesManager();

    bool StartUp(class RSRoot_DX11* _root);
    void CleanAndStop();

    void AddPipeline(
        std::string& _name, class RSPipeline* _pipeline);
    class RSPipeline* GetPipeline(std::string& _name) const;

    void SetPipeline(std::string& _name);
    void SetPipeline(class RSPipeline* _pipeline);
    void ClearCurrentPipelineState();

    void ExecuateCurrentPipeline();
    void ProcessNextPipeline();

private:
    class RSRoot_DX11* mRootPtr;
    class RSPipeline* mCurrentPipeline;
    class RSPipeline* mNextPipeline;
    std::unordered_map<std::string, class RSPipeline*> mPipelineMap;
};

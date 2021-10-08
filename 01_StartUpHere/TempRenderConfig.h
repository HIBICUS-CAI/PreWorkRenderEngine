#pragma once

struct RENDER_CONFIG
{
    bool mDeferredRenderingEnable = true;
    bool mForceSingleThreadEnable = false;
    bool mSecongShadowEnable = true;
    unsigned int mBlurLoopCount = 4;
};

RENDER_CONFIG GetRenderConfig();

#pragma once

struct RENDER_CONFIG
{
    bool mDeferredRenderingEnable = true;
    bool mForceSingleThreadEnable = false;
    bool mSecongShadowEnable = true;
    unsigned int mSsaoBlurLoopCount = 4;
    unsigned int mBloomBlurLoopCount = 1;
};

RENDER_CONFIG GetRenderConfig();

#define _CRT_SECURE_NO_WARNINGS

#include "TempRenderConfig.h"
#include <cstdio>
#include <rapidjson\filereadstream.h>
#include <rapidjson\document.h>

RENDER_CONFIG GetRenderConfig()
{
    RENDER_CONFIG rc = {};

    std::FILE* fp = std::fopen(
        "Assets\\temp-scene-render-config.json", "rb");
    if (!fp) { return rc; }
    char* readBuf = new char[65536];
    if (!readBuf) { return rc; }

    rapidjson::FileReadStream is(
        fp, readBuf, 65536 * sizeof(char));
    rapidjson::Document doc = {};
    doc.ParseStream(is);
    delete[] readBuf;
    std::fclose(fp);

    rc.mDeferredRenderingEnable =
        doc["deferred-rendering-enable"].GetBool();
    rc.mForceSingleThreadEnable =
        doc["force-singlethread-enable"].GetBool();
    rc.mSecongShadowEnable =
        doc["second-shadow-enable"].GetBool();
    rc.mBlurLoopCount =
        doc["blur-loop-count"].GetUint();

    return rc;
}

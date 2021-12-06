#include "RSUtilityFunctions.h"
#include <stdlib.h>
#include <time.h>

namespace Tool
{
    int Align(int _value, int _alignment)
    {
        return (_value + (_alignment - 1)) & ~(_alignment - 1);
    }

    float RandomVariance(float median, float variance)
    {
        static bool hasInited = false;
        if (!hasInited)
        {
            srand((unsigned int)time(nullptr));
            hasInited = true;
        }

        float fUnitRandomValue = (float)rand() / (float)RAND_MAX;
        float fRange = variance * fUnitRandomValue;
        return median - variance + (2.0f * fRange);
    }
}

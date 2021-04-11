#pragma once

#include "ID_ExportMacro.h"

enum class INPUT_TYPE
{
    DIRECTINPUT,
    XINPUT,
    UNKNOWN
};

class INPUTDEVICE_EXPORT InputDeviceBase
{
public:
    InputDeviceBase();

    virtual INPUT_TYPE GetInputType() = 0;

private:
    INPUT_TYPE mInputType;
};


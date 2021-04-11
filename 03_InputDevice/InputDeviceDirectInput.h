#pragma once
#include "InputDeviceBase.h"

class INPUTDEVICE_EXPORT InputDeviceDirectInput :
    public InputDeviceBase
{
public:
    InputDeviceDirectInput();

    virtual INPUT_TYPE GetInputType();
};


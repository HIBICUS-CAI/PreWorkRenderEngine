#pragma once

#include "ID_BasicMacro.h"
#include <dinput.h>
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

    LPDIRECTINPUTDEVICE8 GetDIDeviceHandle();

    DWORD GetXIDeviceHandle();

    LPDIRECTINPUTDEVICE8 mDIDeviceHandle;

private:
    DWORD mXIDeviceHandle;
};


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

enum class INPUT_DEVICE_TYPE
{
    KEYBOARD,
    MOUSE,
    GAMEPAD
};

struct DIKEYBOARDSTATUSMINE
{
    UCHAR Status[MAX_KEYBOARDS_KEY_NUM];
};

class INPUTDEVICE_EXPORT InputDeviceBase
{
public:
    InputDeviceBase(INPUT_DEVICE_TYPE deviceType);

    virtual ~InputDeviceBase();

    virtual INPUT_TYPE GetInputType() = 0;

    INPUT_DEVICE_TYPE GetInputDeviceType();

    LPDIRECTINPUTDEVICE8 GetDIDeviceHandle();

    DWORD GetXIDeviceHandle();

    virtual HRESULT PollDeviceStatus() = 0;

    virtual const LPVOID GetDeviceStatus() = 0;

    LPDIRECTINPUTDEVICE8 mDIDeviceHandle;

private:
    DWORD mXIDeviceHandle;

    INPUT_DEVICE_TYPE mDeviceType;
};


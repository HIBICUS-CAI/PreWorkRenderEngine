#pragma once
#include "InputDeviceBase.h"

class INPUTDEVICE_EXPORT InputDeviceDirectInput :
    public InputDeviceBase
{
public:
    InputDeviceDirectInput(INPUT_DEVICE_TYPE deviceType);

    ~InputDeviceDirectInput();

    virtual INPUT_TYPE GetInputType();

    virtual HRESULT PollDeviceStatus();

    virtual const LPVOID GetDeviceStatus();

    virtual const bool IsKeyBeingPushed(UINT keyCode);
    virtual const LONG GetXPositionOffset();
    virtual const LONG GetYPositionOffset();
    virtual const LONG GetZPositionOffset();
    virtual const LONG GetXRotationOffset();
    virtual const LONG GetYRotationOffset();
    virtual const LONG GetZRotationOffset();

private:
    LPVOID mDeviceStatus;
    WORD mDeviceStatusSize;
};


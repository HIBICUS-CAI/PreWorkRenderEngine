#pragma once
#include "InputDeviceBase.h"
#include <Xinput.h>
#include <map>

class InputDeviceXInput :
    public InputDeviceBase
{
public:
    InputDeviceXInput(DWORD xiDeviceHandle);

    ~InputDeviceXInput();

    virtual INPUT_TYPE GetInputType();

    virtual HRESULT PollDeviceStatus();

    virtual const LPVOID GetDeviceStatus();

    virtual const bool IsKeyBeingPushed(UINT keyCode);
    virtual const bool HasKeyPushedInLastFrame(UINT keyCode);
    virtual const LONG GetXPositionOffset();
    virtual const LONG GetYPositionOffset();
    virtual const LONG GetZPositionOffset();
    virtual const LONG GetXRotationOffset();
    virtual const LONG GetYRotationOffset();
    virtual const LONG GetZRotationOffset();

private:
    XINPUT_STATE* mDeviceStatus;

    std::map<UINT, WORD> mXIKeyCodeToNorm;
};


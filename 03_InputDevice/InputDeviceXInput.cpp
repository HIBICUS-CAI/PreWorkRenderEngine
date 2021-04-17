#include "ID_Common.h"
#include "InputDeviceXInput.h"

InputDeviceXInput::InputDeviceXInput(DWORD xiDeviceHandle) :
    InputDeviceBase(INPUT_DEVICE_TYPE::GAMEPAD, xiDeviceHandle),
    mDeviceStatus(nullptr)
{
    if (xiDeviceHandle < 4 && xiDeviceHandle >= 0)
    {
        mDeviceStatus = new XINPUT_STATE();
    }
}

InputDeviceXInput::~InputDeviceXInput()
{
    if (mDeviceStatus)
    {
        delete mDeviceStatus;
    }
}

INPUT_TYPE InputDeviceXInput::GetInputType()
{
    return INPUT_TYPE::XINPUT;
}

HRESULT InputDeviceXInput::PollDeviceStatus()
{
    if (!mDeviceStatus)
    {
        return E_FAIL;
    }

    DWORD result = XInputGetState(
        GetXIDeviceHandle(), mDeviceStatus);

    if (result == ERROR_SUCCESS)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

const LPVOID InputDeviceXInput::GetDeviceStatus()
{
    return (LPVOID)mDeviceStatus;
}

const bool InputDeviceXInput::IsKeyBeingPushed(UINT keyCode)
{
    //---------
    return false;
}

const bool InputDeviceXInput::HasKeyPushedInLastFrame(
    UINT keyCode)
{
    //---------
    return false;
}

const LONG InputDeviceXInput::GetXPositionOffset()
{
    //---------
    return 0;
}

const LONG InputDeviceXInput::GetYPositionOffset()
{
    //---------
    return 0;
}

const LONG InputDeviceXInput::GetZPositionOffset()
{
    //---------
    return 0;
}

const LONG InputDeviceXInput::GetXRotationOffset()
{
    //---------
    return 0;
}

const LONG InputDeviceXInput::GetYRotationOffset()
{
    //---------
    return 0;
}

const LONG InputDeviceXInput::GetZRotationOffset()
{
    //---------
    return 0;
}

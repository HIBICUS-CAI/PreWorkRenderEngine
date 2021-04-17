#include "ID_Common.h"
#include "InputDeviceXInput.h"

#define INPUT_DEADZONE (0.24f * FLOAT(0x7FFF))

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
    if (!mDeviceStatus)
    {
        return 0;
    }

    SHORT xOffset = mDeviceStatus->Gamepad.sThumbLX;
    if (xOffset<INPUT_DEADZONE && xOffset>-INPUT_DEADZONE)
    {
        xOffset = 0;
    }

    return (LONG)((FLOAT)xOffset / (FLOAT)(0x7FFF) * 1000.f);
}

const LONG InputDeviceXInput::GetYPositionOffset()
{
    if (!mDeviceStatus)
    {
        return 0;
    }

    SHORT yOffset = mDeviceStatus->Gamepad.sThumbLY;
    if (yOffset<INPUT_DEADZONE && yOffset>-INPUT_DEADZONE)
    {
        yOffset = 0;
    }

    return -(LONG)((FLOAT)yOffset / (FLOAT)(0x7FFF) * 1000.f);
}

const LONG InputDeviceXInput::GetZPositionOffset()
{
    if (!mDeviceStatus)
    {
        return 0;
    }

    return (LONG)((FLOAT)mDeviceStatus->Gamepad.bLeftTrigger /
        (FLOAT)(255) * 1000.f);
}

const LONG InputDeviceXInput::GetXRotationOffset()
{
    if (!mDeviceStatus)
    {
        return 0;
    }

    SHORT xOffset = mDeviceStatus->Gamepad.sThumbRX;
    if (xOffset<INPUT_DEADZONE && xOffset>-INPUT_DEADZONE)
    {
        xOffset = 0;
    }

    return (LONG)((FLOAT)xOffset / (FLOAT)(0x7FFF) * 1000.f);
}

const LONG InputDeviceXInput::GetYRotationOffset()
{
    if (!mDeviceStatus)
    {
        return 0;
    }

    SHORT yOffset = mDeviceStatus->Gamepad.sThumbRY;
    if (yOffset<INPUT_DEADZONE && yOffset>-INPUT_DEADZONE)
    {
        yOffset = 0;
    }

    return -(LONG)((FLOAT)yOffset / (FLOAT)(0x7FFF) * 1000.f);
}

const LONG InputDeviceXInput::GetZRotationOffset()
{
    if (!mDeviceStatus)
    {
        return 0;
    }

    return (LONG)((FLOAT)mDeviceStatus->Gamepad.bRightTrigger /
        (FLOAT)(255) * 1000.f);
}

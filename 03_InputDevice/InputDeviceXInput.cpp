#include "ID_Common.h"
#include "InputDeviceXInput.h"

InputDeviceXInput::InputDeviceXInput(DWORD xiDeviceHandle) :
    InputDeviceBase(INPUT_DEVICE_TYPE::GAMEPAD, xiDeviceHandle)
{

}

InputDeviceXInput::~InputDeviceXInput()
{

}

INPUT_TYPE InputDeviceXInput::GetInputType()
{
    return INPUT_TYPE::XINPUT;
}

HRESULT InputDeviceXInput::PollDeviceStatus()
{
    //---------
    return S_OK;
}

const LPVOID InputDeviceXInput::GetDeviceStatus()
{
    //---------
    return nullptr;
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

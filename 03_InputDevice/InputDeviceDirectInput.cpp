#include "ID_Common.h"
#include "InputDeviceDirectInput.h"

InputDeviceDirectInput::InputDeviceDirectInput(
    INPUT_DEVICE_TYPE deviceType) :
    InputDeviceBase(deviceType),
    mDeviceStatus(nullptr),
    mDeviceStatusSize(0)
{
    switch (deviceType)
    {
    case INPUT_DEVICE_TYPE::KEYBOARD:
        mDeviceStatus = new DIKEYBOARDSTATUSMINE();
        mDeviceStatusSize = sizeof(DIKEYBOARDSTATUSMINE);
        break;
    case INPUT_DEVICE_TYPE::MOUSE:
        mDeviceStatus = new DIMOUSESTATE2();
        mDeviceStatusSize = sizeof(DIMOUSESTATE2);
        break;
    case INPUT_DEVICE_TYPE::GAMEPAD:
        break;
    default:
        break;
    }
}

InputDeviceDirectInput::~InputDeviceDirectInput()
{
    if (mDeviceStatus)
    {
        delete mDeviceStatus;
    }

    if (mDIDeviceHandle)
    {
        mDIDeviceHandle->Unacquire();
        mDIDeviceHandle->Release();
    }
}

INPUT_TYPE InputDeviceDirectInput::GetInputType()
{
    return INPUT_TYPE::DIRECTINPUT;
}

const LPVOID InputDeviceDirectInput::GetDeviceStatus()
{
    return mDeviceStatus;
}

HRESULT InputDeviceDirectInput::PollDeviceStatus()
{
    HRESULT hr = S_OK;
    hr = mDIDeviceHandle->Poll();
    if (FAILED(hr))
    {
        hr = mDIDeviceHandle->Acquire();
        while (hr == DIERR_INPUTLOST)
        {
            hr = mDIDeviceHandle->Acquire();
        }

        return S_OK;
    }

    hr = mDIDeviceHandle->GetDeviceState(
        mDeviceStatusSize, mDeviceStatus);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

const bool InputDeviceDirectInput::IsKeyBeingPushed(UINT keyCode)
{
    switch (GetInputDeviceType())
    {
    case INPUT_DEVICE_TYPE::KEYBOARD:
        if (keyCode < 0x01 || keyCode > 0xED)
        {
            return false;
        }
        else
        {
            DIKEYBOARDSTATUSMINE* status =
                (DIKEYBOARDSTATUSMINE*)mDeviceStatus;
            if (status->Status[keyCode] & 0x80)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    case INPUT_DEVICE_TYPE::MOUSE:
        if (keyCode < MOUSE_BTN_OFFSET ||
            keyCode > MOUSE_BTN_OFFSET + 7)
        {
            return false;
        }
        else
        {
            DIMOUSESTATE2* status =
                (DIMOUSESTATE2*)mDeviceStatus;
            if (status->
                rgbButtons[keyCode - MOUSE_BTN_OFFSET] & 0x80)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    case INPUT_DEVICE_TYPE::GAMEPAD:
        break;
    default:
        break;
    }
    //--------------------
    return false;
}

const LONG InputDeviceDirectInput::GetXPositionOffset()
{
    LONG offsetX = 0;
    switch (GetInputDeviceType())
    {
    case INPUT_DEVICE_TYPE::MOUSE:
    {
        DIMOUSESTATE2* status =
            (DIMOUSESTATE2*)mDeviceStatus;
        offsetX = status->lX;
        break;
    }
    case INPUT_DEVICE_TYPE::GAMEPAD:
    {
        break;
    }
    default:
        break;
    }

    return offsetX;
}

const LONG InputDeviceDirectInput::GetYPositionOffset()
{
    LONG offsetY = 0;
    switch (GetInputDeviceType())
    {
    case INPUT_DEVICE_TYPE::MOUSE:
    {
        DIMOUSESTATE2* status =
            (DIMOUSESTATE2*)mDeviceStatus;
        offsetY = status->lY;
        break;
    }
    case INPUT_DEVICE_TYPE::GAMEPAD:
    {
        break;
    }
    default:
        break;
    }

    return offsetY;
}

const LONG InputDeviceDirectInput::GetZPositionOffset()
{
    LONG offsetZ = 0;
    switch (GetInputDeviceType())
    {
    case INPUT_DEVICE_TYPE::MOUSE:
    {
        DIMOUSESTATE2* status =
            (DIMOUSESTATE2*)mDeviceStatus;
        offsetZ = status->lZ;
        break;
    }
    case INPUT_DEVICE_TYPE::GAMEPAD:
    {
        break;
    }
    default:
        break;
    }

    return offsetZ;
}

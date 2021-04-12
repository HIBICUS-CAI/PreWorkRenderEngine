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
        mDeviceStatus = new DIMOUSESTATE();
        mDeviceStatusSize = sizeof(DIMOUSESTATE);
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
        if (keyCode < 0x01 || keyCode>0xED)
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
        break;
    case INPUT_DEVICE_TYPE::GAMEPAD:
        break;
    default:
        break;
    }
    //--------------------
    return false;
}

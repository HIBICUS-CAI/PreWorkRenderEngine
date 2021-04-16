#include "ID_Common.h"
#include "InputDeviceDirectInput.h"

InputDeviceDirectInput::InputDeviceDirectInput(
    INPUT_DEVICE_TYPE deviceType) :
    InputDeviceBase(deviceType),
    mDeviceStatus(nullptr),
    mDeviceStatusSize(0)
{
    for (int i = 0; i < sizeof(mButtonsStatusBeforeThisPoll); i++)
    {
        mButtonsStatusBeforeThisPoll[i] = false;
    }

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
        mDeviceStatus = new DIJOYSTATE2();
        mDeviceStatusSize = sizeof(DIJOYSTATE2);
        for (int i = 0; i < 4; i++)
        {
            DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
            status->rgdwPOV[i] = 1;
        }
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

    // store this status to backup before being updated
    switch (GetInputDeviceType())
    {
    case INPUT_DEVICE_TYPE::KEYBOARD:
        for (UINT i = KB_ESCAPE;
            i < KB_MEDIASELECT + KB_ESCAPE;
            i++)
        {
            DIKEYBOARDSTATUSMINE* status =
                (DIKEYBOARDSTATUSMINE*)mDeviceStatus;
            if (status->Status[i] & 0x80)
            {
                mButtonsStatusBeforeThisPoll[i] = true;
            }
            else
            {
                mButtonsStatusBeforeThisPoll[i] = false;
            }
        }
        break;
    case INPUT_DEVICE_TYPE::MOUSE:
        for (UINT i = 0x00; i < 0x08; i++)
        {
            DIMOUSESTATE2* status =
                (DIMOUSESTATE2*)mDeviceStatus;
            if (status->rgbButtons[i] & 0x80)
            {
                mButtonsStatusBeforeThisPoll[i + MOUSE_BTN_OFFSET]
                    = true;
            }
            else
            {
                mButtonsStatusBeforeThisPoll[i + MOUSE_BTN_OFFSET]
                    = false;
            }
        }
        break;
    case INPUT_DEVICE_TYPE::GAMEPAD:
        for (UINT i = 0x00; i < 0x14; i++)
        {
            DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
            if (i < 0x0C)
            {
                if (status->rgbButtons[i] & 0x80)
                {
                    mButtonsStatusBeforeThisPoll
                        [i + GAMEPAD_BTN_OFFSET] = true;
                }
                else
                {
                    mButtonsStatusBeforeThisPoll
                        [i + GAMEPAD_BTN_OFFSET] = false;
                }
            }
            else
            {
                for (UINT i = 0x00; i < 0x08; i++)
                {
                    mButtonsStatusBeforeThisPoll
                        [i + GP_UPDIRBTN] = false;
                }
                if (status->rgdwPOV[0] == 1)
                {
                    break;
                }
                UINT dirOffset = status->rgdwPOV[0] / 4500;
                if (dirOffset > 7)
                {
                    break;
                }
                else
                {
                    mButtonsStatusBeforeThisPoll
                        [dirOffset + GP_UPDIRBTN] = true;
                }
            }
        }
        break;
    default:
        break;
    }

    hr = mDIDeviceHandle->GetDeviceState(
        mDeviceStatusSize, mDeviceStatus);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

const bool InputDeviceDirectInput::HasKeyPushedInLastFrame(
    UINT keyCode)
{
    return mButtonsStatusBeforeThisPoll[keyCode];
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
            keyCode > MOUSE_BTN_OFFSET + 0x07)
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
        if (keyCode < GAMEPAD_BTN_OFFSET ||
            keyCode > GAMEPAD_BTN_OFFSET + 0x13)
        {
            return false;
        }
        else
        {
            DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
            if (keyCode < GP_UPDIRBTN)
            {
                if (status->
                    rgbButtons[keyCode - GAMEPAD_BTN_OFFSET] &
                    0x80)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                bool result = false;

                switch (keyCode)
                {
                case GP_UPDIRBTN:
                    if (status->rgdwPOV[0] == 0)
                    {
                        result = true;
                    }
                    break;
                case GP_UPRIGHTDIRBTN:
                    if (status->rgdwPOV[0] == 4500)
                    {
                        result = true;
                    }
                    break;
                case GP_RIGHTDIRBTN:
                    if (status->rgdwPOV[0] == 9000)
                    {
                        result = true;
                    }
                    break;
                case GP_DOWNRIGHTDIRBTN:
                    if (status->rgdwPOV[0] == 13500)
                    {
                        result = true;
                    }
                    break;
                case GP_DOWNDIRBTN:
                    if (status->rgdwPOV[0] == 18000)
                    {
                        result = true;
                    }
                    break;
                case GP_DOWNLEFTDIRBTN:
                    if (status->rgdwPOV[0] == 22500)
                    {
                        result = true;
                    }
                    break;
                case GP_LEFTDIRBTN:
                    if (status->rgdwPOV[0] == 27000)
                    {
                        result = true;
                    }
                    break;
                case GP_UPLEFTDIRBTN:
                    if (status->rgdwPOV[0] == 31500)
                    {
                        result = true;
                    }
                    break;
                default:
                    break;
                }

                return result;
            }
        }
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
        DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
        offsetX = status->lX;
        if (offsetX <= 100 && offsetX >= -100)
        {
            offsetX = 0;
        }
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
        DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
        offsetY = status->lY;
        if (offsetY <= 100 && offsetY >= -100)
        {
            offsetY = 0;
        }
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
        DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
        offsetZ = status->lZ;
        if (offsetZ <= 100 && offsetZ >= -100)
        {
            offsetZ = 0;
        }
        break;
    }
    default:
        break;
    }

    return offsetZ;
}

const LONG InputDeviceDirectInput::GetXRotationOffset()
{
    LONG offsetX = 0;

    if (GetInputDeviceType() != INPUT_DEVICE_TYPE::GAMEPAD)
    {
        return offsetX;
    }

    DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
    offsetX = (status->lRx + 1000) / 2;

    return offsetX;
}

const LONG InputDeviceDirectInput::GetYRotationOffset()
{
    LONG offsetY = 0;

    if (GetInputDeviceType() != INPUT_DEVICE_TYPE::GAMEPAD)
    {
        return offsetY;
    }

    DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
    offsetY = (status->lRy + 1000) / 2;

    return offsetY;
}

const LONG InputDeviceDirectInput::GetZRotationOffset()
{
    LONG offsetZ = 0;

    if (GetInputDeviceType() != INPUT_DEVICE_TYPE::GAMEPAD)
    {
        return offsetZ;
    }

    DIJOYSTATE2* status = (DIJOYSTATE2*)mDeviceStatus;
    offsetZ = status->lRz;
    if (offsetZ <= 100 && offsetZ >= -100)
    {
        offsetZ = 0;
    }

    return offsetZ;
}

#include "ID_Common.h"
#include "InputDeviceXInput.h"

#define INPUT_DEADZONE (0.24f * FLOAT(0x7FFF))

InputDeviceXInput::InputDeviceXInput(DWORD xiDeviceHandle) :
    InputDeviceBase(INPUT_DEVICE_TYPE::GAMEPAD, xiDeviceHandle),
    mDeviceStatus(nullptr), mDeviceStatusBeforeThisPoll(nullptr)
{
    if (xiDeviceHandle < 4 && xiDeviceHandle >= 0)
    {
        mDeviceStatus = new XINPUT_STATE();
        mDeviceStatusBeforeThisPoll = new XINPUT_STATE();
    }

    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_LEFTBTN,
            XINPUT_GAMEPAD_X));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_BOTTOMBTN,
            XINPUT_GAMEPAD_A));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_RIGHTBTN,
            XINPUT_GAMEPAD_B));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_TOPBTN,
            XINPUT_GAMEPAD_Y));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_LEFTFORESHDBTN,
            XINPUT_GAMEPAD_LEFT_SHOULDER));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_RIGHTFORESHDBTN,
            XINPUT_GAMEPAD_RIGHT_SHOULDER));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_LEFTMENUBTN,
            XINPUT_GAMEPAD_BACK));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_RIGHTMENUBTN,
            XINPUT_GAMEPAD_START));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_LEFTSTICKBTN,
            XINPUT_GAMEPAD_LEFT_THUMB));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_RIGHTSTICKBTN,
            XINPUT_GAMEPAD_RIGHT_THUMB));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_UPDIRBTN,
            XINPUT_GAMEPAD_DPAD_UP));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_RIGHTDIRBTN,
            XINPUT_GAMEPAD_DPAD_RIGHT));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_DOWNDIRBTN,
            XINPUT_GAMEPAD_DPAD_DOWN));
    mXIKeyCodeToNorm.insert(
        std::make_pair(
            GP_LEFTDIRBTN,
            XINPUT_GAMEPAD_DPAD_LEFT));
}

InputDeviceXInput::~InputDeviceXInput()
{
    if (mDeviceStatus)
    {
        delete mDeviceStatus;
    }
    if (mDeviceStatusBeforeThisPoll)
    {
        delete mDeviceStatusBeforeThisPoll;
    }
}

INPUT_TYPE InputDeviceXInput::GetInputType()
{
    return INPUT_TYPE::XINPUT;
}

HRESULT InputDeviceXInput::PollDeviceStatus()
{
    if (mDeviceStatus && mDeviceStatusBeforeThisPoll)
    {
        *mDeviceStatusBeforeThisPoll = *mDeviceStatus;
    }
    else
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
    if (!mDeviceStatus)
    {
        return false;
    }

    switch (keyCode)
    {
    case GP_LEFTBACKSHDBTN:
        return (GetZPositionOffset() > 0) ? true : false;

    case GP_RIGHTBACKSHDBTN:
        return (GetZRotationOffset() > 0) ? true : false;

    case GP_UPRIGHTDIRBTN:
        return
            ((mDeviceStatus->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_UP) &&
                (mDeviceStatus->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_RIGHT)) ? true : false;

    case GP_DOWNRIGHTDIRBTN:
        return
            ((mDeviceStatus->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_DOWN) &&
                (mDeviceStatus->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_RIGHT)) ? true : false;

    case GP_DOWNLEFTDIRBTN:
        return
            ((mDeviceStatus->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_DOWN) &&
                (mDeviceStatus->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_LEFT)) ? true : false;

    case GP_UPLEFTDIRBTN:
        return
            ((mDeviceStatus->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_UP) &&
                (mDeviceStatus->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_LEFT)) ? true : false;

    default:
        break;
    }

    WORD xiKeyCode;
    bool hasFound = false;
    for (auto it = mXIKeyCodeToNorm.begin();
        it != mXIKeyCodeToNorm.end(); it++)
    {
        if (it->first == keyCode)
        {
            xiKeyCode = it->second;
            hasFound = true;
            break;
        }
    }

    if (!hasFound)
    {
        return false;
    }
    else
    {
        return (mDeviceStatus->Gamepad.wButtons & xiKeyCode) ?
            true : false;
    }
}

const bool InputDeviceXInput::HasKeyPushedInLastFrame(
    UINT keyCode)
{
    if (!mDeviceStatusBeforeThisPoll)
    {
        return false;
    }

    switch (keyCode)
    {
    case GP_LEFTBACKSHDBTN:
        return (mDeviceStatusBeforeThisPoll->
            Gamepad.bLeftTrigger > 0) ? true : false;

    case GP_RIGHTBACKSHDBTN:
        return (mDeviceStatusBeforeThisPoll->
            Gamepad.bRightTrigger > 0) ? true : false;

    case GP_UPRIGHTDIRBTN:
        return
            ((mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_UP) &&
                (mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_RIGHT)) ? true : false;

    case GP_DOWNRIGHTDIRBTN:
        return
            ((mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_DOWN) &&
                (mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_RIGHT)) ? true : false;

    case GP_DOWNLEFTDIRBTN:
        return
            ((mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_DOWN) &&
                (mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_LEFT)) ? true : false;

    case GP_UPLEFTDIRBTN:
        return
            ((mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                XINPUT_GAMEPAD_DPAD_UP) &&
                (mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
                    XINPUT_GAMEPAD_DPAD_LEFT)) ? true : false;

    default:
        break;
    }

    WORD xiKeyCode;
    bool hasFound = false;
    for (auto it = mXIKeyCodeToNorm.begin();
        it != mXIKeyCodeToNorm.end(); it++)
    {
        if (it->first == keyCode)
        {
            xiKeyCode = it->second;
            hasFound = true;
            break;
        }
    }

    if (!hasFound)
    {
        return false;
    }
    else
    {
        return (mDeviceStatusBeforeThisPoll->Gamepad.wButtons &
            xiKeyCode) ?
            true : false;
    }
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

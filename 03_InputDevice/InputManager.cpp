#include "ID_Common.h"
#include "InputManager.h"
#include "InputDeviceDirectInput.h"
#include "InputDeviceXInput.h"

LPDIRECTINPUT8 InputManager::mpDirectInput = nullptr;

InputManager::InputManager(WindowWIN32* wnd) :
    mhInstance(wnd->GetWndInstance()),
    mhWindow(wnd->GetWndHandle()),
    mpKeyBoard(nullptr), mpMouse(nullptr)
{
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        mpGamePads[i] = nullptr;
    }
}

HRESULT InputManager::CreateDirectInputMain()
{
    return DirectInput8Create(
        GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
        IID_IDirectInput8, (VOID**)&mpDirectInput, nullptr);
}

void InputManager::CloseDirectInputMain()
{
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        if (mpGamePads[i] &&
            (mpGamePads[i]->GetInputType() ==
                INPUT_TYPE::DIRECTINPUT))
        {
            mpGamePads[i]->GetDIDeviceHandle()->Release();
        }
    }
    if (mpMouse)
    {
        mpMouse->mDIDeviceHandle->Release();
    }
    if (mpKeyBoard)
    {
        mpKeyBoard->mDIDeviceHandle->Release();
    }

    if (mpDirectInput)
    {
        mpDirectInput->Release();
    }
}

void InputManager::EnumAllInputDevices()
{
    if (!mpDirectInput)
    {
        return;
    }

    HRESULT hr = S_OK;

    // keyboard
    mpKeyBoard = new InputDeviceDirectInput(
        INPUT_DEVICE_TYPE::KEYBOARD);
    hr = mpDirectInput->CreateDevice(GUID_SysKeyboard,
        &(mpKeyBoard->mDIDeviceHandle), nullptr);
    if (FAILED(hr))
    {
        delete mpKeyBoard;
        mpKeyBoard = nullptr;
    }
    else
    {
        hr = mpKeyBoard->mDIDeviceHandle->SetCooperativeLevel(
            mhWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        if (FAILED(hr))
        {
            delete mpKeyBoard;
            mpKeyBoard = nullptr;
        }
        else
        {
            hr = mpKeyBoard->mDIDeviceHandle->SetDataFormat(
                &c_dfDIKeyboard);
            if (FAILED(hr))
            {
                delete mpKeyBoard;
                mpKeyBoard = nullptr;
            }
        }
    }

    // mouse
    mpMouse = new InputDeviceDirectInput(
        INPUT_DEVICE_TYPE::MOUSE);
    hr = mpDirectInput->CreateDevice(GUID_SysMouse,
        &(mpMouse->mDIDeviceHandle), nullptr);
    if (FAILED(hr))
    {
        delete mpMouse;
        mpMouse = nullptr;
    }
    else
    {
        hr = mpMouse->mDIDeviceHandle->SetCooperativeLevel(
            mhWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        if (FAILED(hr))
        {
            delete mpMouse;
            mpMouse = nullptr;
        }
        else
        {
            hr = mpMouse->mDIDeviceHandle->SetDataFormat(
                &c_dfDIMouse2);
            if (FAILED(hr))
            {
                delete mpMouse;
                mpMouse = nullptr;
            }
        }
    }

    // gamepads
    // 1 - xinput
    // 2 - directinput
    hr = mpDirectInput->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        EnumGamePadCallBack,
        &mpGamePads, DIEDFL_ATTACHEDONLY
    );
    if (FAILED(hr))
    {
        return;
    }
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        if (mpGamePads[i] &&
            mpGamePads[i]->GetInputType() ==
            INPUT_TYPE::DIRECTINPUT)
        {
            if (
                FAILED(hr =
                    mpGamePads[i]->mDIDeviceHandle->
                    SetDataFormat(&c_dfDIJoystick2)) ||
                FAILED(hr =
                    mpGamePads[i]->mDIDeviceHandle->
                    SetCooperativeLevel(mhWindow,
                        DISCL_EXCLUSIVE | DISCL_FOREGROUND)) ||
                FAILED(hr =
                    mpGamePads[i]->mDIDeviceHandle->
                    EnumObjects(DIEnumGamePadObjCallBack,
                        &mpGamePads[i], DIDFT_ALL)))
            {
                delete mpGamePads[i];
                for (int j = i; j < MAX_INPUTDEVICE_NUM - 1; j++)
                {
                    mpGamePads[j] = mpGamePads[j + 1];
                }
                mpGamePads[MAX_INPUTDEVICE_NUM - 1] = nullptr;
                --i;
            }
        }
    }
}

BOOL CALLBACK InputManager::EnumGamePadCallBack(
    const DIDEVICEINSTANCE* pdiDeviceInst,
    VOID* pContext)
{
    auto pGamePads =
        reinterpret_cast<InputDeviceBase**>(pContext);

    int index = -1;
    bool inXI = false;
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        if (!pGamePads[i])
        {
            static int xiIndex = 0;
            XINPUT_STATE xs;
            DWORD xResult = XInputGetState(i, &xs);
            if (xResult == ERROR_SUCCESS)
            {
                pGamePads[i] = new InputDeviceXInput(xiIndex);
                ++xiIndex;
                index = i;
                inXI = true;
                break;
            }

            pGamePads[i] =
                new InputDeviceDirectInput(
                    INPUT_DEVICE_TYPE::GAMEPAD);
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return DIENUM_STOP;
    }

    if (!inXI)
    {
        mpDirectInput->CreateDevice(
            pdiDeviceInst->guidInstance,
            &(pGamePads[index]->mDIDeviceHandle), nullptr);
    }

    return DIENUM_CONTINUE;
}

BOOL CALLBACK InputManager::DIEnumGamePadObjCallBack(
    const DIDEVICEOBJECTINSTANCE* pdiDeviceObjInst,
    VOID* pContext)
{
    auto pGamePad =
        reinterpret_cast<InputDeviceBase**>(pContext);

    if (pdiDeviceObjInst->dwType & DIDFT_AXIS)
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = pdiDeviceObjInst->dwType;
        diprg.lMin = -1000;
        diprg.lMax = +1000;

        // Set the range for the axis
        if (FAILED((*pGamePad)->mDIDeviceHandle->
            SetProperty(DIPROP_RANGE, &diprg.diph)))
        {
            return DIENUM_STOP;
        }
    }

    return DIENUM_CONTINUE;
}

InputDeviceBase* InputManager::GetKeyBoard()
{
    return mpKeyBoard;
}

InputDeviceBase* InputManager::GetMouse()
{
    return mpMouse;
}

InputDeviceBase* InputManager::GetGamePadByOffset(int offset)
{
    if (offset >= MAX_INPUTDEVICE_NUM)
    {
        return nullptr;
    }

    return mpGamePads[offset];
}

HRESULT InputManager::PollAllInputDevices()
{
    HRESULT hr = S_OK;
    HRESULT fhr = S_OK;

    if (mpKeyBoard)
    {
        hr = mpKeyBoard->PollDeviceStatus();
        if (FAILED(hr))
        {
            fhr = hr;
        }
    }
    if (mpMouse)
    {
        hr = mpMouse->PollDeviceStatus();
        if (FAILED(hr))
        {
            fhr = hr;
        }
    }
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        if (mpGamePads[i])
        {
            hr = mpGamePads[i]->PollDeviceStatus();
        }
        if (FAILED(hr))
        {
            fhr = hr;
        }
    }

    return fhr;
}

const bool InputManager::IsThisKeyBeingPushedInSingle(
    UINT keyCode)
{
    bool keyboard = false;
    bool mouse = false;
    bool gamepad = false;
    if (mpKeyBoard)
    {
        keyboard = mpKeyBoard->IsKeyBeingPushed(keyCode);
    }
    if (mpMouse)
    {
        mouse = mpMouse->IsKeyBeingPushed(keyCode);
    }
    if (mpGamePads[0])
    {
        gamepad = mpGamePads[0]->IsKeyBeingPushed(keyCode);
    }

    return (keyboard || mouse || gamepad);
}

const bool InputManager::IsThisKeyHasBeenPushedInSingle(
    UINT keyCode)
{
    bool confirm1 = IsThisKeyBeingPushedInSingle(keyCode);

    if (!confirm1)
    {
        return confirm1;
    }

    bool kconfirm2 = false;
    bool mconfirm2 = false;
    bool gconfirm2 = false;

    if (mpKeyBoard)
    {
        kconfirm2 = mpKeyBoard->HasKeyPushedInLastFrame(keyCode);
    }
    if (mpMouse)
    {
        mconfirm2 = mpMouse->HasKeyPushedInLastFrame(keyCode);
    }
    if (mpGamePads[0])
    {
        gconfirm2 = mpGamePads[0]->
            HasKeyPushedInLastFrame(keyCode);
    }

    return (!(kconfirm2 || mconfirm2 || gconfirm2) && confirm1);
}

const MOUSE_OFFSET InputManager::GetMouseOffset()
{
    MOUSE_OFFSET mo;
    mo.x = mpMouse->GetXPositionOffset();
    mo.y = mpMouse->GetYPositionOffset();

    return mo;
}

const bool InputManager::IsMouseScrollingUp()
{
    return mpMouse->GetZPositionOffset() > 0;
}

const bool InputManager::IsMouseScrollingDown()
{
    return mpMouse->GetZPositionOffset() < 0;
}

const STICK_OFFSET
InputManager::GetGamePadLeftStickOffset(
    int gamepadOffset)
{
    STICK_OFFSET so;
    if (mpGamePads[gamepadOffset])
    {
        if (mpGamePads[gamepadOffset]->GetInputType() ==
            INPUT_TYPE::DIRECTINPUT)
        {
            so.x = mpGamePads[gamepadOffset]->
                GetXPositionOffset();
            so.y = mpGamePads[gamepadOffset]->
                GetYPositionOffset();
        }
        else
        {
            so.x = 0;
            so.y = 0;
        }
    }
    else
    {
        so.x = 0;
        so.y = 0;
    }

    return so;
}

const STICK_OFFSET
InputManager::GetGamePadRightStickOffset(
    int gamepadOffset)
{
    STICK_OFFSET so;
    if (mpGamePads[gamepadOffset])
    {
        if (mpGamePads[gamepadOffset]->GetInputType() ==
            INPUT_TYPE::DIRECTINPUT)
        {
            so.x = mpGamePads[gamepadOffset]->
                GetZPositionOffset();
            so.y = mpGamePads[gamepadOffset]->
                GetZRotationOffset();
        }
        else
        {
            so.x = 0;
            so.y = 0;
        }
    }
    else
    {
        so.x = 0;
        so.y = 0;
    }

    return so;
}

const BACKSHD_OFFSET
InputManager::GetGamePadLeftBackShdBtnOffset(
    int gamepadOffset)
{
    if (!mpGamePads[gamepadOffset])
    {
        return 0;
    }

    if (mpGamePads[gamepadOffset]->GetInputType() ==
        INPUT_TYPE::DIRECTINPUT)
    {
        return mpGamePads[gamepadOffset]->GetXRotationOffset();
    }
    else
    {
        return 0;
    }
}

const BACKSHD_OFFSET
InputManager::GetGamePadRightBackShdBtnOffset(
    int gamepadOffset)
{
    if (!mpGamePads[gamepadOffset])
    {
        return 0;
    }

    if (mpGamePads[gamepadOffset]->GetInputType() ==
        INPUT_TYPE::DIRECTINPUT)
    {
        return mpGamePads[gamepadOffset]->GetYRotationOffset();
    }
    else
    {
        return 0;
    }
}

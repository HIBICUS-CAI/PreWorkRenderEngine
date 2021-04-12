#include "ID_Common.h"
#include "InputManager.h"
#include "InputDeviceDirectInput.h"

InputManager::InputManager(WindowWIN32* wnd) :
    mhInstance(wnd->GetWndInstance()),
    mhWindow(wnd->GetWndHandle()), mpDirectInput(nullptr),
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
    LPDIRECTINPUTDEVICE8 tempDevice = nullptr;
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

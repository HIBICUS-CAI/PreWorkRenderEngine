#include "ID_Common.h"
#include "InputManager.h"

InputManager::InputManager(HINSTANCE hInstance) :
    mhInstance(hInstance), mpDirectInput(nullptr)
{
    for (int i = 0; i < MAX_INPUTDEVICE_NUM; i++)
    {
        mpKeyBoards[i] = nullptr;
        mpMice[i] = nullptr;
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

    // keyboard

    // mice

    // gamepads
}

InputDeviceBase* InputManager::GetKeyBoardByOffset(int offset)
{
    if (offset >= MAX_INPUTDEVICE_NUM)
    {
        return nullptr;
    }

    return mpKeyBoards[offset];
}

InputDeviceBase* InputManager::GetMouseByOffset(int offset)
{
    if (offset >= MAX_INPUTDEVICE_NUM)
    {
        return nullptr;
    }

    return mpMice[offset];
}

InputDeviceBase* InputManager::GetGamePadByOffset(int offset)
{
    if (offset >= MAX_INPUTDEVICE_NUM)
    {
        return nullptr;
    }

    return mpGamePads[offset];
}

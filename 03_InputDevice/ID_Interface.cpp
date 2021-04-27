#include "ID_Common.h"
#include "ID_Interface.h"
#include "WM_Interface.h"

namespace InputInterface
{
    InputManager* g_InputManager = nullptr;

    bool StartUp()
    {
        g_InputManager = new InputManager(
            WindowInterface::GetWindowPtr());

        HRESULT hr = g_InputManager->CreateDirectInputMain();
        if (FAILED(hr))
        {
            delete g_InputManager;
            g_InputManager = nullptr;
            return false;
        }

        g_InputManager->EnumAllInputDevices();

        return true;
    }

    void CleanAndStop()
    {
        if (g_InputManager)
        {
            g_InputManager->CloseDirectInputMain();
            delete g_InputManager;
            g_InputManager = nullptr;
        }
    }

    InputManager* GetInputManagerPtr()
    {
        return g_InputManager;
    }

    bool PollDevices()
    {
        HRESULT hr = g_InputManager->PollAllInputDevices();
        if (FAILED(hr))
        {
            return false;
        }

        return true;
    }

    bool IsKeyDownInSingle(UINT keyCode)
    {
        return g_InputManager->IsThisKeyBeingPushedInSingle(
            keyCode);
    }
    bool IsKeyPushedInSingle(UINT keyCode)
    {
        return g_InputManager->IsThisKeyHasBeenPushedInSingle(
            keyCode);
    }
    STICK_OFFSET LeftStickOffset(int gamepadOffset)
    {
        return g_InputManager->GetGamePadLeftStickOffset(
            gamepadOffset);
    }
    STICK_OFFSET RightStickOffset(int gamepadOffset)
    {
        return g_InputManager->GetGamePadRightStickOffset(
            gamepadOffset);
    }
    BACKSHD_OFFSET LeftBackShdBtnOffset(int gamepadOffset)
    {
        return g_InputManager->GetGamePadLeftBackShdBtnOffset(
            gamepadOffset);
    }
    BACKSHD_OFFSET RightBackShdBtnOffset(int gamepadOffset)
    {
        return g_InputManager->GetGamePadRightBackShdBtnOffset(
            gamepadOffset);
    }
    MOUSE_OFFSET GetMouseOffset()
    {
        return g_InputManager->GetMouseOffset();
    }
    bool IsMouseScrollingUp()
    {
        return g_InputManager->IsMouseScrollingUp();
    }
    bool IsMouseScrollingDown()
    {
        return g_InputManager->IsMouseScrollingDown();
    }
}

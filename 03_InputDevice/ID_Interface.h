#pragma once

#include "ID_ExportMacro.h"
#include "ID_BasicMacro.h"
#include "InputManager.h"

namespace InputInterface
{
    bool INPUTDEVICE_EXPORT StartUp();

    void INPUTDEVICE_EXPORT CleanAndStop();

    InputManager INPUTDEVICE_EXPORT* GetInputManagerPtr();

    bool INPUTDEVICE_EXPORT PollDevices();

    bool INPUTDEVICE_EXPORT IsKeyDownInSingle(UINT keyCode);
    bool INPUTDEVICE_EXPORT IsKeyPushedInSingle(UINT keyCode);
    STICK_OFFSET INPUTDEVICE_EXPORT LeftStickOffset(
        int gamepadOffset = 0);
    STICK_OFFSET INPUTDEVICE_EXPORT RightStickOffset(
        int gamepadOffset = 0);
    BACKSHD_OFFSET INPUTDEVICE_EXPORT LeftBackShdBtnOffset(
        int gamepadOffset = 0);
    BACKSHD_OFFSET INPUTDEVICE_EXPORT RightBackShdBtnOffset(
        int gamepadOffset = 0);
    MOUSE_OFFSET INPUTDEVICE_EXPORT GetMouseOffset();
    bool INPUTDEVICE_EXPORT IsMouseScrollingUp();
    bool INPUTDEVICE_EXPORT IsMouseScrollingDown();
}

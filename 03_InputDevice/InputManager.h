#pragma once

#include "ID_BasicMacro.h"
#include <Windows.h>
#include <dinput.h>
#include <Xinput.h>
#include "WindowWIN32.h"
#include "InputDeviceBase.h"

typedef POINT MOUSE_OFFSET;
typedef POINT STICK_OFFSET;
typedef LONG BACKSHD_OFFSET;

class InputManager
{
public:
    InputManager(WindowWIN32* wnd);

    HRESULT CreateDirectInputMain();
    void CloseDirectInputMain();

    void EnumAllInputDevices();
    HRESULT PollAllInputDevices();

    const bool IsThisKeyBeingPushedInSingle(UINT keyCode);
    const bool IsThisKeyHasBeenPushedInSingle(UINT keyCode);
    const STICK_OFFSET GetGamePadLeftStickOffset(
        int gamepadOffset = 0);
    const STICK_OFFSET GetGamePadRightStickOffset(
        int gamepadOffset = 0);
    const BACKSHD_OFFSET GetGamePadLeftBackShdBtnOffset(
        int gamepadOffset = 0);
    const BACKSHD_OFFSET GetGamePadRightBackShdBtnOffset(
        int gamepadOffset = 0);
    const MOUSE_OFFSET GetMouseOffset();
    const bool IsMouseScrollingUp();
    const bool IsMouseScrollingDown();

    InputDeviceBase* GetKeyBoard();
    InputDeviceBase* GetMouse();
    InputDeviceBase* GetGamePadByOffset(int offset = 0);

private:
    static BOOL CALLBACK EnumGamePadCallBack(
        const DIDEVICEINSTANCE* pdiDeviceInst,
        VOID* pContext);
    static BOOL CALLBACK DIEnumGamePadObjCallBack(
        const DIDEVICEOBJECTINSTANCE* pdiDeviceObjInst,
        VOID* pContext);

    HINSTANCE mhInstance;
    HWND mhWindow;

    static LPDIRECTINPUT8 mpDirectInput;

    InputDeviceBase* mpKeyBoard;
    InputDeviceBase* mpMouse;
    InputDeviceBase* mpGamePads[MAX_INPUTDEVICE_NUM];
};


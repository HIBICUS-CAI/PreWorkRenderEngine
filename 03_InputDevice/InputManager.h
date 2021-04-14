#pragma once

#include "ID_BasicMacro.h"
#include <Windows.h>
#include <dinput.h>
#include <dinputd.h>
#include "WindowWIN32.h"
#include "ID_ExportMacro.h"
#include "InputDeviceBase.h"

typedef POINT MOUSE_OFFSET;

struct DI_ENUM_GAMEPAD_CONTEXT
{
    DIJOYCONFIG* pPreferredJoyCfg;
    bool bPreferredJoyCfgValid;
};

class INPUTDEVICE_EXPORT InputManager
{
public:
    InputManager(WindowWIN32* wnd);

    HRESULT CreateDirectInputMain();
    void CloseDirectInputMain();

    void EnumAllInputDevices();
    HRESULT PollAllInputDevices();

    const bool IsThisKeyBeingPushedInSingle(UINT keyCode);
    const MOUSE_OFFSET GetMouseOffset();
    const bool IsMouseScrollingUp();
    const bool IsMouseScrollingDown();

    InputDeviceBase* GetKeyBoard();
    InputDeviceBase* GetMouse();
    InputDeviceBase* GetGamePadByOffset(int offset = 0);

private:
    static BOOL CALLBACK DIEnumGamePadCallBack(
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


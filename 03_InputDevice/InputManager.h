#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <dinput.h>
#include "ID_ExportMacro.h"
#include "InputDeviceBase.h"

#define MAX_INPUTDEVICE_NUM 4

class INPUTDEVICE_EXPORT InputManager
{
public:
    InputManager(HINSTANCE hInstance);

    HRESULT CreateDirectInputMain();
    void CloseDirectInputMain();

    void EnumAllInputDevices();

    InputDeviceBase* GetKeyBoardByOffset(int offset = 0);
    InputDeviceBase* GetMouseByOffset(int offset = 0);
    InputDeviceBase* GetGamePadByOffset(int offset = 0);

private:
    HINSTANCE mhInstance;

    LPDIRECTINPUT8 mpDirectInput;

    InputDeviceBase* mpKeyBoards[MAX_INPUTDEVICE_NUM];
    InputDeviceBase* mpMice[MAX_INPUTDEVICE_NUM];
    InputDeviceBase* mpGamePads[MAX_INPUTDEVICE_NUM];
};


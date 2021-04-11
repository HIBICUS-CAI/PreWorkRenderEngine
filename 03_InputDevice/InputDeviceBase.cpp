#include "ID_Common.h"
#include "InputDeviceBase.h"

InputDeviceBase::InputDeviceBase(INPUT_DEVICE_TYPE deviceType) :
    mDIDeviceHandle(nullptr),
    mXIDeviceHandle(MAX_INPUTDEVICE_NUM),
    mDeviceType(deviceType)
{
    
}

InputDeviceBase::~InputDeviceBase()
{
    
}

LPDIRECTINPUTDEVICE8 InputDeviceBase::GetDIDeviceHandle()
{
    return mDIDeviceHandle;
}

DWORD InputDeviceBase::GetXIDeviceHandle()
{
    return mXIDeviceHandle;
}

INPUT_DEVICE_TYPE InputDeviceBase::GetInputDeviceType()
{
    return mDeviceType;
}

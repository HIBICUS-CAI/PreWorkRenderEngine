#include "ID_Common.h"
#include "InputDeviceBase.h"

InputDeviceBase::InputDeviceBase() :
    mDIDeviceHandle(nullptr), 
    mXIDeviceHandle(MAX_INPUTDEVICE_NUM)
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

#include "ID_Common.h"
#include "InputDeviceDirectInput.h"

InputDeviceDirectInput::InputDeviceDirectInput():
    InputDeviceBase()
{

}

INPUT_TYPE InputDeviceDirectInput::GetInputType()
{
    return INPUT_TYPE::DIRECTINPUT;
}

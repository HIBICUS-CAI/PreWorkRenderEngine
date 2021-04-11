#pragma once

#ifdef MY03INPUTDEVICE_EXPORTS
#define INPUTDEVICE_EXPORT __declspec(dllexport)
#else
#define INPUTDEVICE_EXPORT __declspec(dllimport)
#endif // MY03INPUTDEVICE_EXPORTS

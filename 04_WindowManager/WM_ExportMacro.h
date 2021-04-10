#pragma once

#ifdef MY04WINDOWMANAGER_EXPORTS
#define WINDOWMANAGER_EXPORT __declspec(dllexport)
#else
#define WINDOWMANAGER_EXPORT __declspec(dllimport)
#endif // MY04WINDOWMANAGER_EXPORTS

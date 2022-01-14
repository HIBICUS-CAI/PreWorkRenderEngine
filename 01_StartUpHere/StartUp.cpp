#include <Windows.h>
#include "WM_Interface.h"
#include "ID_Interface.h"
#include "RSRoot_DX11.h"

#include "Scene3D.h"

enum class SCENE_TYPE
{
    S3D,
    S2D,
    SHD2D
};

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    WindowInterface::StartUp();
    WindowInterface::CreateInitWindow("a test window",
#ifdef _DEBUG
        hInstance, iCmdShow, 1280, 720);
#else
        hInstance, iCmdShow, 1280, 720, true);
#endif // _DEBUG
    InputInterface::StartUp();
    HWND hwnd = WindowInterface::GetWindowPtr()->GetWndHandle();
    RSRoot_DX11* root = new RSRoot_DX11();
    if (!root->StartUp(hwnd)) { return -1; }

    SCENE_TYPE scene = SCENE_TYPE::S3D;
    switch (scene)
    {
    case SCENE_TYPE::S3D: if (!Run3DLoop(root)) { return -2; } break;
    case SCENE_TYPE::S2D: break;
    case SCENE_TYPE::SHD2D: break;
    default: return -2;
    }

    return 0;
}
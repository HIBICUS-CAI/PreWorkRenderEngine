#include <Windows.h>
#include "WM_Interface.h"
#include "ID_Interface.h"
#include "TempMesh.h"
#include "RSRoot_DX11.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    WindowInterface::StartUp();
    WindowInterface::CreateInitWindow("a test window",
        hInstance, iCmdShow);
    InputInterface::StartUp();

    TempMesh* mesh = new TempMesh();
    if (!mesh->Load("Dragon.FBX.meshdata", MESH_FILE_TYPE::BIN))
    {
        return -1;
    }

    RSRoot_DX11* root = new RSRoot_DX11();
    if (!root->StartUp(
        WindowInterface::GetWindowPtr()->GetWndHandle()))
    {
        return -2;
    }

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            InputInterface::PollDevices();

            if (InputInterface::IsKeyPushedInSingle(KB_ESCAPE))
            {
                PostQuitMessage(0);
            }
        }
    }

    root->CleanAndStop();
    delete root;
    delete mesh;

    return (int)msg.wParam;
}
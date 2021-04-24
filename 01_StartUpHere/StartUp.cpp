#include <Windows.h>
#include "WindowWIN32.h"
#include "tempD3d.h"
#include "tempMesh.h"
#include "InputManager.h"
//-----------------------------------------------
#include <stdio.h>
void tempShowMousePos(LONG x, LONG y, LONG z)
{
    char str[64] = "";
    sprintf_s(str, sizeof(str), "MOUSE:: X->%ld Y->%ld Z->%ld\n",
        x, y, z);
    OutputDebugString(str);
}
//-----------------------------------------------

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR szCmdLine,
    _In_ int iCmdShow
)
{
    WindowWIN32* w = new WindowWIN32();
    w->CreateMyWindow("a test window",
        hInstance, iCmdShow, false);

    //---------------------------------------------
    InputManager im(w);
    im.CreateDirectInputMain();
    im.EnumAllInputDevices();

    if (FAILED(TEMP::InitD3D11Device(w->GetWndHandle())))
    {
        return -1;
    }
#ifdef SHOW_CUBE
    if (FAILED(TEMP::PrepareCube(w->GetWndHandle())))
    {
        return -2;
    }
#endif // SHOW_CUBE

    //--------------------------------
    static TEMP::Mesh* testMesh = new TEMP::Mesh(
        TEMP::GetD3DDevicePointer());
    if (!testMesh->Load("spider.fbx"))
    {
        return -3;
    }
    TEMP::PrepareMeshD3D(TEMP::GetD3DDevicePointer(),
        w->GetWndHandle());
    //--------------------------------

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
            im.PollAllInputDevices();
            //TEMP::Render();

            TEMP::TempMeshBegin();
            testMesh->Draw(TEMP::GetD3DDevContPointer());
            TEMP::TempMeshEnd();
            //-------------------------
            if (im.IsThisKeyBeingPushedInSingle(KB_ESCAPE))
            {
                PostQuitMessage(0);
            }

            if (im.IsThisKeyBeingPushedInSingle(GP_LEFTSTICKBTN))
            {
                tempShowMousePos(1, 0, 0);
            }
            if (im.IsThisKeyHasBeenPushedInSingle(GP_LEFTDIRBTN))
            {
                tempShowMousePos(0, 0, 0);
            }

            TEMP::setLightOffsetX(
                (FLOAT)im.GetMouseOffset().x);
            TEMP::setLightOffsetY(
                (FLOAT)im.GetMouseOffset().y);
        }
    }

    testMesh->DeleteThisMesh();
    delete testMesh;

    TEMP::CleanupDevice();
    //--------------------------------------
    im.CloseDirectInputMain();
    //--------------------------------------
    delete w;

    return (int)msg.wParam;
}
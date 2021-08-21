#include <Windows.h>
#include "WM_Interface.h"
#include "ID_Interface.h"
#include "tempD3d.h"
#include "tempMesh.h"
#include "tempMyMesh.h"
#include "ShadowTex.h"
#include "SsaoTexs.h"
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

//#define LIGHT_BY_KEY

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

    if (FAILED(TEMP::InitD3D11Device(
        WindowInterface::GetWindowPtr()->GetWndHandle())))
    {
        return -1;
    }

    if (FAILED(TEMP::PrepareBasicRender(
        WindowInterface::GetWindowPtr()->GetWndHandle())))
    {
        return -2;
    }

    //--------------------------------
    static TEMP::Mesh* testMesh = new TEMP::Mesh(
        TEMP::GetD3DDevicePointer());
    if (!testMesh->Load("man.fbx"))
    {
        return -3;
    }
    TEMP::PrepareMeshD3D(TEMP::GetD3DDevicePointer(),
        WindowInterface::GetWindowPtr()->GetWndHandle());

    TEMP::MyMesh* testmyMesh = new TEMP::MyMesh(
        TEMP::GetD3DDevicePointer());
    std::vector<MESH_VERTEX> v;
    std::vector<UINT> i;
    std::vector<MESH_TEXTURE> t;
    if (TEMP::PrepareTempMyMesh1(&v, &i, &t))
    {
        testmyMesh->CreateSub(v, i, t);
        v.clear();
    }
    if (TEMP::PrepareTempMyMesh2(&v, &i, &t))
    {
        testmyMesh->CreateSub(v, i, t);
        v.clear();
    }

    testmyMesh->SetPosition({ 0.f,-3.f,2.f });
    testmyMesh->SetScale({ 4.f,4.f,4.f });
    //--------------------------------

    //--------------------------------
    static ShadowTex* shadow = nullptr;
    shadow = new ShadowTex();
    if (!shadow->Init(TEMP::GetD3DDevicePointer(),
        TEMP::GetD3DDevContPointer(), 1280, 720))
    {
        return -4;
    }
    //--------------------------------

    //--------------------------------
    static SsaoTexs* ssao = nullptr;
    static bool goSsao = false;
    ssao = new SsaoTexs();
    if (!ssao->Init(TEMP::GetD3DDevicePointer(),
        TEMP::GetD3DDevContPointer(), 1280, 720))
    {
        return -5;
    }
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
            InputInterface::PollDevices();

            TEMP::SetVPShaderForShadow();
            TEMP::UpdateLightAndSth();

            TEMP::SetVPShaderForShadow();
            shadow->SetRenderTarget();
            shadow->ClearRenderTarget();
            testMesh->DrawShadowDepth(TEMP::GetD3DDevContPointer());
            testmyMesh->DrawShadowDepth(TEMP::GetD3DDevContPointer());
            shadow->UnBoundDSV();

            if (goSsao)
            {
                TEMP::SetVPShaderForAoNormal();
                ssao->SetNormalRenderTarget();
                testMesh->DrawSsaoNormal(TEMP::GetD3DDevContPointer());
                testmyMesh->DrawSsaoNormal(TEMP::GetD3DDevContPointer());
                shadow->UnBoundDSV();
            }

            TEMP::SetVPShaderForNormal();
            TEMP::TempRenderBegin();
            testMesh->Draw(TEMP::GetD3DDevContPointer());
            testmyMesh->Draw(TEMP::GetD3DDevContPointer());
            TEMP::TempRenderEnd();

            //-------------------------
            if (InputInterface::IsKeyPushedInSingle(KB_ESCAPE))
            {
                PostQuitMessage(0);
            }

            if (InputInterface::IsKeyDownInSingle(
                GP_LEFTSTICKBTN))
            {
                tempShowMousePos(1, 0, 0);
            }
            if (InputInterface::IsKeyPushedInSingle(
                GP_LEFTDIRBTN))
            {
                tempShowMousePos(0, 0, 0);
            }
            if (InputInterface::IsKeyPushedInSingle(
                GP_RIGHTFORESHDBTN))
            {
                goSsao = !goSsao;
            }

#ifdef LIGHT_BY_KEY
            TEMP::setLightOffsetX(
                (FLOAT)InputInterface::GetMouseOffset().x);
            TEMP::setLightOffsetY(
                (FLOAT)InputInterface::GetMouseOffset().y);
#else
            TEMP::setLightOffsetX(
                (FLOAT)InputInterface::RightStickOffset().x /
                300.f);
            TEMP::setLightOffsetY(
                (FLOAT)InputInterface::RightStickOffset().y /
                300.f);
#endif // LIGHT_BY_KEY
        }
    }

    shadow->ClearAndStop();
    delete shadow;

    testMesh->DeleteThisMesh();
    delete testMesh;
    testmyMesh->DeleteThisMesh();
    delete testmyMesh;

    TEMP::CleanupDevice();
    InputInterface::CleanAndStop();
    WindowInterface::CleanAndStop();

    return (int)msg.wParam;
}
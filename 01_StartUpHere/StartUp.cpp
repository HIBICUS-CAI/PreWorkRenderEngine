#include <Windows.h>
#include "WM_Interface.h"
#include "ID_Interface.h"
#include "TempMesh.h"
#include "RSRoot_DX11.h"
#include "TempPipeline_Diffuse.h"
#include "RSCamera.h"
#include "RSCamerasContainer.h"
#include "RSPipelinesManager.h"

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
    if (!mesh->Load("Dragon.FBX.json",
        MESH_FILE_TYPE::JSON))
    {
        return -1;
    }

    RSRoot_DX11* root = new RSRoot_DX11();
    if (!root->StartUp(
        WindowInterface::GetWindowPtr()->GetWndHandle()))
    {
        return -2;
    }

    TempGeoMesh* floor = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGrid(
            50.f, 50.f, 10, 10,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    floor->AddInstanceData(
        { 0.f,-5.f,0.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* walls = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateBox(
            5.f, 5.f, 5.f, 1,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    walls->AddInstanceData(
        { 0.f,1.25f,25.f },
        { 0.f,0.f,0.f },
        { 10.f,2.5f,0.125f });
    walls->AddInstanceData(
        { 0.f,1.25f,-25.f },
        { 0.f,0.f,0.f },
        { 10.f,2.5f,0.125f });
    walls->AddInstanceData(
        { 25.f,1.25f,0.f },
        { 0.f,DirectX::XM_PIDIV2,0.f },
        { 10.f,2.5f,0.125f });
    walls->AddInstanceData(
        { -25.f,1.25f,0.f },
        { 0.f,DirectX::XM_PIDIV2,0.f },
        { 10.f,2.5f,0.125f });

    mesh->Process(root->MeshHelper());

    PassRootToTempPipeline(root);
    if (!CreateTempPipeline())
    {
        return -3;
    }

    std::string name = "temp-cam";
    CAM_INFO ci = {};
    ci.mType = LENS_TYPE::PERSPECTIVE;
    ci.mPosition = { 0.f,0.f,0.f };
    ci.mLookAt = { 0.f,0.f,1.f };
    ci.mUpVec = { 0.f,1.f,0.f };
    ci.mNearFarZ = { 1.f,100.f };
    ci.mPFovyAndRatio = { DirectX::XM_PIDIV4,16.f / 9.f };
    ci.mOWidthAndHeight = { 12.8f,7.2f };
    auto cam = root->CamerasContainer()->CreateRSCamera(
        name, &ci);

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

            auto sticksL = InputInterface::LeftStickOffset();
            auto sticksR = InputInterface::RightStickOffset();
            float vertL = (float)-sticksL.x / 1000000.f;
            float horiL = (float)-sticksL.y / 1000000.f;
            float vertR = (float)sticksR.x / 5000000.f;
            float horiR = (float)-sticksR.y / 5000000.f;
            cam->TranslateRSCamera({ vertL,0.f,horiL });
            cam->RotateRSCamera({ horiR,vertR,0.f });

            mesh->UploadDrawCall(root->DrawCallsPool(), root);
            floor->UploadDrawCall(root->DrawCallsPool(), root);
            walls->UploadDrawCall(root->DrawCallsPool(), root);
            root->PipelinesManager()->ExecuateCurrentPipeline();
        }
    }

    mesh->Release(root->MeshHelper());
    floor->Release(root->MeshHelper());
    walls->Release(root->MeshHelper());
    root->CleanAndStop();
    delete root;
    delete mesh;
    delete floor;
    delete walls;

    return (int)msg.wParam;
}
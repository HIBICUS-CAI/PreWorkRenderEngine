#include <Windows.h>
#include "WM_Interface.h"
#include "ID_Interface.h"
#include "TempMesh.h"
#include "RSRoot_DX11.h"
#include "TempPipeline_Diffuse.h"
#include "TempPipeline_Light.h"
#include "RSCamera.h"
#include "RSCamerasContainer.h"
#include "RSPipelinesManager.h"
#include "RSDrawCallsPool.h"
#include "RSDevices.h"

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
            {}, "dark-red.jpg"));
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

    TempGeoMesh* box = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateBox(
            4.f, 4.f, 4.f, 1,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "sand.jpg"));
    box->AddInstanceData(
        { 15.f,-3.f,15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { -15.f,-3.f,15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { 15.f,-3.f,-15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { -15.f,-3.f,-15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });

    TempGeoMesh* pillars = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateCylinder(
            2.5f, 1.5f, 30.f, 15, 5,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "dark-wood.jpg"));
    pillars->AddInstanceData(
        { 20.f,10.f,20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { -20.f,10.f,20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { -20.f,10.f,-20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { 20.f,10.f,-20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { 0.f,10.f,20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { 0.f,10.f,-20.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { 20.f,10.f,0.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });
    pillars->AddInstanceData(
        { -20.f,10.f,0.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* sphere1 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateSphere(
            2.f, 15, 15,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    sphere1->AddInstanceData(
        { 15.f,1.f,15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* sphere2 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateSphere(
            2.f, 5, 5,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    sphere2->AddInstanceData(
        { 15.f,1.f,-15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* geosph1 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGeometrySphere(
            2.f, 2,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    geosph1->AddInstanceData(
        { -15.f,1.f,15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* geosph2 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGeometrySphere(
            2.f, 1,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    geosph2->AddInstanceData(
        { -15.f,1.f,-15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempSpriteMesh* sp1 = new TempSpriteMesh(root->MeshHelper()->
        GeoGenerate()->CreateSpriteRect(
            LAYOUT_TYPE::NORMAL_TANGENT_TEX,
            "title.png"));
    sp1->AddInstanceData({ -500.f,300.f }, { 160.f,90.f },
        { 1.f,0.f,1.f,0.2f });

    TempSpriteMesh* sp2 = new TempSpriteMesh(root->MeshHelper()->
        GeoGenerate()->CreateSpriteRect(
            LAYOUT_TYPE::NORMAL_TANGENT_TEX,
            "mode.png"));
    sp2->AddInstanceData({ 500.f,300.f }, { 160.f,90.f },
        { 1.f,1.f,1.f,1.f });

    mesh->Process(root->MeshHelper());

    PassRootToTempWireFramePipeline(root);
    PassRootToTempLightPipeline(root);
    if (!CreateTempWireFramePipeline())
    {
        return -3;
    }
    if (!CreateTempLightPipeline())
    {
        return -4;
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

    name = "temp-ui-cam";
    ci = {};
    ci.mType = LENS_TYPE::ORTHOGRAPHIC;
    ci.mPosition = { 0.f,0.f,0.f };
    ci.mLookAt = { 0.f,0.f,1.f };
    ci.mUpVec = { 0.f,1.f,0.f };
    ci.mNearFarZ = { 1.f,100.f };
    ci.mPFovyAndRatio = { DirectX::XM_PIDIV4,16.f / 9.f };
    ci.mOWidthAndHeight = { 1280.f,720.f };
    root->CamerasContainer()->CreateRSCamera(
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

            static bool inWireframe = false;
            static std::string pipeName = "";
            static float speedFactor = 2.f;

            if (InputInterface::IsKeyPushedInSingle(
                GP_RIGHTFORESHDBTN))
            {
                inWireframe = !inWireframe;
                if (inWireframe)
                {
                    pipeName = "test-pipeline";
                    speedFactor = 0.5f;
                }
                else
                {
                    pipeName = "light-pipeline";
                    speedFactor = 2.f;
                }
                root->PipelinesManager()->SetPipeline(pipeName);
                root->PipelinesManager()->ProcessNextPipeline();
            }

            auto sticksL = InputInterface::LeftStickOffset();
            auto sticksR = InputInterface::RightStickOffset();
            float vertL = (float)-sticksL.x / 100000.f * speedFactor;
            float horiL = (float)-sticksL.y / 100000.f * speedFactor;
            float vertR = (float)sticksR.x / 1000000.f * speedFactor;
            float horiR = (float)-sticksR.y / 1000000.f * speedFactor;
            cam->TranslateRSCamera({ vertL,0.f,horiL });
            cam->RotateRSCamera({ horiR,vertR,0.f });

            mesh->UploadDrawCall(root->DrawCallsPool(), root);
            floor->UploadDrawCall(root->DrawCallsPool(), root);
            walls->UploadDrawCall(root->DrawCallsPool(), root);
            pillars->UploadDrawCall(root->DrawCallsPool(), root);
            box->UploadDrawCall(root->DrawCallsPool(), root);
            sphere1->UploadDrawCall(root->DrawCallsPool(), root);
            sphere2->UploadDrawCall(root->DrawCallsPool(), root);
            geosph1->UploadDrawCall(root->DrawCallsPool(), root);
            geosph2->UploadDrawCall(root->DrawCallsPool(), root);
            sp1->UploadDrawCall(root->DrawCallsPool(), root);
            sp2->UploadDrawCall(root->DrawCallsPool(), root);
            root->PipelinesManager()->ExecuateCurrentPipeline();

            root->Devices()->PresentSwapChain();
            root->DrawCallsPool()->ClearAllDrawCallsInPipes();
        }
    }

    mesh->Release(root->MeshHelper());
    floor->Release(root->MeshHelper());
    walls->Release(root->MeshHelper());
    pillars->Release(root->MeshHelper());
    box->Release(root->MeshHelper());
    sphere1->Release(root->MeshHelper());
    sphere2->Release(root->MeshHelper());
    geosph1->Release(root->MeshHelper());
    geosph2->Release(root->MeshHelper());
    sp1->Release(root->MeshHelper());
    sp2->Release(root->MeshHelper());
    root->CleanAndStop();
    delete root;
    delete mesh;
    delete floor;
    delete walls;
    delete pillars;
    delete box;
    delete sphere1;
    delete sphere2;
    delete geosph1;
    delete geosph2;
    delete sp1;
    delete sp2;

    return (int)msg.wParam;
}
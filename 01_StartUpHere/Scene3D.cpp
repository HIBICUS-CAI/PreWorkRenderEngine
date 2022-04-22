#include "ID_Interface.h"
#include "RSRoot_DX11.h"
#include "TempRenderConfig.h"
#include "TempMesh.h"
#include "TempPipeline_Diffuse.h"
#include "TempPipeline_Light.h"
#include "RSCamera.h"
#include "RSCamerasContainer.h"
#include "RSLightsContainer.h"
#include "RSParticlesContainer.h"
#include "RSPipelinesManager.h"
#include "RSDrawCallsPool.h"
#include "RSDevices.h"
#include <cstdlib>
#include <ctime>
#include <bullet\btBulletDynamicsCommon.h>

bool Run3DLoop(RSRoot_DX11* _rootPtr)
{
    RSRoot_DX11* root = _rootPtr;

    TempMesh* mesh = new TempMesh();
    if (!mesh->Load("Dragon.FBX.json",
        MESH_FILE_TYPE::JSON))
    {
        return false;
    }

    TempGeoMesh* floor = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGrid(
            50.f, 50.f, 10, 10,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "tile.dds"));
    floor->CreateBumpedTex("tile_nmap.dds",
        root->Devices(), root->ResourceManager());
    floor->AddInstanceData(
        { 0.f,-5.f,0.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* walls = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateBox(
            5.f, 5.f, 5.f, 1,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "bricks.dds"));
    walls->CreateBumpedTex("bricks_nmap.dds",
        root->Devices(), root->ResourceManager());
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
        { 15.f,3.f,15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { -15.f,3.f,15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { 15.f,3.f,-15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });
    box->AddInstanceData(
        { -15.f,3.f,-15.f },
        { 0.f,0.f,0.f },
        { 1.f,1.f,1.f });

    TempGeoMesh* pillars = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateCylinder(
            2.5f, 1.5f, 30.f, 15, 5,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "bricks2.dds"));
    pillars->CreateBumpedTex("bricks2_nmap.dds",
        root->Devices(), root->ResourceManager());
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
        { 15.f,10.f,15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* sphere2 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateSphere(
            2.f, 5, 5,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    sphere2->AddInstanceData(
        { 15.f,10.f,-15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* geosph1 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGeometrySphere(
            2.f, 2,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    geosph1->AddInstanceData(
        { -15.f,10.f,15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempGeoMesh* geosph2 = new TempGeoMesh(root->MeshHelper()->
        GeoGenerate()->CreateGeometrySphere(
            2.f, 1,
            LAYOUT_TYPE::NORMAL_TANGENT_TEX, false,
            {}, "white.jpg"));
    geosph2->AddInstanceData(
        { -15.f,10.f,-15.f }, { 0.f,0.f,0.f }, { 1.f,1.f,1.f });

    TempSpriteMesh* sp1 = new TempSpriteMesh(root->MeshHelper()->
        GeoGenerate()->CreateSpriteRect(
            LAYOUT_TYPE::NORMAL_TANGENT_TEX,
            "title.png"));
    sp1->AddInstanceData({ -500.f,300.f }, { 160.f,90.f },
        { 0.f,0.5f,0.5f,0.4f });

    TempSpriteMesh* sp2 = new TempSpriteMesh(root->MeshHelper()->
        GeoGenerate()->CreateSpriteRect(
            LAYOUT_TYPE::NORMAL_TANGENT_TEX,
            "mode.png"));
    sp2->AddInstanceData({ 500.f,300.f }, { 160.f,90.f },
        { 1.f,1.f,1.f,1.f });

    mesh->Process(root->MeshHelper());

    std::string name = "temp-cam";
    CAM_INFO ci = {};
    ci.mType = LENS_TYPE::PERSPECTIVE;
    ci.mPosition = { 20.f,20.f,-20.f };
    ci.mLookAt = { -1.f,-1.f,1.f };
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
    root->CamerasContainer()->CreateRSCamera(name, &ci);

    name = "direct-light-1";
    LIGHT_INFO li = {};
    li.mType = LIGHT_TYPE::DIRECT;
    li.mWithShadow = true;
    li.mPosition = { 0.f,30.f,-30.f };
    li.mDirection = { 0.f,-1.f,1.f };
    li.mStrength = { 0.8f,0.8f,0.8f };
    li.mSpotPower = 2.f;
    li.mFalloffStart = 5.f;
    li.mFalloffEnd = 15.f;
    ci = {};
    ci.mType = LENS_TYPE::ORTHOGRAPHIC;
    ci.mPosition = li.mPosition;
    ci.mLookAt = li.mDirection;
    ci.mUpVec = { 0.f,1.f,1.f };
    ci.mNearFarZ = { 1.f,100.f };
    ci.mPFovyAndRatio = { DirectX::XM_PIDIV4,16.f / 9.f };
    ci.mOWidthAndHeight = { 12.8f * 9.5f,7.2f * 9.5f };
    auto light1 = root->LightsContainer()->CreateRSLight(
        name, &li);
    root->LightsContainer()->CreateLightCameraFor(name, &ci);

    name = "direct-light-2";
    li = {};
    li.mType = LIGHT_TYPE::DIRECT;
    li.mWithShadow = GetRenderConfig().mSecongShadowEnable;
    li.mPosition = { -30.f,30.f,0.f };
    li.mDirection = { 1.f,-1.f,0.f };
    li.mStrength = { 0.8f,0.8f,0.8f };
    li.mSpotPower = 2.f;
    li.mFalloffStart = 5.f;
    li.mFalloffEnd = 15.f;
    ci = {};
    ci.mType = LENS_TYPE::ORTHOGRAPHIC;
    ci.mPosition = li.mPosition;
    ci.mLookAt = li.mDirection;
    ci.mUpVec = { 1.f,1.f,0.f };
    ci.mNearFarZ = { 1.f,100.f };
    ci.mPFovyAndRatio = { DirectX::XM_PIDIV4,16.f / 9.f };
    ci.mOWidthAndHeight = { 12.8f * 9.5f,7.2f * 9.5f };
    auto light2 = root->LightsContainer()->CreateRSLight(
        name, &li);
    root->LightsContainer()->CreateLightCameraFor(name, &ci);

    name = "point-light-1";
    li = {};
    li.mType = LIGHT_TYPE::POINT;
    li.mWithShadow = false;
    li.mPosition = { -17.5f,0.f,-17.5f };
    li.mDirection = { 1.f,-1.f,0.f };
    li.mStrength = { 0.9f,0.f,0.9f };
    li.mSpotPower = 2.f;
    li.mFalloffStart = 3.f;
    li.mFalloffEnd = 8.f;
    auto light3 = root->LightsContainer()->CreateRSLight(
        name, &li);

    root->LightsContainer()->InsertAmbientLight("temp-ambient",
        { 0.3f,0.3f,0.3f,1.f });
    root->LightsContainer()->SetCurrentAmbientLight("temp-ambient");

    name = "ptc-emitter-1";
    PARTICLE_EMITTER_INFO pei = {};
    pei.mAcceleration = { 0.f,-9.8f,0.f };
    pei.mEmitNumPerSecond = 600.f;
    pei.mEnableStreak = true;
    pei.mLifeSpan = 100.f;
    pei.mOffsetEndColor = { 0.f,0.f,0.f,0.f };
    pei.mOffsetEndSize = 0.f;
    pei.mOffsetStartColor = { 1.f,0.f,0.f,1.f };
    pei.mOffsetStartSize = 0.5f;
    pei.mParticleMass = 0.1f;
    pei.mPosition = { 0.f,20.f,0.f };
    pei.mPosVariance = { 1.f,1.f,1.f };
    pei.mTextureID = PARTICLE_TEXTURE::WHITE_CIRCLE;
    pei.mVelocity = { 0.f,9.f,0.f };
    pei.mVelVariance = 0.5f;
    root->ParticlesContainer()->CreateRSParticleEmitter(
        name, &pei);

    name = "ptc-emitter-2";
    pei = {};
    pei.mAcceleration = { 0.02f,0.2f,0.f };
    pei.mEmitNumPerSecond = 10.f;
    pei.mEnableStreak = false;
    pei.mLifeSpan = 10000.f;
    pei.mOffsetEndColor = { 0.f,0.f,0.f,0.f };
    pei.mOffsetEndSize = 0.1f;
    pei.mOffsetStartColor = { 1.f,1.f,1.f,1.f };
    pei.mOffsetStartSize = 3.f;
    pei.mParticleMass = 0.01f;
    pei.mPosition = { 10.f,20.f,10.f };
    pei.mPosVariance = { 2.f,2.f,2.f };
    pei.mTextureID = PARTICLE_TEXTURE::WHITE_SMOKE;
    pei.mVelocity = { 0.f,0.5f,0.f };
    pei.mVelVariance = 0.5f;
    root->ParticlesContainer()->CreateRSParticleEmitter(
        name, &pei);

    if (!CreateTempWireFramePipeline()) { return false; }
    if (!CreateTempLightPipeline()) { return false; }

    btDefaultCollisionConfiguration* collisionConfiguration =
        new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher =
        new btCollisionDispatcher(collisionConfiguration);
    btBroadphaseInterface* overlappingPairCache =
        new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver =
        new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld* dynamicsWorld =
        new btDiscreteDynamicsWorld(
            dispatcher, overlappingPairCache,
            solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    {
        btCollisionShape* groundShape =
            new btBoxShape(btVector3(
                btScalar(25.f), btScalar(1.f), btScalar(25.f)));
        collisionShapes.push_back(groundShape);

        btTransform groundTransform = {};
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0.f, -6.f, 0.f));
        btScalar mass = 0.f;
        bool isDynamic = false;
        btVector3 localInertia(0.f, 0.f, 0.f);
        if (isDynamic)
            groundShape->calculateLocalInertia(mass, localInertia);
        btDefaultMotionState* myMotionState =
            new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo
            rbInfo(mass, myMotionState,
                groundShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        dynamicsWorld->addRigidBody(body);
    }

    {
        btCollisionShape* wallShape =
            new btBoxShape(btVector3(
                btScalar(25.f), btScalar(6.25f), btScalar(0.312f)));
        collisionShapes.push_back(wallShape);

        {
            btTransform wallTransform = {};
            wallTransform.setIdentity();
            wallTransform.setOrigin(btVector3(
                0.f, 1.25f, 25.f));
            wallTransform.setRotation(btQuaternion(
                0.f, 0.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
            {
                wallShape->calculateLocalInertia(
                    mass, localInertia);
            }
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(wallTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    wallShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform wallTransform = {};
            wallTransform.setIdentity();
            wallTransform.setOrigin(btVector3(
                0.f, 1.25f, -25.f));
            wallTransform.setRotation(btQuaternion(
                0.f, 0.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
            {
                wallShape->calculateLocalInertia(
                    mass, localInertia);
            }
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(wallTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    wallShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform wallTransform = {};
            wallTransform.setIdentity();
            wallTransform.setOrigin(btVector3(
                25.f, 1.25f, 0.f));
            wallTransform.setRotation(btQuaternion(
                DirectX::XM_PIDIV2, 0.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
            {
                wallShape->calculateLocalInertia(
                    mass, localInertia);
            }
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(wallTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    wallShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform wallTransform = {};
            wallTransform.setIdentity();
            wallTransform.setOrigin(btVector3(
                -25.f, 1.25f, 0.f));
            wallTransform.setRotation(btQuaternion(
                DirectX::XM_PIDIV2, 0.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
            {
                wallShape->calculateLocalInertia(
                    mass, localInertia);
            }
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(wallTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    wallShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
    }

    {
        btCollisionShape* pillarShape =
            new btBoxShape(btVector3(
                btScalar(2.5f), btScalar(15.f), btScalar(2.5f)));
        collisionShapes.push_back(pillarShape);

        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                20.f, 10.f, 20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                -20.f, 10.f, 20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                -20.f, 10.f, -20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                20.f, 10.f, -20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                0.f, 10.f, 20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                0.f, 10.f, -20.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                20.f, 10.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
        {
            btTransform pillarTransform = {};
            pillarTransform.setIdentity();
            pillarTransform.setOrigin(btVector3(
                -20.f, 10.f, 0.f));
            btScalar mass = 0.f;
            bool isDynamic = false;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                pillarShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(pillarTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    pillarShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
        }
    }

    btRigidBody* boxRigid[4] = { nullptr };
    {
        btCollisionShape* boxShape =
            new btBoxShape(btVector3(
                btScalar(2.f), btScalar(2.f), btScalar(2.f)));
        collisionShapes.push_back(boxShape);

        {
            btTransform boxTransform = {};
            boxTransform.setIdentity();
            boxTransform.setOrigin(btVector3(15.f, 3.f, -15.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                boxShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(boxTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    boxShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            boxRigid[0] = body;
        }
        {
            btTransform boxTransform = {};
            boxTransform.setIdentity();
            boxTransform.setOrigin(btVector3(-15.f, 3.f, -15.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                boxShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(boxTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    boxShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            boxRigid[1] = body;
        }
        {
            btTransform boxTransform = {};
            boxTransform.setIdentity();
            boxTransform.setOrigin(btVector3(15.f, 3.f, 15.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                boxShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(boxTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    boxShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            boxRigid[2] = body;
        }
        {
            btTransform boxTransform = {};
            boxTransform.setIdentity();
            boxTransform.setOrigin(btVector3(-15.f, 3.f, 15.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                boxShape->calculateLocalInertia(mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(boxTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    boxShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            boxRigid[3] = body;
        }
    }

    btRigidBody* sphereRigid[4] = { nullptr };
    {
        btCollisionShape* sphereShape =
            new btSphereShape(btScalar(2.f));
        collisionShapes.push_back(sphereShape);

        {
            btTransform sphereTransform = {};
            sphereTransform.setIdentity();
            sphereTransform.setOrigin(btVector3(12.f, 20.f, -12.f));
            btScalar mass = 20.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                sphereShape->calculateLocalInertia(
                    mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(sphereTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    sphereShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            sphereRigid[0] = body;
        }
        {
            btTransform sphereTransform = {};
            sphereTransform.setIdentity();
            sphereTransform.setOrigin(btVector3(12.f, 10.f, 12.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                sphereShape->calculateLocalInertia(
                    mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(sphereTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    sphereShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            sphereRigid[1] = body;
        }
        {
            btTransform sphereTransform = {};
            sphereTransform.setIdentity();
            sphereTransform.setOrigin(btVector3(-12.f, 10.f, -15.f));
            btScalar mass = 15.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                sphereShape->calculateLocalInertia(
                    mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(sphereTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    sphereShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            sphereRigid[2] = body;
        }
        {
            btTransform sphereTransform = {};
            sphereTransform.setIdentity();
            sphereTransform.setOrigin(btVector3(-12.f, 10.f, 15.f));
            btScalar mass = 55.f;
            bool isDynamic = true;
            btVector3 localInertia(0.f, 0.f, 0.f);
            if (isDynamic)
                sphereShape->calculateLocalInertia(
                    mass, localInertia);
            btDefaultMotionState* myMotionState =
                new btDefaultMotionState(sphereTransform);
            btRigidBody::btRigidBodyConstructionInfo
                rbInfo(mass, myMotionState,
                    sphereShape, localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            dynamicsWorld->addRigidBody(body);
            sphereRigid[3] = body;
        }
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

            static bool inWireframe = false;
            static std::string pipeName = "";
            static float speedFactor = 2.f;

            if (InputInterface::IsKeyPushedInSingle(
                GP_RIGHTFORESHDBTN) ||
                InputInterface::IsKeyPushedInSingle(KB_P))
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

            static bool pointLightBoom = false;
            if (InputInterface::IsKeyPushedInSingle(
                GP_LEFTFORESHDBTN) ||
                InputInterface::IsKeyPushedInSingle(KB_L))
            {
                std::string name = "";
                if (pointLightBoom)
                {
                    for (int i = 0; i < 64; i++)
                    {
                        name = "point-light-" + std::to_string(i + 2);
                        root->LightsContainer()->DeleteRSLight(name, false);
                    }
                }
                else
                {
                    std::srand((UINT)std::time(nullptr) + std::rand());
                    for (int i = 0; i < 64; i++)
                    {
                        int range = 4000;
                        int basic = range / -2;
                        float x = (float)(std::rand() % range + basic) / 100.f;
                        float z = (float)(std::rand() % range + basic) / 100.f;
                        range = 1000;
                        basic = range / -2;
                        float y = (float)(std::rand() % range + basic) / 100.f;
                        range = 1000;
                        float r = (float)(std::rand() % range) / 1000.f;
                        float g = (float)(std::rand() % range) / 1000.f;
                        float b = (float)(std::rand() % range) / 1000.f;
                        name = "point-light-" + std::to_string(i + 2);
                        li = {};
                        li.mType = LIGHT_TYPE::POINT;
                        li.mWithShadow = false;
                        li.mPosition = { x,y,z };
                        li.mDirection = { 1.f,-1.f,0.f };
                        li.mStrength = { r,g,b };
                        li.mSpotPower = 2.f;
                        li.mFalloffStart = 3.f;
                        li.mFalloffEnd = 8.f;
                        root->LightsContainer()->CreateRSLight(name, &li);
                        root->LightsContainer()->
                            CreateLightBloom(name,
                                root->MeshHelper()->GeoGenerate()->
                                CreateBox(1.f, 1.f, 1.f, 0,
                                    LAYOUT_TYPE::NORMAL_COLOR));
                    }
                }
                pointLightBoom = !pointLightBoom;
            }

            auto sticksL = InputInterface::LeftStickOffset();
            auto sticksR = InputInterface::RightStickOffset();
            float vertL = (float)-sticksL.x / 100000.f * speedFactor;
            float horiL = (float)-sticksL.y / 100000.f * speedFactor;
            float vertR = (float)sticksR.x / 1000000.f * speedFactor;
            float horiR = (float)-sticksR.y / 1000000.f * speedFactor;
            cam->TranslateRSCamera({ vertL,0.f,horiL });
            cam->RotateRSCamera(horiR, vertR);

            dynamicsWorld->stepSimulation(1.f / 60.f, 3);
            btTransform trans = {};
            DirectX::XMFLOAT3 position = {};
            DirectX::XMFLOAT3 rotation = {};
            for (int i = 0; i < 4; i++)
            {
                boxRigid[i]->getMotionState()->
                    getWorldTransform(trans);
                position.x = trans.getOrigin().getX();
                position.y = trans.getOrigin().getY();
                position.z = -trans.getOrigin().getZ();
                rotation.x = trans.getRotation().getX();
                rotation.y = trans.getRotation().getY();
                rotation.z = -trans.getRotation().getZ();
                box->UpdateInstanceData(
                    position, rotation, (size_t)i);
                sphereRigid[i]->getMotionState()->
                    getWorldTransform(trans);
                position.x = trans.getOrigin().getX();
                position.y = trans.getOrigin().getY();
                position.z = -trans.getOrigin().getZ();
                rotation.x = trans.getRotation().getX();
                rotation.y = trans.getRotation().getY();
                rotation.z = -trans.getRotation().getZ();
                if (i == 0)
                {
                    sphere1->UpdateInstanceData(
                        position, rotation, 0);
                }
                else if (i == 1)
                {
                    sphere2->UpdateInstanceData(
                        position, rotation, 0);
                }
                else if (i == 2)
                {
                    geosph1->UpdateInstanceData(
                        position, rotation, 0);
                }
                else if (i == 3)
                {
                    geosph2->UpdateInstanceData(
                        position, rotation, 0);
                }
            }

            for (size_t i = 0; i < 1; i++)
            {
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
                root->LightsContainer()->UploadLightBloomDrawCall();
            }

            root->PipelinesManager()->ExecuateCurrentPipeline();

            root->Devices()->PresentSwapChain();

            root->DrawCallsPool()->ClearAllDrawCallsInPipes();
        }
    }

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1;
        i >= 0; i--)
    {
        btCollisionObject* obj =
            dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
    collisionShapes.clear();

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

    return true;
}

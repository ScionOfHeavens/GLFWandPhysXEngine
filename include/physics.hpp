#include <PxPhysicsAPI.h>

using namespace physx;

#define PVD_HOST "127.0.0.1"

const PxVec3 G_GRAVITY(0.0f, -0.81f, 0.0f);
const PxReal TIME_STEP = 1.0f / 60.0f;

PxFoundation*           gFoundation = nullptr;
PxPhysics*              gPhysics    = nullptr;
PxScene*                gScene      = nullptr;
PxDefaultCpuDispatcher* gDispatcher = nullptr;
PxMaterial*             gMaterial   = nullptr;
PxRigidDynamic*         gDynamicBox = nullptr;

void initPhysics(/*bool interactive*/)
{
    static PxDefaultAllocator gAllocator;
    static PxDefaultErrorCallback gErrorCallback;

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    PxTolerancesScale scale;
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = G_GRAVITY;
    gDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.8f);

    // Статическая плоскость (земля)
    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);

    // Динамический куб (размер 1х1х1, полуэкстенты 0.5)
	physx::PxQuat rotation(physx::PxPi / 4.0f, physx::PxVec3(1.0f, 0.0f, 0.0f));
    PxTransform boxTransform(PxVec3(0.0f, 10.0f, 0.0f), rotation);
    PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
    gDynamicBox = PxCreateDynamic(*gPhysics, boxTransform, boxGeometry, *gMaterial, 10.0f);
    gScene->addActor(*gDynamicBox);
}

PxMat44 stepPhysics()
{
	gScene->simulate(TIME_STEP);
	gScene->fetchResults(true);
	PxTransform pose = gDynamicBox->getGlobalPose();
    return PxMat44(pose);
}
	
void CleanupPhysics() {
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    PX_RELEASE(gFoundation);
}

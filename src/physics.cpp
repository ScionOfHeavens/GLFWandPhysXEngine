#include <PxPhysicsAPI.h>

#include "physics.hpp"

using namespace physx;  

PxVec3 Physics::gravityAcceleration{0.f, -9.81f, 0.f};
PxReal Physics::physicsTimeStep{1.0f/60.0f};

PxFoundation*           Physics::foundation = nullptr;
PxPhysics*              Physics::physics = nullptr;
PxScene*                Physics::scene = nullptr;
PxDefaultCpuDispatcher* Physics::dispatcher = nullptr;
PxMaterial*             Physics::material = nullptr;
PxRigidDynamic*         Physics::dynamicBox = nullptr;
        
PxDefaultAllocator      Physics::allocator;
PxDefaultErrorCallback  Physics::errorCallback;

        
void Physics::initPhysics()
{
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
    PxTolerancesScale scale;
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale);

    PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = gravityAcceleration;
    dispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    scene = physics->createScene(sceneDesc);

    material = physics->createMaterial(0.5f, 0.5f, 0.2f);
}

void Physics::CreatePlane() {
    PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);
    scene->addActor(*groundPlane);
}

void Physics::CreateCube() {
    physx::PxQuat rotation(physx::PxPi / 4.0f, physx::PxVec3(1.0f, 0.0f, 0.0f));
    PxTransform boxTransform(PxVec3(0.0f, 10.0f, 0.0f), rotation);
    PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
    dynamicBox = PxCreateDynamic(*physics, boxTransform, boxGeometry, *material, 10.0f);
    scene->addActor(*dynamicBox);
}

PxMat44 Physics::stepPhysics()
{
    scene->simulate(physicsTimeStep);
    scene->fetchResults(true);
    PxTransform pose = dynamicBox->getGlobalPose();
    return PxMat44(pose);
}

void Physics::CleanupPhysics() {
    PX_RELEASE(scene);
    PX_RELEASE(dispatcher);
    PX_RELEASE(physics);
    PX_RELEASE(foundation);
}
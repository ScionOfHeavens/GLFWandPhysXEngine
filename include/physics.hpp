#include <PxPhysicsAPI.h>

using namespace physx;

#define PVD_HOST "127.0.0.1"

class Physics{
    public:
        static void initPhysics();
        static void CreatePlane();
        static void CreateCube();
        static PxMat44 stepPhysics();
        static void CleanupPhysics();

    private:
        static PxVec3 gravityAcceleration; 
        static PxReal physicsTimeStep;
        static PxFoundation*           foundation;
        static PxPhysics*              physics;
        static PxScene*                scene;
        static PxDefaultCpuDispatcher* dispatcher;
        static PxMaterial*             material;
        static PxRigidDynamic*         dynamicBox;
        
        static PxDefaultAllocator allocator;
        static PxDefaultErrorCallback errorCallback;
};







	

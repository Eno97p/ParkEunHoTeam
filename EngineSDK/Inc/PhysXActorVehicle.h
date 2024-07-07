#pragma once


#include"Base.h"

struct  PhysXIntegrationState
{
    PxVehiclePhysXActor physxActor;
    PxVehiclePhysXSteerState physxSteerState;
    PxVehiclePhysXConstraints physxConstraints; //��������    //�ʿ��ұ�?  �ʹ� ū��..

    PX_FORCE_INLINE void setToDefault()
    {
        physxActor.setToDefault();
        physxSteerState.setToDefault();
        physxConstraints.setToDefault();
    }
    

};

BEGIN(Engine)

class ENGINE_DLL PhysXActorVehicle
{
public:
    PhysXActorVehicle() = default;
    virtual ~PhysXActorVehicle() = default;


public:
    HRESULT setUpActor(PxScene& scene, const PxTransform& pose, const char* vehicleName);
        

private:
    PhysXIntegrationState m_physxIntegrationState;

    
};

END
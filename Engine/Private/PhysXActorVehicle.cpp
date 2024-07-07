#include "PhysXActorVehicle.h"

HRESULT PhysXActorVehicle::setUpActor(PxScene& scene, const PxTransform& pose, const char* vehicleName)
{
	m_physxIntegrationState.physxActor.rigidBody->setGlobalPose(pose);
	if(!scene.addActor(*m_physxIntegrationState.physxActor.rigidBody))
	{
		return E_FAIL;
	}
	
	if(vehicleName)
	{
		m_physxIntegrationState.physxActor.rigidBody->setName(vehicleName);
	}

	

	return S_OK;
}

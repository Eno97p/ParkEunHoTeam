#include "..\Public\PhysXComponent_Vehicle.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"

PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
PxU32 gNbPhysXMaterialFrictions = 0;
PxReal gPhysXDefaultMaterialFriction = 1.0f;

PxMaterial* gMaterial = nullptr;

CPhysXComponent_Vehicle::CPhysXComponent_Vehicle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_Vehicle::CPhysXComponent_Vehicle(const CPhysXComponent_Vehicle & rhs)
	: CPhysXComponent{ rhs }
{
}

HRESULT CPhysXComponent_Vehicle::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CPhysXComponent_Vehicle::Initialize(void * pArg)
{

	if (FAILED(CreateActor()))
		return E_FAIL;


	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent_Vehicle::Render()
{
	
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));


		m_pShader->Begin(1);

		for (auto& pPhysXBuffer : m_pBuffer)
		{
			if (nullptr != pPhysXBuffer)
			{
				pPhysXBuffer->Bind_Buffers();
				pPhysXBuffer->Render();
			}


		}

	return S_OK;
}
#endif

void CPhysXComponent_Vehicle::Tick(const _float fTimeDelta)
{
	VehicleMiddleParams& middleParams = m_pVehicle->getMiddleParams();

	const float maxEngineTorque = 5.0f;  // 최대 엔진 토크 (Nm)
	const float accelerationRate = 100.0f;  // 가속도 증가율 (Nm/s)
	const float decelerationRate = 50.0f;  // 감속도 감소율 (Nm/s)

	// 예시: 키보드 입력 처리
	if (GetAsyncKeyState('W') & 0x8000)
	{
		// 전진
		middleParams.engineTorque += accelerationRate * fTimeDelta;
		if (middleParams.engineTorque > maxEngineTorque)
		{
			middleParams.engineTorque = maxEngineTorque;
		}
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		// 후진
		middleParams.engineTorque -= accelerationRate * fTimeDelta;
		if (middleParams.engineTorque < -maxEngineTorque)
		{
			middleParams.engineTorque = -maxEngineTorque;
		}
	}
	else
	{
		// 엔진 토크 해제 (천천히 감속)
		if (middleParams.engineTorque > 0)
		{
			middleParams.engineTorque -= decelerationRate * fTimeDelta;
			if (middleParams.engineTorque < 0)
			{
				middleParams.engineTorque = 0.0f;
			}
		}
		else if (middleParams.engineTorque < 0)
		{
			middleParams.engineTorque += decelerationRate * fTimeDelta;
			if (middleParams.engineTorque > 0)
			{
				middleParams.engineTorque = 0.0f;
			}
		}
	}

	const float maxSteeringAngle = 45.0f; // 45도
	//const float steeringSpeed = 2.0f;

	if (GetAsyncKeyState('A') & 0x8000)
	{
		// 좌회전
		middleParams.steeringAngle = -maxSteeringAngle;
		middleParams.MaxSteeringAngle = 45.0f;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		// 우회전
		middleParams.steeringAngle = maxSteeringAngle;
		middleParams.MaxSteeringAngle = 45.0f;
	}
	else
	{
		// 조향 해제
		middleParams.steeringAngle = 0.0f;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// 제동
		middleParams.brakeTorque = 1000.0f;
	}
	else
	{
		// 제동 해제
		middleParams.brakeTorque = 0.0f;
	}
	
}

void CPhysXComponent_Vehicle::Late_Tick(const _float fTimeDelta)
{
	//PxVehicleRigidBodyComponent


	m_pVehicle->updateSequence(fTimeDelta, m_pVehicleSimulationContext);
}






HRESULT CPhysXComponent_Vehicle::CreateActor()
{

		
	
	
	PxU32 axleWheelIds[PxVehicleLimits::eMAX_NB_WHEELS];
	axleWheelIds[0] = 0;
	axleWheelIds[1] = 1;
	axleWheelIds[2] = 2;
	axleWheelIds[3] = 3;
	m_pPhysXActorVehicle.mBaseParams.axleDescription.addAxle(2, axleWheelIds);

	initMaterialFrictionTable();

	setPhysXIntegrationParams(m_pPhysXActorVehicle.mBaseParams.axleDescription, gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction, m_pPhysXActorVehicle.getPhysXParams());


	if(!m_pPhysXActorVehicle.initialize(*m_pGameInstance->GetPhysics(), PxCookingParams(PxTolerancesScale()), *gMaterial))
		return E_FAIL;
	PxTransform pose(PxVec3(0.0f, 10.5f, 0.0f), PxQuat(PxIdentity));
	m_pPhysXActorVehicle.setUpActor(*m_pGameInstance->GetScene(), pose, "VehicleTest");















	m_pVehicle = CVehicleDefault::Create(4);


	if (nullptr == m_pVehicle)
		return E_FAIL;

	

	//PxTransform Pose = PxTransform(PxVec3(0.0f, 5.0f, 0.0f), PxQuat(PxIdentity));
	//if (FAILED(m_pVehicle->setUpActor(*m_pGameInstance->GetScene(), Pose, "VehicleTest")))
	//		return E_FAIL;
	
	VehicleBeginParams& beginParams = m_pVehicle->getBeginParams();
	beginParams.mass = 1500.0f;		//차량 질량 (kg)
	beginParams.dimensions = PxVec3(4.0f, 1.5f, 2.0f);  // 차량 크기 (m)
	beginParams.numWheels = 4;  // 바퀴 수

	for(PxU32 i = 0; i < 4; i++)
		beginParams.wheelRadius[i] = 0.4f;  // 바퀴 반지름 (m)

	VehicleMiddleParams& middleParams = m_pVehicle->getMiddleParams();
	middleParams.engineTorque = 0.0f;//		500.0f;  // 엔진 토크 (Nm)
	middleParams.brakeTorque = 1000.0f;  // 브레이크 토크 (Nm)
	middleParams.steeringAngle = 0.0f;  // 초기 조향각도 (rad)

	VehicleEndParams& endParams = m_pVehicle->getEndParams();
	endParams.maxSpeed = 10.0f;  // 최대 속도 (km/h)

	if (FAILED(m_pVehicle->createSequence()))
		return E_FAIL;
	if (FAILED(m_pVehicle->createActor(m_pGameInstance->GetPhysics(),m_pGameInstance->GetScene(),"TestName")))
		return E_FAIL;

	//PhysXActorVehicle




	m_pVehicleSimulationContext.setToDefault();
	m_pVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	m_pVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	m_pVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;


	

	m_pVehicleSimulationContext.scale.scale = WORLD_METER;
	m_pVehicleSimulationContext.gravity = m_pGameInstance->GetScene()->getGravity();
	m_pVehicleSimulationContext.physxScene = m_pGameInstance->GetScene();
	m_pVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
		


	//PxRigidDynamic* actor = m_pVehicle->getActor();

	
	


	

	return S_OK;
}

void CPhysXComponent_Vehicle::initMaterialFrictionTable()
{
	gMaterial = m_pGameInstance->GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

	gPhysXMaterialFrictions[0].friction = 1.0f;
	gPhysXMaterialFrictions[0].material = gMaterial;
	gPhysXDefaultMaterialFriction = 1.0f;
	gNbPhysXMaterialFrictions = 1;
}



CPhysXComponent_Vehicle * CPhysXComponent_Vehicle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPhysXComponent_Vehicle*		pInstance = new CPhysXComponent_Vehicle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_Vehicle");
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent_Vehicle::Clone(void * pArg)
{
	CPhysXComponent_Vehicle*		pInstance = new CPhysXComponent_Vehicle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_Vehicle");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_Vehicle::Free()
{
	__super::Free();
	Safe_Release(m_pVehicle);
	//m_pPhysXActorVehicle.destroy();

	

}

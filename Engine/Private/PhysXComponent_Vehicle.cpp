#include "..\Public\PhysXComponent_Vehicle.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"



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

	// ����: Ű���� �Է� ó��
	if (GetAsyncKeyState('W') & 0x8000)
	{
		// ����
		middleParams.engineTorque = 500.0f;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		// ����
		middleParams.engineTorque = -500.0f;
	}
	else
	{
		// ���� ��ũ ����
		middleParams.engineTorque = 0.0f;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		// ��ȸ��
		middleParams.steeringAngle = -0.5f;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		// ��ȸ��
		middleParams.steeringAngle = 0.5f;
	}
	else
	{
		// ���� ����
		middleParams.steeringAngle = 0.0f;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// ����
		middleParams.brakeTorque = 1000.0f;
	}
	else
	{
		// ���� ����
		middleParams.brakeTorque = 0.0f;
	}







	
	
}

void CPhysXComponent_Vehicle::Late_Tick(const _float fTimeDelta)
{
	//m_pVehicle->updateSequence(fTimeDelta, m_pVehicleSimulationContext);
}






HRESULT CPhysXComponent_Vehicle::CreateActor()
{
	m_pVehicle = CVehicleDefault::Create(4);

	if (nullptr == m_pVehicle)
		return E_FAIL;


	
	
	//PxTransform Pose = PxTransform(PxVec3(0.0f, 5.0f, 0.0f), PxQuat(PxIdentity));
	//if (FAILED(m_pVehicle->setUpActor(*m_pGameInstance->GetScene(), Pose, "VehicleTest")))
	//		return E_FAIL;
	
	VehicleBeginParams& beginParams = m_pVehicle->getBeginParams();
	beginParams.mass = 1000.0f;		//���� ���� (kg)
	beginParams.dimensions = PxVec3(2.0f, 1.5f, 4.0f);  // ���� ũ�� (m)
	beginParams.numWheels = 4;  // ���� ��

	for(PxU32 i = 0; i < 4; i++)
		beginParams.wheelRadius[i] = 0.5f;  // ���� ������ (m)

	VehicleMiddleParams& middleParams = m_pVehicle->getMiddleParams();
	middleParams.engineTorque = 500.0f;  // ���� ��ũ (Nm)
	middleParams.brakeTorque = 1000.0f;  // �극��ũ ��ũ (Nm)
	middleParams.steeringAngle = 0.0f;  // �ʱ� ���Ⱒ�� (rad)

	VehicleEndParams& endParams = m_pVehicle->getEndParams();
	endParams.maxSpeed = 250.0f;  // �ִ� �ӵ� (km/h)

	if (FAILED(m_pVehicle->createSequence()))
		return E_FAIL;
	if (FAILED(m_pVehicle->createActor(m_pGameInstance->GetPhysics(),m_pGameInstance->GetScene(),"TestName")))
		return E_FAIL;

	




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
	

}

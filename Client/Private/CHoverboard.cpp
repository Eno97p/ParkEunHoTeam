#include "stdafx.h"
#include "CHoverboard.h"


#include"GameInstance.h"

#include"Camera.h"

CHoverboard::CHoverboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CHoverboard::CHoverboard(const CHoverboard& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHoverboard::Initialize_Prototype()
{
	


	return S_OK;
}

HRESULT CHoverboard::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HoverboardInfo InfoDesc = *static_cast<HoverboardInfo*>(pArg);
		



		//XMVectorSet

		m_matWorld.r[CTransform::STATE::STATE_RIGHT] =		XMVectorSet(InfoDesc.vRight.x, InfoDesc.vRight.y, InfoDesc.vRight.z, 0.0f);
		m_matWorld.r[CTransform::STATE::STATE_UP] =			XMVectorSet(InfoDesc.vUp.x, InfoDesc.vUp.y, InfoDesc.vUp.z, 0.0f);
		m_matWorld.r[CTransform::STATE::STATE_LOOK] =		XMVectorSet(InfoDesc.vLook.x, InfoDesc.vLook.y, InfoDesc.vLook.z, 0.0f);
		m_matWorld.r[CTransform::STATE::STATE_POSITION] =	XMVectorSet(InfoDesc.vPosition.x, InfoDesc.vPosition.y, InfoDesc.vPosition.z, 1.0f);
	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (FAILED(Add_PxActor()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.f, 522.f, 98.f, 1.0f));

	m_fDisolveValue = 0.f;

	return S_OK;
}

void CHoverboard::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 5.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 5.f;
		if (m_fDisolveValue < 0.f)
		{
			m_pGameInstance->Erase(this);
		}
		break;
	default:
		break;
	}
}

void CHoverboard::Tick(_float fTimeDelta)
{
	if (!m_bOnRide)
	{
		return;
	}
	
	CPhysXComponent_Vehicle::VEHICLE_COMMAND* command;
	m_pPhysXCom->GetCommand(command);
	PxVec3 velocity = m_pPhysXCom->GetRigidBody()->getLinearVelocity();
	PxVec3 AngularVelocity = m_pPhysXCom->GetRigidBody()->getAngularVelocity();
	PxRigidBody* rigidBody = m_pPhysXCom->GetRigidBody();

	command->curSpeed = 	velocity.magnitude();
	//PxVec3 velocity = m_pPhysXCom->Get_Actor()
	PxVehicleSteerCommandResponseParams* steerResponse;
	m_pPhysXCom->GetSteerRespon(steerResponse);

	m_bIsMoving = (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_S));
	if (m_bIsMoving)
	{
	
		steerResponse->maxResponse = XMConvertToRadians(45.f); // 원래 값 
		steerResponse->wheelResponseMultipliers[0] = 1.0f;
		steerResponse->wheelResponseMultipliers[1] = 1.0f;
		steerResponse->wheelResponseMultipliers[2] = 0.0f;
		steerResponse->wheelResponseMultipliers[3] = 0.0f;
	

	}
	else
	{
		steerResponse->maxResponse = XMConvertToRadians(90.0f);
		steerResponse->wheelResponseMultipliers[0] = 1.0f;
		steerResponse->wheelResponseMultipliers[1] = 1.0f;
		steerResponse->wheelResponseMultipliers[2] = 1.0f;
		steerResponse->wheelResponseMultipliers[3] = 1.0f;


	}
	if (KEY_TAP(DIK_LSHIFT))
	{
		if (m_bIsBoost)
		{
			m_bIsBoost = false;
			m_pGameInstance->Get_MainCamera()->Set_Fovy(XMConvertToRadians(90.f));
		}
		else
		{
			m_bIsBoost = true;
			m_pGameInstance->Get_MainCamera()->Set_Fovy(XMConvertToRadians(60.f));
		}
	}


	if (KEY_HOLD(DIK_W))
	{
		command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
		command->throttle = m_bIsBoost ? 1.0f : 0.2f;
	

		command->brake = 0.0f;
		command->handbrake = 0.0f;
	}
	else if (KEY_HOLD(DIK_S))
	{
		command->gear = PxVehicleDirectDriveTransmissionCommandState::eREVERSE;
		command->throttle = 0.3f;
		command->brake = 0.0f;
		command->handbrake = 0.0f;
	}
	else
	{
		command->Reset();
	}



	CCamera* Camera = 	m_pGameInstance->Get_Cameras()[CAM_THIRDPERSON];
	_vector vectorCamLook = {}; 
	_vector vectorHoverLook = {};
	if (Camera)
	{
		vectorCamLook = Camera->Get_CamLook();
		vectorCamLook = XMVector3Normalize(vectorCamLook);
	}
	vectorHoverLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	vectorCamLook = XMVector3Normalize(XMVectorSetY(vectorCamLook, 0.0f));
	vectorHoverLook = XMVector3Normalize(XMVectorSetY(vectorHoverLook, 0.0f));
	
	_vector crossProduct = XMVector3Cross(vectorHoverLook, vectorCamLook);
	_float fDot = XMVectorGetX(XMVector3Dot(vectorCamLook, vectorHoverLook));
	fDot = std::clamp(fDot, -1.0f, 1.0f); // 안전을 위해 값 범위 제한
	_float fAngle = std::acos(fDot);


	// 회전 방향 결정
	_float rotationDirection = XMVectorGetY(crossProduct) < 0.0f ? -1.0f : 1.0f;








	// 조향 입력 처리
	float steerInput = 0.0f;
	if (KEY_HOLD(DIK_A))
	{
		if (m_bIsMoving)
			steerInput = -1.0f;
		else
		{
			command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
			command->throttle = 0.5f;
			steerInput = -1.0f;

		}
			
	}
	else if (KEY_HOLD(DIK_D))
	{
		if (m_bIsMoving)
			steerInput = 1.0f;
		else
		{
			command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
			command->throttle = 0.5f;
			steerInput = 1.0f;
		}
		//steerInput = 1.0f;
	}

	// 차체 회전과 바퀴 조향 통합
	float angleThreshold = XMConvertToRadians(10.0f);
	float rotationStrength = 500.0f; // 회전 강도 감소
	float steerStrength = 1.0f;

	if (fAngle > angleThreshold)
	{
		// 차체 회전
		float rotationFactor = (fAngle - angleThreshold) / (XM_PI - angleThreshold);
		PxVec3 torque(0.0f, rotationDirection * rotationFactor * rotationStrength, 0.0f);
		rigidBody->addTorque(torque, PxForceMode::eACCELERATION);

		// 바퀴 조향 감소
		steerStrength = 1.0f - rotationFactor;
	}

	// 최종 조향 적용
	command->steer = steerInput * steerStrength;





	if (KEY_HOLD(DIK_SPACE))
	{
		command->brake = 1.0f;

		command->throttle = 0.0f;
	}





	//// 특정 각도 이상 벌어지면 차량을 카메라 방향으로 회전
	//float angleThreshold = XMConvertToRadians(10.0f); // 각도 임계값 (조정 필요)
	//if (fAngle > angleThreshold)
	//{
	//	float rotationStrength = 1000.0f; // 회전 강도 (조정 필요)
	//	PxVec3 torque(0.0f, rotationDirection * fAngle * rotationStrength, 0.0f);
	//	rigidBody->addTorque(torque, PxForceMode::eACCELERATION);
	//}



	//if (KEY_HOLD(DIK_A))
	//{

	//	if(m_bIsMoving)
	//		command->steer = -1.0f;
	//	else
	//	{
	//		command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
	//		command->throttle = 0.5f;
	//		command->steer = -1.0f;
	//	}

	//}
	//if (KEY_HOLD(DIK_D))
	//{
	//	if (m_bIsMoving)
	//		command->steer = 1.0f;
	//	else
	//	{
	//		command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
	//		command->throttle = 0.5f;
	//		command->steer = 1.0f;
	//	}



	//	//command->steer = 1.0f;
	//}
	
	//if (KEY_HOLD(DIK_7))
	//{
	//	command->handbrake = 1.0f;
	//	command->throttle = 0.0f;
	//}
	//
	m_pPhysXCom->Tick(fTimeDelta);

	
}

void CHoverboard::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	
	


	m_pPhysXCom->Late_Tick(fTimeDelta);


	CPhysXComponent_Vehicle::VEHICLE_COMMAND* command;
	m_pPhysXCom->GetCommand(command);

	PxVec3 ModelY_up = command->transform.p + PxVec3(0.f, 1.f, 0.f);
	PxTransform NewTransform = PxTransform(ModelY_up, command->transform.q);
	_matrix worldMat = 	CPhysXComponent::Convert_PxTrans_To_DxMat(NewTransform);
	m_pTransformCom->Set_WorldMatrix(worldMat);

}

HRESULT CHoverboard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
			return E_FAIL;

		m_pShaderCom->Begin(11);

		m_pModelCom->Render(i);
	}
	return S_OK;

	
}

HRESULT CHoverboard::Add_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hoverboard"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Physx */
	//landObjDesc.mWorldMatrix._41 = 75.f;
	//landObjDesc.mWorldMatrix._42 = 523.f;
	//landObjDesc.mWorldMatrix._43 = 98.f;
	//landObjDesc.mWorldMatrix._44 = 1.f;
	CPhysXComponent_Vehicle::VEHICLE_COMMAND command;
	XMStoreFloat4x4(&command.initTransform, m_matWorld);
	command.wheelConfig = WheelConfiguration(2, 2);
	//command.initTransform = XMMatrixTranslation(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Vehicle"),
		TEXT("Com_PhysX_Vehicle"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &command)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_DisolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CHoverboard::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHoverboard::Add_PxActor()
{
	

	
	return S_OK;
}

CHoverboard* CHoverboard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHoverboard* pInstance = new CHoverboard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHoverboard");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CHoverboard::Clone(void* pArg)
{
	CHoverboard* pInstance = new CHoverboard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHoverboard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHoverboard::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pDisolveTextureCom);
}

#include "stdafx.h"
#include "CHoverboard.h"


#include"GameInstance.h"



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
		m_vPosition = InfoDesc.vPosition;

	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (FAILED(Add_PxActor()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.f, 522.f, 98.f, 1.0f));

	return S_OK;
}

void CHoverboard::Priority_Tick(_float fTimeDelta)
{
}

void CHoverboard::Tick(_float fTimeDelta)
{
	
	
	CPhysXComponent_Vehicle::VEHICLE_COMMAND* command;
	m_pPhysXCom->GetCommand(command);
	PxVec3 velocity = m_pPhysXCom->GetRigidBody()->getLinearVelocity();
	//PxVec3 velocity = m_pPhysXCom->Get_Actor()
	PxVehicleSteerCommandResponseParams* steerResponse;
	m_pPhysXCom->GetSteerRespon(steerResponse);

	m_bIsMoving = KEY_HOLD(DIK_W) || KEY_HOLD(DIK_S);
	if (m_bIsMoving)
	{
	
		steerResponse->maxResponse = XMConvertToRadians(45.f); // ¿ø·¡ °ª 
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

	if (KEY_HOLD(DIK_W))
	{
		command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
		command->throttle = 0.3f;
		command->brake = 0.0f;
		command->handbrake = 0.0f;
	}
	else if (KEY_HOLD(DIK_S))
	{
		command->gear = PxVehicleDirectDriveTransmissionCommandState::eREVERSE;
		command->throttle = 0.1f;
		command->brake = 0.0f;
		command->handbrake = 0.0f;
	}
	else
	{
		command->Reset();
	}


	if (KEY_HOLD(DIK_A))
	{
		if(m_bIsMoving)
			command->steer = -1.0f;
		else
		{
			command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
			command->throttle = 0.1f;
			command->steer = -1.0f;
		}

	}
	if (KEY_HOLD(DIK_D))
	{
		if (m_bIsMoving)
			command->steer = 1.0f;
		else
		{
			command->gear = PxVehicleDirectDriveTransmissionCommandState::eFORWARD;
			command->throttle = 0.1f;
			command->steer = 1.0f;
		}



		//command->steer = 1.0f;
	}
	if (KEY_HOLD(DIK_SPACE))
	{
		command->brake = 1.0f;
		
		command->throttle = 0.0f;
	}
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

		m_pShaderCom->Begin(0);

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
	XMStoreFloat4x4(&command.initTransform, XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z));
	command.WheelCount = 4;
	//command.initTransform = XMMatrixTranslation(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Vehicle"),
		TEXT("Com_PhysX_Vehicle"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &command)))
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
}

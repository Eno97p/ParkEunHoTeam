#include "stdafx.h"
#include "..\Public\PretorianSword.h"
#include "GameInstance.h"
#include "Player.h"

CPretorianSword::CPretorianSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CPretorianSword::CPretorianSword(const CPretorianSword& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CPretorianSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPretorianSword::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	m_fDamage = 8.f;

	return S_OK;
}

void CPretorianSword::Priority_Tick(_float fTimeDelta)
{
	if (*m_pCurWeapon != CPlayer::WEAPON_PRETORIANSWORD) return;

	if (*m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		m_fDamage = 30.f;
	}
	else
	{
		m_fDamage = 8.f;
	}
}

void CPretorianSword::Tick(_float fTimeDelta)
{
	if (*m_pCurWeapon != CPlayer::WEAPON_PRETORIANSWORD) return;

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
			m_eDisolveType = TYPE_INCREASE;
		}
		break;
	default:
		break;
	}

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	_matrix vMatrix = m_pTransformCom->Get_WorldMatrix() * SocketMatrix;
	// 달릴 때 칼 위치 맞추기 위해 보정
	if (*m_pState == CPlayer::STATE_RUN || *m_pState == CPlayer::STATE_WALK)
	{
		vMatrix.r[3].m128_f32[0] -= 0.08f;
		vMatrix.r[3].m128_f32[1] -= 0.32f;
		vMatrix.r[3].m128_f32[2] += 0.07f;
	}
	XMStoreFloat4x4(&m_WorldMatrix, vMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	m_pSpecialColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (*m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		Generate_Trail(5);
	}
	else
	{
		Generate_Trail(7);
	}

	if (*m_pState == CPlayer::STATE_JUMPATTACK || *m_pState == CPlayer::STATE_JUMPATTACK_LAND ||
		*m_pState == CPlayer::STATE_SPECIALATTACK4 || *m_pState == CPlayer::STATE_RCHARGEATTACK || *m_pState == CPlayer::STATE_RUNLATTACK1 ||
		*m_pState == CPlayer::STATE_RUNLATTACK2 || *m_pState == CPlayer::STATE_ROLL || *m_pState == CPlayer::STATE_DASH ||
		*m_pState == CPlayer::STATE_DASH_FRONT || *m_pState == CPlayer::STATE_DASH_BACK || *m_pState == CPlayer::STATE_DASH_LEFT ||
		*m_pState == CPlayer::STATE_DASH_RIGHT)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}
}

void CPretorianSword::Late_Tick(_float fTimeDelta)
{
	if (*m_pCurWeapon != CPlayer::WEAPON_PRETORIANSWORD) return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);
	if (m_pGameInstance->Get_MoveShadow())
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	}
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
		m_pGameInstance->Add_DebugComponent(m_pSpecialColliderCom);
	}

#endif
}

HRESULT CPretorianSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CPretorianSword::Render_Reflection()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_float4x4 ViewMatrix;
	const _float4x4* matCam = m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	// 원래 뷰 행렬 로드
	XMMATRIX mOriginalView = XMLoadFloat4x4(matCam);

	// 카메라 위치 추출
	XMVECTOR vCamPos = XMVector3Transform(XMVectorZero(), mOriginalView);

	// 바닥 평면의 높이 (예: Y = 0)
	float floorHeight = 521.9f;

	// 반사된 카메라 위치 계산 (Y 좌표만 반전)
	XMVECTOR vReflectedCamPos = vCamPos;
	vReflectedCamPos = XMVectorSetY(vReflectedCamPos, 2 * floorHeight - XMVectorGetY(vCamPos));

	// 카메라 방향 벡터 추출
	XMVECTOR vCamLook = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 0, 1, 0), mOriginalView) - vCamPos);
	XMVECTOR vCamUp = XMVector3Normalize(XMVector3Transform(XMVectorSet(0, 1, 0, 0), mOriginalView) - vCamPos);

	// 반사된 카메라 방향 벡터 계산
	XMVECTOR vReflectedCamLook = XMVectorSetY(vCamLook, -XMVectorGetY(vCamLook));
	XMVECTOR vReflectedCamUp = XMVectorSetY(vCamUp, -XMVectorGetY(vCamUp));

	// 반사된 뷰 행렬 생성
	XMMATRIX mReflectedView = XMMatrixLookToLH(vReflectedCamPos, vReflectedCamLook, vReflectedCamUp);

	// 변환된 행렬 저장
	XMStoreFloat4x4(&ViewMatrix, mReflectedView);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(5);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CPretorianSword::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4 fPos = m_pGameInstance->Get_PlayerPos();

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(fPos.x, fPos.y + 10.f, fPos.z - 10.f, 1.f), XMVectorSet(fPos.x, fPos.y, fPos.z, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPretorianSword::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

CCollider* CPretorianSword::Get_Collider()
{
	if (*m_pState == CPlayer::STATE_SPECIALATTACK4)
	{
		return m_pSpecialColliderCom;
	}
	else
	{
		return m_pColliderCom;
	}
}

void CPretorianSword::Set_Active(_bool isActive)
{
	if (m_bIsActive == false && isActive == true)
	{
		if (*m_pState != CPlayer::STATE_SPECIALATTACK4)
		{
			m_pGameInstance->Disable_Echo();
			m_pGameInstance->Play_Effect_Sound(TEXT("Slash2.ogg"), SOUND_PLAYER);
		}
		m_GenerateTrail = true;
	}
	m_bIsActive = isActive;
}

HRESULT CPretorianSword::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(1.5f, 2.f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc2{};
	ColliderDesc2.eType = CCollider::TYPE_OBB;
	ColliderDesc2.vExtents = _float3(7.5f, 10.f, 12.5f);
	ColliderDesc2.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc2.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_pSpecialColliderCom), &ColliderDesc2)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PretorianSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPretorianSword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;
	_float4 fDisolveColor = _float4(0.47f, 0.93f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveColor", &fDisolveColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CPretorianSword* CPretorianSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPretorianSword* pInstance = new CPretorianSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPretorianSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPretorianSword::Clone(void* pArg)
{
	CPretorianSword* pInstance = new CPretorianSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPretorianSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPretorianSword::Free()
{
	__super::Free();

	Safe_Release(m_pSpecialColliderCom);
}

#include "stdafx.h"
#include "..\Public\Body_Mantari.h"

#include "GameInstance.h"
#include "Mantari.h"
#include "Weapon.h"
#include "EffectManager.h"

CBody_Mantari::CBody_Mantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Mantari::CBody_Mantari(const CBody_Mantari& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Mantari::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Mantari::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(rand() % 20, true));

	return S_OK;
}

void CBody_Mantari::Priority_Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 0.5f;
		break;
	default:
		break;
	}
}

void CBody_Mantari::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	*m_pCanCombo = false;
	if (*m_pState == CMantari::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 0.9f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_PARRIED)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
		fAnimSpeed = 0.7f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_WALKLEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 28;
		fAnimSpeed = 1.3f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_WALKRIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 29;
		fAnimSpeed = 1.8f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_WALKFRONT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 27;
		fAnimSpeed = 1.3f; // 1.3
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_WALKBACK)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 26;
		fAnimSpeed = 1.3f;
		m_pModelCom->Set_LerpTime(1.2);
	}
	else if (*m_pState == CMantari::STATE_JUMPATTACK)
	{
		if (m_iPastAnimIndex == 20)
		{
			if (m_pModelCom->Check_CurDuration(0.01f))
			{
				_matrix Mat = XMLoadFloat4x4(&m_WorldMatrix);
				_vector vPos = Mat.r[3];
				_vector vLook = XMVector4Normalize(Mat.r[2]);
				_float4 vStartPos;
				XMStoreFloat4(&vStartPos, vPos);
				vStartPos.y += 1.f;

				EFFECTMGR->Generate_Particle(13, vStartPos, nullptr, XMVectorZero(), 0.f, vLook);
			}
		}




		if (m_iPastAnimIndex < 17 || m_iPastAnimIndex > 25)
		{
			m_iPastAnimIndex = 17;
		}
		if (m_iPastAnimIndex == 24)
		{
			fAnimSpeed = 1.4f; // 1.3
			if (m_pModelCom->Check_CurDuration(0.55))
			{
				_matrix Mat = XMLoadFloat4x4(&m_WorldMatrix);
				_vector vLook = XMVector4Normalize(Mat.r[2]);
				_vector vPos = Mat.r[3] + vLook * 1.2f;
				_float4 vStartPos;
				XMStoreFloat4(&vStartPos, vPos);
				EFFECTMGR->Generate_Particle(23, vStartPos);
				EFFECTMGR->Generate_Particle(23, vStartPos, nullptr, XMVectorSet(0.f,1.f,0.f,0.f), 90.f);
			}
		}
		else if(m_iPastAnimIndex == 22)
			fAnimSpeed = 2.3f; // 2
		else
			fAnimSpeed = 1.f; // 1


		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(1.2);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 1.8f && m_fDamageTiming < 2.2f) // m_fDamageTiming > 2.f && m_fDamageTiming < 2.2f
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CMantari::STATE_ATTACK1)
	{
		if (m_iPastAnimIndex < 0 || m_iPastAnimIndex > 1)
		{
			m_iPastAnimIndex = 0;
		}
		
		if(m_iPastAnimIndex == 0)
			fAnimSpeed = 0.75f; // 0.75
		else
			fAnimSpeed = 1.7f; // 1.7
			
		if (m_iPastAnimIndex == 1) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(1.3);
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 0.8f && m_fDamageTiming < 1.0f)
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CMantari::STATE_ATTACK2)
	{
		if (m_iPastAnimIndex < 2 || m_iPastAnimIndex > 4)
		{
			m_iPastAnimIndex = 2;
			fAnimSpeed = 0.9f;
		}
		if(m_iPastAnimIndex == 3)
			fAnimSpeed = 1.5f; // 1.2 
		if (m_iPastAnimIndex == 4) *m_pCanCombo = true;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(1.3);
		if (m_iPastAnimIndex == 3)
		{ 
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CMantari::STATE_ATTACK3)
	{
		if (m_pModelCom->Check_CurDuration(0.20) && m_iPastAnimIndex == 6)
		{
			_float4 PartPos = XM3TO4(m_pWeapon->Get_Collider_Center());
			PartPos.y = m_WorldMatrix._42;
			EFFECTMGR->Generate_Particle(17, PartPos);
			EFFECTMGR->Generate_Particle(18, PartPos);
			EFFECTMGR->Generate_Particle(42, PartPos);
			EFFECTMGR->Generate_Particle(43, PartPos);
			
		}


		if (m_iPastAnimIndex < 5 || m_iPastAnimIndex > 6)
		{
			m_iPastAnimIndex = 5;
		}
		if (m_iPastAnimIndex == 6)
		{
			*m_pCanCombo = true;
			fAnimSpeed = 1.4f; // 1.2
		}else
			fAnimSpeed = 0.9f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;
		m_pModelCom->Set_LerpTime(1.3);
		if (m_iPastAnimIndex > 5)
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CMantari::STATE_CIRCLEATTACK)
	{
		if (m_iPastAnimIndex < 7 || m_iPastAnimIndex > 10)
		{
			m_iPastAnimIndex = 7;
		}
		if(m_iPastAnimIndex == 9)
			fAnimSpeed = 1.5f; // 1.3
		else if(m_iPastAnimIndex == 7)
			fAnimSpeed = 2.f;
		else
			fAnimSpeed = 1.f;
		m_pModelCom->Set_LerpTime(1.3);
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = m_iPastAnimIndex;

		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 0.8f && m_fDamageTiming < 1.f)
		{
			m_pWeapon->Set_Active();
		}
		else
		{
			m_pWeapon->Set_Active(false);
		}
	}
	else if (*m_pState == CMantari::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		fAnimSpeed = 0.8f;
		m_pModelCom->Set_LerpTime(1.3);
	}
	else if (*m_pState == CMantari::STATE_REVIVE)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 12;
		fAnimSpeed = 1.f;
	}

	if (*m_pState == CMantari::STATE_JUMPATTACK)
	{
		m_bMotionBlur = true;
	}
	else
	{
		m_bMotionBlur = false;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	// 여러 애니메이션을 이어서 재생할 때는 보간하지 않음
	if ((m_iPastAnimIndex >= 8 && m_iPastAnimIndex < 11) || (m_iPastAnimIndex >= 18 && m_iPastAnimIndex < 26) ||
		(m_iPastAnimIndex >= 1 && m_iPastAnimIndex < 2) || (m_iPastAnimIndex >= 3 && m_iPastAnimIndex < 5) ||
		(m_iPastAnimIndex >= 6 && m_iPastAnimIndex < 7))
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_bAnimFinished = false;
	_bool animFinished = m_pModelCom->Get_AnimFinished();
	if (animFinished)
	{
		if (AnimDesc.iAnimIndex >= 7 && AnimDesc.iAnimIndex < 10)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 0 && AnimDesc.iAnimIndex < 1)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 2 && AnimDesc.iAnimIndex < 4)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 5 && AnimDesc.iAnimIndex < 6)
		{
			m_iPastAnimIndex++;
		}
		else if (AnimDesc.iAnimIndex >= 17 && AnimDesc.iAnimIndex < 25)
		{
			m_iPastAnimIndex++;
		}
		else
		{
			m_iPastAnimIndex = 99999;
			m_bAnimFinished = true;
		}
	}
	if (m_bAnimFinished)
	{
		m_pWeapon->Set_Active(false);
		m_fDamageTiming = 0.f;
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Mantari::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONDECAL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_REFLECTION, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//}


}

HRESULT CBody_Mantari::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}

		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}

#pragma region 모션블러
	m_PrevWorldMatrix = m_WorldMatrix;
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러

	return S_OK;
}

HRESULT CBody_Mantari::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Mantari::Render_Reflection()
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

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(10);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Mantari::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Mantari::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mantari"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Mantari::Bind_ShaderResources()
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
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CBody_Mantari* CBody_Mantari::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Mantari* pInstance = new CBody_Mantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Mantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Mantari::Clone(void* pArg)
{
	CBody_Mantari* pInstance = new CBody_Mantari(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Mantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Mantari::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
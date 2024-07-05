#include "Body_Homonculus.h"

#include "GameInstance.h"
#include "Homonculus.h"

CBody_Homonculus::CBody_Homonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Homonculus::CBody_Homonculus(const CBody_Homonculus& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Homonculus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Homonculus::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(11, true));

	return S_OK;
}

void CBody_Homonculus::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Homonculus::Tick(_float fTimeDelta)
{
	Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Homonculus::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

HRESULT CBody_Homonculus::Render()
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
			/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;*/
		}
		else if (i == 1)
		{
			/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
				return E_FAIL;*/
		}

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Homonculus::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_Homonculus::Render_LightDepth()
{
	return S_OK;
}

HRESULT CBody_Homonculus::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CBody_Homonculus::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBody_Homonculus::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 11, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CHomonculus::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 11;
	}
	else if (*m_pState == CHomonculus::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
	}
	else if (*m_pState == CHomonculus::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 10;
	}
	else if (*m_pState == CHomonculus::STATE_DEFAULTATTACK_1)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
	}
	else if (*m_pState == CHomonculus::STATE_DEFAULTATTACK_2)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
	}
	else if (*m_pState == CHomonculus::STATE_DEFAULTATTACK_3)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
	}
	else if (*m_pState == CHomonculus::STATE_DOWNATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
	}
	else if (*m_pState == CHomonculus::STATE_FULLATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
	}
	else if (*m_pState == CHomonculus::STATE_GO)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
	}
	else if (*m_pState == CHomonculus::STATE_LEFT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 13;
	}
	else if (*m_pState == CHomonculus::STATE_RIGHT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 14;
	}
	else if (*m_pState == CHomonculus::STATE_PARRY)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
	}
	else if (*m_pState == CHomonculus::STATE_WAKEUP)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
	}


	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false;
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);
}

CBody_Homonculus* CBody_Homonculus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Homonculus* pInstance = new CBody_Homonculus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Homonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Homonculus::Clone(void* pArg)
{
	CBody_Homonculus* pInstance = new CBody_Homonculus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Homonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Homonculus::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}

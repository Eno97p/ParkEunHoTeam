#include "Body_LGGun.h"

#include "GameInstance.h"
#include "Legionnaire_Gun.h"

CBody_LGGun::CBody_LGGun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_LGGun::CBody_LGGun(const CBody_LGGun& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_LGGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_LGGun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(4, true));

	return S_OK;
}

void CBody_LGGun::Priority_Tick(_float fTimeDelta)
{
}

void CBody_LGGun::Tick(_float fTimeDelta)
{
	Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_LGGun::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

HRESULT CBody_LGGun::Render()
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

HRESULT CBody_LGGun::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_LGGun::Render_LightDepth()
{
	return S_OK;
}

HRESULT CBody_LGGun::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
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

HRESULT CBody_LGGun::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBody_LGGun::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 16, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CLegionnaire_Gun::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 4;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 12;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_WAKEUP)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_KNOCKDOWN)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_WALK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 14;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_RUN)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_BACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 13;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_LEFT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_RIGHT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 17;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_GUNATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_CASTING)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_MELEEATTACK1)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_MELEEATTACK2)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 8;
	}
	else if (*m_pState == CLegionnaire_Gun::STATE_MELEEATTACK3)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false;
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);
}

CBody_LGGun* CBody_LGGun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_LGGun* pInstance = new CBody_LGGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_LGGun::Clone(void* pArg)
{
	CBody_LGGun* pInstance = new CBody_LGGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_LGGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_LGGun::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}

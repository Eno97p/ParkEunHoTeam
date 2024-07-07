#include "Body_Ghost.h"

#include "GameInstance.h"
#include "Ghost.h"
#include "Weapon.h"

CBody_Ghost::CBody_Ghost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Ghost::CBody_Ghost(const CBody_Ghost& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Ghost::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Ghost::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(17, true));

	return S_OK;
}

void CBody_Ghost::Priority_Tick(_float fTimeDelta)
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

void CBody_Ghost::Tick(_float fTimeDelta)
{
	Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CBody_Ghost::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
	if (m_isAnimFinished)
	{
		m_fDamageTiming = 0.f;
	}
}

HRESULT CBody_Ghost::Render()
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

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY)))
		//	return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Ghost::Render_Distortion()
{
	return S_OK;
}

HRESULT CBody_Ghost::Render_LightDepth()
{
	return S_OK;
}

HRESULT CBody_Ghost::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ghost"),
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

HRESULT CBody_Ghost::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CBody_Ghost::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 17, true };
	_float fAnimSpeed = 1.f;

	m_pWeapon->Set_Active(false);
	if (*m_pState == CGhost::STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 17;
	}
	else if (*m_pState == CGhost::STATE_MOVE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 18;
	}
	else if (*m_pState == CGhost::STATE_DEAD)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
	}
	else if (*m_pState == CGhost::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
	}
	else if (*m_pState == CGhost::STATE_PARRIED)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		fAnimSpeed = 1.f;
	}
	else if (*m_pState == CGhost::STATE_DEFAULTATTACK_1)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
	}
	else if (*m_pState == CGhost::STATE_DEFAULTATTACK_2)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
	}
	else if (*m_pState == CGhost::STATE_DEFAULTATTACK_3)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		m_pWeapon->Set_Active();
	}
	else if (*m_pState == CGhost::STATE_DEFAULTATTACK_4)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
	}
	else if (*m_pState == CGhost::STATE_DOWNATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		m_fDamageTiming += fTimeDelta;
		if (m_fDamageTiming > 0.9f && m_fDamageTiming < 1.2f)
		{
			m_pWeapon->Set_Active();
		}
	}
	else if (*m_pState == CGhost::STATE_LEFT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 19;
	}
	else if (*m_pState == CGhost::STATE_RIGHT)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 20;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true;
	if (AnimDesc.iAnimIndex == 1 || AnimDesc.iAnimIndex == 2 || AnimDesc.iAnimIndex == 3)
	{
		isLerp = false;
	}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);
}

CBody_Ghost* CBody_Ghost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Ghost* pInstance = new CBody_Ghost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Ghost::Clone(void* pArg)
{
	CBody_Ghost* pInstance = new CBody_Ghost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Ghost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Ghost::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

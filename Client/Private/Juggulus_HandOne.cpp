#include "Juggulus_HandOne.h"

#include "GameInstance.h"
#include "Boss_Juggulus.h"

CJuggulus_HandOne::CJuggulus_HandOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CJuggulus_HandOne::CJuggulus_HandOne(const CJuggulus_HandOne& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CJuggulus_HandOne::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJuggulus_HandOne::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(3, true));

	return S_OK;
}

void CJuggulus_HandOne::Priority_Tick(_float fTimeDelta)
{
}

void CJuggulus_HandOne::Tick(_float fTimeDelta)
{
	//Change_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

void CJuggulus_HandOne::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	//m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

HRESULT CJuggulus_HandOne::Render()
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

HRESULT CJuggulus_HandOne::Render_Distortion()
{
	return S_OK;
}

HRESULT CJuggulus_HandOne::Render_LightDepth()
{
	return S_OK;
}

HRESULT CJuggulus_HandOne::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandOne"),
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

HRESULT CJuggulus_HandOne::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CJuggulus_HandOne::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 3, true };
	_float fAnimSpeed = 1.f;

	if (*m_pState == CBoss_Juggulus::STATE_HANDONE_ATTACK)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 16;
		fAnimSpeed = 1.f;
	}


	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = true; // false
	//// 여러 애니메이션을 재생할 때 마지막 애니메이션은 보간 필요
	//if (m_iPastAnimIndex == 37 || m_iPastAnimIndex == 28 || (m_iPastAnimIndex == 0 && *m_pState != CPlayer::STATE_ATTACK))
	//{
	//	isLerp = true;
	//}
	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);
}

CJuggulus_HandOne* CJuggulus_HandOne::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJuggulus_HandOne* pInstance = new CJuggulus_HandOne(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CJuggulus_HandOne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJuggulus_HandOne::Clone(void* pArg)
{
	CJuggulus_HandOne* pInstance = new CJuggulus_HandOne(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CJuggulus_HandOne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJuggulus_HandOne::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}

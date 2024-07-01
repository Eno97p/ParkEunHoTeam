#include "Juggulus_Hammer.h"

#include "GameInstance.h"

CJuggulus_Hammer::CJuggulus_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CJuggulus_Hammer::CJuggulus_Hammer(const CJuggulus_Hammer& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CJuggulus_Hammer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJuggulus_Hammer::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-80.f));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -6.5f, -1.f, 1.f));

	return S_OK;
}

void CJuggulus_Hammer::Priority_Tick(_float fTimeDelta)
{
}

void CJuggulus_Hammer::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CJuggulus_Hammer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CJuggulus_Hammer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CJuggulus_Hammer::Render_LightDepth()
{
	return S_OK;
}

HRESULT CJuggulus_Hammer::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHammer"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CJuggulus_Hammer::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CJuggulus_Hammer* CJuggulus_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJuggulus_Hammer* pInstance = new CJuggulus_Hammer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CJuggulus_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJuggulus_Hammer::Clone(void* pArg)
{
	CJuggulus_Hammer* pInstance = new CJuggulus_Hammer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CJuggulus_Hammer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJuggulus_Hammer::Free()
{
	__super::Free();
}

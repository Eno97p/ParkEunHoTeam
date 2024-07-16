#include "stdafx.h"
#include "..\Public\Decal.h"

#include "GameInstance.h"
#include "ComputeShader_Buffer.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(155.f, 522.f, 113.f, 1.f));

	return S_OK;
}

void CDecal::Priority_Tick(_float fTimeDelta)
{
	//s
}

void CDecal::Tick(_float fTimeDelta)
{
}

void CDecal::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DECAL, this);
}

HRESULT CDecal::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	_vector v;
	_matrix mat = XMMatrixInverse(&v, m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

HRESULT CDecal::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Decal"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecal::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

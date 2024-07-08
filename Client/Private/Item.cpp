#include "stdafx.h"
#include "..\Public\Item.h"

#include "GameInstance.h"
#include "PartObject.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.03f, 0.03f, 0.03f);

	Set_Texture();

	return S_OK;
}

void CItem::Set_Texture()
{

	m_pTextureTransformCom->Scaling(0.3f, 0.3f, 0.3f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 524.f, 98.f, 1.f));
	m_pTextureTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pTextureTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
	m_pTextureTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		XMVectorGetX(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)),
		XMVectorGetY(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)) - 0.1f,
		XMVectorGetZ(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));
}

void CItem::Priority_Tick(_float fTimeDelta)
{
}

void CItem::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);

	m_pTextureTransformCom->Set_State(CTransform::STATE_LOOK, m_pGameInstance->Get_CamPosition() - m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTextureTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pTextureTransformCom->Get_State(CTransform::STATE_LOOK)));
	m_pTextureTransformCom->Set_State(CTransform::STATE_UP, XMVector3Cross(m_pTextureTransformCom->Get_State(CTransform::STATE_LOOK), m_pTextureTransformCom->Get_State(CTransform::STATE_RIGHT)));
	m_pTextureTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
}

void CItem::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CItem::Render()
{
	if (FAILED(m_pTextureTransformCom->Bind_ShaderResource(m_pTextureShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTextureShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pTextureShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CItem::Render_Bloom()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pTextureShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTextureShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pTextureShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
}

HRESULT CItem::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CItem::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_TextureShader"), reinterpret_cast<CComponent**>(&m_pTextureShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTextureTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTextureTransformCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureShaderCom);
	Safe_Release(m_pTextureTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

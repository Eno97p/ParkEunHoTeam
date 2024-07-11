#include "stdafx.h"
#include "..\Public\Item.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"

#include "ItemData.h"
#include "Inventory.h"
#include "UIGroup_DropItem.h"

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

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

	return S_OK;
}

void CItem::Set_Texture()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.f, 524.f, 98.f, 1.f));
	m_pTextureTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pTextureTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
	m_pTextureTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		XMVectorGetX(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)),
		XMVectorGetY(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)) + 0.1f,
		XMVectorGetZ(m_pTextureTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));
}

void CItem::Priority_Tick(_float fTimeDelta)
{
}

void CItem::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);

	m_pTextureTransformCom->BillBoard();

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
	{
		// Inventory
		// ItemData를 생성해서 Inventory에 넣어주고 Player는 Inventory를 참조해서(싱글톤) UI에 띄우거나 상호작용 등?
		CInventory::GetInstance()->Add_DropItem();

		//CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
		//pUIDesc.eLevel = LEVEL_STATIC;
		//pUIDesc.wszTextureName = pItemData->Get_TextureName();
		//m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

		// 아이템 획득 로직
		m_pGameInstance->Erase(this);
	}
}

void CItem::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
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
	if (FAILED(m_pTextureTransformCom->Bind_ShaderResource(m_pTextureShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pTextureShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTextureShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pTextureShaderCom->Begin(3);

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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

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
	m_pTextureTransformCom->Initialize(nullptr);

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(7.f, 7.f, 7.f);
	ColliderDesc.vCenter = _float3(0.f, 5.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
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

	m_pPlayer = nullptr;

	Safe_Release(m_pTextureShaderCom);
	Safe_Release(m_pTextureTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
}

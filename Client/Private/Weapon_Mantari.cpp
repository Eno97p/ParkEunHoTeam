#include "stdafx.h"
#include "..\Public\Weapon_Mantari.h"

#include "GameInstance.h"
#include "Player.h"

CWeapon_Mantari::CWeapon_Mantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Mantari::CWeapon_Mantari(const CWeapon_Mantari& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWeapon_Mantari::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Mantari::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CWeapon_Mantari::Priority_Tick(_float fTimeDelta)
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

void CWeapon_Mantari::Tick(_float fTimeDelta)
{

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

#ifdef _DEBUG
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
#endif

	// 몬스터 무기와 플레이어 충돌처리
	if (Get_Active())
	{
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{
			if (m_pPlayer->Get_State() == CPlayer::STATE_PARRY)
			{
				m_bIsParried = true;
				m_pPlayer->Parry_Succeed();
			}
			else
			{
				m_pPlayer->PlayerHit(10);
			}

		}
	}
}

void CWeapon_Mantari::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
#endif
}

HRESULT CWeapon_Mantari::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CWeapon_Mantari::Render_LightDepth()
{

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	//_float4x4		ViewMatrix, ProjMatrix;

	///* 광원 기준의 뷰 변환행렬. */
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 30.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
	//	return E_FAIL;

	//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//	m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
	//		return E_FAIL;

	//	m_pShaderCom->Begin(1);

	//	m_pModelCom->Render(i);
	//}

	return S_OK;
}

HRESULT CWeapon_Mantari::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.4f, 1.5f, 0.4f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Mantari"),
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

HRESULT CWeapon_Mantari::Bind_ShaderResources()
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

CWeapon_Mantari* CWeapon_Mantari::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Mantari* pInstance = new CWeapon_Mantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWeapon_Mantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Mantari::Clone(void* pArg)
{
	CWeapon_Mantari* pInstance = new CWeapon_Mantari(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWeapon_Mantari");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Mantari::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}

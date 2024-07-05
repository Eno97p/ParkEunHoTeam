#include "stdafx.h"
#include "..\Public\CorruptedSword.h"

#include "GameInstance.h"

CCorruptedSword::CCorruptedSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CCorruptedSword::CCorruptedSword(const CCorruptedSword& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CCorruptedSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCorruptedSword::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	return S_OK;
}

void CCorruptedSword::Priority_Tick(_float fTimeDelta)
{
}

void CCorruptedSword::Tick(_float fTimeDelta)
{

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CCorruptedSword::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCorruptedSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (i == 0)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
				return E_FAIL;
		}
		else if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				return E_FAIL;
		}

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CCorruptedSword::Render_LightDepth()
{

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	//_float4x4		ViewMatrix, ProjMatrix;

	///* ±¤¿ø ±âÁØÀÇ ºä º¯È¯Çà·Ä. */
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

HRESULT CCorruptedSword::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(1.5f, 2.f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CorruptedSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCorruptedSword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CCorruptedSword* CCorruptedSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCorruptedSword* pInstance = new CCorruptedSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCorruptedSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCorruptedSword::Clone(void* pArg)
{
	CCorruptedSword* pInstance = new CCorruptedSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCorruptedSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCorruptedSword::Free()
{
	__super::Free();
}

#include "stdafx.h"
#include "..\Public\CircleSphere.h"

#include "GameInstance.h"
#include "Player.h"

CCircleSphere::CCircleSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCircleSphere::CCircleSphere(const CCircleSphere& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCircleSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCircleSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 525.f, 85.f, 1.f)); // Test
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_fPlayerY = XMVectorGetY(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	return S_OK;
}

void CCircleSphere::Priority_Tick(_float fTimeDelta)
{
	m_fShootDelay -= fTimeDelta;
}

void CCircleSphere::Tick(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_fShootDelay > 0.f)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(3.f));	
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else
	{
		if (!m_bLookatPlayer)
		{
			m_pTransformCom->Set_Speed(40.f);
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_bLookatPlayer = true;
		}
		if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < m_fPlayerY)
		{
			// ¿©±â¼­ Æø¹ß ÀÌÆåÆ® Àç»ý
			m_pGameInstance->Erase(this);
		}
	}
}

void CCircleSphere::Late_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (m_pColliderCom->Intersect(m_pPlayer->Get_Collider()) == CCollider::COLL_START)
	{
		m_pPlayer->PlayerHit(10);
		// ¿©±â¼­ Æø¹ß ÀÌÆåÆ® Àç»ý
		m_pGameInstance->Erase(this);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CCircleSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCircleSphere::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCircleSphere::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCircleSphere* CCircleSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCircleSphere* pInstance = new CCircleSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCircleSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCircleSphere::Clone(void* pArg)
{
	CCircleSphere* pInstance = new CCircleSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCircleSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCircleSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}

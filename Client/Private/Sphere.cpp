#include "stdafx.h"
#include "..\Public\Sphere.h"

#include "GameInstance.h"
#include "Player.h"
#include "EffectManager.h"

#include "Monster.h"


CSphere::CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CSphere::CSphere(const CSphere& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(159.f, 538.f, 85.f, 1.f)); // Test
	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_fPlayerY = XMVectorGetY(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->LookAt(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);

	m_pJuggulus = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Boss")).front());
	return S_OK;
}

void CSphere::Priority_Tick(_float fTimeDelta)
{
}

void CSphere::Tick(_float fTimeDelta)
{
	
	m_pTransformCom->Go_Straight(fTimeDelta);
	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < m_fPlayerY)
	{
		// ���⼭ ���� ����Ʈ ���
		_matrix Mat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
		_vector vPos = Mat.r[3];
		_float4 vStartPos;
		XMStoreFloat4(&vStartPos, vPos);
		EFFECTMGR->Generate_Particle(14, vStartPos);
		EFFECTMGR->Generate_Particle(15, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
		m_pGameInstance->Erase(this);
	}
}

void CSphere::Late_Tick(_float fTimeDelta)
{
	_float4 fPos;
	XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsParried)
	{
		m_eColltype = m_pColliderCom->Intersect(m_pPlayer->Get_Collider());
		if (m_eColltype == CCollider::COLL_START)
		{

			if (m_pPlayer->Get_Parry())
			{
				EFFECTMGR->Generate_Particle(4, fPos, nullptr, XMVectorZero(), 0.f, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				EFFECTMGR->Generate_Particle(5, fPos);
				m_bIsParried = true;
				m_pPlayer->Parry_Succeed();
				// ���� Ÿ����
				CTransform* bossTransform = dynamic_cast<CTransform*>(m_pJuggulus->Get_Component(TEXT("Com_Transform")));
				_vector vBossPos = bossTransform->Get_State(CTransform::STATE_POSITION);
				vBossPos.m128_f32[1] += 30.f;
				m_pTransformCom->LookAt(vBossPos);
			}
			else
			{
				m_pPlayer->PlayerHit(10);
				// ���⼭ ���� ����Ʈ ���
				_matrix Mat = XMLoadFloat4x4(m_pTransformCom->Get_WorldFloat4x4());
				_vector vPos = Mat.r[3];
				_float4 vStartPos;
				XMStoreFloat4(&vStartPos, vPos);
				EFFECTMGR->Generate_Particle(14, vStartPos);
				EFFECTMGR->Generate_Particle(15, vStartPos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
				m_pGameInstance->Erase(this);
			}

		}

	}
	else
	{
		if (m_pColliderCom->Intersect(m_pJuggulus->Get_Collider()) == CCollider::COLL_START)
		{
			m_pJuggulus->Add_Hp(-10);
			// ���⼭ ���� ����Ʈ ���
			m_pGameInstance->Erase(this);
		}
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSphere::Render()
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

HRESULT CSphere::Add_Components()
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

HRESULT CSphere::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CSphere* CSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphere* pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSphere::Clone(void* pArg)
{
	CSphere* pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSphere::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pColliderCom);
}

#include "EventTrigger.h"
#include "Map_Element.h"
#include "GameInstance.h"
#include "Player.h"
#include "Elevator.h"

CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (nullptr != pArg)
	{

		CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		//m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//TRIGGER STATE SET
		m_eTRIGState = (TRIGGER_TYPE)pDesc->TriggerType;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CEventTrigger::Tick(_float fTimeDelta)
{

	

//	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Collider")));
	if (nullptr != pPlayerCollider)
	{
		if (CCollider::COLL_START == m_pColliderCom->Intersect(pPlayerCollider))
		{
			switch (m_eTRIGState)
			{
			case TRIG_TUTORIAL_BOSSENCOUNTER:
			{
				CMap_Element::MAP_ELEMENT_DESC pDesc = {};
				_matrix vMat = { 1.4f, 0.f, 0.f, 0.f,
				0.f, 10.f, 0.f, 0.f,
				0.f, 0.f, 1.4f, 0.f,
				154.009f, 531.828f, 96.989f, 1.f };
				XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
				pDesc.wstrModelName = TEXT("Prototype_Component_Model_BasicDonut");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_FakeWall"), &pDesc);
			}
			break;
			case TRIG_JUGGLAS_SPAWNSECONDROOM:
			{
				m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), 0));
				CMap_Element::MAP_ELEMENT_DESC pDesc = {};
				_float4x4 vMat;
				XMStoreFloat4x4(&vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
				pDesc.iInstanceCount = 1;
				pDesc.WorldMats.emplace_back(&vMat);
				pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle2");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);
			}
			break;
			case TRIG_JUGGLAS_SPAWNTHIRDROOM:
			{
				//m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), 0));
				CMap_Element::MAP_ELEMENT_DESC pDesc = {};
				_float4x4 vMat;
				XMStoreFloat4x4(&vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
				pDesc.iInstanceCount = 1;
				
				pDesc.WorldMats.emplace_back(&vMat);
				pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle3");
				pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle4");
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);
			}
			break;
			case TRIG_VIEWCHANGE_TTOS:
			{
				m_pGameInstance->Set_MainCamera(2);
			}
			break;
			case TRIG_VIEWCHANGE_STOT:
			{
				m_pGameInstance->Set_MainCamera(1);
			}
			break;
			case TRIG_ASCEND_ELEVATOR:
			{
				dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0))->Ascend(XMVectorSet(-310.f, 69.f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
			}
			break;
			case TRIG_DESCEND_ELEVATOR:
			{
				dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0))->Descend(XMVectorSet(-310.f, 5.6f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
			}
			break;
			default:
				break;
			}
			m_pGameInstance->Erase(this);
		}
	}


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

}

HRESULT CEventTrigger::Render()
{
	return S_OK;
}

HRESULT CEventTrigger::Add_Components(void* pArg)
{

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_AABB;

	// 월드 매트릭스에서 스케일 추출
	_float3 vScale;
	vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	// 기본 크기에 스케일 적용
	ColliderDesc.vExtents = _float3(0.5f * vScale.x, 1.f * vScale.y, 0.5f * vScale.z);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{


	return S_OK;
}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
	CEventTrigger* pInstance = new CEventTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}

#include "Legionnaire_Gun.h"

#include "GameInstance.h"

CLegionnaire_Gun::CLegionnaire_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CLegionnaire_Gun::CLegionnaire_Gun(const CMonster& rhs)
	: CMonster{ rhs }
{
}

HRESULT CLegionnaire_Gun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLegionnaire_Gun::Initialize(void* pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 1.f; // 수정 필요
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_iCurHp = 100;
	m_iState = STATE_IDLE;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;


	return S_OK;
}

void CLegionnaire_Gun::Priority_Tick(_float fTimeDelta)
{
}

void CLegionnaire_Gun::Tick(_float fTimeDelta)
{
	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Tick(fTimeDelta);
}

void CLegionnaire_Gun::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Late_Tick(fTimeDelta);
}

HRESULT CLegionnaire_Gun::Render()
{
	return S_OK;
}

HRESULT CLegionnaire_Gun::Add_Components()
{
	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLegionnaire_Gun::Add_PartObjects()
{
	return S_OK;
}

HRESULT CLegionnaire_Gun::Add_Nodes()
{
	return S_OK;
}

void CLegionnaire_Gun::Check_AnimFinished()
{
}

NodeStates CLegionnaire_Gun::Dead(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::Hit(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::WakeUp(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::KnockDown(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::Idle(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::Walk(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::Run(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::WalkBack(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::WalkLeft(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::WalkRight(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::GunAttack(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::Casting(_float fTimedelta)
{
	return NodeStates();
}

NodeStates CLegionnaire_Gun::MeleeAttack(_float fTimedelta)
{
	return NodeStates();
}

CLegionnaire_Gun* CLegionnaire_Gun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLegionnaire_Gun* pInstance = new CLegionnaire_Gun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLegionnaire_Gun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLegionnaire_Gun::Clone(void* pArg)
{
	CLegionnaire_Gun* pInstance = new CLegionnaire_Gun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLegionnaire_Gun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLegionnaire_Gun::Free()
{
	__super::Free();

	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject.second);
	m_PartObjects.clear();
}

#include "Homonculus.h"

#include "GameInstance.h"


CHomonculus::CHomonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CHomonculus::CHomonculus(const CHomonculus& rhs)
    : CMonster{ rhs }
{
}

HRESULT CHomonculus::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHomonculus::Initialize(void* pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 3.f; // 수정 필요
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

void CHomonculus::Priority_Tick(_float fTimeDelta)
{
}

void CHomonculus::Tick(_float fTimeDelta)
{
	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
}

void CHomonculus::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
}

HRESULT CHomonculus::Render()
{
	return S_OK;
}

HRESULT CHomonculus::Add_Components()
{
	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHomonculus::Add_PartObjects()
{
	// Body


    return S_OK;
}

HRESULT CHomonculus::Add_Nodes()
{
	return S_OK;
}

void CHomonculus::Check_AnimFinished()
{
}

NodeStates CHomonculus::Dead(_float fTimeDelta)
{
    return NodeStates();
}

NodeStates CHomonculus::Hit(_float fTimeDelta)
{
    return NodeStates();
}

NodeStates CHomonculus::Idle(_float fTimeDelta)
{
    return NodeStates();
}

NodeStates CHomonculus::Move(_float fTimeDelta)
{
    return NodeStates();
}

CHomonculus* CHomonculus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHomonculus* pInstance = new CHomonculus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHomonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHomonculus::Clone(void* pArg)
{
	CHomonculus* pInstance = new CHomonculus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHomonculus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHomonculus::Free()
{
	__super::Free();

	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}

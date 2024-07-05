#include "Homonculus.h"

#include "GameInstance.h"
#include "Body_Homonculus.h"

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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, 0.f, 0.f, 1.f));

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
	CPartObject::PARTOBJ_DESC pPartDesc{};
	pPartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pPartDesc.fSpeedPerSec = 0.f;
	pPartDesc.fRotationPerSec = 0.f;
	pPartDesc.pState = &m_iState;
	pPartDesc.eLevel = m_eLevel;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Homonculus"), &pPartDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

    return S_OK;
}

HRESULT CHomonculus::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CHomonculus::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CHomonculus::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Parry"), bind(&CHomonculus::Parry, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CHomonculus::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("FullAtackCool"), 10.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("FullAtackCool"), TEXT("FullAttack"), bind(&CHomonculus::Full_Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("DownAttackCool"), 7.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DownAttackCool"), TEXT("DownAttack"), bind(&CHomonculus::Down_Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("DefaultAttackCool"), 4.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DefaultAttackCool"), TEXT("DefaultAttack"), bind(&CHomonculus::Default_Attack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Move"), bind(&CHomonculus::Move, this, std::placeholders::_1));

	return S_OK;
}

void CHomonculus::Check_AnimFinished()
{
	vector<CGameObject*>::iterator iter = m_PartObjects.begin();
	m_isAnimFinished = dynamic_cast<CBody_Homonculus*>(*iter)->Get_AnimFinished();
}

NodeStates CHomonculus::Dead(_float fTimeDelta)
{
	if (0 >= m_iCurHp)
	{
		m_iState = STATE_DEAD;

		if (m_isAnimFinished)
		{
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CHomonculus::Hit(_float fTimeDelta)
{
	if (m_isHit)
	{
		m_iState = STATE_HIT;

		if (m_isAnimFinished)
		{
			m_isHit = false;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CHomonculus::Parry(_float fTimeDelta)
{
	// 패딩

	return FAILURE;
}

NodeStates CHomonculus::Idle(_float fTimeDelta)
{
	m_iState = STATE_IDLE;

    return SUCCESS;
}

NodeStates CHomonculus::Move(_float fTimeDelta)
{
    return FAILURE;
}

NodeStates CHomonculus::Default_Attack(_float fTimeDelta)
{
	if (STATE_DEFAULTATTACK_2 != m_iState)
	{
		m_isDefaultAttack = true;
	}

	if (m_isDefaultAttack)
	{
		if (STATE_DEFAULTATTACK_1 != m_iState && STATE_DEFAULTATTACK_2 != m_iState)
		{
			m_iState = STATE_DEFAULTATTACK_1;
			m_isAnimFinished = false;
		}

		if (m_isAnimFinished)
		{
			if (STATE_DEFAULTATTACK_1 == m_iState)
			{
				m_iState = STATE_DEFAULTATTACK_2;
			}
			else
			{
				m_isDefaultAttack = false;
			}
		}
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CHomonculus::Down_Attack(_float fTimeDelta)
{
	if (STATE_DEFAULTATTACK_1 == m_iState || STATE_DEFAULTATTACK_2 == m_iState)
	{
		return FAILURE;
	}

	if (STATE_IDLE == m_iState)
	{
		m_isAnimFinished = false;
	}

	if (!m_isAnimFinished)
	{
		m_iState = STATE_DOWNATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CHomonculus::Full_Attack(_float fTimeDelta)
{
	if (STATE_DOWNATTACK == m_iState || STATE_DEFAULTATTACK_1 == m_iState || STATE_DEFAULTATTACK_2 == m_iState)
	{
		return FAILURE;
	}

	if (STATE_IDLE == m_iState)
	{
		m_isAnimFinished = false;
	}

	if (!m_isAnimFinished)
	{
		m_iState = STATE_FULLATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
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

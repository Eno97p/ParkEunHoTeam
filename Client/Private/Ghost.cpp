#include "Ghost.h"

#include "GameInstance.h"
#include "Body_Ghost.h"

CGhost::CGhost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CGhost::CGhost(const CGhost& rhs)
	: CMonster{ rhs }
{
}

HRESULT CGhost::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGhost::Initialize(void* pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 0.f, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	return S_OK;
}

void CGhost::Priority_Tick(_float fTimeDelta)
{
}

void CGhost::Tick(_float fTimeDelta)
{
	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
}

void CGhost::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
}

HRESULT CGhost::Render()
{
	return S_OK;
}

HRESULT CGhost::Add_Components()
{
	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGhost::Add_PartObjects()
{
	// Body
	CPartObject::PARTOBJ_DESC pPartDesc{};
	pPartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pPartDesc.fSpeedPerSec = 0.f;
	pPartDesc.fRotationPerSec = 0.f;
	pPartDesc.pState = &m_iState;
	pPartDesc.eLevel = m_eLevel;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Ghost"), &pPartDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

	return S_OK;
}

HRESULT CGhost::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CGhost::Idle, this, std::placeholders::_1));


	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CGhost::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CGhost::Hit, this, std::placeholders::_1));
	
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("DefaultAttackCool"), 8.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DefaultAttackCool"), TEXT("DefaultAttack"), bind(&CGhost::Default_Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("DownAttackCool"), 5.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DownAttackCool"), TEXT("DownAttack"), bind(&CGhost::Down_Attack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Move"), bind(&CGhost::Move, this, std::placeholders::_1));

	return S_OK;
}

void CGhost::Check_AnimFinished()
{
	vector<CGameObject*>::iterator iter = m_PartObjects.begin();
	m_isAnimFinished = dynamic_cast<CBody_Ghost*>(*iter)->Get_AnimFinished();
}

NodeStates CGhost::Dead(_float fTimeDelta)
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

NodeStates CGhost::Hit(_float fTimeDelta)
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

NodeStates CGhost::Idle(_float fTimeDelta)
{
	m_iState = STATE_IDLE;

	return SUCCESS;
}

NodeStates CGhost::Move(_float fTimeDelta)
{

	return FAILURE;
}

NodeStates CGhost::Default_Attack(_float fTimeDelta)
{
	if (STATE_DOWNATTACK == m_iState)
	{
		return FAILURE;
	}

	if (STATE_DEFAULTATTACK_4 != m_iState)
	{
		m_isDefaultAttack = true;
	}

	if (m_isDefaultAttack)
	{
		if (STATE_DEFAULTATTACK_1 != m_iState && STATE_DEFAULTATTACK_2 != m_iState && STATE_DEFAULTATTACK_3 != m_iState && STATE_DEFAULTATTACK_4 != m_iState)
		{
			m_iState = STATE_DEFAULTATTACK_1;
			m_isAnimFinished = false;
		}

		if (m_isAnimFinished)
		{
			switch (m_iState)
			{
			case STATE_DEFAULTATTACK_1:
				m_iState = STATE_DEFAULTATTACK_2;
				break;
			case STATE_DEFAULTATTACK_2:
				m_iState = STATE_DEFAULTATTACK_3;
				break;
			case STATE_DEFAULTATTACK_3:
				m_iState = STATE_DEFAULTATTACK_4;
				break;
			case STATE_DEFAULTATTACK_4:
				m_isDefaultAttack = false;
				break;
			default:
				break;
			}
		}
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CGhost::Down_Attack(_float fTimeDelta)
{
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

CGhost* CGhost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGhost* pInstance = new CGhost(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CGhost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGhost::Clone(void* pArg)
{
	CGhost* pInstance = new CGhost(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CGhost");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGhost::Free()
{
	__super::Free();

	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}

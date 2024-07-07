#include "Legionnaire_Gun.h"

#include "GameInstance.h"
#include "Body_LGGun.h"
#include "LGGun_Weapon.h"

CLegionnaire_Gun::CLegionnaire_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CLegionnaire_Gun::CLegionnaire_Gun(const CLegionnaire_Gun& rhs)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-10.f, 0.f, 0.f, 1.f));

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
		pPartObject->Tick(fTimeDelta);
}

void CLegionnaire_Gun::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
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
	// Body
	CPartObject::PARTOBJ_DESC pPartDesc{};
	pPartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pPartDesc.fSpeedPerSec = 0.f;
	pPartDesc.fRotationPerSec = 0.f;
	pPartDesc.pState = &m_iState;
	pPartDesc.eLevel = m_eLevel;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_LGGun"), &pPartDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

	// Weapon
	CWeapon::WEAPON_DESC WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.eLevel = m_eLevel;
	WeaponDesc.pCombinedTransformationMatrix = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")))->Get_BoneCombinedTransformationMatrix("Root_Gun");

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_LGGun_Weapon"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);


	return S_OK;
}

HRESULT CLegionnaire_Gun::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CLegionnaire_Gun::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CLegionnaire_Gun::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("KnockDown"), bind(&CLegionnaire_Gun::KnockDown, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("WakeUp"), bind(&CLegionnaire_Gun::WakeUp, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CLegionnaire_Gun::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Casting"), bind(&CLegionnaire_Gun::Casting, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("MeleeAttack"), bind(&CLegionnaire_Gun::MeleeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("GunAttackCool"), 3.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("GunAttackCool"), TEXT("GunAttack"), bind(&CLegionnaire_Gun::GunAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Move"), bind(&CLegionnaire_Gun::Move, this, std::placeholders::_1));

	return S_OK;
}

void CLegionnaire_Gun::Check_AnimFinished()
{
	vector<CGameObject*>::iterator iter = m_PartObjects.begin();
	m_isAnimFinished = dynamic_cast<CBody_LGGun*>(*iter)->Get_AnimFinished();
}

NodeStates CLegionnaire_Gun::Dead(_float fTimedelta)
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

NodeStates CLegionnaire_Gun::Hit(_float fTimedelta)
{
	if (m_isHit) // 이 부분을 State으로 분기 처리 해도 
	{
		if (m_iState != STATE_HIT)
		{
			// HP 처리

		}

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

NodeStates CLegionnaire_Gun::WakeUp(_float fTimedelta)
{
	if (m_isParry)
	{
		if (STATE_KNOCKDOWN == m_iState)
		{
			m_isAnimFinished = false;
		}

		m_iState = STATE_WAKEUP;

		if (m_isAnimFinished)
		{
			m_isParry = false;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CLegionnaire_Gun::KnockDown(_float fTimedelta)
{
	if (m_isParry && STATE_WAKEUP != m_iState)
	{
		m_iState = STATE_KNOCKDOWN;

		if (m_isAnimFinished)
		{
			return FAILURE;
		}

		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CLegionnaire_Gun::Idle(_float fTimedelta)
{
	m_iState = STATE_IDLE;

	return SUCCESS; // ?
}

NodeStates CLegionnaire_Gun::Move(_float fTimeDelta)
{
	// 상황에 따라 상태 처리 다르게(앞뒤좌우)



	return FAILURE;
}

NodeStates CLegionnaire_Gun::GunAttack(_float fTimedelta)
{
	if (!m_isAnimFinished)
	{
		m_iState = STATE_GUNATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CLegionnaire_Gun::Casting(_float fTimedelta)
{
	// Player와의 거리가 일정 이하면 근접 공격 준비?
	if (STATE_GUNATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_isAttackDistance && !m_isMeleeAttack)
	{
		if (3.f >= m_fCastingTimer)
		{
			m_fCastingTimer += fTimedelta;
			m_iState = STATE_CASTING;
			return RUNNING;
		}
		else
		{
			m_fCastingTimer = 0.f;
			m_isMeleeAttack = true;
			return FAILURE;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CLegionnaire_Gun::MeleeAttack(_float fTimedelta)
{
	if (STATE_CASTING == m_iState)
	{
		m_iState = STATE_MELEEATTACK1;
		m_isAnimFinished = false;
	}

	if (m_isMeleeAttack)
	{
		if (m_isAnimFinished) // 애니메이션이 종료되었다면
		{
			switch (m_iState)
			{
			case STATE_MELEEATTACK1:
			{
				m_iState = STATE_MELEEATTACK2;
				break;
			}
			case STATE_MELEEATTACK2:
			{
				m_iState = STATE_MELEEATTACK3;
				break;
			}
			case STATE_MELEEATTACK3:
			{
				m_isMeleeAttack = false;
				break;
			}
			default:
				break;
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}	
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
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}

#include "Legionnaire_Gun.h"

#include "GameInstance.h"
#include "Body_LGGun.h"
#include "Weapon_Gun_LGGun.h"
#include "Weapon_Sword_LGGun.h"
#include "Weapon_Arrow_LGGun.h"

#include "UIGroup_MonsterHP.h"

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

	m_fCurHp = 100.f;
	m_iState = STATE_IDLE;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 522.f, 98.f, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Create_UI();

	return S_OK;
}

void CLegionnaire_Gun::Priority_Tick(_float fTimeDelta)
{
	if (m_fDeadDelay < 2.f)
	{
		m_fDeadDelay -= fTimeDelta;
		if (m_fDeadDelay < 0.f)
		{
			m_pGameInstance->Erase(this);
		}
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);

	if (m_pArrow && m_fArrowLifeTime > 0.f)
	{
		m_fArrowLifeTime -= fTimeDelta;
		if (m_fArrowLifeTime < 0.f)
		{
			m_pGameInstance->Erase(m_pArrow);
			m_pArrow = nullptr;
			m_fArrowLifeTime = 3.f;
		}
	}
}

void CLegionnaire_Gun::Tick(_float fTimeDelta)
{
	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	Set_Weapon();
	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부
	if (!m_pArrow || (m_pArrow && m_eColltype == CCollider::COLL_NOCOLL))
	{
		CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
		
		m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
		if (!pPlayerWeapon->Get_Active())
		{
			m_eColltype = CCollider::COLL_NOCOLL;
		}
	}

	if (m_pArrow)
	{
		CWeapon_Arrow_LGGun* pReversedArrow = m_pArrow;
		if (m_pArrow->Get_IsParried())
		{
			m_eColltype = m_pColliderCom->Intersect(m_pArrow->Get_Collider());
			if (m_eColltype == CCollider::COLL_START)
			{
				m_pGameInstance->Erase(m_pArrow);
				m_pArrow = nullptr;
				m_fArrowLifeTime = 3.f;
			}
		}
	}

	m_pPhysXCom->Tick(fTimeDelta);

	Update_UI(-0.3f);
	m_pUI_HP->Tick(fTimeDelta);
}

void CLegionnaire_Gun::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
	m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pUI_HP->Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CLegionnaire_Gun::Render()
{
	return S_OK;
}

void CLegionnaire_Gun::Set_Idle()
{
	if (m_eMode == MODE_IDLE)
	{
		m_iState = STATE_IDLE;
	}
	else if (m_eMode == MODE_GUN)
	{
		m_iState = STATE_IDLE_GUN;
	}
	else if (m_eMode == MODE_MELEE)
	{
		m_iState = STATE_IDLE_MELEE;
	}
}

void CLegionnaire_Gun::Set_Weapon()
{
	switch (m_eMode)
	{
	case MODE_IDLE:
		static_cast<CWeapon*>(m_PartObjects[1])->Set_RenderAvailable(false);
		static_cast<CWeapon*>(m_PartObjects[2])->Set_RenderAvailable(false);
		break;
	case MODE_GUN:
		static_cast<CWeapon*>(m_PartObjects[1])->Set_RenderAvailable(true);
		static_cast<CWeapon*>(m_PartObjects[2])->Set_RenderAvailable(false);
		break;
	case MODE_MELEE:
		static_cast<CWeapon*>(m_PartObjects[1])->Set_RenderAvailable(false);
		static_cast<CWeapon*>(m_PartObjects[2])->Set_RenderAvailable(true);
		break;
	}
}

HRESULT CLegionnaire_Gun::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 7.f;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(160.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 525.f, 98.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
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
	pPartDesc.pCanCombo = &m_bCanCombo;

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

	CGameObject* pGun = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Gun_LGGun"), &WeaponDesc);
	if (nullptr == pGun)
		return E_FAIL;
	m_PartObjects.emplace_back(pGun);
	dynamic_cast<CBody_LGGun*>(pBody)->Set_Gun(dynamic_cast<CWeapon*>(pGun));

	CGameObject* pSword = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Sword_LGGun"), &WeaponDesc);
	if (nullptr == pSword)
		return E_FAIL;
	m_PartObjects.emplace_back(pSword);
	dynamic_cast<CBody_LGGun*>(pBody)->Set_Sword(dynamic_cast<CWeapon*>(pSword));

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
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CLegionnaire_Gun::Hit, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Parried"), bind(&CLegionnaire_Gun::Parried, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("MeleeAttack"), bind(&CLegionnaire_Gun::MeleeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("GunAttack"), bind(&CLegionnaire_Gun::GunAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("DetectCool"), 3.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DetectCool"), TEXT("Detect"), bind(&CLegionnaire_Gun::Detect, this, std::placeholders::_1));
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
	if (m_iState == STATE_DEAD)
	{
		if (m_isAnimFinished)
		{
			if (!m_bDead)
			{
				m_bDead = true;
				for (_uint i = 0; i < m_PartObjects.size(); i++)
				{
					dynamic_cast<CPartObject*>(m_PartObjects[i])->Set_DisolveType(CPartObject::TYPE_DECREASE);
				}
				m_fDeadDelay -= 0.001f;
			}
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
	switch (m_eColltype)
	{
	case CCollider::COLL_START:
		m_iState = STATE_HIT;
		Add_Hp(-10);
		return RUNNING;
		break;
	case CCollider::COLL_CONTINUE:
		m_iState = STATE_HIT;
		return RUNNING;
		break;
	case CCollider::COLL_FINISH:
		m_iState = STATE_HIT;
		break;
	case CCollider::COLL_NOCOLL:
		break;
	}

	if(m_iState == STATE_HIT)

	if (m_iState == STATE_HIT)
	{
		if (m_isAnimFinished)
		{
			m_eColltype = CCollider::COLL_NOCOLL;
			Set_Idle();
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}

	return FAILURE;
}

NodeStates CLegionnaire_Gun::Parried(_float fTimeDelta)
{
	if (static_cast<CWeapon*>(m_PartObjects[1])->Get_Active() && !m_pArrow)
	{
		list<CGameObject*> ArrowList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"));
		m_pArrow = dynamic_cast<CWeapon_Arrow_LGGun*>(ArrowList.back());
		//Safe_AddRef(m_pArrow);
	}

	if (dynamic_cast<CWeapon_Sword_LGGun*>(m_PartObjects[2])->Get_IsParried() && m_iState != STATE_PARRIED)
	{
		m_iState = STATE_PARRIED;
	}

	if (m_iState == STATE_PARRIED)
	{
		if (m_isAnimFinished)
		{
			dynamic_cast<CWeapon_Sword_LGGun*>(m_PartObjects[2])->Set_IsParried(false);
			Set_Idle();
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CLegionnaire_Gun::GunAttack(_float fTimedelta)
{
	if (m_iState == STATE_MELEEATTACK1 || m_iState == STATE_MELEEATTACK2)
	{
		return COOLING;
	}

	if (m_iState == STATE_GUNATTACK)
	{
		if (m_isAnimFinished)
		{
			Set_Idle();
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
	return FAILURE;
}

NodeStates CLegionnaire_Gun::MeleeAttack(_float fTimeDelta)
{
	if (m_eMode == MODE_MELEE && (m_iState == STATE_MELEEATTACK1 || m_iState == STATE_MELEEATTACK2))
	{
		if (m_iState == STATE_MELEEATTACK2)
		{
			m_fBackStepTime -= fTimeDelta;
			if (m_fBackStepTime < 0.f && m_fBackStepTime > -0.6f)
			{
				if (m_fBackStepTime > -0.1f)
				{
					m_pPhysXCom->Go_Jump(fTimeDelta);
				}
				m_pPhysXCom->Go_BackWard(fTimeDelta * 3.f);
			}
		}

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
				if (m_fLengthFromPlayer < ATTACKRANGE)
				{
					m_iState = RandomInt(2, 7);
				}
				else
				{
					m_iState = STATE_RUN;
				}
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
		m_fBackStepTime = 1.1f;
		return FAILURE;
	}
}

NodeStates CLegionnaire_Gun::Detect(_float fTimeDelta)
{
	if (m_fLengthFromPlayer > DETECTRANGE)
	{
		m_eMode = MODE_IDLE;
		return SUCCESS;
	}
	else if (m_fLengthFromPlayer > MELEERANGE)
	{
		m_eMode = MODE_GUN;
		m_iState = STATE_GUNATTACK;
		return SUCCESS;
	}
	else if (m_fLengthFromPlayer > 2.f)
	{
		m_eMode = MODE_MELEE;
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_iState = STATE_RUN;
		return SUCCESS;
	}
	else
	{
		m_eMode = MODE_MELEE;
		m_iState = STATE_MELEEATTACK1;
		return SUCCESS;
	}
}

NodeStates CLegionnaire_Gun::Move(_float fTimeDelta)
{
	if (m_iState == STATE_IDLE || m_iState == STATE_IDLE_GUN || m_iState == STATE_IDLE_MELEE)
	{
		return COOLING;
	}

	if (m_iState == STATE_LEFT)
	{
		m_pPhysXCom->Go_OrbitCW(fTimeDelta * 0.5f, m_pPlayerTransform);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			Set_Idle();
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_RIGHT)
	{
		m_pPhysXCom->Go_OrbitCCW(fTimeDelta * 0.5f, m_pPlayerTransform);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			Set_Idle();
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_BACK)
	{
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_BackWard(fTimeDelta * 0.5f);
		m_fMoveTime -= fTimeDelta;
		if (m_fMoveTime <= 0.f)
		{
			Set_Idle();
			m_fMoveTime = 2.f;
		}
	}
	else if (m_iState == STATE_RUN)
	{
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_Straight(fTimeDelta);
		if (m_fLengthFromPlayer < 2.f)
		{
			Set_Idle();
			return FAILURE;
		}
	}

	return RUNNING;
}

NodeStates CLegionnaire_Gun::Idle(_float fTimeDelta)
{
	m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	Set_Idle();
	return SUCCESS;
}

void CLegionnaire_Gun::Add_Hp(_int iValue)
{
	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0.f)
	{
		m_iState = STATE_DEAD;
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
	Safe_Release(m_pArrow);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}

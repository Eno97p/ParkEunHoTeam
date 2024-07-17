#include "Boss_Juggulus.h"

#include "GameInstance.h"
#include "Body_Juggulus.h"
#include "PartObject.h"
#include "Juggulus_Hammer.h"
#include "Juggulus_HandOne.h"
#include "Juggulus_HandTwo.h"
#include "Juggulus_HandThree.h"

#include "UIGroup_BossHP.h"

CBoss_Juggulus::CBoss_Juggulus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{pDevice, pContext}
{
}

CBoss_Juggulus::CBoss_Juggulus(const CBoss_Juggulus& rhs)
	: CMonster{rhs}
{
}

HRESULT CBoss_Juggulus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Juggulus::Initialize(void* pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 3.f; // 수정 필요
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_fCurHp = 100;
	m_iState = STATE_IDLE_FIRST;
	m_ePhase = PHASE_ONE;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 510.f, 98.f, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_fMaxHp = 100.f;
	m_fCurHp = m_fMaxHp;

	Create_BossUI(CUIGroup_BossHP::BOSSUI_JUGGULUS);
	m_pUI_HP->Set_Rend(true); // 일단 출력 X

	return S_OK;
}

void CBoss_Juggulus::Priority_Tick(_float fTimeDelta)
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
		pPartObject.second->Priority_Tick(fTimeDelta);
	m_isAnimFinished = dynamic_cast<CBody_Juggulus*>((*m_PartObjects.find("Body")).second)->Get_AnimFinished();
}

void CBoss_Juggulus::Tick(_float fTimeDelta)
{
	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Tick(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_P))
		m_fCurHp = 10.f;

	m_pUI_HP->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pColliderCom->Reset();
	}
	else
	{
		m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	}

	//m_pPhysXCom->Tick(fTimeDelta);

	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Set_Ratio((m_fCurHp / m_fMaxHp));
	m_pUI_HP->Tick(fTimeDelta);

	if (m_fCircleSphereSpawnTime < CIRCLESPHERESPAWNTIME)
	{
		m_fCircleSphereSpawnTime -= fTimeDelta;

		if (m_fCircleSphereSpawnTime < 2.f && m_iCircleSphereCount == 0)
		{
			CGameObject::GAMEOBJECT_DESC transformDesc;
			transformDesc.fSpeedPerSec = 3.f;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &transformDesc);
			m_iCircleSphereCount++;
		}
		else if (m_fCircleSphereSpawnTime < 1.33f && m_iCircleSphereCount == 1)
		{
			CGameObject::GAMEOBJECT_DESC transformDesc;
			transformDesc.fSpeedPerSec = 3.f;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &transformDesc);
			m_iCircleSphereCount++;
		}
		else if (m_fCircleSphereSpawnTime < 0.66f && m_iCircleSphereCount == 2)
		{
			CGameObject::GAMEOBJECT_DESC transformDesc;
			transformDesc.fSpeedPerSec = 3.f;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_GameObjects"), TEXT("Prototype_GameObject_CircleSphere"), &transformDesc);
			m_iCircleSphereCount = 0;
			m_fCircleSphereSpawnTime = CIRCLESPHERESPAWNTIME;
		}
	}


}

void CBoss_Juggulus::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Late_Tick(fTimeDelta);
	m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pUI_HP->Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CBoss_Juggulus::Render()
{
	return S_OK;
}

void CBoss_Juggulus::Add_Hp(_int iValue)
{
	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0.f)
	{
		m_iState = STATE_DEAD;
	}
}

HRESULT CBoss_Juggulus::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(2.f, 8.f, 2.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 10.f;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(160.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 510.f, 98.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Juggulus::Add_PartObjects()
{
	// Juggulus Body
	CPartObject::PARTOBJ_DESC PartDesc{};
	PartDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	PartDesc.fSpeedPerSec = 0.f;
	PartDesc.fRotationPerSec = 0.f;
	PartDesc.pState = &m_iState;
	PartDesc.eLevel = m_eLevel;
	PartDesc.pCurHp = &m_fCurHp;
	PartDesc.pMaxHp = &m_fMaxHp;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Juggulus"), &PartDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace("Body", pBody);

	// Hand One
	CGameObject* pHandOne = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandOne"), &PartDesc);
	if (nullptr == pHandOne)
		return E_FAIL;
	m_PartObjects.emplace("Hand_One", pHandOne);

	// Hand Two
	CGameObject* pHandTwo = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandTwo"), &PartDesc);
	if (nullptr == pHandTwo)
		return E_FAIL;
	m_PartObjects.emplace("Hand_Two", pHandTwo);

	// Hand Three
	CGameObject* pHandThree = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_HandThree"), &PartDesc);
	if (nullptr == pHandThree)
		return E_FAIL;
	m_PartObjects.emplace("Hand_Three", pHandThree);

	return S_OK;
}

HRESULT CBoss_Juggulus::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);

	//m_pBehaviorCom->Add_CoolDown(TEXT("Top_Selector"), TEXT("AttackCool"), 3.f); 	// Attack 간 coolTime
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("OneP_Attack"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("TwoP_Attack"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CBoss_Juggulus::Idle, this, std::placeholders::_1));


	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CBoss_Juggulus::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("NextPhase"), bind(&CBoss_Juggulus::NextPhase, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("CreateHammer"), bind(&CBoss_Juggulus::CreateHammer, this, std::placeholders::_1));

	// 2Phase Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("Select_PatternCool"), 5.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("Select_PatternCool"), TEXT("Select_Pattern"), bind(&CBoss_Juggulus::Select_Pattern, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("HammerAttack"), bind(&CBoss_Juggulus::HammerAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("FlameAttack"), bind(&CBoss_Juggulus::FlameAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("SphereAttack"), bind(&CBoss_Juggulus::SphereAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("TwoP_Attack"), TEXT("ThunderAttack"), bind(&CBoss_Juggulus::ThunderAttack, this, std::placeholders::_1));

	return S_OK;
}

HRESULT CBoss_Juggulus::Create_Hammer()
{
	CWeapon::WEAPON_DESC WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.eLevel = m_eLevel;

	map<string, CGameObject*>::iterator body = m_PartObjects.find("Body");
	CModel* pModelCom = dynamic_cast<CModel*>((*body).second->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Root_Hammer");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Juggulus_Hammer"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_PartObjects.emplace("Hammer", pWeapon);

	dynamic_cast<CBody_Juggulus*>((*body).second)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

	return S_OK;
}

void CBoss_Juggulus::Check_AnimFinished()
{
	map<string, CGameObject*>::iterator hand = m_PartObjects.find("Hand_One");
	if (hand != m_PartObjects.end())
	{
		m_isHandAnimFinished = dynamic_cast<CJuggulus_HandOne*>((*hand).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator hand_two = m_PartObjects.find("Hand_Two");
	if (hand_two != m_PartObjects.end())
	{
		m_isHandTwoAnimFinished = dynamic_cast<CJuggulus_HandTwo*>((*hand_two).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator hand_three = m_PartObjects.find("Hand_Three");
	if (hand_three != m_PartObjects.end())
	{
		m_isHandThreeAnimFinished = dynamic_cast<CJuggulus_HandThree*>((*hand_three).second)->Get_AnimFinished();
	}

	map<string, CGameObject*>::iterator body = m_PartObjects.find("Body");
	if (body != m_PartObjects.end())
	{
		m_isAnimFinished = dynamic_cast<CBody_Juggulus*>((*body).second)->Get_AnimFinished();
	}
}

NodeStates CBoss_Juggulus::Dead(_float fTimedelta)
{
	if (0.f >= m_fCurHp)
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

NodeStates CBoss_Juggulus::NextPhase(_float fTimedelta)
{
	if (m_iState == STATE_CREATE_HAMMER)
	{
		return COOLING;
	}

	if (80.f >= m_fCurHp && PHASE_ONE == m_ePhase) // || m_pGameInstance->Key_Down(DIK_P)
	{
		if (m_iState != STATE_NEXTPHASE)
		{
			// 손 삭제, 몬스터 삭제
			Safe_Release((*m_PartObjects.find("Hand_One")).second);
			Safe_Release((*m_PartObjects.find("Hand_Two")).second);
			Safe_Release((*m_PartObjects.find("Hand_Three")).second);
			m_PartObjects.erase("Hand_One");
			m_PartObjects.erase("Hand_Two");
			m_PartObjects.erase("Hand_Three");
		}

		m_iState = STATE_NEXTPHASE;

		if (m_isAnimFinished)
		{
			m_iState = STATE_CREATE_HAMMER;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::CreateHammer(_float fTimeDelta)
{
	if (!m_isHammerCreate && m_iState == STATE_CREATE_HAMMER)
	{
		m_fHammerCreationDelay -= fTimeDelta;
		if (m_fHammerCreationDelay < 0.f && m_fHammerCreationDelay > -1.f)
		{
			Create_Hammer();
			m_fHammerCreationDelay = -1.1f;
		}
		if (m_isAnimFinished)
		{
			m_ePhase = PHASE_TWO;
			m_isHammerCreate = true;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::Idle(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase)
	{
		m_iState = STATE_IDLE_FIRST;
	}
	else if(PHASE_TWO == m_ePhase)
	{
		m_iState = STATE_IDLE_SEC;
	}
	return RUNNING; // SUCCESS
}

//NodeStates CBoss_Juggulus::HandOne_Targeting(_float fTimeDelta)
//{
//	if (PHASE_TWO == m_ePhase || STATE_HANDTWO_SCOOP == m_iState || STATE_HANDTWO_ATTACK == m_iState || m_isHandOne_On)
//	{
//		return FAILURE;
//	}
//
//	if (3.f >= m_fTargettingTimer) // 다른 행동을 하고 있지 않고 타이머가 아직 채워지지 않았으면
//	{
//		m_fTargettingTimer += fTimeDelta;
//		m_iState = STATE_HANDONE_TARGETING; // 플레이어 위치에 등장한 후 몇 초 동안 타게팅
//		CJuggulus_HandOne* handOne = static_cast<CJuggulus_HandOne*>((*m_PartObjects.find("Hand_One")).second);
//		handOne->Chase_Player();
//		return RUNNING;
//	}
//	else
//	{
// 		m_isHandOne_On = true; // Hand One 공격 활성화
//		m_fTargettingTimer = 0.f;
//		return SUCCESS; // 이때부터 쿨타임 도는 것
//	}
//}

//NodeStates CBoss_Juggulus::HandOne_Attack(_float fTimeDelta)
//{
//	if (PHASE_TWO == m_ePhase || STATE_HANDTWO_SCOOP == m_iState || STATE_HANDTWO_ATTACK == m_iState)
//	{
//		return FAILURE;
//	}
//
//	if (m_isHandOne_On)
//	{
//		if(STATE_HANDONE_TARGETING == m_iState)
//			m_isHandAnimFinished = false;
//
//		m_iState = STATE_HANDONE_ATTACK;
//
//		if (m_isHandAnimFinished)
//		{
//			m_isHandOne_On = false;
//		}
//
//		return RUNNING;
//	}
//	else
//	{
//		return FAILURE;
//	}
//}

//NodeStates CBoss_Juggulus::HandTwo_Scoop(_float fTimeDedelta)
//{
//	if (PHASE_TWO == m_ePhase || STATE_HANDTHREE_ATTACK == m_iState || m_isHandTwo_On)
//	{
//		return FAILURE;
//	}
//
//	if (!m_isHandTwoAnimFinished)
//	{
//		m_iState = STATE_HANDTWO_SCOOP;
//		return RUNNING;
//	}
//	else
//	{
//		m_isHandTwo_On = true;
//		return SUCCESS;
//	}
//}

//NodeStates CBoss_Juggulus::HandTwo_Attack(_float fTimeDelta)
//{
//	if (PHASE_TWO == m_ePhase || STATE_HANDTHREE_ATTACK == m_iState)
//	{
//		return FAILURE;
//	}
//
//	if (m_isHandTwo_On)
//	{
//		if (STATE_HANDTWO_SCOOP == m_iState)
//			m_isHandTwoAnimFinished = false;
//
//		m_iState = STATE_HANDTWO_ATTACK;
//
//		if (m_isHandTwoAnimFinished)
//		{
//			m_isHandTwo_On = false;
//
//		}
//		return RUNNING;
//	}
//	else
//	{
//		return FAILURE;
//	}
//}

//NodeStates CBoss_Juggulus::HandThree_Attack(_float fTimeDelta)
//{
//	if (PHASE_TWO == m_ePhase)
//	{
//		return FAILURE;
//	}
//
//	if (!m_isHandThreeAnimFinished)
//	{
//		m_iState = STATE_HANDTHREE_ATTACK;
//		return RUNNING;
//	}
//	else
//	{
//		return SUCCESS;
//	}
//}

NodeStates CBoss_Juggulus::Select_Pattern(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_FLAME_ATTACK == m_iState || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState
		|| STATE_HAMMER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	_int iRand = RandomInt(0, 3);
	switch (iRand)
	{
	case 0:
		m_iState = STATE_HAMMER_ATTACK;
		break;
	case 1:
		m_iState = STATE_FLAME_ATTACK;
		break;
	case 2:
		m_iState = STATE_SPHERE_ATTACK;
		break;
	case 3:
		m_iState = STATE_THUNDER_ATTACK;
		break;
	}
	return SUCCESS;
}

NodeStates CBoss_Juggulus::HammerAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_FLAME_ATTACK == m_iState || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_HAMMER_ATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::FlameAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_FLAME_ATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::SphereAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_iState == STATE_SPHERE_ATTACK)
	{
		if (m_fCircleSphereSpawnTime == CIRCLESPHERESPAWNTIME)
		{
			m_fCircleSphereSpawnTime -= 0.01f;
		}
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;
			
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::ThunderAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase)
	{
		return FAILURE;
	}

	if (m_iState == STATE_THUNDER_ATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE_SEC;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

CBoss_Juggulus* CBoss_Juggulus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Juggulus*		pInstance = new CBoss_Juggulus(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBoss_Juggulus");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CBoss_Juggulus::Clone(void* pArg)
{
	CBoss_Juggulus* pInstance = new CBoss_Juggulus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBoss_Juggulus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Juggulus::Free()
{
	__super::Free();

	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject.second);
	m_PartObjects.clear();
}

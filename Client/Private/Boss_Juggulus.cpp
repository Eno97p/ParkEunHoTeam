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

	pDesc->fSpeedPerSec = 3.f; // ���� �ʿ�
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_iCurHp = 100;
	m_iState = STATE_IDLE_FIRST;
	m_ePhase = PHASE_ONE;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(1.5f, 1.5f, 1.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 522.f, 98.f, 1.f));

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Create_BossUI(CUIGroup_BossHP::BOSSUI_JUGGULUS);

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
		m_iCurHp = 10;

	m_pUI_HP->Tick(fTimeDelta);
}

void CBoss_Juggulus::Late_Tick(_float fTimeDelta)
{
	Key_Input();

	for (auto& pPartObject : m_PartObjects)
		pPartObject.second->Late_Tick(fTimeDelta);

	m_pUI_HP->Late_Tick(fTimeDelta);
}

HRESULT CBoss_Juggulus::Render()
{
	return S_OK;
}

HRESULT CBoss_Juggulus::Add_Components()
{
	//CPhysXComponent::PHYSX_DESC		PhysXDesc{};
	//PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	//XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	//PhysXDesc.pComponent = m_pModelCom;
	//PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
	//	TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
	//	return E_FAIL;;
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

	//m_pBehaviorCom->Add_CoolDown(TEXT("Top_Selector"), TEXT("AttackCool"), 3.f); 	// Attack �� coolTime
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("OneP_Attack"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Attack_Selector"), TEXT("TwoP_Attack"), CBehaviorTree::Selector);

	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CBoss_Juggulus::Idle, this, std::placeholders::_1));


	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CBoss_Juggulus::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("NextPhase"), bind(&CBoss_Juggulus::NextPhase, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("CreateHammer"), bind(&CBoss_Juggulus::CreateHammer, this, std::placeholders::_1));

	// 1Phase Attack
	// Hand One Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("OneP_Attack"), TEXT("HandOneTargettingCool"), 10.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("HandOneTargettingCool"), TEXT("HandOne_Targetting"), bind(&CBoss_Juggulus::HandOne_Targeting, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("OneP_Attack"), TEXT("HandOne_Attack"), bind(&CBoss_Juggulus::HandOne_Attack, this, std::placeholders::_1));
	
	// Hand Two Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("OneP_Attack"), TEXT("HandTwoScoopCool"), 8.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("HandTwoScoopCool"), TEXT("HandTwo_Scoop"), bind(&CBoss_Juggulus::HandTwo_Scoop, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("OneP_Attack"), TEXT("HandTwo_Attack"), bind(&CBoss_Juggulus::HandTwo_Attack, this, std::placeholders::_1));

	// Hand Three Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("OneP_Attack"), TEXT("HandThreeScoopCool"), 6.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("HandThreeScoopCool"), TEXT("HandThree_Attack"), bind(&CBoss_Juggulus::HandThree_Attack, this, std::placeholders::_1));

	// 2Phase Attack
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("HammerCool"), 8.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("HammerCool"), TEXT("HammerAttack"), bind(&CBoss_Juggulus::HammerAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("FlameCool"), 5.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("FlameCool"), TEXT("FlameAttack"), bind(&CBoss_Juggulus::FlameAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("SphereCool"), 4.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("SphereCool"), TEXT("SphereAttack"), bind(&CBoss_Juggulus::SphereAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("TwoP_Attack"), TEXT("ThunderCool"), 3.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("ThunderCool"), TEXT("ThunderAttack"), bind(&CBoss_Juggulus::ThunderAttack, this, std::placeholders::_1));


	return S_OK;
}

void CBoss_Juggulus::Key_Input()
{
	//// �׽�Ʈ�� Ű���� �Լ�
	//if (m_pGameInstance->Key_Down(DIK_P))
	//{
	//	m_iState++;
	//}
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

	return S_OK;
}

void CBoss_Juggulus::Check_AnimFinished()
{
	map<string, CGameObject*>::iterator hand = m_PartObjects.find("Hand_One");
	m_isHandAnimFinished = dynamic_cast<CJuggulus_HandOne*>((*hand).second)->Get_AnimFinished();

	map<string, CGameObject*>::iterator hand_two = m_PartObjects.find("Hand_Two");
	m_isHandTwoAnimFinished = dynamic_cast<CJuggulus_HandTwo*>((*hand_two).second)->Get_AnimFinished();

	map<string, CGameObject*>::iterator hand_three = m_PartObjects.find("Hand_Three");
	m_isHandThreeAnimFinished = dynamic_cast<CJuggulus_HandThree*>((*hand_three).second)->Get_AnimFinished();

	map<string, CGameObject*>::iterator body = m_PartObjects.find("Body");
	m_isAnimFinished = dynamic_cast<CBody_Juggulus*>((*body).second)->Get_AnimFinished();
}

NodeStates CBoss_Juggulus::Dead(_float fTimedelta)
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

NodeStates CBoss_Juggulus::NextPhase(_float fTimedelta)
{
	if (10 >= m_iCurHp && PHASE_ONE == m_ePhase) // || m_pGameInstance->Key_Down(DIK_P)
	{
		m_iState = STATE_NEXTPHASE;

		if (m_isAnimFinished)
		{
			m_ePhase = PHASE_TWO;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::CreateHammer(_float fTimedelta)
{
	if (!m_isHammerCreate && PHASE_TWO == m_ePhase)
	{
		m_iState = STATE_CREATE_HAMMER;

		// Hammer�� nullptr�̶�� �ϳ� ������ֱ�
		map<string, CGameObject*>::iterator weapon = m_PartObjects.find("Hammer");
		if (m_PartObjects.end() == weapon)
			Create_Hammer();

		if (m_isAnimFinished)
		{
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

NodeStates CBoss_Juggulus::HandOne_Targeting(_float fTimeDelta)
{
	if (PHASE_TWO == m_ePhase || STATE_HANDTWO_SCOOP == m_iState || STATE_HANDTWO_ATTACK == m_iState || m_isHandOne_On)
	{
		return FAILURE;
	}

	if (3.f >= m_fTargettingTimer) // �ٸ� �ൿ�� �ϰ� ���� �ʰ� Ÿ�̸Ӱ� ���� ä������ �ʾ�����
	{
		m_fTargettingTimer += fTimeDelta;
		m_iState = STATE_HANDONE_TARGETING; // �÷��̾� ��ġ�� ������ �� �� �� ���� Ÿ����
		return RUNNING;
	}
	else
	{
		m_isHandOne_On = true; // Hand One ���� Ȱ��ȭ
		m_fTargettingTimer = 0.f;
		return SUCCESS; // �̶����� ��Ÿ�� ���� ��
	}
}

NodeStates CBoss_Juggulus::HandOne_Attack(_float fTimeDelta)
{
	if (PHASE_TWO == m_ePhase || STATE_HANDTWO_SCOOP == m_iState || STATE_HANDTWO_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_isHandOne_On)
	{
		if(STATE_HANDONE_TARGETING == m_iState)
			m_isHandAnimFinished = false;

		m_iState = STATE_HANDONE_ATTACK;

		if (m_isHandAnimFinished)
		{
			m_isHandOne_On = false;
		}

		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::HandTwo_Scoop(_float fTimeDedelta)
{
	if (PHASE_TWO == m_ePhase || STATE_HANDTHREE_ATTACK == m_iState || m_isHandTwo_On)
	{
		return FAILURE;
	}

	if (!m_isHandTwoAnimFinished)
	{
		m_iState = STATE_HANDTWO_SCOOP;
		return RUNNING;
	}
	else
	{
		m_isHandTwo_On = true;
		return SUCCESS;
	}
}

NodeStates CBoss_Juggulus::HandTwo_Attack(_float fTimeDelta)
{
	if (PHASE_TWO == m_ePhase || STATE_HANDTHREE_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (m_isHandTwo_On)
	{
		if (STATE_HANDTWO_SCOOP == m_iState)
			m_isHandTwoAnimFinished = false;

		m_iState = STATE_HANDTWO_ATTACK;

		if (m_isHandTwoAnimFinished)
		{
			m_isHandTwo_On = false;

		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CBoss_Juggulus::HandThree_Attack(_float fTimeDelta)
{
	if (PHASE_TWO == m_ePhase)
	{
		return FAILURE;
	}

	if (!m_isHandThreeAnimFinished)
	{
		m_iState = STATE_HANDTHREE_ATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CBoss_Juggulus::FlameAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (!m_isAnimFinished)
	{
		m_iState = STATE_FLAME_ATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CBoss_Juggulus::HammerAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_FLAME_ATTACK == m_iState || STATE_SPHERE_ATTACK == m_iState || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (!m_isAnimFinished)
	{
		m_iState = STATE_HAMMER_ATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CBoss_Juggulus::SphereAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase || STATE_THUNDER_ATTACK == m_iState)
	{
		return FAILURE;
	}

	if (!m_isAnimFinished)
	{
		m_iState = STATE_SPHERE_ATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
	}
}

NodeStates CBoss_Juggulus::ThunderAttack(_float fTimeDelta)
{
	if (PHASE_ONE == m_ePhase)
		return FAILURE;

	if (!m_isAnimFinished)
	{
		m_iState = STATE_THUNDER_ATTACK;
		return RUNNING;
	}
	else
	{
		return SUCCESS;
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

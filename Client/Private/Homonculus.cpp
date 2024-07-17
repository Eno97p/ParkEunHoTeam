#include "Homonculus.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Body_Homonculus.h"
#include "Weapon_Homonculus.h"

#include "UIGroup_MonsterHP.h"
#include "EffectManager.h"

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

	m_fCurHp = 100.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 522.f, 98.f, 1.f)); // Test

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Create_UI();

	return S_OK;
}

void CHomonculus::Priority_Tick(_float fTimeDelta)
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
}

void CHomonculus::Tick(_float fTimeDelta)
{
	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	Check_AnimFinished();

	m_pBehaviorCom->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pColliderCom->Reset();
		m_eColltype = CCollider::COLL_NOCOLL;
	}

	m_pPhysXCom->Tick(fTimeDelta);

	Update_UI(1.5f);
	m_pUI_HP->Tick(fTimeDelta);
}

void CHomonculus::Late_Tick(_float fTimeDelta)
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

HRESULT CHomonculus::Render()
{
	return S_OK;
}

HRESULT CHomonculus::Add_Components()
{
	if (FAILED(Add_BehaviorTree()))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.8f, 2.f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 10.f;
	PhysXDesc.height = 2.f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(160.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 522.f, 98.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
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

	/* 무기객체를 복제해온다. */
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;

	CModel* pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("B_Thumb_L_1");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Homonculus"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	dynamic_cast<CBody_Homonculus*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

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
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Explosion"), bind(&CHomonculus::Explosion, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CHomonculus::Hit, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Parried"), bind(&CHomonculus::Parried, this, std::placeholders::_1));


	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("DefaultAttack"), bind(&CHomonculus::Default_Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("DownAttack"), bind(&CHomonculus::Down_Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("FullAttack"), bind(&CHomonculus::Full_Attack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("DetectCool"), 3.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DetectCool"), TEXT("Detect"), bind(&CHomonculus::Detect, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Move"), bind(&CHomonculus::Move, this, std::placeholders::_1));

	return S_OK;
}

void CHomonculus::Check_AnimFinished()
{
	vector<CGameObject*>::iterator iter = m_PartObjects.begin();
	m_isAnimFinished = dynamic_cast<CBody_Homonculus*>(*iter)->Get_AnimFinished();
}

NodeStates CHomonculus::Explosion(_float fTimeDelta)
{
	if (m_iState == STATE_EXPLOSION)
	{

		if (m_isAnimFinished)
		{
			_matrix Worldmat = m_pTransformCom->Get_WorldMatrix();
			_float4 vParticlePos;
			XMStoreFloat4(&vParticlePos, Worldmat.r[3]);
			EFFECTMGR->Generate_Particle(8, vParticlePos);
			vParticlePos.y += 1.5f;
			EFFECTMGR->Generate_Particle(9, vParticlePos, nullptr, XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
			// 폭발 이펙트
			if (m_fLengthFromPlayer < EXPLOSIONRANGE)
			{
				m_pPlayer->PlayerHit(50);
				
			}
			m_iState = STATE_DEAD;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CHomonculus::Dead(_float fTimeDelta)
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

NodeStates CHomonculus::Hit(_float fTimeDelta)
{
	switch (m_eColltype)
	{
	case CCollider::COLL_START:
	{
		_matrix vMat = m_pTransformCom->Get_WorldMatrix();
		_float3 vOffset = { 0.f,1.f,0.f };
		_vector vStartPos = XMVector3TransformCoord(XMLoadFloat3(&vOffset), vMat);
		_float4 vResult;
		XMStoreFloat4(&vResult, vStartPos);
		_int Random = RandomSign();
		EFFECTMGR->Generate_Particle(0, vResult, nullptr, XMVector3Normalize(vMat.r[2]), Random * 90.f);
		EFFECTMGR->Generate_Particle(1, vResult, nullptr);
		EFFECTMGR->Generate_Particle(2, vResult, nullptr);
		m_iState = STATE_HIT;
		m_isDefaultAttack = false;
		Add_Hp(-10);
		return RUNNING;
		break;
	}
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

	if (m_iState == STATE_HIT && m_isAnimFinished)
	{
		m_iState = STATE_IDLE;
		return SUCCESS;
	}

	return FAILURE;
}

NodeStates CHomonculus::Parried(_float fTimeDelta)
{
	if (dynamic_cast<CWeapon_Homonculus*>(m_PartObjects[1])->Get_IsParried() && m_iState != STATE_PARRIED)
	{
		m_pPlayer->Set_ParriedMonsterTransform(m_pTransformCom);
		m_iState = STATE_PARRIED;
	}

	if (m_iState == STATE_PARRIED)
	{
		if (m_isAnimFinished)
		{
			dynamic_cast<CWeapon_Homonculus*>(m_PartObjects[1])->Set_IsParried(false);
			m_iState = STATE_IDLE;
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

NodeStates CHomonculus::Default_Attack(_float fTimeDelta)
{
	if (STATE_DOWNATTACK == m_iState || STATE_FULLATTACK == m_iState)
	{
		return COOLING;
	}

	if (m_isDefaultAttack)
	{
		if (STATE_DEFAULTATTACK_1 != m_iState && STATE_DEFAULTATTACK_2 != m_iState && STATE_DEFAULTATTACK_3 != m_iState)
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
				m_isDefaultAttack = false;
				if (m_fLengthFromPlayer < 2.f)
				{
					m_iState = STATE_IDLE;
				}
				else
				{
					m_iState = STATE_MOVE;
				}
				break;
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

NodeStates CHomonculus::Down_Attack(_float fTimeDelta)
{
	if (STATE_FULLATTACK == m_iState)
	{
		return COOLING;
	}

	if (m_iState == STATE_DOWNATTACK)
	{
		if (m_isAnimFinished)
		{
			if (m_fLengthFromPlayer < 2.f)
			{
				m_iState = STATE_IDLE;
			}
			else
			{
				m_iState = STATE_MOVE;
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CHomonculus::Full_Attack(_float fTimeDelta)
{
	if (m_iState == STATE_FULLATTACK)
	{
		if (m_isAnimFinished)
		{
			if (m_fLengthFromPlayer < 2.f)
			{
				m_iState = STATE_IDLE;
			}
			else
			{
				m_iState = STATE_MOVE;
			}
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CHomonculus::Detect(_float fTimeDelta)
{
	if (m_fLengthFromPlayer > DETECTRANGE)
	{
		return FAILURE;
	}
	else if (m_fLengthFromPlayer > ATTACKRANGE)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_iState = STATE_MOVE;
		return SUCCESS;
	}
	else
	{
		_int iRand = RandomInt(0, 2);
		switch (iRand)
		{
		case 0:
			m_isDefaultAttack = true;
			break;
		case 1:
			m_iState = STATE_DOWNATTACK;
			break;
		case 2:
			m_iState = STATE_FULLATTACK;
			break;
		}
		return SUCCESS;
	}
}

NodeStates CHomonculus::Move(_float fTimeDelta)
{
	if (m_iState == STATE_IDLE)
	{
		return COOLING;
	}

	if (m_iState == STATE_MOVE)
	{
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_pPhysXCom->Go_Straight(fTimeDelta);
		if (m_fLengthFromPlayer < 2.f)
		{
			m_iState = STATE_IDLE;
			return FAILURE;
		}
	}

	return RUNNING;
}

NodeStates CHomonculus::Idle(_float fTimeDelta)
{
	m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_iState = STATE_IDLE;
	return SUCCESS;
}

void CHomonculus::Add_Hp(_int iValue)
{
	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0.f)
	{
		m_iState = STATE_EXPLOSION;
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

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}

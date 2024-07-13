#include "stdafx.h"
#include "Player.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Clone.h"
#include "Body_Player.h"

#include"CHitReport.h"
#include "EffectManager.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	pDesc->fSpeedPerSec = 3.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;
	// m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
	m_bAnimFinished = dynamic_cast<CBody_Player*>(m_PartObjects.front())->Get_AnimFinished();
	if (m_fStaminaRecoverDelay > 0.f)
	{
		m_fStaminaRecoverDelay -= fTimeDelta;
	}
	else if (m_fStaminaRecoverDelay < 0.f)
	{
		Add_Stamina(fTimeDelta * 20.f);
	}
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_fButtonCooltime != 0.f)
	{
		m_fButtonCooltime += fTimeDelta;
	}
	if (m_fButtonCooltime > BUTTONCOOLTIME)
	{
		m_fButtonCooltime = 0.f;
	}

	if (m_fJumpCooltime != 0.f)
	{
		m_fJumpCooltime += fTimeDelta;
	}
	if (m_fJumpCooltime > JUMPCOOLTIME)
	{
		m_fJumpCooltime = 0.f;
	}

	if (!m_pPhysXCom->Get_IsJump())
	{
		m_bJumping = false;
		m_bDoubleJumping = false;
		m_pPhysXCom->Set_JumpSpeed(10.f);

	}
	else
	{
		m_bJumping = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_T) & 0x80)
	{
		CLandObject::LANDOBJ_DESC		LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
		LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Distortion"), &LandObjDesc);
	}

	if (m_pGameInstance->Key_Down(DIKEYBOARD_9))		//레이어 삭제 테스트
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"));
	}

	m_pBehaviorCom->Update(fTimeDelta);

	if (m_iState != STATE_DASH)
	{
		m_pPhysXCom->Tick(fTimeDelta);
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	list<CGameObject*> ObjectLis;
	ObjectLis = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	if (m_pGameInstance->Key_Down(DIK_7))
	{
		CTransform* transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
	}

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
	m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_AABB;
	ColliderDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = 10.f;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(140.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 528.f, 98.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	//PhysXDesc.position = PxExtendedVec3(0.f, PhysXDesc.height * 0.5f + PhysXDesc.radius + 5.f,0.f);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	PhysXDesc.pName = "Player";
	PhysXDesc.filterData.word0 = Engine::CollisionGropuID::GROUP_PLAYER;
	//PhysXDesc.filterData.word1 = Engine::CollisionGropuID::GROUP_ENVIRONMENT | Engine::CollisionGropuID::GROUP_ENEMY;
	CHitReport::GetInstance()->SetShapeHitCallback([this](PxControllerShapeHit const& hit){this->OnShapeHit(hit);});
	
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.pCanCombo = &m_bCanCombo;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Player"), &BodyDesc);
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

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_sword");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_WhisperSword_Anim"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	dynamic_cast<CBody_Player*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon));

	return S_OK;
}

CGameObject* CPlayer::Get_Weapon()
{
	return m_PartObjects[1];
}

void CPlayer::PlayerHit(_float fValue)
{
	if (m_bParrying || m_iState == STATE_ROLL || m_bParry) return;
	m_iState = STATE_HIT;
	m_bLAttacking = false;
	m_bRAttacking = false;
	m_bIsRunAttack = false;
	if (m_bRunning)
	{
		m_pTransformCom->Set_Speed(RUNSPEED);
	}
	else
	{
		m_pTransformCom->Set_Speed(WALKSPEED);
	}
	Add_Hp(-fValue);
}

void CPlayer::Parry_Succeed()
{
	_float4x4 WeaponMat = *static_cast<CPartObject*>(m_PartObjects[1])->Get_Part_Mat();
	_float4 vParticlePos = { WeaponMat._41,WeaponMat._42,WeaponMat._43,1.f };
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	PlayerPos.y += 1.f;
	EFFECTMGR->Generate_Distortion(0, PlayerPos);
	EFFECTMGR->Generate_Particle(6, vParticlePos);
	//EFFECTMGR->Generate_Particle(7, vParticlePos);
	m_bParry = true;
}

HRESULT CPlayer::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CPlayer::Idle, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Revive"), bind(&CPlayer::Revive, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CPlayer::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CPlayer::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Counter"), bind(&CPlayer::Counter, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("ParryCool"), 0.5f);
	m_pBehaviorCom->Add_Action_Node(TEXT("ParryCool"), TEXT("Parry"), bind(&CPlayer::Parry, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("JumpAttack"), bind(&CPlayer::JumpAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("RollAttack"), bind(&CPlayer::RollAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("LChargeAttack"), bind(&CPlayer::LChargeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("RChargeAttack"), bind(&CPlayer::RChargeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("LAttack"), bind(&CPlayer::LAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("RAttack"), bind(&CPlayer::RAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("DashCool"), 1.5f);
	m_pBehaviorCom->Add_Action_Node(TEXT("DashCool"), TEXT("Dash"), bind(&CPlayer::Dash, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Jump"), bind(&CPlayer::Jump, this, std::placeholders::_1));
	m_pBehaviorCom->Add_CoolDown(TEXT("Move_Selector"), TEXT("RollCool"), 0.1f);
	m_pBehaviorCom->Add_Action_Node(TEXT("RollCool"), TEXT("Roll"), bind(&CPlayer::Roll, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("UseItem"), bind(&CPlayer::UseItem, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Buff"), bind(&CPlayer::Buff, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("MoveTo"), bind(&CPlayer::Move, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CPlayer::Revive(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_L))
	{
		m_iState = STATE_REVIVE;
	}

	if (m_iState == STATE_REVIVE)
	{
		m_iState = STATE_REVIVE;

		if (m_bAnimFinished)
		{
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

NodeStates CPlayer::Dead(_float fTimeDelta)
{
	if (m_iState == STATE_DEAD)
	{
		if (m_bAnimFinished)
		{
			m_iState = STATE_IDLE;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CPlayer::Hit(_float fTimeDelta)
{
	if (m_iState == STATE_COUNTER || m_bParry)
	{
		return COOLING;
	}

	if (m_iState == STATE_HIT)
	{
		if (m_bAnimFinished)
		{
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}

	return FAILURE;
}

NodeStates CPlayer::Counter(_float fTimeDelta)
{
	if (m_fCurStamina < 10.f)
	{
		return COOLING;
	}

	if (m_bParry && m_iState != STATE_COUNTER)
	{
		m_iState = STATE_PARRY;
		fSlowValue = 0.2f;
		if (m_fSlowDelay <= 0.2f)
		{
			m_fSlowDelay += fTimeDelta;
		}
		else if (m_fSlowDelay > 0.2f)
		{
			m_fSlowDelay = 0.f;
			fSlowValue = 1.f;
			m_bParry = false;
			m_pParriedMonsterTransform = nullptr;
		}
	}

	// 패링 성공 && 왼클릭
	if (m_bParry && (GetKeyState(VK_LBUTTON) & 0x8000) && m_iState != STATE_COUNTER && m_pParriedMonsterTransform)
	{
		// 일정거리 이하일 때 카운터 발동
		if (XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pParriedMonsterTransform->Get_State(CTransform::STATE_POSITION))) < 4.f)
		{
			m_bParrying = false;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
			m_iState = STATE_COUNTER;
			fSlowValue = 0.2f;
			m_fSlowDelay = 0.f;
		}
	}

	if (m_iState == STATE_COUNTER)
	{
		Move_Counter();
		if (m_fSlowDelay <= 0.2f)
		{
			m_fSlowDelay += fTimeDelta;
		}
		else if (m_fSlowDelay > 0.2f)
		{
			m_fSlowDelay = 0.f;
			fSlowValue = 1.f;
		}
		if (m_bAnimFinished)
		{
			m_fSlowDelay = 0.f;
			m_bStaminaCanDecrease = true;
			m_bParry = false;
			m_pParriedMonsterTransform = nullptr;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
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

void CPlayer::Move_Counter()
{
	// 플레이어 위치 보정
	_float3 fScale = m_pTransformCom->Get_Scaled();
	_vector vMonsterLook = XMVector3Normalize(m_pParriedMonsterTransform->Get_State(CTransform::STATE_LOOK));
	_vector vMonsterPos = m_pParriedMonsterTransform->Get_State(CTransform::STATE_POSITION);

	_vector vPlayerLook = XMVector3Normalize(-vMonsterLook) * fScale.z;
	_vector vPlayerRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerLook)) * fScale.x;
	_vector vPlayerUp = XMVector3Normalize(XMVector3Cross(vPlayerLook, vPlayerRight)) * fScale.y;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vPlayerRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vPlayerUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vPlayerLook);
	m_pPhysXCom->Set_Position(XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vMonsterPos + vMonsterLook * 2.f, 0.1f));
}

NodeStates CPlayer::Parry(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH
		|| m_bLAttacking || m_bRAttacking || m_fLChargeAttack != 0.f || m_fRChargeAttack != 0.f
		|| m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_Q) && !m_bParrying)
	{
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		m_bParrying = true;
		
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
	}

	if (m_bParrying)
	{
		m_iState = STATE_PARRY;

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			if (!m_bDisolved_Yaak)
			{
				static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
				m_bDisolved_Yaak = true;
			}
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
			m_bParrying = false;
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

NodeStates CPlayer::JumpAttack(_float fTimeDelta)
{
	if (m_fCurStamina < 10.f && m_bStaminaCanDecrease)
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000) && m_bJumping && m_iState != STATE_DASH && !m_bLAttacking)
	{
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		if (m_fFightIdle == 0.f && !m_bDisolved_Weapon)
		{

			static_cast<CPartObject*>(m_PartObjects[1])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Weapon = true;
		}
		m_iState = STATE_JUMPATTACK;
		m_bLAttacking = true;
		m_pPhysXCom->Set_JumpSpeed(-2.f);
		m_pPhysXCom->Go_Jump(fTimeDelta);
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
	}

	if (m_bLAttacking && (m_iState == STATE_JUMPATTACK || m_iState == STATE_JUMPATTACK_LAND))
	{
		if (m_bAnimFinished && m_iState == STATE_JUMPATTACK_LAND)
		{
			m_bStaminaCanDecrease = true;
			m_bLAttacking = false;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
			return SUCCESS;
		}
		else
		{
			/*m_pPhysXCom->Tick(fTimeDelta);*/
			if (!m_pPhysXCom->Get_IsJump())
			{
				m_pPhysXCom->Set_JumpSpeed(10.f);
				m_iState = STATE_JUMPATTACK_LAND;
			}
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CPlayer::RollAttack(_float fTimeDelta)
{
	if (m_bJumping || m_iState == STATE_DASH || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000) && m_iState == STATE_ROLL && !m_bLAttacking)
	{
		if (m_bStaminaCanDecrease)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		m_iState = STATE_ROLLATTACK;
		m_bLAttacking = true;
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
	}

	if (m_bLAttacking && m_iState == STATE_ROLLATTACK)
	{
		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_bLAttacking = false;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
			if (m_bRunning)
			{
				m_pTransformCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pTransformCom->Set_Speed(WALKSPEED);
			}
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

NodeStates CPlayer::LChargeAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH
		|| m_bRAttacking || m_fRChargeAttack > 0.f || m_iState == STATE_USEITEM || 
		(m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000))
	{
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		if (m_fFightIdle == 0.f && !m_bDisolved_Weapon)
		{
			static_cast<CPartObject*>(m_PartObjects[1])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Weapon = true;
		}
		if (!m_bLAttacking)
		{
			m_fLChargeAttack += fTimeDelta;
		}
		if (m_bCanCombo)
		{
			if (m_fCurStamina < 10.f && m_bStaminaCanDecrease)
			{
				return COOLING;
			}
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
			m_iAttackCount++;
			m_bCanCombo = false;
		}
	}
	else if (m_fLChargeAttack > 0.f && m_fLChargeAttack < 0.4f)
	{
		Add_Stamina(-10.f);
		m_fLChargeAttack = 0.f;
		m_bLAttacking = true;
		if (m_bRunning && ((GetKeyState('W') & 0x8000) || (GetKeyState('S') & 0x8000)))
		{
			m_bStaminaCanDecrease = true;
			Add_Stamina(-10.f);
			m_bIsRunAttack = true;
		}
	}

	if (m_fLChargeAttack != 0.f)
	{
		m_iState = STATE_LCHARGEATTACK;
		if (m_fLChargeAttack >= 0.7f)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_iAttackCount = 1;
			m_fLChargeAttack = 0.f;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
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

NodeStates CPlayer::RChargeAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH
		|| m_bLAttacking || m_fLChargeAttack > 0.f || m_iState == STATE_USEITEM
		|| (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if ((GetKeyState(VK_RBUTTON) & 0x8000))
	{
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		if (m_fFightIdle == 0.f && !m_bDisolved_Weapon)
		{
			static_cast<CPartObject*>(m_PartObjects[1])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Weapon = true;
		}
		if (!m_bRAttacking)
		{
			m_fRChargeAttack += fTimeDelta;
		}
		if (m_bCanCombo)
		{
			if (m_fCurStamina < 10.f && m_bStaminaCanDecrease)
			{
				return COOLING;
			}
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
			m_iAttackCount++;
			m_bCanCombo = false;
		}
	}
	else if (m_fRChargeAttack > 0.f && m_fRChargeAttack < 0.4f)
	{
		if (m_bStaminaCanDecrease)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		m_fRChargeAttack = 0.f;
		m_bRAttacking = true;
	}

	if (m_fRChargeAttack != 0.f)
	{
		if (m_fRChargeAttack >= 1.f)
		{
			if (m_bStaminaCanDecrease)
			{
				// 스테미나 조절할 것
				Add_Stamina(-10.f);
			}
		}
		m_iState = STATE_RCHARGEATTACK;

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_iAttackCount = 1;
			m_fRChargeAttack = 0.f;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
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

NodeStates CPlayer::LAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH
		|| m_bRAttacking || m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if (!m_bIsRunAttack)
	{
		m_iAttackCount %= 4;
	}
	else
	{
		m_iAttackCount %= 3;
	}

	if (m_iAttackCount == 0) m_iAttackCount = 1;
	if (m_bLAttacking)
	{
		if (m_bIsRunAttack)
		{
			switch (m_iAttackCount)
			{
			case 1:
				m_iState = STATE_RUNLATTACK1;
				break;
			case 2:
				m_iState = STATE_RUNLATTACK2;
				break;
			default:
				break;
			}

			m_pPhysXCom->Speed_Scaling(0.95f);
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else
		{
			switch (m_iAttackCount)
			{
			case 1:
				m_iState = STATE_LATTACK1;
				break;
			case 2:
				m_iState = STATE_LATTACK2;
				break;
			case 3:
				m_iState = STATE_LATTACK3;
				break;
			default:
				break;
			}
		}

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			m_bIsRunAttack = false;
			m_bLAttacking = false;
			m_iAttackCount++;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
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

NodeStates CPlayer::RAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	m_iAttackCount %= 3;
	if (m_iAttackCount == 0) m_iAttackCount = 1;
	if (m_bRAttacking)
	{
		switch (m_iAttackCount)
		{
		case 1:
			m_iState = STATE_RATTACK1;
			break;
		case 2:
			m_iState = STATE_RATTACK2;
			break;
		default:
			break;
		}

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_bRAttacking = false;
			m_iAttackCount++;
			m_fFightIdle = 0.01f;
			m_iState = STATE_FIGHTIDLE;
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

NodeStates CPlayer::Dash(_float fTimeDelta)
{
	if (!m_bJumping || m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_E) && m_bJumping && m_iState != STATE_DASH)
	{
		m_pPhysXCom->Set_JumpSpeed(0.f);
		m_pPhysXCom->Go_Jump(fTimeDelta);
		m_iState = STATE_DASH;
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
	}

	if (m_iState == STATE_DASH)
	{
		m_pPhysXCom->Set_Speed(ROLLSPEED);
		m_pTransformCom->Set_Speed(1.f);
		if (GetKeyState('S') & 0x8000)
		{
			m_pPhysXCom->Go_BackWard(fTimeDelta);
		}
		if (GetKeyState('W') & 0x8000)
		{
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}

		m_fAnimDelay += fTimeDelta;
		m_fCloneDelay += fTimeDelta;
		m_iState = STATE_DASH;

		// 잔상 생성
		if (m_fCloneDelay > CLONEDELAY)
		{
			CClone::CLONE_DESC		CloneDesc{};
			_uint iState = STATE_DASH;
			_float4x4 vPos = *m_pTransformCom->Get_WorldFloat4x4();
			CloneDesc.pParentMatrix = &vPos;
			CloneDesc.pState = &iState;
			CloneDesc.fAnimDelay = m_fAnimDelay;

			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Clone"), &CloneDesc);
			m_fCloneDelay = 0.f;
		}

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_iState = STATE_IDLE;
			m_fCloneDelay = 0.f;
			m_fAnimDelay = 0.f;
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		m_fCloneDelay = 0.f;
		m_fAnimDelay = 0.f;
		return FAILURE;
	}

}

NodeStates CPlayer::Jump(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) && m_fJumpCooltime == 0.f && (!m_bJumping || !m_bDoubleJumping))
	{
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
		m_fJumpCooltime += 0.01f;
		if (m_bJumping)
		{
			m_bDoubleJumping = true;
			m_iState = STATE_DOUBLEJUMPSTART;
		}
		else
		{
			m_bJumping = true;
			m_iState = STATE_JUMPSTART;
		}
		m_pPhysXCom->Go_Jump(fTimeDelta);

		m_bIsLanded = false;
	}

	if (m_bJumping)
	{

		/*m_pPhysXCom->Tick(fTimeDelta);*/

		if (GetKeyState('A') & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}
		else if (GetKeyState('D') & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
		if (GetKeyState('W') & 0x8000)
		{
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else if (GetKeyState('S') & 0x8000)
		{
			m_pPhysXCom->Go_BackWard(fTimeDelta);
		}

		if (m_bAnimFinished)
		{
			if (m_iState == STATE_JUMPSTART || m_iState == STATE_DOUBLEJUMPSTART)
			{
				m_iState = STATE_JUMP;
			}
		}

		return RUNNING;
	}
	else
	{
		if (!m_bIsLanded)
		{
			m_bStaminaCanDecrease = true;
			if (m_fFightIdle > ATTACKPOSTDELAY)
			{
				m_iState = STATE_IDLE;
			}
			else
			{
				m_iState = STATE_FIGHTIDLE;
				m_fFightIdle = 0.01f;
			}
			m_bIsLanded = true;
		}
		return FAILURE;
	}

	return FAILURE;
}

NodeStates CPlayer::Roll(_float fTimeDelta)
{
	if (m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_E) && m_iState != STATE_ROLL)
	{
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		m_iState = STATE_ROLL;
	}

	if (m_iState == STATE_ROLL)
	{
		m_pPhysXCom->Set_Speed(ROLLSPEED);
		if (GetKeyState('S') & 0x8000)
		{
			m_pPhysXCom->Go_BackWard(fTimeDelta);
		}
		if (GetKeyState('W') & 0x8000)
		{
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}

		m_fAnimDelay += fTimeDelta;
		m_fCloneDelay += fTimeDelta;

		// 잔상 생성
		if (m_fCloneDelay > CLONEDELAY)
		{
			CClone::CLONE_DESC		CloneDesc{};
			_uint iState = STATE_ROLL;
			_float4x4 vPos = *m_pTransformCom->Get_WorldFloat4x4();
			CloneDesc.pParentMatrix = &vPos;
			CloneDesc.pState = &iState;
			CloneDesc.fAnimDelay = m_fAnimDelay;

			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Clone"), &CloneDesc);
			m_fCloneDelay = 0.f;
		}

		if (m_bAnimFinished)
		{
			m_bStaminaCanDecrease = true;
			m_iState = STATE_IDLE;
			m_fCloneDelay = 0.f;
			m_fAnimDelay = 0.f;
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	else
	{
		m_fCloneDelay = 0.f;
		m_fAnimDelay = 0.f;
		return FAILURE;
	}
}

NodeStates CPlayer::UseItem(_float fTimeDelta)
{
	if (m_iState == STATE_BUFF)
	{
		return COOLING;
	}

	if (GetKeyState('Z') & 0x8000 && m_iState != STATE_USEITEM)
	{
		m_iState = STATE_USEITEM;
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
	}

	if (m_iState == STATE_USEITEM)
	{
		if (m_bAnimFinished)
		{
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
		m_fCloneDelay = 0.f;
		m_fAnimDelay = 0.f;
		return FAILURE;
	}
}

NodeStates CPlayer::Buff(_float fTimeDelta)
{
	if (GetKeyState('X') & 0x8000 && m_iState != STATE_BUFF)
	{
		m_iState = STATE_BUFF;
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
	}

	if (m_iState == STATE_BUFF)
	{
		if (m_bAnimFinished)
		{
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
		m_fCloneDelay = 0.f;
		m_fAnimDelay = 0.f;
		return FAILURE;
	}
}

NodeStates CPlayer::Move(_float fTimeDelta)
{
	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}
	else if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if ((GetKeyState(VK_LSHIFT) & 0x8000) && m_fButtonCooltime == 0.f)
	{
		m_bRunning = !m_bRunning;
		if (m_bRunning)
		{
			m_pPhysXCom->Set_Speed(RUNSPEED);
		}
		else
		{
			m_pPhysXCom->Set_Speed(WALKSPEED);
		}
		m_fButtonCooltime = 0.001f;
	}

	if (GetKeyState('W') & 0x8000)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		m_pPhysXCom->Go_Straight(fTimeDelta);
		if (m_bRunning)
		{
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{
			m_iState = STATE_WALK;
		}
		return SUCCESS;
	}
	else if (GetKeyState('S') & 0x8000)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		m_pPhysXCom->Go_BackWard(fTimeDelta);
		if (m_bRunning)
		{
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{
			m_iState = STATE_WALK;
		}
		return SUCCESS;
	}
	else
	{
		if (m_fFightIdle > ATTACKPOSTDELAY || m_fFightIdle == 0.f)
		{
			m_iState = STATE_IDLE;
		}
		else
		{
			m_iState = STATE_FIGHTIDLE;
		}
		return FAILURE;
	}
}

NodeStates CPlayer::Idle(_float fTimeDelta)
{
	if (m_iState == STATE_FIGHTIDLE)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		m_fFightIdle += fTimeDelta;
		if (m_fFightIdle > ATTACKPOSTDELAY)
		{
			m_iAttackCount = 1;
			m_fFightIdle = 0.f;
			m_iState = STATE_IDLE;
		}
		else if (m_fFightIdle > ATTACKPOSTDELAY - 0.2f)
		{
			static_cast<CPartObject*>(m_PartObjects[1])->Set_DisolveType(CPartObject::TYPE_DECREASE);
		}

	}
	else if (m_iState == STATE_IDLE)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		m_bDisolved_Weapon = false;
		m_iState = STATE_IDLE;
	}
	return RUNNING;
}

void CPlayer::Add_Hp(_float iValue)
{
	m_fCurHp = min(m_fMaxHp, max(0.f, m_fCurHp + iValue));
	if (m_fCurHp == 0)
	{
		m_iState = STATE_DEAD;
	}
}

void CPlayer::Add_Stamina(_float iValue)
{
	if (iValue > 0.f)
	{
		m_fCurStamina = min(m_fMaxStamina, max(0.f, m_fCurStamina + iValue));
	}
	else if (m_bStaminaCanDecrease)
	{
		m_fStaminaRecoverDelay = STAMINARECOVERDELAY;
		m_fCurStamina = min(m_fMaxStamina, max(0.f, m_fCurStamina + iValue));
		m_bStaminaCanDecrease = false;
	}
	
}

void CPlayer::Add_Mp(_float iValue)
{
	m_fCurMp = min(m_fMaxMp, max(0.f, m_fCurMp + iValue));
}


void CPlayer::OnShapeHit(const PxControllerShapeHit& hit)
{

	PxFilterData hitObjectFilterData = hit.shape->getSimulationFilterData();
	// 충돌한 객체가 무기(검)인 경우
	if (hitObjectFilterData.word0 & CollisionGropuID::GROUP_WEAPON)
	{
		// 무기와의 충돌은 무시 (이미 필터 셰이더에서 처리되었지만, 추가 안전장치로 사용)
		return;
	}
	// 충돌한 객체가 환경(지형, 벽 등)인 경우
	if (hitObjectFilterData.word0 & CollisionGropuID::GROUP_ENVIRONMENT)
	{
		// 환경과의 충돌 처리 (예: 이동 제한, 슬라이딩 등)
		int temp = 0;
	}
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
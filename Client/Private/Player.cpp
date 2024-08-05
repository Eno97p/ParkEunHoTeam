#include "stdafx.h"
#include "Player.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Clone.h"
#include "Body_Player.h"

#include"CHitReport.h"
#include "EffectManager.h"
#include "ThirdPersonCamera.h"
#include "CHoverBoard.h"
#include "Monster.h"

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
	pDesc->fRotationPerSec = XMConvertToRadians(720.f);
	m_InitialPosition = { pDesc->mWorldMatrix._41, pDesc->mWorldMatrix._42, pDesc->mWorldMatrix._43 }; //초기 위치 설정

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	m_iLevel = 1;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_PlayerPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	// Test
	if (m_pGameInstance->Get_DIKeyState(DIK_B))
	{
		m_pGameInstance->Set_MotionBlur(true);
	}
	else
	{
		m_pGameInstance->Set_MotionBlur(false);
	}

	if (m_pGameInstance->Key_Down(DIK_H))
	{
		_float4 TorPos;
		XMStoreFloat4(&TorPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		EFFECTMGR->Generate_Tornado(0, TorPos, this);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_C) && m_fButtonCooltime == 0.f)
	{
		if (m_bIsCloaking)
		{
			m_bIsCloaking = false;
		}
		else if(m_fCurMp >= 10.f)
		{
			m_bIsCloaking = true;
			Add_Mp(-10.f);
		}
		m_fButtonCooltime = 0.001f;
	}

	if (!m_pCameraTransform)
	{
		list<CGameObject*> CameraList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"));
		auto iter = ++CameraList.begin();
		m_pCameraTransform = dynamic_cast<CTransform*>((*iter)->Get_Component(TEXT("Com_Transform")));
		Safe_AddRef(m_pCameraTransform);
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
	m_bAnimFinished = dynamic_cast<CBody_Player*>(m_PartObjects.front())->Get_AnimFinished();
	if (m_fStaminaRecoverDelay > 0.f)
	{
		m_fStaminaRecoverDelay -= fTimeDelta;
	}
	else if (m_fStaminaRecoverDelay < 0.f)
	{
		Add_Stamina(fTimeDelta * 100.f);
	}

	if (m_bIsCloaking)
	{
		Add_Mp(fTimeDelta * -1.f);
	}

	//마나는 자동회복 X
	if (m_fCurMp <= 0.f)
	{
		m_bIsCloaking = false;
	}
}

void CPlayer::Tick(_float fTimeDelta)
{
	Change_Weapon();

	if (m_bRided)
	{
		m_pTransformCom->Set_WorldMatrix(m_pHoverBoardTransform->Get_WorldMatrix());
	}

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

	m_pBehaviorCom->Update(fTimeDelta);

	if (m_bRiding)
	{
		m_pPhysXCom->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	if (m_iState != STATE_DASH)
	{
		m_pPhysXCom->Tick(fTimeDelta);
	}

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());


	m_fParticleAcctime -= fTimeDelta;
	if (m_fParticleAcctime < 0.f)
	{
		m_fParticleAcctime = 0.1f;
		_float4 vParticlePos;
		XMStoreFloat4(&vParticlePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vParticlePos.y += 1.f;
		EFFECTMGR->Generate_Particle(10, vParticlePos);
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


	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	CPhysXComponent_Character::ControllerDesc		PhysXDesc;
	PhysXDesc.pTransform = m_pTransformCom;
	PhysXDesc.fJumpSpeed = JUMPSPEED;
	PhysXDesc.height = 1.0f;			//캡슐 높이
	PhysXDesc.radius = 0.5f;		//캡슐 반지름
	PhysXDesc.position = PxExtendedVec3(m_InitialPosition.x, PhysXDesc.height * 0.5f + PhysXDesc.radius + m_InitialPosition.y, m_InitialPosition.z);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.001f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.1f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	PhysXDesc.pName = "Player";
	PhysXDesc.filterData.word0 = Engine::CollisionGropuID::GROUP_PLAYER;
	PhysXDesc.filterData.word1 = (Engine::CollisionGropuID::GROUP_ENVIRONMENT | Engine::CollisionGropuID::GROUP_ENEMY) & ~GROUP_NONCOLLIDE;
	PhysXDesc.pGameObject = this;
	PhysXDesc.pFilterCallBack = [this](const PxController& a, const PxController& b) {return this->OnFilterCallback(a, b); };
	CHitReport::GetInstance()->SetShapeHitCallback([this](const PxControllerShapeHit& hit){this->OnShapeHit(hit);});
	CHitReport::GetInstance()->SetControllerHitCallback([this](const PxControllersHit& hit){this->OnControllerHit(hit);});
	//CHitReport::GetInstance()->SetFilterCallback([this](const PxController& a, const PxController& b) {return this->OnFilterCallback(a, b); });
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.pCanCombo = &m_bCanCombo;
	BodyDesc.pIsCloaking = &m_bIsCloaking;
	BodyDesc.pCurWeapon = &m_iCurWeapon;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Player"), &BodyDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);

	/* 무기객체를 복제해온다. */
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pCurWeapon = &m_iCurWeapon;

	CModel* pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_sword");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DurgaSword"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	CGameObject* pWeapon2 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_PretorianSword"), &WeaponDesc);
	if (nullptr == pWeapon2)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon2);

	CGameObject* pWeapon3 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RadamantheSword"), &WeaponDesc);
	if (nullptr == pWeapon3)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon3);

	CGameObject* pWeapon4 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_WhisperSword_Anim"), &WeaponDesc);
	if (nullptr == pWeapon4)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon4);

	dynamic_cast<CBody_Player*>(m_PartObjects[0])->Set_Weapon(dynamic_cast<CWeapon*>(m_PartObjects[WEAPON_DURGASWORD + 1]), WEAPON_DURGASWORD);
	dynamic_cast<CBody_Player*>(m_PartObjects[0])->Set_Weapon(dynamic_cast<CWeapon*>(m_PartObjects[WEAPON_PRETORIANSWORD + 1]), WEAPON_PRETORIANSWORD);
	dynamic_cast<CBody_Player*>(m_PartObjects[0])->Set_Weapon(dynamic_cast<CWeapon*>(m_PartObjects[WEAPON_RADAMANTHESWORD + 1]), WEAPON_RADAMANTHESWORD);
	dynamic_cast<CBody_Player*>(m_PartObjects[0])->Set_Weapon(dynamic_cast<CWeapon*>(m_PartObjects[WEAPON_ELISH + 1]), WEAPON_ELISH);
	return S_OK;
}

CGameObject* CPlayer::Get_Weapon()
{
	return m_PartObjects[m_iCurWeapon + 1];
}

void CPlayer::PlayerHit(_float fValue)
{
	if (m_bParrying || m_iState == STATE_ROLL || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_iState == STATE_SPECIALATTACK || 
		m_iState == STATE_SPECIALATTACK2 || m_iState == STATE_SPECIALATTACK3 || m_iState == STATE_SPECIALATTACK4 || m_bParry ) return;
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

	CUI_Manager::GetInstance()->Set_ScreenBloodRend(true);
}

void CPlayer::Parry_Succeed()
{
	vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());
	dynamic_cast<CThirdPersonCamera*>(cams[1])->Set_ZoomIn();
	//dynamic_cast<CThirdPersonCamera*>(m_pGameInstance->Get_MainCamera())->Set_ZoomIn();
	_float4x4 WeaponMat = *static_cast<CPartObject*>(m_PartObjects[1])->Get_Part_Mat();
	_float4 vParticlePos = { WeaponMat._41,WeaponMat._42,WeaponMat._43,1.f };
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	PlayerPos.y += 1.f;
	EFFECTMGR->Generate_Distortion(2, PlayerPos);
	EFFECTMGR->Generate_Particle(6, vParticlePos);
	m_bParry = true;
	m_fSlowDelay = 0.f;
	fSlowValue = 0.2f;
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
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("SpecialAttack"), bind(&CPlayer::SpecialAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("LChargeAttack"), bind(&CPlayer::LChargeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("RChargeAttack"), bind(&CPlayer::RChargeAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("LAttack"), bind(&CPlayer::LAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("RAttack"), bind(&CPlayer::RAttack, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Slide"), bind(&CPlayer::Slide, this, std::placeholders::_1));
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
		m_bIsCloaking = false;
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
		m_bIsCloaking = false;
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
		m_bIsCloaking = false;
		if (m_bAnimFinished)
		{
			if (m_bRiding)
			{
				m_iState = STATE_SLIDE;
			}
			else
			{
				m_fFightIdle = 0.01f;
				m_iState = STATE_FIGHTIDLE;
			}
			m_fLChargeAttack = false;
			m_fRChargeAttack = false;
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
	if (m_bRiding)
	{
		return COOLING;
	}

	if (m_bParry && m_iState != STATE_COUNTER)
	{
		m_bIsCloaking = false;
		m_iState = STATE_PARRY;
		m_fParryFrame = 0.f;
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
			m_pParriedMonsterFloat4x4 = nullptr;
		}
	}

	// 패링 성공 && 왼클릭
	if (m_bParry && (GetKeyState(VK_LBUTTON) & 0x8000) && m_iState != STATE_COUNTER && m_pParriedMonsterFloat4x4)
	{
		_vector vPos = XMVectorSet(m_pParriedMonsterFloat4x4->_41, m_pParriedMonsterFloat4x4->_42, m_pParriedMonsterFloat4x4->_43, 1.f);
		// 일정거리 이하일 때 카운터 발동
		if (XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPos)) < 4.f)
		{
			vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());
			dynamic_cast<CThirdPersonCamera*>(cams[1])->Set_ZoomIn();
			m_bParrying = false;
			m_bStaminaCanDecrease = true;
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
			vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());
			dynamic_cast<CThirdPersonCamera*>(cams[1])->Set_ZoomOut();

			m_fSlowDelay = 0.f;

			m_bStaminaCanDecrease = true;
			m_bParry = false;
			m_pParriedMonsterFloat4x4 = nullptr;
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
	_vector vMonsterLook = XMVector3Normalize(XMVectorSet(m_pParriedMonsterFloat4x4->_31, m_pParriedMonsterFloat4x4->_32, m_pParriedMonsterFloat4x4->_33, 0.f));
	_vector vMonsterPos =XMVectorSet(m_pParriedMonsterFloat4x4->_41, m_pParriedMonsterFloat4x4->_42, m_pParriedMonsterFloat4x4->_43, 1.f);

	_vector vPlayerLook = XMVector3Normalize(-vMonsterLook) * fScale.z;
	_vector vPlayerRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerLook)) * fScale.x;
	_vector vPlayerUp = XMVector3Normalize(XMVector3Cross(vPlayerLook, vPlayerRight)) * fScale.y;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vPlayerRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vPlayerUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vPlayerLook);
	m_pPhysXCom->Set_Position(XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vMonsterPos + vMonsterLook * 1.5f, 0.1f));
}

NodeStates CPlayer::Parry(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_iState == STATE_RIDE || m_iState == STATE_SLIDE
		|| m_bLAttacking || m_bRAttacking || m_fLChargeAttack != 0.f || m_fRChargeAttack != 0.f || m_fSpecialAttack != 0.f
		|| m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease) || m_bRiding)
	{
		return COOLING;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_Q) && m_iState != STATE_PARRY)
	{
		m_bIsCloaking = false;
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}
		m_iState = STATE_PARRY;
		m_bStaminaCanDecrease = true;
		m_fParryFrame = 0.f;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
	}

	if (m_iState == STATE_PARRY/* && !m_bParry*/)
	{
		m_fParryFrame += fTimeDelta;
		if (m_fParryFrame > PARRYSTART && m_fParryFrame < PARRYEND)
		{
			m_bParrying = true;
		}
		else 
		{
			m_bParrying = false;
		}
		
		if (m_bAnimFinished)
		{
			m_fParryFrame = 0.f;
			vector<CCamera*> cams = (m_pGameInstance->Get_Cameras());
			dynamic_cast<CThirdPersonCamera*>(cams[1])->Set_ZoomOut();
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
	if (m_bRiding)
	{
		return COOLING;
	}

	m_fSlowDelay = 0.f;
	fSlowValue = 1.f;

	if (m_fCurStamina < 10.f && m_bStaminaCanDecrease)
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000) && m_bJumping && m_iState != STATE_DASH && !m_bLAttacking)
	{
		m_bIsCloaking = false;
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
		
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
		m_fJumpAttackdelay = 0.7f;
	}

	if (m_bLAttacking && (m_iState == STATE_JUMPATTACK || m_iState == STATE_JUMPATTACK_LAND))
	{
		m_fJumpAttackdelay -= fTimeDelta;
		if (m_fJumpAttackdelay > 0.f)
		{
			m_pPhysXCom->Set_JumpSpeed(0.f);
			m_pPhysXCom->Go_Jump(fTimeDelta);
		}
		else if (m_fJumpAttackdelay < 0.f && m_fJumpAttackdelay > -0.2f)
		{
			m_pPhysXCom->Set_JumpSpeed(-20.f);
			m_pPhysXCom->Go_Jump(fTimeDelta);
			m_fJumpAttackdelay = -0.3f;
		}
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
				m_pPhysXCom->Set_JumpSpeed(JUMPSPEED);
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
	if (m_bJumping || m_iState == STATE_DASH || (m_fCurStamina < 10.f && m_bStaminaCanDecrease) || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT)
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000) && m_iState == STATE_ROLL && !m_bLAttacking)
	{
		m_bIsCloaking = false;
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

NodeStates CPlayer::SpecialAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_bRiding
		|| m_bRAttacking || m_fLChargeAttack > 0.f || m_fRChargeAttack > 0.f || m_iState == STATE_USEITEM ||
		(m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	switch (m_iCurWeapon)
	{
	case WEAPON_DURGASWORD:
		return Special3(fTimeDelta);
		break;
	case WEAPON_PRETORIANSWORD:
		return Special4(fTimeDelta);
		break;
	case WEAPON_RADAMANTHESWORD:
		return Special2(fTimeDelta);
		break;
	case WEAPON_ELISH:
		return Special1(fTimeDelta);
		break;
	}
	return FAILURE;
}

NodeStates CPlayer::Special1(_float fTimeDelta)
{
	if ((GetKeyState(VK_LBUTTON) & 0x8000) && (GetKeyState(VK_RBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
		if (m_iState != STATE_SPECIALATTACK)
		{
			m_fSpecialAttack += fTimeDelta;
		}
	}
	else if (m_fSpecialAttack > 0.f && m_fSpecialAttack < 1.f)
	{
		m_fSpecialAttack = 0.f;
		return FAILURE;
	}

	if (m_fSpecialAttack != 0.f)
	{
		if (m_iState == STATE_SPECIALATTACK)
		{
			m_fSpecialAttack += fTimeDelta;
		}
		m_iState = STATE_SPECIALATTACK;
		if (m_fSpecialAttack >= 1.f)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
			if (m_fSpecialAttack <= 1.15f)
			{
				m_pPhysXCom->Set_Speed(100.f);
				m_pPhysXCom->Go_Straight(fTimeDelta);
			}
			else if(m_fSpecialAttack > 2.f)
			{
				fSlowValue = 0.05f;
			}

			if (m_fSpecialAttack >= 2.f + BRISDELAY)
			{
				if (m_fBRIS < 2.f + BRISDELAY)
				{
					m_fBRIS += fTimeDelta * 2.f / BRISDELAY;
				}
				m_pGameInstance->Set_BRIS(0.1f);
				m_pGameInstance->Set_Mirror(m_fBRIS - 2.f);
			}
			else if (m_fSpecialAttack >= 2.f)
			{
				// 2.f : 갈라지는 시간(x - 1.f = 갈라지는 총 시간)
				if (m_fBRIS < 2.f)
				{
					m_fBRIS += fTimeDelta * 2.f / BRISDELAY;
				}
				m_pGameInstance->Set_BRIS(m_fBRIS * 0.1f / 2.f);
			}
		}
		if (m_bAnimFinished)
		{
			fSlowValue = 1.f;
			m_fBRIS = 0.f;
			m_pGameInstance->Set_BRIS(m_fBRIS);
			m_pGameInstance->Set_Mirror(m_fBRIS);
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			m_bStaminaCanDecrease = true;
			m_iAttackCount = 1;
			m_fSpecialAttack = 0.f;
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

NodeStates CPlayer::Special2(_float fTimeDelta)
{
	if ((GetKeyState(VK_LBUTTON) & 0x8000) && (GetKeyState(VK_RBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
		if (m_iState != STATE_SPECIALATTACK2)
		{
			m_fSpecialAttack += fTimeDelta;
		}
	}
	else if (m_fSpecialAttack > 0.f && m_fSpecialAttack < 1.f)
	{
		m_fSpecialAttack = 0.f;
		return FAILURE;
	}

	if (m_fSpecialAttack != 0.f)
	{
		if (m_iState == STATE_SPECIALATTACK2)
		{
			m_fSpecialAttack += fTimeDelta;
		}
		m_iState = STATE_SPECIALATTACK2;
		if (m_fSpecialAttack >= 1.f)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		if (m_bAnimFinished)
		{
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			m_bStaminaCanDecrease = true;
			m_iAttackCount = 1;
			m_fSpecialAttack = 0.f;
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

NodeStates CPlayer::Special3(_float fTimeDelta)
{
	if ((GetKeyState(VK_LBUTTON) & 0x8000) && (GetKeyState(VK_RBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
		if (m_iState != STATE_SPECIALATTACK3)
		{
			m_fSpecialAttack += fTimeDelta;
		}
	}
	else if (m_fSpecialAttack > 0.f && m_fSpecialAttack < 0.5f)
	{
		m_fSpecialAttack = 0.f;
		return FAILURE;
	}

	if (m_fSpecialAttack != 0.f)
	{
		if (m_iState == STATE_SPECIALATTACK3)
		{
			m_fSpecialAttack += fTimeDelta;
		}
		m_iState = STATE_SPECIALATTACK3;
		if (m_fSpecialAttack >= 0.5f)
		{
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		if (m_bAnimFinished)
		{
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			m_bStaminaCanDecrease = true;
			m_fSpecialAttack = 0.f;
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

NodeStates CPlayer::Special4(_float fTimeDelta)
{
	if ((GetKeyState(VK_LBUTTON) & 0x8000) && (GetKeyState(VK_RBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
		if (m_iState != STATE_SPECIALATTACK4)
		{
			m_fSpecialAttack += fTimeDelta;
		}
		fSlowValue = 0.1f;
	}
	else if (m_fSpecialAttack > 0.f && m_fSpecialAttack < 0.1f)
	{
		m_fSpecialAttack = 0.f;
		return FAILURE;
	}

	if (m_fSpecialAttack != 0.f)
	{
		if (m_iState == STATE_SPECIALATTACK4)
		{
			m_fSpecialAttack += fTimeDelta;
		}
		m_iState = STATE_SPECIALATTACK4;
		if (m_fSpecialAttack >= 0.1f)
		{
			fSlowValue = 1.f;
			// 스테미나 조절할 것
			Add_Stamina(-10.f);
		}
		if (m_bAnimFinished)
		{
			if (m_bRunning)
			{
				m_pPhysXCom->Set_Speed(RUNSPEED);
			}
			else
			{
				m_pPhysXCom->Set_Speed(WALKSPEED);
			}
			m_bStaminaCanDecrease = true;
			m_iAttackCount = 1;
			m_fSpecialAttack = 0.f;
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

NodeStates CPlayer::LChargeAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_bRiding
		|| m_bRAttacking || m_fRChargeAttack > 0.f || m_iState == STATE_USEITEM || 
		(m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
			if (m_fCurStamina < 10.f)
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
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_bRiding
		|| m_bLAttacking || m_fLChargeAttack > 0.f || m_iState == STATE_USEITEM
		|| (m_fCurStamina < 10.f && m_bStaminaCanDecrease))
	{
		return COOLING;
	}

	if ((GetKeyState(VK_RBUTTON) & 0x8000))
	{
		m_bIsCloaking = false;
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
			if (m_fCurStamina < 10.f)
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
	else if (m_fRChargeAttack > 0.f && m_fRChargeAttack < 0.2f)
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
		if (m_fRChargeAttack >= 0.3f)
		{
			if (m_bStaminaCanDecrease)
			{
				// 스테미나 조절할 것
				Add_Stamina(-10.f);
			}
			// 락온 상태일 때
			if (CThirdPersonCamera::m_bIsTargetLocked)
			{
				_float3 fScale = m_pTransformCom->Get_Scaled();

				_vector vDir = XMLoadFloat4(&CThirdPersonCamera::m_vLockedTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vDir.m128_f32[1] = 0.f;

				_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
				_vector vUp = XMVector3Cross(vDir, vRight);

				_float fLength = XMVectorGetX(XMVector3Length(vDir));

				vDir = XMVector3Normalize(vDir);
				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
				m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir);
				m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);
				m_pPhysXCom->Go_Straight(fTimeDelta * fLength);
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
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_bRiding
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
		if (CanBackAttack() || m_iState == STATE_BACKATTACK)
		{
			m_iState = STATE_BACKATTACK;
		}
		else if (m_bIsRunAttack)
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

_bool CPlayer::CanBackAttack()
{
	list<CGameObject*>& MonsterList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Monster"));
	for (auto iter : MonsterList)
	{
		if (static_cast<CMonster*>(iter)->CanBackAttack())
		{
			return true;
		}
	}
	return false;
}

NodeStates CPlayer::RAttack(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_bJumping || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_iState == STATE_USEITEM || (m_fCurStamina < 10.f && m_bStaminaCanDecrease) ||
		m_bRiding)
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

void CPlayer::Generate_HoverBoard()
{
	if (m_pGameInstance->Get_DIKeyState(DIK_R) && m_fButtonCooltime == 0.f && !m_bRided)
	{
		m_fButtonCooltime = 0.001f;
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 fPos = _float3(vPos.m128_f32[0] + vLook.m128_f32[0] * 3.f, vPos.m128_f32[1] + vLook.m128_f32[1] * 3.f, vPos.m128_f32[2] + vLook.m128_f32[2] * 3.f);
		CHoverboard::HoverboardInfo hoverboardInfo;
		hoverboardInfo.vPosition = fPos;
		hoverboardInfo.vLook = _float3(vLook.m128_f32[0], vLook.m128_f32[1], vLook.m128_f32[2]);
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Vehicle"));
		m_pHoverBoard = dynamic_cast<CHoverboard*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HoverBoard"), &hoverboardInfo));
		m_pGameInstance->CreateObject_Self(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Vehicle"), m_pHoverBoard);
		m_pHoverBoardTransform = dynamic_cast<CTransform*>(m_pHoverBoard->Get_Component(TEXT("Com_Transform")));
		m_pHoverBoard->Set_DisolveType(CHoverboard::TYPE_INCREASE);
	}
}

NodeStates CPlayer::Slide(_float fTimeDelta)
{
	if (m_iState == STATE_ROLL || m_iState == STATE_DASH || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_iState == STATE_USEITEM)
	{
		return COOLING;
	}

	Generate_HoverBoard();

	if (m_pGameInstance->Get_DIKeyState(DIK_F) && m_fButtonCooltime == 0.f && m_pHoverBoard)
	{
		m_iState = STATE_JUMP;
		if (m_bRiding)
		{
			m_pHoverBoard->Set_DisolveType(CHoverboard::TYPE_DECREASE);
			m_pHoverBoard = nullptr;
			m_pHoverBoardTransform = nullptr;
		}
		m_bRiding = !m_bRiding;
		m_bJumping = true;
		m_fButtonCooltime = 0.001f;
		m_pPhysXCom->Go_Jump(fTimeDelta);
	}

	if (m_iState == STATE_JUMP && m_bRiding)
	{
		if (m_bAnimFinished)
		{
			m_bRided = true;
			m_bJumping = false;
			m_iState = STATE_SLIDE;
			m_pPhysXCom->Set_Gravity(false);
			m_pHoverBoard->On_Ride();
		}
		else
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vGoalPos = m_pHoverBoardTransform->Get_State(CTransform::STATE_POSITION);
			
			m_pTransformCom->LookAt_For_LandObject(vGoalPos);

			vGoalPos.m128_f32[1] = vPos.m128_f32[1];
			_float fLength = XMVectorGetX(XMVector3Length(vPos - vGoalPos));
			if (fLength > 0.1f)
			{
				m_pTransformCom->Go_Straight(fTimeDelta);
			}
		}
		return RUNNING;
	}

	if (m_iState == STATE_SLIDE)
	{
		return RUNNING;
	}

	return FAILURE;
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
		m_pPhysXCom->Set_Speed(15.f);
		m_pTransformCom->Set_Speed(1.f);
		m_pPhysXCom->Go_Straight(fTimeDelta);

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

			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Clone"), &CloneDesc);
			m_fCloneDelay = 0.f;
		}

		if (m_bAnimFinished)
		{
			if (m_bFalling)
			{
				m_bFalling = false;
				return RUNNING;
			}
			else
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
	// 바닥에 닿으면
	if (!m_pPhysXCom->Get_IsJump())
	{
		m_bJumping = false;
		m_bFalling = false;
		m_bDoubleJumping = false;
		m_pPhysXCom->Set_JumpSpeed(JUMPSPEED);
	}
	if (!m_bFalling && m_pPhysXCom->Get_CurrentJumpSpeed() < 0.f)
	{
		// 아래로 걸어갈때 발동 안하기 위해 값 적당히 조절할것
		_float fLengthFromGround = m_pPhysXCom->Get_LengthFromGround();
		if (fLengthFromGround > 2.f)
		{
			m_bJumping = true;
			m_bFalling = true;
			m_iState = STATE_JUMP;
		}
	}
	else if (m_bFalling)
	{
		m_iState = STATE_JUMP;
	}

	if (m_iState == STATE_ROLL || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT || m_iState == STATE_USEITEM)
	{
		return COOLING;
	}
	
	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) && m_fJumpCooltime == 0.f && (!m_bJumping || !m_bDoubleJumping) && (m_fCurStamina >= 10.f || m_bStaminaCanDecrease))
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
		_float3 fScale = m_pTransformCom->Get_Scaled();
		_vector vRight;
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		vRight = XMVector3Cross(vUp, vLook);
		vLook = XMVector3Cross(vRight, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
		m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);

		m_pPhysXCom->Set_Gravity();
		m_bRided = false;

		if (GetKeyState('A') & 0x8000)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else if (GetKeyState('D') & 0x8000)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else if (GetKeyState('S') & 0x8000)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
			m_pPhysXCom->Go_Straight(fTimeDelta);
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

	if (m_pGameInstance->Get_DIKeyState(DIK_E) && m_iState != STATE_ROLL && m_iState != STATE_DASH_FRONT && m_iState != STATE_DASH_BACK &&
		m_iState != STATE_DASH_LEFT && m_iState != STATE_DASH_RIGHT)
	{
		m_bStaminaCanDecrease = true;
		// 스테미나 조절할 것
		Add_Stamina(-10.f);
		if (!m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_DECREASE);
			m_bDisolved_Yaak = true;
		}

		if (CThirdPersonCamera::m_bIsTargetLocked && !m_bRunning)
		{
			if (GetKeyState('S') & 0x8000)
			{
				m_iState = STATE_DASH_BACK;
			}
			else if (GetKeyState('A') & 0x8000)
			{
				m_iState = STATE_DASH_LEFT;
			}
			else if (GetKeyState('D') & 0x8000)
			{
				m_iState = STATE_DASH_RIGHT;
			}
			else
			{
				m_iState = STATE_DASH_FRONT;
			}
		}
		else
		{
			m_iState = STATE_ROLL;
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		
	}

	if (m_iState == STATE_ROLL || m_iState == STATE_DASH_FRONT || m_iState == STATE_DASH_BACK ||
		m_iState == STATE_DASH_LEFT || m_iState == STATE_DASH_RIGHT)
	{
		m_pPhysXCom->Set_Speed(ROLLSPEED);
		if (CThirdPersonCamera::m_bIsTargetLocked && !m_bRunning)
		{
			if (m_iState == STATE_DASH_BACK)
			{
				m_pPhysXCom->Go_BackWard(fTimeDelta);
			}
			else if (m_iState == STATE_DASH_LEFT)
			{
				m_pPhysXCom->Go_Left(fTimeDelta);
			}
			else if (m_iState == STATE_DASH_RIGHT)
			{
				m_pPhysXCom->Go_Right(fTimeDelta);
			}
			else if (m_iState == STATE_DASH_FRONT)
			{
				m_pPhysXCom->Go_Straight(fTimeDelta);
			}
		}
		else
		{
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}

		m_fAnimDelay += fTimeDelta;
		m_fCloneDelay += fTimeDelta;

		// 잔상 생성
		if (m_fCloneDelay > CLONEDELAY)
		{
			CClone::CLONE_DESC		CloneDesc{};
			_uint iState = m_iState;
			_float4x4 vPos = *m_pTransformCom->Get_WorldFloat4x4();
			CloneDesc.pParentMatrix = &vPos;
			CloneDesc.pState = &iState;
			CloneDesc.fAnimDelay = m_fAnimDelay;

			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Clone"), &CloneDesc);
			m_fCloneDelay = 0.f;
		}

		if (m_bAnimFinished)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
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
		EFFECTMGR->Generate_HealEffect(0, m_pTransformCom->Get_WorldFloat4x4());
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
	if (m_fCurStamina < 1.f)
	{
		m_bRunning = false;
	}

	if (m_bRunning)
	{
		m_pPhysXCom->Set_Speed(RUNSPEED);
	}
	else
	{
		m_pPhysXCom->Set_Speed(WALKSPEED);
	}

	if ((GetKeyState(VK_LSHIFT) & 0x8000) && m_fButtonCooltime == 0.f)
	{
		m_bRunning = !m_bRunning;
		m_fButtonCooltime = 0.001f;
	}

	if (GetKeyState('A') & 0x8000)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		if (m_bRunning)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
			m_pPhysXCom->Go_Straight(fTimeDelta);
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{

			if (CThirdPersonCamera::m_bIsTargetLocked)
			{
				// 왼쪽으로 가기
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_Left(fTimeDelta);
				m_iState = STATE_LOCKON_LEFT;
			}
			else
			{
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
				m_pPhysXCom->Go_Straight(fTimeDelta);
				m_iState = STATE_WALK;
			}
		}
		return SUCCESS;
	}
	else if (GetKeyState('D') & 0x8000)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		if (m_bRunning)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
			m_pPhysXCom->Go_Straight(fTimeDelta);
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{

			if (CThirdPersonCamera::m_bIsTargetLocked)
			{
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_Right(fTimeDelta);
				m_iState = STATE_LOCKON_RIGHT;
			}
			else
			{
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_RIGHT));
				m_pPhysXCom->Go_Straight(fTimeDelta);
				m_iState = STATE_WALK;
			}
		}
		return SUCCESS;
	}
	else if (GetKeyState('W') & 0x8000)
	{
		if (m_bDisolved_Yaak)
		{
			static_cast<CPartObject*>(m_PartObjects[0])->Set_DisolveType(CPartObject::TYPE_INCREASE);
			m_bDisolved_Yaak = false;
		}
		if (m_bRunning)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
			m_pPhysXCom->Go_Straight(fTimeDelta);
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{

			if (CThirdPersonCamera::m_bIsTargetLocked)
			{
				// 앞으로 가기
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_Straight(fTimeDelta);
				m_iState = STATE_LOCKON_STRAIGHT;
			}
			else
			{
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_Straight(fTimeDelta);
				m_iState = STATE_WALK;
			}
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
		if (m_bRunning)
		{
			m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
			m_pPhysXCom->Go_Straight(fTimeDelta);
			m_iState = STATE_RUN;
			m_bStaminaCanDecrease = true;
			// 스테미나 조절할 것
			Add_Stamina(-fTimeDelta * 5.f);
		}
		else
		{

			if (CThirdPersonCamera::m_bIsTargetLocked)
			{
				// 뒤로 가기
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_BackWard(fTimeDelta);
				m_iState = STATE_LOCKON_BACKWARD;
			}
			else
			{
				m_pTransformCom->TurnToTarget(fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pCameraTransform->Get_State(CTransform::STATE_LOOK));
				m_pPhysXCom->Go_Straight(fTimeDelta);
				m_iState = STATE_WALK;
			}
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

void CPlayer::Change_Weapon()
{
	if (m_iState == STATE_JUMPATTACK || m_iState == STATE_JUMPATTACK_LAND || m_iState == STATE_ROLLATTACK || m_iState == STATE_SPECIALATTACK || 
		m_iState == STATE_SPECIALATTACK2 || m_iState == STATE_SPECIALATTACK3 || m_iState == STATE_SPECIALATTACK4 || 
		m_iState == STATE_LCHARGEATTACK || m_iState == STATE_RCHARGEATTACK || m_iState == STATE_BACKATTACK || m_iState == STATE_LATTACK1 ||
		m_iState == STATE_LATTACK2 || m_iState == STATE_LATTACK3 || m_iState == STATE_RATTACK1 || m_iState == STATE_RATTACK2 || 
		m_iState == STATE_RUNLATTACK1 || m_iState == STATE_RUNLATTACK2 || m_iState == STATE_RUNRATTACK || m_iState == STATE_COUNTER)
	{
		return;
	}

	//무기 바꾸는 부분
	if (m_pGameInstance->Get_DIKeyState(DIK_1))
	{
		m_iCurWeapon = WEAPON_DURGASWORD;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIK_2))
	{
		m_iCurWeapon = WEAPON_PRETORIANSWORD;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIK_3))
	{
		m_iCurWeapon = WEAPON_RADAMANTHESWORD;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIK_4))
	{
		m_iCurWeapon = WEAPON_ELISH;
	}
}

void CPlayer::OnShapeHit(const PxControllerShapeHit& hit)
{
	// 
	const char* Test = hit.actor->getName();

	PxFilterData hitObjectFilterData = hit.shape->getSimulationFilterData();
	// 충돌한 객체가 무기(검)인 경우
	if (hitObjectFilterData.word0 & CollisionGropuID::GROUP_WEAPON)
	{
		// 무기와의 충돌은 무시 (이미 필터 셰이더에서 처리되었지만, 추가 안전장치로 사용)
		return;
	}
	// 충돌한 객체가 환경(지형, 벽 등)인 경우
	if (hitObjectFilterData.word0 & CollisionGropuID::GROUP_NONCOLLIDE)
	{
		// 환경과의 충돌 처리 (예: 이동 제한, 슬라이딩 등)
		int temp = 0;
	}
}

void CPlayer::OnControllerHit(const PxControllersHit& hit)
{
	// 컨트롤러끼리 충돌 했을 때 


	void* Test = hit.controller->getUserData();
	void* temp1 = hit.other->getUserData();
	//static_cast<CPlayer*>(Test)
	
	
}

bool CPlayer::OnFilterCallback(const PxController& Caller, const PxController& Ohter)
{
	// CPhysXComponent_Character의 Move함수가 호출됐을 때 (사실상 계속 호출되고 있음 CPhysXComponent_Character의Tick함수 때문에 )


	void* Test = Caller.getUserData();
	void* temp1 = Ohter.getUserData();
	
	return true;
	
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
	Safe_Release(m_pCameraTransform);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
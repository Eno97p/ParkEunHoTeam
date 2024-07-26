#include "stdafx.h"
#include "Andras.h"
#include "..\Public\Andras.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Body_Andras.h"
#include "Weapon_Andras.h"
#include "RushSword.h"
#include "EffectManager.h"

#include "UIGroup_BossHP.h"
#include "TargetLock.h"

CAndras::CAndras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CAndras::CAndras(const CAndras& rhs)
	: CMonster{ rhs }
{
}

HRESULT CAndras::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndras::Initialize(void* pArg)
{
	CLandObject::LANDOBJ_DESC* pDesc = (CLandObject::LANDOBJ_DESC*)pArg;

	pDesc->fSpeedPerSec = MOVESPEED;
	pDesc->fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	m_pPhysXCom->Set_Speed(MOVESPEED);

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_fMaxHp = 100.f;
	m_fCurHp = m_fMaxHp;
	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */

	Create_BossUI(CUIGroup_BossHP::BOSSUI_ANDRAS);

	// Target Lock
	vector<CGameObject*>::iterator body = m_PartObjects.begin();
	if (FAILED(Create_TargetLock(dynamic_cast<CModel*>((*body)->Get_Component(TEXT("Com_Model"))), "Andras-Spine", XMVectorSet(-0.13f, -0.4f, 0.f, 1.f), 10.f)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	return S_OK;
}

void CAndras::Priority_Tick(_float fTimeDelta)
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
	m_isAnimFinished = dynamic_cast<CBody_Andras*>(m_PartObjects.front())->Get_AnimFinished();
}

void CAndras::Tick(_float fTimeDelta)
{
	m_fLengthFromPlayer = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (m_pGameInstance->Get_DIKeyState(DIK_N))
	{
		m_bTrigger = true;
	}

	if (m_bTrigger)
	{
		m_pBehaviorCom->Update(fTimeDelta);
	}

	if (!m_bDead)
	{
		for (auto& pPartObject : m_PartObjects)
			pPartObject->Tick(fTimeDelta);
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// 플레이어 무기와 몬스터의 충돌 여부

	CWeapon* pPlayerWeapon = dynamic_cast<CWeapon*>(m_pPlayer->Get_Weapon());
	if (!pPlayerWeapon->Get_Active())
	{
		m_pColliderCom->Reset();
		m_eColltype = CCollider::COLL_NOCOLL;
	}
	else
	{
		m_eColltype = m_pColliderCom->Intersect(pPlayerWeapon->Get_Collider());
	}

	m_pPhysXCom->Tick(fTimeDelta);

	dynamic_cast<CUIGroup_BossHP*>(m_pUI_HP)->Set_Ratio((m_fCurHp / m_fMaxHp));
	m_pUI_HP->Tick(fTimeDelta);

	if (m_bIsLocked)
		m_pTargetLock->Tick(fTimeDelta);
}

void CAndras::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		for (auto& pPartObject : m_PartObjects)
			pPartObject->Late_Tick(fTimeDelta);
	}

	m_pPhysXCom->Late_Tick(fTimeDelta);

	m_pUI_HP->Late_Tick(fTimeDelta);

	if (m_bIsLocked)
	{
		m_pTargetLock->Late_Tick(fTimeDelta);
	}


#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif
}

HRESULT CAndras::Render()
{
	return S_OK;
}

void CAndras::Chase_Player(_float fTimeDelta)
{
	_float3 fScale = m_pTransformCom->Get_Scaled();

	_vector vDir = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vDir.m128_f32[1] = 0.f;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	_vector vUp = XMVector3Cross(vDir, vRight);

	vDir = XMVector3Normalize(vDir);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir);
	m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);
	m_pPhysXCom->Go_Straight(fTimeDelta * m_fLengthFromPlayer);
}

HRESULT CAndras::Add_Components()
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
	PhysXDesc.position = PxExtendedVec3(m_vInitialPos.x, PhysXDesc.height * 0.5f + PhysXDesc.radius + m_vInitialPos.y, m_vInitialPos.z);	//제일 중요함 지형과 겹치지 않는 위치에서 생성해야함. 겹쳐있으면 땅으로 떨어짐 예시로 Y값 강제로 +5해놈
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);	//마찰력,반발력,보통의 반발력
	PhysXDesc.stepOffset = 0.5f;		//오를 수 있는 최대 높이 //이 값보다 높은 지형이 있으면 오르지 못함.
	PhysXDesc.upDirection = PxVec3(0.f, 1.f, 0.f);  //캡슐의 위 방향
	PhysXDesc.slopeLimit = cosf(XMConvertToRadians(45.f));		//오를 수 있는 최대 경사 각도
	PhysXDesc.contactOffset = 0.001f;	//물리적인 오차를 줄이기 위한 값	낮을 수록 정확하나 높을 수록 안정적
	PhysXDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;	//오를 수 없는 지형에 대한 처리
	//PhysXDesc.maxJumpHeight = 0.5f;	//점프 할 수 있는 최대 높이
	//PhysXDesc.invisibleWallHeight = 2.0f;	//캐릭터가 2.0f보다 높이 점프하는 경우 보이지 않는 벽 생성
	PhysXDesc.pName = "Andras";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;

	return S_OK;
}



HRESULT CAndras::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CBody_Andras::BODY_ANDRAS_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iState;
	BodyDesc.pCanCombo = &m_bCanCombo;
	BodyDesc.bSprint = &m_bSprint;

	CGameObject* pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Andras"), &BodyDesc);
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

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_Sword_1");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Andras"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon);

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_Sword_2");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon2 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Andras2"), &WeaponDesc);
	if (nullptr == pWeapon2)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon2);

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_Sword_3");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon3 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Andras3"), &WeaponDesc);
	if (nullptr == pWeapon3)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon3);

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_Sword_4");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon4 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Andras4"), &WeaponDesc);
	if (nullptr == pWeapon4)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon4);

	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Bone_Sword_5");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	CGameObject* pWeapon5 = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Weapon_Andras5"), &WeaponDesc);
	if (nullptr == pWeapon5)
		return E_FAIL;
	m_PartObjects.emplace_back(pWeapon5);

	dynamic_cast<CBody_Andras*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon), 0);
	dynamic_cast<CBody_Andras*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon2), 1);
	dynamic_cast<CBody_Andras*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon3), 2);
	dynamic_cast<CBody_Andras*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon4), 3);
	dynamic_cast<CBody_Andras*>(pBody)->Set_Weapon(dynamic_cast<CWeapon*>(pWeapon5), 4);

	return S_OK;
}

_bool CAndras::Intersect(PART ePartObjID, const wstring& strComponetTag, CCollider* pTargetCollider)
{
	//CCollider* pPartObjCollider = dynamic_cast<CCollider*>(m_PartObjects[ePartObjID]->Get_Component(strComponetTag));
	//
	//return pTargetCollider->Intersect(pPartObjCollider);

	return false;
}

HRESULT CAndras::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CAndras::Idle, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Dead"), bind(&CAndras::Dead, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Hit_Selector"), TEXT("Hit"), bind(&CAndras::Hit, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Attack"), bind(&CAndras::Attack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("SprintAttack"), bind(&CAndras::SprintAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("GroundAttack"), bind(&CAndras::GroundAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("KickAttack"), bind(&CAndras::KickAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("LaserAttack"), bind(&CAndras::LaserAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("BabylonAttack"), bind(&CAndras::BabylonAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("ShootingStarAttack"), bind(&CAndras::ShootingStarAttack, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Select_Pattern"), bind(&CAndras::Select_Pattern, this, std::placeholders::_1));

	m_pBehaviorCom->Add_Action_Node(TEXT("Move_Selector"), TEXT("Backstep"), bind(&CAndras::Backstep, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CAndras::Dead(_float fTimeDelta)
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

NodeStates CAndras::Hit(_float fTimeDelta)
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
		Add_Hp(-10);
		m_pUI_HP->Set_Rend(true); // >> 임의로 피격 시 Render 하긴 하는데 나중에 보스 대면 시 Render하는 것으로 변경할 것
		return RUNNING;
		break;
	}
	case CCollider::COLL_CONTINUE:
		break;
	case CCollider::COLL_FINISH:
		break;
	case CCollider::COLL_NOCOLL:
		break;
	}

	return FAILURE;
}

NodeStates CAndras::Attack(_float fTimeDelta)
{
	if (m_iState == STATE_ATTACK1 || m_iState == STATE_ATTACK2 || m_iState == STATE_ATTACK3 || m_iState == STATE_ATTACK4)
	{
		if (m_isAnimFinished)
		{
			switch (m_iState)
			{
			case STATE_ATTACK1:
				if (m_fLengthFromPlayer > 5.f)
				{
					m_iState = STATE_SPRINTATTACK;
				}
				else
				{
					m_iState = STATE_ATTACK2;
				}
				return RUNNING;
				break;
			case STATE_ATTACK2:
				if (m_fLengthFromPlayer > 5.f)
				{
					m_iState = STATE_SPRINTATTACK;
				}
				else
				{
					m_iState = STATE_ATTACK3;
				}
				return RUNNING;
				break;
			case STATE_ATTACK3:
				if (m_fLengthFromPlayer > 5.f)
				{
					m_iState = STATE_SPRINTATTACK;
				}
				else
				{
					m_iState = STATE_ATTACK4;
				}
				return RUNNING;
				break;
			case STATE_ATTACK4:
				m_iState = STATE_IDLE;
				m_bDashBack = true;
				return SUCCESS;
				break;
			}
		}
		else
		{
			return RUNNING;
		}
	}

	if (m_iState == STATE_DASHRIGHT)
	{
		if (m_fLengthFromPlayer < 3.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_iState = STATE_ATTACK1;
			return SUCCESS;
		}
		else if (m_fLengthFromPlayer < 5.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else
		{
			m_pPhysXCom->Go_RightFront(fTimeDelta * 0.5f);
		}
		
		if (m_isAnimFinished)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_iState = STATE_DASHLEFT;
		}
		return RUNNING;
	}
	else if (m_iState == STATE_DASHLEFT)
	{
		if (m_fLengthFromPlayer < 3.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_iState = STATE_ATTACK1;
			return SUCCESS;
		}
		else if (m_fLengthFromPlayer < 5.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pPhysXCom->Go_Straight(fTimeDelta);
		}
		else
		{
			m_pPhysXCom->Go_LeftFront(fTimeDelta * 0.5f);
		}

		if (m_isAnimFinished)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_iState = STATE_DASHRIGHT;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}

	return FAILURE;
}

NodeStates CAndras::SprintAttack(_float fTimeDelta)
{
	if (m_iState == STATE_SPRINTATTACK)
	{
		if (m_fLengthFromPlayer > 3.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			if (m_bSprint)
			{
				m_pPhysXCom->Go_Straight(fTimeDelta);
			}
		}
		else if (m_fLengthFromPlayer <= 3.f)
		{
			m_bSprint = false;
		}

		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = true;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}

	return FAILURE;
}

NodeStates CAndras::GroundAttack(_float fTimeDelta)
{
	if (m_iState == STATE_GROUNDATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CAndras::KickAttack(_float fTimeDelta)
{
	if (m_iState == STATE_KICKATTACK)
	{
		if (m_fLengthFromPlayer > 3.f)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			if (m_bSprint)
			{
				m_pPhysXCom->Go_Straight(fTimeDelta);
			}
		}
		else if (m_fLengthFromPlayer <= 3.f)
		{
			m_bSprint = false;
		}

		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = true;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}
	return FAILURE;
}

NodeStates CAndras::LaserAttack(_float fTimeDelta)
{
	if (m_iState == STATE_LASERATTACK)
	{
		if (m_isAnimFinished)
		{
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}
	return FAILURE;
}

NodeStates CAndras::BabylonAttack(_float fTimeDelta)
{
	if (m_iState == STATE_BABYLONATTACK)
	{
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_fSpawnCoolTime -= fTimeDelta;
		m_fSpawnDelay -= fTimeDelta;
		if (m_fSpawnCoolTime < 0.f && m_fSpawnDelay < 0.f)
		{
			m_fSpawnCoolTime = SPAWNCOOLTIME;
			_float fHeight = 3.f + RandomFloat(0.f, 5.f);
			//칼 생성 위치(랜덤)
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT)
				* RandomFloat(-5.f, 5.f) + XMVectorSet(0.f, fHeight, 0.f, 0.f);
			
			_vector vAndrasPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			vPlayerPos.m128_f32[1] = vAndrasPos.m128_f32[1];
			vAndrasPos.m128_f32[1] += fHeight;

			// 칼 Right
			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			// 칼 Up
			_vector vUp = XMVector3Normalize(vPlayerPos - vAndrasPos);
			// 칼 Look
			_vector vLook = -XMVector3Cross(vUp, vRight); 
			
			CRushSword::RUSH_DESC pDesc;
			pDesc.fHeight = vPlayerPos.m128_f32[1];
			pDesc.meshNum = RandomInt(0, 17);
			pDesc.mWorldMatrix._11 = vRight.m128_f32[0];
			pDesc.mWorldMatrix._12 = vRight.m128_f32[1];
			pDesc.mWorldMatrix._13 = vRight.m128_f32[2];
			pDesc.mWorldMatrix._21 = vUp.m128_f32[0];
			pDesc.mWorldMatrix._22 = vUp.m128_f32[1];
			pDesc.mWorldMatrix._23 = vUp.m128_f32[2];
			pDesc.mWorldMatrix._31 = vLook.m128_f32[0];
			pDesc.mWorldMatrix._32 = vLook.m128_f32[1];
			pDesc.mWorldMatrix._33 = vLook.m128_f32[2];
			pDesc.mWorldMatrix._41 = vPos.m128_f32[0];
			pDesc.mWorldMatrix._42 = vPos.m128_f32[1];
			pDesc.mWorldMatrix._43 = vPos.m128_f32[2];
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Sword"), TEXT("Prototype_GameObject_Weapon_RushSword"), &pDesc);
		}

		if (m_isAnimFinished)
		{
			m_fSpawnDelay = 2.f;
			m_fSpawnCoolTime = SPAWNCOOLTIME;
			m_iState = STATE_IDLE;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}

	return FAILURE;
}

NodeStates CAndras::ShootingStarAttack(_float fTimeDelta)
{
	if (m_iState == STATE_SHOOTINGSTARATTACK)
	{
		m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		m_fSpawnCoolTime -= fTimeDelta;
		m_fSpawnDelay -= fTimeDelta;
		if (m_fSpawnCoolTime < 0.f && m_fSpawnDelay < 0.f)
		{
			m_fSpawnCoolTime = SPAWNCOOLTIME;
			//칼 생성 위치(랜덤)
			_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(RandomFloat(-2.f, 2.f), 10.f, RandomFloat(-2.f, 2.f), 0.f);

			CRushSword::RUSH_DESC pDesc;
			pDesc.fHeight = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] + 0.5f;
			pDesc.meshNum = RandomInt(0, 17);
			pDesc.mWorldMatrix._11 = -1.f;
			pDesc.mWorldMatrix._12 = 0.f;
			pDesc.mWorldMatrix._13 = 0.f;
			pDesc.mWorldMatrix._21 = 0.f;
			pDesc.mWorldMatrix._22 = -1.f;
			pDesc.mWorldMatrix._23 = 0.f;
			pDesc.mWorldMatrix._41 = vPos.m128_f32[0];
			pDesc.mWorldMatrix._42 = vPos.m128_f32[1];
			pDesc.mWorldMatrix._43 = vPos.m128_f32[2];
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Sword"), TEXT("Prototype_GameObject_Weapon_RushSword"), &pDesc);
		}

		if (m_isAnimFinished)
		{
			m_fSpawnCoolTime = SPAWNCOOLTIME;
			m_iState = STATE_IDLE;
			m_fSpawnDelay = 2.f;
			m_bDashBack = false;
			return SUCCESS;
		}
		return RUNNING;
	}

	return FAILURE;
}

NodeStates CAndras::Select_Pattern(_float fTimeDelta)
{
	if ((m_iState == STATE_DASHBACK && m_isAnimFinished) || !m_bDashBack)
	{
		m_fTurnDelay = 0.5f;
		_uint i;
		if (m_iPhase == 1)
		{
			i = RandomInt(0, 1);
		}
		else if (m_iPhase == 2)
		{
			i = RandomInt(0, 6);
		}

		switch (i)
		{
		case 0:
			//Attack
			m_iState = STATE_DASHRIGHT;
			break;
		case 1:
			//SprintAttack
			m_iState = STATE_SPRINTATTACK;
			break;
		case 2:
			//GroundAttack
			m_iState = STATE_GROUNDATTACK;
			break;
		case 3:
			//KickAttack
			m_iState = STATE_KICKATTACK;
			break;
		case 4:
			//LaserAttack
			m_iState = STATE_LASERATTACK;
			break;
		case 5:
			//BabylonAttack
			m_iState = STATE_BABYLONATTACK;
			break;
		case 6:
			//ShootingStarAttack
			m_iState = STATE_SHOOTINGSTARATTACK;
			break;
		}
		return SUCCESS;
	}

	if (m_fTurnDelay > 0.f)
	{
		m_fTurnDelay -= fTimeDelta;
		m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		return RUNNING;
	}
	else if(m_iState != STATE_DASHLEFT && m_iState != STATE_DASHRIGHT)
	{
		m_iState = STATE_DASHBACK;
		return COOLING;
	}
}

NodeStates CAndras::Backstep(_float fTimeDelta)
{
	if (m_iState == STATE_DASHBACK)
	{
		if (m_isAnimFinished)
		{
			m_pTransformCom->LookAt_For_LandObject(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		}
		else
		{
			m_pPhysXCom->Go_BackWard(fTimeDelta);
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}

}

NodeStates CAndras::Idle(_float fTimeDelta)
{
	m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_iState = STATE_IDLE;
	return SUCCESS;
}

void CAndras::Add_Hp(_int iValue)
{
	m_fCurHp = min(m_fMaxHp, max(0, m_fCurHp + iValue));
	if (m_fCurHp == 0)
	{
		m_iState = STATE_DEAD;
	}
}

CAndras* CAndras::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndras* pInstance = new CAndras(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAndras::Clone(void* pArg)
{
	CAndras* pInstance = new CAndras(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndras");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndras::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pBehaviorCom);

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
class CBehaviorTree;
class CModel;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CPlayer final : public CLandObject
{
#define	CLONEDELAY 0.15f
#define BUTTONCOOLTIME 0.5f
#define JUMPCOOLTIME 0.3f
#define WALKSPEED 3.5f // 3
#define RUNSPEED 6.5f // 6
#define ROLLSPEED 10.f
#define ATTACKPOSTDELAY 1.5f
#define STAMINARECOVERDELAY 1.5f
#define PARRYSTART 0.01f
#define PARRYEND 0.3f
#define JUMPSPEED 13.f

public:
	enum PART { PART_BODY, PART_WEAPON, PART_END };
	enum STATE {
		STATE_IDLE, STATE_FIGHTIDLE, STATE_WALK, STATE_LOCKON_STRAIGHT, STATE_LOCKON_BACKWARD, STATE_LOCKON_LEFT, STATE_LOCKON_RIGHT, STATE_RUN, STATE_JUMPSTART, STATE_DOUBLEJUMPSTART, STATE_JUMP, STATE_LAND,
		STATE_PARRY, STATE_JUMPATTACK, STATE_JUMPATTACK_LAND, STATE_ROLLATTACK, STATE_LCHARGEATTACK, STATE_RCHARGEATTACK, STATE_BACKATTACK,
		STATE_LATTACK1, STATE_LATTACK2, STATE_LATTACK3, STATE_RATTACK1, STATE_RATTACK2, STATE_RUNLATTACK1, STATE_RUNLATTACK2, STATE_RUNRATTACK, 
		STATE_COUNTER, STATE_ROLL, STATE_HIT, STATE_DASH, STATE_DASH_FRONT, STATE_DASH_BACK, STATE_DASH_LEFT, STATE_DASH_RIGHT, STATE_USEITEM, STATE_BUFF, STATE_DEAD, STATE_REVIVE, STATE_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	_uint Get_State() { return m_iState; }
	CGameObject* Get_Weapon();
	void PlayerHit(_float iValue);
	_float Get_HpRatio() { return m_fCurHp / m_fMaxHp; }
	_float Get_StaminaRatio() { return m_fCurStamina / m_fMaxStamina; }
	_float Get_MpRatio() { return m_fCurMp / m_fMaxMp; }
	_bool Get_Parry() { return m_bParrying; }
	void Parry_Succeed();
	void Set_ParriedMonsterFloat4x4(const _float4x4* pMatrix) { m_pParriedMonsterFloat4x4 = pMatrix; }
	_bool Get_Cloaking() { return m_bIsCloaking; }
	void Set_Cloaking(_bool bCloaking) { m_bIsCloaking = bCloaking; }

private:
	HRESULT Add_Nodes();

private:
	NodeStates Revive(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Counter(_float fTimeDelta);
	void Move_Counter();
	NodeStates Parry(_float fTimeDelta);
	NodeStates JumpAttack(_float fTimeDelta);
	NodeStates RollAttack(_float fTimeDelta);
	NodeStates LChargeAttack(_float fTimeDelta);
	NodeStates RChargeAttack(_float fTimeDelta);
	NodeStates LAttack(_float fTimeDelta);
	NodeStates RAttack(_float fTimeDelta);
	NodeStates Dash(_float fTimeDelta);
	NodeStates Jump(_float fTimeDelta);
	NodeStates Roll(_float fTimeDelta);
	NodeStates UseItem(_float fTimeDelta);
	NodeStates Buff(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates Idle(_float fTimeDelta);
	void Add_Hp(_float iValue);
	void Add_Stamina(_float iValue);
	void Add_Mp(_float iValue);

private:
	vector<class CGameObject*>					m_PartObjects;
	_uint										m_iState = { 0 };
	class CPhysXComponent_Character* m_pPhysXCom = { nullptr };
	CBehaviorTree* m_pBehaviorCom = { nullptr };
	_float										m_fButtonCooltime = 0.f;
	_float										m_fJumpCooltime = 0.f;

#pragma region 상태제어 bool변수
	_bool										m_bJumping = false;
	_bool										m_bFalling = false;
	_bool										m_bDoubleJumping = false;
	_bool										m_bLAttacking = false;
	_bool										m_bRAttacking = false;
	_bool										m_bRunning = false;
	// 패링중
	_bool										m_bParrying = false;
	// 패링 성공
	_bool										m_bParry = false;
	_bool										m_bIsLanded = false;
	_bool										m_bIsRunAttack = false;
	_bool										m_bDisolved_Weapon = false;
	_bool										m_bDisolved_Yaak = false;
	_bool										m_bStaminaCanDecrease = true;
	_bool										m_bIsCloaking = false;
#pragma endregion 상태제어 bool변수

	_float										m_fFightIdle = 0.f;
	_float										m_fLChargeAttack = 0.f;
	_float										m_fRChargeAttack = 0.f;
	_bool										m_bAnimFinished = false;
	_float										m_fAnimDelay = 0.f;
	_float										m_fCloneDelay = 0.f;
	_uint										m_iAttackCount = 1;
	_bool										m_bCanCombo = false;
	_float										m_fSlowDelay = 0.f;
	_float										m_fParryFrame = 0.f;
	_float										m_fJumpAttackdelay = 0.7f;
	_float										m_fStaminaRecoverDelay = STAMINARECOVERDELAY;
	const _float4x4* m_pParriedMonsterFloat4x4 = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

#pragma region 플레이어 스탯
	_float m_fMaxHp = 100.f;
	_float m_fCurHp = m_fMaxHp;

	_float m_fMaxStamina = 100.f;
	_float m_fCurStamina = m_fMaxStamina;

	_float m_fMaxMp = 100.f;
	_float m_fCurMp = m_fMaxMp;

	_float m_iMoney = 0;
#pragma endregion 플레이어 스탯

	_float m_fParticleAcctime = 0.f;

	//초기 위치
	_float3 m_InitialPosition = { 0.f, 0.f, 0.f };
private:
	void OnShapeHit(const PxControllerShapeHit& hit);


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
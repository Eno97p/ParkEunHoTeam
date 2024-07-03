#pragma once

#include "Monster.h"

BEGIN(Client)

class CBoss_Juggulus final : public CMonster
{
public:
	enum STATE { STATE_IDLE_FIRST = 0, STATE_IDLE_SEC, STATE_NEXTPHASE, STATE_CREATE_HAMMER,
		STATE_HANDONE_TARGETING, STATE_HANDONE_ATTACK,
		STATE_HANDTWO_SCOOP, STATE_HANDTWO_ATTACK,
		STATE_HANDTHREE_ATTACK,
		STATE_FLAME_ATTACK, STATE_HAMMER_ATTACK, STATE_SPHERE_ATTACK, STATE_THUNDER_ATTACK, STATE_DEAD, STATE_END };
	enum PHASE { PHASE_ONE, PHASE_TWO, PHASE_END };

private:
	CBoss_Juggulus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Juggulus(const CMonster& rhs);
	virtual ~CBoss_Juggulus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool							m_isHammerCreate = { false };
	_bool							m_isAttackDone = { true }; 
	_bool							m_isPhaseChanged = { false };
	_bool							m_isHandOne_On = { false };
	_bool							m_isHandTwo_On = { false };
	_bool							m_isHandAnimFinished = { false };
	_bool							m_isHandTwoAnimFinished = { false };
	_bool							m_isHandThreeAnimFinished = { false };

	_float							m_fTargettingTimer = { 0.f };

	map<string, class CGameObject*>	m_PartObjects;

	PHASE							m_ePhase = { PHASE_END };

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	virtual HRESULT		Add_Nodes() override;

	void				Key_Input();
	HRESULT				Create_Hammer();
	void				Check_AnimFinished();

private:
	NodeStates			Dead(_float fTimedelta);
	NodeStates			NextPhase(_float fTimedelta);
	NodeStates			CreateHammer(_float fTimedelta);
	NodeStates			Idle(_float fTimeDelta);

	NodeStates			HandOne_Targeting(_float fTimeDelta);
	NodeStates			HandOne_Attack(_float fTimeDelta);
	NodeStates			HandTwo_Scoop(_float fTimeDedelta);
	NodeStates			HandTwo_Attack(_float fTimeDelta);
	NodeStates			HandThree_Attack(_float fTimeDelta);

	NodeStates			FlameAttack(_float fTimeDelta);
	NodeStates			HammerAttack(_float fTimeDelta);
	NodeStates			SphereAttack(_float fTimeDelta);
	NodeStates			ThunderAttack(_float fTimeDelta);

public:
	static CBoss_Juggulus*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
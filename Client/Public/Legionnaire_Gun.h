#pragma once

#include "Monster.h"

BEGIN(Client)

class CLegionnaire_Gun final : public CMonster
{
public:
	enum STATE { STATE_IDLE = 0, STATE_DEAD, STATE_HIT, STATE_WAKEUP, STATE_KNOCKDOWN,
		STATE_WALK, STATE_RUN, STATE_BACK, STATE_LEFT, STATE_RIGHT,
		STATE_GUNATTACK, STATE_CASTING, STATE_MELEEATTACK1, STATE_MELEEATTACK2, STATE_MELEEATTACK3, STATE_END };

private:
	CLegionnaire_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLegionnaire_Gun(const CLegionnaire_Gun& rhs);
	virtual ~CLegionnaire_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool							m_isParry = { false };
	_bool							m_isHit = { false };
	_bool							m_isAttackDistance = { false };
	_bool							m_isMeleeAttack = { false };

	_float							m_fCastingTimer = { 0.f };

	vector<class CGameObject*>		m_PartObjects;

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	virtual HRESULT		Add_Nodes() override;

	void				Check_AnimFinished();

private:
	NodeStates			Dead(_float fTimedelta);
	NodeStates			Hit(_float fTimedelta);
	NodeStates			WakeUp(_float fTimedelta);
	NodeStates			KnockDown(_float fTimedelta);
	NodeStates			Idle(_float fTimedelta);

	NodeStates			Move(_float fTimeDelta);

	NodeStates			GunAttack(_float fTimedelta);
	NodeStates			Casting(_float fTimedelta);
	NodeStates			MeleeAttack(_float fTimedelta);

public:
	static CLegionnaire_Gun*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
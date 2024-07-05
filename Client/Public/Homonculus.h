#pragma once

#include "Monster.h"

BEGIN(Client)

class CHomonculus final : public CMonster
{
public:
	enum STATE { STATE_IDLE = 0, STATE_DEAD, STATE_HIT, STATE_PARRY, STATE_WAKEUP,
		STATE_DEFAULTATTACK_1, STATE_DEFAULTATTACK_2, STATE_DEFAULTATTACK_3, STATE_DOWNATTACK, STATE_FULLATTACK,
		STATE_GO, STATE_LEFT, STATE_RIGHT, STATE_END };

private:
	CHomonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHomonculus(const CHomonculus& rhs);
	virtual ~CHomonculus() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool							m_isHit = { false };
	_bool							m_isDefaultAttack = { false };

	vector<class CGameObject*>		m_PartObjects;

private:
	HRESULT				Add_Components();
	HRESULT				Add_PartObjects();

	virtual HRESULT		Add_Nodes() override;

	void				Check_AnimFinished();

private:
	NodeStates			Dead(_float fTimeDelta);
	NodeStates			Hit(_float fTimeDelta);
	NodeStates			Parry(_float fTimeDelta);
	NodeStates			Idle(_float fTimeDelta);

	NodeStates			Move(_float fTimeDelta);

	NodeStates			Default_Attack(_float fTimeDelta);
	NodeStates			Down_Attack(_float fTimeDelta);
	NodeStates			Full_Attack(_float fTimeDelta);

public:
	static CHomonculus*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
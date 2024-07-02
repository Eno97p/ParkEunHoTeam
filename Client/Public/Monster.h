#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Player.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CBehaviorTree;
END

BEGIN(Client)

class CMonster abstract : public CLandObject
{
public:
	typedef struct Mst_Desc : public LANDOBJ_DESC
	{
		LEVEL		eLevel;
	}MST_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Add_BehaviorTree();

protected:
	_bool							m_isAnimFinished = { false };

	_uint							m_iHP = { 0 };
	_uint							m_iState = { 0 };

	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	class CPhysXComponent_Character*				m_pPhysXCom = { nullptr };
	CBehaviorTree*					m_pBehaviorCom = { nullptr };

	LEVEL							m_eLevel = { LEVEL_END };

	CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };

protected:
	virtual HRESULT		Add_Nodes();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
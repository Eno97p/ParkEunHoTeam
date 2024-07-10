#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Player.h"

#include "UIGroup_BossHP.h"

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
	_float			Get_Ratio() { return m_fCurHp / m_fMaxHp; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	//FOR LOCKON
	_vector Get_MonsterPos();

protected:
	HRESULT Add_BehaviorTree();

protected:
	_bool							m_isAnimFinished = { false };

#pragma region 몬스터 스탯
	_float m_fMaxHp = 100.f;
	_float m_fCurHp = m_fMaxHp;
#pragma endregion 몬스터 스탯

	_bool m_bDead = false;
	_float m_fDeadDelay = 2.f;
	_float	m_fLengthFromPlayer = 0.f;

	_uint							m_iState = { 0 };

	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	class CPhysXComponent_Character*				m_pPhysXCom = { nullptr };
	CBehaviorTree*					m_pBehaviorCom = { nullptr };

	LEVEL							m_eLevel = { LEVEL_END };

	CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	
	// UI
	class CUIGroup*		m_pUI_HP = { nullptr };

protected:
	virtual HRESULT		Add_Nodes();

	void				Create_UI();
	void				Create_BossUI(CUIGroup_BossHP::BOSSUI_NAME eBossName); // 인자에 값을 넣어주는 게 좋을 듯함

	void				Update_UI(_float fHeight = 0.f);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
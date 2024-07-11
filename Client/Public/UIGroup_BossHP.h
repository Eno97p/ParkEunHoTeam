#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_BossHP final : public CUIGroup
{
public:
	// enum으로 어떤 보스의 UI인지 구분?
	enum BOSSUI_NAME { BOSSUI_JUGGULUS, BOSSUI_MANTARI, BOSSUI_END };
	typedef struct UIGroup_BossHP_Desc : public UIGROUP_DESC
	{
		BOSSUI_NAME		eBossUIName;
	}UIGROUP_BOSSHP_DESC;

private:
	CUIGroup_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_BossHP(const CUIGroup_BossHP& rhs);
	virtual ~CUIGroup_BossHP() = default;

public:
	void			Set_Ratio(_float fRatio) { m_fHPRatio = fRatio; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint					m_iDamage = { 0 }; // 누적 데미지
	_float					m_fHPRatio = { 0.f };

	vector<CUI*>			m_vecUI;

	BOSSUI_NAME				m_eBossUIName = { BOSSUI_END };

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_BossHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
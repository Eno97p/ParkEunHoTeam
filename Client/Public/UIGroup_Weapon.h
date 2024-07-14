#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;
class CUI_Slot;

class CUIGroup_Weapon final : public CUIGroup
{
public:
	enum TAB_TYPE { TAB_L, TAB_R, TAB_END };

private:
	CUIGroup_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Weapon(const CUIGroup_Weapon& rhs);
	virtual ~CUIGroup_Weapon() = default;

public:
	TAB_TYPE		Get_TabType() { return m_eTabType; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_Weapon_Add();

private:
	vector<CUI*>			m_vecUI;
	vector<CUI_Slot*>		m_vecSlot;

	TAB_TYPE				m_eTabType = { TAB_END };

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();
	HRESULT					Create_EquipSlot();
	HRESULT					Create_Fontaine();
	HRESULT					Create_Tab();

	void					Change_Tab();

public:
	static CUIGroup_Weapon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIGroup;
class CUI_Activate;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	_bool				Get_MenuPageState();
	void				Set_MenuPageOpen();

	CUIGroup*			Get_UIGroup(string strKey);

	void				Set_MenuPage(_bool isOpen, string PageKey);

	CUI_Activate*		Get_Activate() { return m_pUI_Activate; }

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

	void				Render_UIGroup(_bool isRender, string strKey);

	void				Update_Inventory_Add(_uint iSlotIdx);
	void				Update_InvSub_Quick_Add();
	void				Update_Quick_Add(CItemData* pItemData);
	void				Update_Weapon_Add();
	void				Update_EquipWeapon_Add(_uint iEquipSlotIdx);
	void				Update_EquipWeapon_Delete(_uint iEquipSlotIdx);

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	map<string, CUIGroup*>	m_mapUIGroup;
	CUI_Activate*			m_pUI_Activate = { nullptr };

private:
	HRESULT		Create_UI();

	void		Key_Input();

public:
	virtual void Free() override;
};

END
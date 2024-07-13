#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIGroup;

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

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

	void				Render_UIGroup(_bool isRender, string strKey);

	//UIGroup_Inventory의 Slot에 Create Icon을 해주어야 하는 상황
	// 인벤토리의 해당 index를 넘겨주어 거기에 해당하는 slot에다가 처리하도록?
	void				Update_Inventory_Add(_uint iSlotIdx);
	void				Update_InvSub_Quick_Add();

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	map<string, CUIGroup*>	m_mapUIGroup;

private:
	HRESULT		Create_UI();

	void		Key_Input();

public:
	virtual void Free() override;
};

END
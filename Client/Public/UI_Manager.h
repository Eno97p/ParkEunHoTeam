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

public:
	void				Set_MenuPage(_bool isOpen, string PageKey);

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

	void				Render_Logo(_bool isRender);
	void				Render_Loading(_bool isRender);
	void				Render_HUD(_bool isRender);

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
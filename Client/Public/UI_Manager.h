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
	void				Set_ScreenBloodRend(_bool isRend);
	void				Set_Broken(_bool isRend);
	_bool				Get_Dash();
	void				Set_Dash(_bool isRend);
	_bool				Get_isMouseOn();
	void				Set_KeyActivate(_bool isKeyActivate) { m_isKeyActivate = isKeyActivate; }

	void				Set_isShopOn(_bool isOn) { m_isShopOn = isOn; }

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

	void				Render_UIGroup(_bool isRender, string strKey);

	void				Update_Inventory_Add(_uint iSlotIdx);
	void				Update_InvSub_Quick_Add(_uint iSlotIdx);
	void				Update_Quick_Add(class CItemData* pItemData, _int iInvenIdx);
	void				Update_Quick_InvSlot_Add(_uint iSlotIdx);
	void				Update_Weapon_Add();
	void				Update_EquipWeapon_Add(_uint iEquipSlotIdx);
	void				Update_EquipWeapon_Delete(_uint iEquipSlotIdx);
	void				Update_Skill_Add();
	void				Update_EquipSkill_Add(_uint iEquipSlotIdx);
	void				Update_EquipSkill_Delete(_uint iEquipSlotIdx);

	void				Resset_Player();

	HRESULT				Create_FadeInOut_Dissolve(_bool isFadeIn);
	void				Delete_FadeInOut(_bool isFadeIn);
	_bool				Get_isFadeAnimEnd(_bool isFadeIn);

	// RedDot 관련 함수
	HRESULT				Create_RedDot_MenuBtn(_bool isInv);

private:
	_bool								m_isKeyActivate = { true }; // 키보드와의 상호작용 활성화 여부
	_bool								m_isShopOn = { false }; // 상점 활성화 여부

	CGameInstance*						m_pGameInstance = { nullptr };

	map<string, CUIGroup*>				m_mapUIGroup;
	class CUI_ScreenBlood*				m_pScreenBlood = { nullptr };
	class CUI_Broken*					m_pBroken = { nullptr };
	vector<class CUI_Dash*>				m_vecDash;
	class CQTE*							m_pQTE = { nullptr };
	class CUI_FadeInOut*				m_pFadeOut = { nullptr };
	class CUI_FadeInOut*				m_pFadeIn = { nullptr };

	// Red Dot 로직의 추가

	// Inventory에서 새로운 아이템이 들어오면 RedDot을 활성화
	// >> Item의 경우, Weapon의 경우, Skill의 경우를 나누어서 구현
	// >> Item의 경우 Inv와 Quick Inv 등의 Slot에 적용 + Menu의 Btn에 적용
	// >> Weapon의 경우 Weapon의 Slot + Menu의 Btn에 적용
	// >> Skill의 경우 Skill의 Slot(인데 처음에 나오면 안 되고 Tab의 상태에 따라) + Menu의 Btn에 적용
	// Inventory에 무언가 들어오면 UI Manager의 함수를 호출 > Red Dot을 활성화 하는 함수
	// 그러면 UI Manager에서 해당 로직에 맞는 녀석을 타고 들어가 RedDot을 생성하는 식! 으로 진행하면 될듯

	// Inventory에서 함수를 호출했다! > 이 함수는...
	// >> Menu Btn와 분기에 따라 Inv 혹은 Weapon의 Slot에 RedDot 객체를 생성해주는 녀석



private:
	HRESULT		Create_UI();

	void		Key_Input();

public:
	virtual void Free() override;
};

END
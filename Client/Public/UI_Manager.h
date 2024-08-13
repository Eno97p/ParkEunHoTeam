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

private:
	_bool								m_isKeyActivate = { true }; // Ű������� ��ȣ�ۿ� Ȱ��ȭ ����
	_bool								m_isShopOn = { false }; // ���� Ȱ��ȭ ����

	CGameInstance*						m_pGameInstance = { nullptr };

	map<string, CUIGroup*>				m_mapUIGroup;
	class CUI_ScreenBlood*				m_pScreenBlood = { nullptr };
	class CUI_Broken*					m_pBroken = { nullptr };
	vector<class CUI_Dash*>				m_vecDash;

	// BuffTimer �� ������ �ֵ��� �ϱ�(��� ������)
	//vector<class CUIGroup_BuffTimer*>	m_vecBuffTimers; // >>> �̷��� ���� ���� �ƴϸ� map�� �־ �ٸ� UIGroup���̶� ���� ��������?
	// Ȱ���ϴ� ����� ���� �ٸ��� �� (BuffTimer�� ��� : Render ���ζ� ��� ���� ��� Tick�� ���� ��ҵ��� �߰��ǰų� �˾Ƽ� ���ŵǴ� ���̶�...)
	// �ٵ� Tick������ LateTick ������ �Ÿ� �ϴ� �Ŷ� �ٸ� Group��� �Բ� map�� �־ ��� ���� �� ���� �ϱ�����

private:
	HRESULT		Create_UI();

	void		Key_Input();

public:
	virtual void Free() override;
};

END
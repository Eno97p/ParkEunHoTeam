#pragma once

#include "UI_Interaction.h"
#include "UIGroup_Weapon.h"

BEGIN(Client)
class CUI_Slot_Frame;
class CUI_ItemIcon;
class CItemData;

class CUI_Slot final : public CUI_Interaction
{
public:
	enum SLOT_TYPE { SLOT_QUICK, SLOT_INV, SLOT_WEAPON, SLOT_INVSUB, SLOT_END };
	typedef struct UI_Slot_Desc : public UI_DESC
	{
		_uint						iSlotIdx;
		UISORT_PRIORITY				eUISort;
		SLOT_TYPE					eSlotType;
		CUIGroup_Weapon::TAB_TYPE	eTabType;
	}UI_SLOT_DESC;

private:
	CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Slot(const CUI_Slot& rhs);
	virtual ~CUI_Slot() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	HRESULT			Create_ItemIcon_Inv();
	HRESULT			Create_ItemIcon_SubQuick(_uint iSlotIdx);
	HRESULT			Create_ItemIcon_Quick(CItemData* pItemData);
	HRESULT			Create_ItemIcon_Weapon();

	void			Change_TabType(CUIGroup_Weapon::TAB_TYPE eTabType);

private:
	_uint						m_iSlotIdx = { 0 };
	wstring						m_wszItemName = TEXT("");
	wstring						m_wszItemExplain = TEXT("");

	UISORT_PRIORITY				m_eUISort = { SORT_END };
	SLOT_TYPE					m_eSlotType = { SLOT_END };

	CUI_Slot_Frame*				m_pSelectFrame = { nullptr };
	CUI_ItemIcon*				m_pItemIcon = { nullptr };

	CUIGroup_Weapon::TAB_TYPE	m_eTabType = { CUIGroup_Weapon::TAB_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_Frame();

	void	Open_SubPage();
	void	Render_Font();

	void	Update_ItemIcon_TabChange();

public:
	static CUI_Slot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
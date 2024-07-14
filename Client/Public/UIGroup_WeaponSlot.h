#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI_ItemIcon;

class CUIGroup_WeaponSlot final : public CUIGroup
{
private:
	CUIGroup_WeaponSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_WeaponSlot(const CUIGroup_WeaponSlot& rhs);
	virtual ~CUIGroup_WeaponSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_QuickSlot(wstring wstrTextureName);

private:
	_uint					m_iQuickIdx = { 0 }; // Quick Access¿« Index

	vector<CUI*>			m_vecUI;
	CUI_ItemIcon*			m_pWeaponSlot;
	CUI_ItemIcon*			m_pSkillSlot;
	CUI_ItemIcon*			m_pQuickSlot;

private:
	HRESULT					Create_UI();

	void					Key_Input();

public:
	static CUIGroup_WeaponSlot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
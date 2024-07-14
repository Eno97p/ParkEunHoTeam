#pragma once

#include "Base.h"
#include "Client_Defines.h"

#include "Item.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CItemData;

class CInventory final : public CBase
{
	DECLARE_SINGLETON(CInventory)

public:
	CInventory();
	virtual ~CInventory() = default;

public:
	CItemData*							Get_ItemData(_uint iSlotIdx);
	_uint								Get_vecItemSize() { return m_vecItem.size(); }
	_uint								Get_QuickSize() { return m_vecQuickAccess.size(); }
	_uint								Get_WeaponSize() { return m_vecWeapon.size(); }
	vector<class CItemData*>*			Get_QuickAccess() { return &m_vecQuickAccess; }
	vector<class CItemData*>*			Get_Weapons() { return &m_vecWeapon; }

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

	HRESULT	Initialize_DefaultItem();
	HRESULT	Add_DropItem(CItem::ITEM_NAME eItemType);
	HRESULT	Add_QuickAccess(CItemData* pItemData);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

private:
	vector<class CItemData*>		m_vecItem;
	vector<class CItemData*>		m_vecQuickAccess;
	vector<class CItemData*>		m_vecWeapon;
	// 무기, 서브무기 등 추가 필요

public:
	virtual void Free() override;
};

END
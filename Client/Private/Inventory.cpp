#include "Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"
#include "ItemData.h"
#include "UIGroup_DropItem.h"
#include "UIGroup_WeaponSlot.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CItemData* CInventory::Get_ItemData(_uint iSlotIdx)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < iSlotIdx; ++i)
		++item;

	return (*item);
}

HRESULT CInventory::Initialize()
{
	if (FAILED(Initialize_DefaultItem()))
		return E_FAIL;

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
}

HRESULT CInventory::Initialize_DefaultItem()
{
	// 게임 처음 시작 시 기본적으로 가지고 있는 아이템 
	// Weapon에 추가
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_CATHARSIS;

	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	// Inventory에 ItemData 추가
	CItemData::ITEMDATA_DESC pDesc{};
	
	pDesc.isDropTem = true;
	pDesc.eDropItemName = eItemType;
	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
		++item;
	pUIDesc.eItemName = (*item)->Get_ItemName();
	pUIDesc.wszTextureName = (*item)->Get_TextureName();
	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData)
{
	// Inventory에서 현재 선택한 아이템을 QuickAccess에 등록
	// 현재 m_vecItem에 들어가 있는, 선택한 그 아이템을 m_vecQuickAccess에 넣어주기
	m_vecQuickAccess.emplace_back(pItemData);

	// InvSub QuickAccess Slot UI에 출력하기
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add();

	// QuickAccess에도 출력 필요
	CUI_Manager::GetInstance()->Update_Quick_Add(pItemData);

	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_QuickSlot(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Add_EquipWeapon(CItemData* pItemData, _uint iEquipSlotIdx)
{
	// Weapon의 Slot에서 현재 선택한 아이템을 EquipWeapon에 넣어주기
	// 순서대로가 아니라 Weapon Slot에서 선택한 녀석을 Equip Weapon Slot 중 선택한 슬롯에다가 넣어주어야 함 >>> 슬롯의 Index 정보도 필요할 듯함(UI를 띄울 때도 이를 활용)
	// 애초에 Inventory의 Equip Weapon에서부터 순서가 필요함 >> vector가 아니라 list?    m_listEquipWeapon >>> list가 아니라 배열?
	m_arrEquipWeapon[iEquipSlotIdx] = pItemData;

	// Weapon Equip Slot UI에 출력
	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx);


	return S_OK;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	for (auto& pQuickItem : m_vecQuickAccess)
		Safe_Release(pQuickItem);

	for (auto& pWeaponItem : m_vecWeapon)
		Safe_Release(pWeaponItem);

	for (auto& pEquipWeaponItem : m_arrEquipWeapon)
		Safe_Release(pEquipWeaponItem);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

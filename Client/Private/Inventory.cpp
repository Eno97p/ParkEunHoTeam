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
    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	// Inventory에 ItemData 추가
	CItemData::ITEMDATA_DESC pDesc{};
		
	pDesc.eItemName = eItemType;
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

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	for (auto& pQuickItem : m_vecQuickAccess)
		Safe_Release(pQuickItem);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

#include "Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"
#include "ItemData.h"
#include "UIGroup_DropItem.h"

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
	CItemData::DROPITEM_DESC pDesc{};
	pDesc.isDropTem = true;
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

	// 여기서 UI Manager로 접근하여 UIGroup Inventory를 갱신?
	CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

	return S_OK;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

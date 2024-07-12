#include "Inventory.h"

#include "GameInstance.h"
#include "Player.h"
#include "ItemData.h"
#include "UIGroup_DropItem.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInventory::Initialize()
{
    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
}

HRESULT CInventory::Add_DropItem()
{
	// Inventory�� ItemData �߰�
	CItemData::DROPITEM_DESC pDesc{};
	pDesc.isDropTem = true;
	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI ���
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
		++item;
	pUIDesc.eDropItemName = (*item)->Get_DropItemName();
	pUIDesc.wszTextureName = (*item)->Get_TextureName();
	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	// ���⼭ UI Manager�� �����Ͽ� UIGroup Inventory�� ����?

	return S_OK;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

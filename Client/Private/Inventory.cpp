#include "Inventory.h"

#include "GameInstance.h"
#include "Player.h"
#include "ItemData.h"

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
	CItemData::DROPITEM_DESC pDesc{};
	pDesc.isDropTem = true;
	//CItemData* pItemData = ;

	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	return S_OK;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

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

HRESULT CInventory::Add_Item(CItemData* pItemData)
{
	m_vecItem.emplace_back(pItemData);

	return S_OK;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

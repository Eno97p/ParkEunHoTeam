#include "Inventory.h"

#include "GameInstance.h"
#include "Player.h"

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

void CInventory::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

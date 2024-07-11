#pragma once

#include "Base.h"
#include "Client_Defines.h"

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
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

	HRESULT	Add_DropItem();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

private:
	vector<class CItemData*>		m_vecItem;

public:
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CInventory final : public CBase
{
	DECLARE_SINGLETON(CInventory)

public:
	CInventory();
	virtual ~CInventory() = default;

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };

public:
	virtual void Free() override;
};

END
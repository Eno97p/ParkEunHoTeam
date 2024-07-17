#include "Npc.h"

#include "GameInstance.h"
#include "Player.h"

CNpc::CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CNpc::CNpc(const CNpc& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CNpc::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNpc::Initialize(void* pArg)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_eLevel = pDesc->eLevel;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pPlayerTransform = dynamic_cast<CTransform*>(dynamic_cast<CPlayer*>(PlayerList.front())->Get_Component(TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

void CNpc::Priority_Tick(_float fTimeDelta)
{
}

void CNpc::Tick(_float fTimeDelta)
{
}

void CNpc::Late_Tick(_float fTimeDelta)
{
}

HRESULT CNpc::Render()
{
	return S_OK;
}

void CNpc::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayerTransform);
}

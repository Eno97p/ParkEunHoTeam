#include "NPC_Rlya.h"

#include "GameInstance.h"

CNPC_Rlya::CNPC_Rlya(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc{ pDevice, pContext }
{
}

CNPC_Rlya::CNPC_Rlya(const CNPC_Rlya& rhs)
	: CNpc{ rhs }
{
}

HRESULT CNPC_Rlya::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Rlya::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Rlya::Priority_Tick(_float fTimeDelta)
{
}

void CNPC_Rlya::Tick(_float fTimeDelta)
{
}

void CNPC_Rlya::Late_Tick(_float fTimeDelta)
{
}

HRESULT CNPC_Rlya::Render()
{
	return S_OK;
}

HRESULT CNPC_Rlya::Add_Components()
{
	return S_OK;
}

HRESULT CNPC_Rlya::Add_PartObjects()
{
	return S_OK;
}

CNPC_Rlya* CNPC_Rlya::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Rlya* pInstance = new CNPC_Rlya(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNPC_Rlya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Rlya::Clone(void* pArg)
{
	CNPC_Rlya* pInstance = new CNPC_Rlya(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNPC_Rlya");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Rlya::Free()
{
	__super::Free();
}

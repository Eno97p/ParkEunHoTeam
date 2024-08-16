#include "Hedgehog.h"
#include "GameInstance.h"
CHedgehog::CHedgehog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CHedgehog::CHedgehog(const CHedgehog& rhs)
	:CGameObject(rhs)
{
}

HRESULT CHedgehog::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CHedgehog::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CHedgehog::Priority_Tick(_float fTimeDelta)
{
}

void CHedgehog::Tick(_float fTimeDelta)
{
}

void CHedgehog::Late_Tick(_float fTimeDelta)
{
}

HRESULT CHedgehog::Add_Components()
{
	return E_NOTIMPL;
}

HRESULT CHedgehog::Add_Child_Effects()
{
	return E_NOTIMPL;
}

CHedgehog* CHedgehog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CHedgehog::Clone(void* pArg)
{
	return nullptr;
}

void CHedgehog::Free()
{
}

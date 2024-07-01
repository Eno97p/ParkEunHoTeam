#include "Mantari.h"
#include "GameInstance.h"

CMantari::CMantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMantari::CMantari(const CMonster& rhs)
    : CMonster{ rhs }
{
}

HRESULT CMantari::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMantari::Initialize(void* pArg)
{


    return S_OK;
}

void CMantari::Priority_Tick(_float fTimeDelta)
{
}

void CMantari::Tick(_float fTimeDelta)
{
}

void CMantari::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMantari::Render()
{
    return S_OK;
}

HRESULT CMantari::Add_Components()
{
    if (FAILED(Add_BehaviorTree()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMantari::Add_PartObjects()
{

    return S_OK;
}

HRESULT CMantari::Add_Nodes()
{
    return S_OK;
}

CMantari* CMantari::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CMantari::Clone(void* pArg)
{
    return nullptr;
}

void CMantari::Free()
{
}

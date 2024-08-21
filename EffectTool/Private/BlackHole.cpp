#include "BlackHole.h"
#include "GameInstance.h"
CBlackHole::CBlackHole(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CBlackHole::CBlackHole(const CBlackHole& rhs)
	:CGameObject(rhs)
{
}

HRESULT CBlackHole::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackHole::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	m_OwnDesc = make_unique<BLACKHOLE>(*((BLACKHOLE*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	if (FAILED(Add_ChildEffects()))
		return E_FAIL;
	return S_OK;
}

void CBlackHole::Priority_Tick(_float fTimeDelta)
{
}

void CBlackHole::Tick(_float fTimeDelta)
{
	for (auto& iter : m_ChildEffects)
	{
		iter->Tick(fTimeDelta);
	}
}

void CBlackHole::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_ChildEffects)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CBlackHole::Add_ChildEffects()
{
	m_OwnDesc->SphereDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_OwnDesc->RingDesc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

	CGameObject* Sphere =  m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BlackSphere"), &m_OwnDesc->SphereDesc);
	m_ChildEffects.emplace_back(Sphere);

	CGameObject* Ring = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BlackHoleRing"), &m_OwnDesc->RingDesc);
	m_ChildEffects.emplace_back(Ring);

	return S_OK;
}

CBlackHole* CBlackHole::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlackHole* pInstance = new CBlackHole(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlackHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBlackHole::Clone(void* pArg)
{
	CBlackHole* pInstance = new CBlackHole(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlackHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlackHole::Free()
{
	__super::Free();
	for (auto& iter : m_ChildEffects)
		Safe_Release(iter);
}

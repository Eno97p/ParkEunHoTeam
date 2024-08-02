#include "FirePillar.h"
#include "GameInstance.h"
CFirePillar::CFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CFirePillar::CFirePillar(const CFirePillar& rhs)
	:CGameObject(rhs)
{
}

HRESULT CFirePillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFirePillar::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	m_OwnDesc = make_shared<FIREPILLAR>(*((FIREPILLAR*)pArg));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_OwnDesc->vStartPos));
	return S_OK;
}

void CFirePillar::Priority_Tick(_float fTimeDelta)
{
}

void CFirePillar::Tick(_float fTimeDelta)
{
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
	{
		m_pGameInstance->Erase(this);
	}
}

void CFirePillar::Late_Tick(_float fTimeDelta)
{
}

HRESULT CFirePillar::Add_Components()
{
	return S_OK;
}

HRESULT CFirePillar::Add_Child_Effects()
{
	return S_OK;
}

CFirePillar* CFirePillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFirePillar* pInstance = new CFirePillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFirePillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFirePillar::Clone(void* pArg)
{
	CFirePillar* pInstance = new CFirePillar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFirePillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFirePillar::Free()
{
	__super::Free();
}

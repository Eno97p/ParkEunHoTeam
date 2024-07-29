#include "AndrasLazer.h"
#include "GameInstance.h"
CAndrasLazer::CAndrasLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CAndrasLazer::CAndrasLazer(const CAndrasLazer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CAndrasLazer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrasLazer::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<ANDRAS_LAZER_TOTALDESC>(*((ANDRAS_LAZER_TOTALDESC*)pArg));

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Add_Child_Effects()))
		return E_FAIL;


	return S_OK;
}

void CAndrasLazer::Priority_Tick(_float fTimeDelta)
{
}

void CAndrasLazer::Tick(_float fTimeDelta)
{
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
		m_pGameInstance->Erase(this);

}

void CAndrasLazer::Late_Tick(_float fTimeDelta)
{
}

HRESULT CAndrasLazer::Add_Components()
{
	return S_OK;
}

HRESULT CAndrasLazer::Add_Child_Effects()
{
	m_OwnDesc->BaseDesc.ParentMatrix = m_OwnDesc->ShooterMat;
	m_OwnDesc->CylinderDesc.ParentMatrix = m_OwnDesc->ShooterMat;
	m_OwnDesc->RainDesc.ParentMatrix = m_OwnDesc->ShooterMat;
	m_OwnDesc->ScrewDesc.ParentMatrix = m_OwnDesc->ShooterMat;
	m_OwnDesc->ElectricDesc.ParentMatrix = m_OwnDesc->ShooterMat;

	m_OwnDesc->BaseDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->CylinderDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->RainDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->ScrewDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;
	m_OwnDesc->ElectricDesc.fMaxLifeTime = m_OwnDesc->fLifeTime;

	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"),
		TEXT("Prototype_GameObject_Andras_LazerBase"), &m_OwnDesc->BaseDesc);
	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"),
		TEXT("Prototype_GameObject_Andras_LazerCylinder"), &m_OwnDesc->CylinderDesc);
	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"),
		TEXT("Prototype_GameObject_Andras_Screw"), &m_OwnDesc->ScrewDesc);
	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"),
		TEXT("Prototype_GameObject_ElectricCylinder"), &m_OwnDesc->ElectricDesc);
	m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"),
		TEXT("Prototype_GameObject_AndrasRain"), &m_OwnDesc->RainDesc);

	return S_OK;
}

CAndrasLazer* CAndrasLazer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrasLazer* pInstance = new CAndrasLazer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndrasLazer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAndrasLazer::Clone(void* pArg)
{
	CAndrasLazer* pInstance = new CAndrasLazer(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndrasLazer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAndrasLazer::Free()
{
	__super::Free();

}

#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"

#include "Player.h"
#include "Particle_Rect.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLandObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster & rhs)
	: CLandObject{ rhs }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	MST_DESC* pDesc = static_cast<MST_DESC*>(pArg);

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_eLevel = pDesc->eLevel;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
}

void CMonster::Tick(_float fTimeDelta)
{
}

void CMonster::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::Add_Nodes()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	for (size_t i = 0; i < 2; i++)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

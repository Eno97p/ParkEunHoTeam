#include "NPC_Rlya.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Body_Rlya.h"
#include "Player.h"
#include "UI_Activate.h"

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
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(pArg);

	pDesc->fSpeedPerSec = 3.f; // 수정 필요
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(160.f, 522.f, 98.f, 1.f)); // Test

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CNPC_Rlya::Priority_Tick(_float fTimeDelta)
{
	m_pBody->Priority_Tick(fTimeDelta);
}

void CNPC_Rlya::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	Check_Distance();
}

void CNPC_Rlya::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);
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
	// Body
	CPartObject::PARTOBJ_DESC pDesc{};
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pDesc.fSpeedPerSec = 0.f;
	pDesc.fRotationPerSec = 0.f;
	pDesc.eLevel = m_eLevel;

	m_pBody = dynamic_cast<CBody_Rlya*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Rlya"), &pDesc));

	return S_OK;
}

void CNPC_Rlya::Check_Distance()
{
	_vector vBetween = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector4Length(vBetween));

	if (ACTIVATE_DISTANCE >= fDistance)
		CUI_Manager::GetInstance()->Get_Activate()->Set_isRend(true);
	else
		CUI_Manager::GetInstance()->Get_Activate()->Set_isRend(false);
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

	Safe_Release(m_pBody);
}

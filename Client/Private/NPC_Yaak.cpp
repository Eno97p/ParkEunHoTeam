#include "NPC_Yaak.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "Body_Yaak.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"
#include "ItemData.h"

CNPC_Yaak::CNPC_Yaak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CNpc{ pDevice, pContext }
{
}

CNPC_Yaak::CNPC_Yaak(const CNPC_Yaak& rhs)
    : CNpc{ rhs }
{
}

HRESULT CNPC_Yaak::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Yaak::Initialize(void* pArg)
{
	NPC_DESC Desc;

	Desc.eLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(75.f, 521.f, 100.f, 1.f)); // Test

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_iDialogCnt = 3;

	Create_Script();

	return S_OK;
}

void CNPC_Yaak::Priority_Tick(_float fTimeDelta)
{
}

void CNPC_Yaak::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	m_pActivateUI->Tick(fTimeDelta);

	if (m_isScriptOn)
	{
		m_pScriptUI->Tick(fTimeDelta);

		Key_Input();
	}
}

void CNPC_Yaak::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);

	if (Check_Distance())
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_F) && !m_isScriptOn)
		{
			m_pScriptUI->Set_Rend(true);
			if (m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("첫 번째 스크립트"));
			m_isScriptOn = true;
		}
	}

	if (m_isScriptOn)
		m_pScriptUI->Late_Tick(fTimeDelta);
}

HRESULT CNPC_Yaak::Render()
{
	return S_OK;
}

HRESULT CNPC_Yaak::Add_PartObjects()
{
	// Body
	CPartObject::PARTOBJ_DESC pDesc{};
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pDesc.fSpeedPerSec = 0.f;
	pDesc.fRotationPerSec = 0.f;
	pDesc.eLevel = m_eLevel;

	m_pBody = dynamic_cast<CBody_Yaak*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Yaak"), &pDesc));

	return S_OK;
}

HRESULT CNPC_Yaak::Create_Script()
{
	CUIGroup_Script::UIGROUP_SCRIPT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eNpcType = CUIGroup_Script::NPC_YAAK;

	m_pScriptUI = dynamic_cast<CUIGroup_Script*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Script"), &pDesc));
	if (nullptr == m_pScriptUI)
		return E_FAIL;

	return S_OK;
}

void CNPC_Yaak::Key_Input()
{
}

CNPC_Yaak* CNPC_Yaak::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Yaak* pInstance = new CNPC_Yaak(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNPC_Yaak");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Yaak::Clone(void* pArg)
{
	CNPC_Yaak* pInstance = new CNPC_Yaak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNPC_Yaak");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Yaak::Free()
{
	__super::Free();

	Safe_Release(m_pBody);
}

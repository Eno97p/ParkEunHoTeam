#include "NPC_Rlya.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Body_Rlya.h"
#include "Player.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"

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

	m_iDialogCnt = 3;

	Create_Script();

	return S_OK;
}

void CNPC_Rlya::Priority_Tick(_float fTimeDelta)
{
	m_pBody->Priority_Tick(fTimeDelta);
}

void CNPC_Rlya::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	m_pActivateUI->Tick(fTimeDelta);

	if (m_isScriptOn)
	{
		m_pScriptUI->Tick(fTimeDelta);

		Key_Input();
	}
}

void CNPC_Rlya::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);

	if (Check_Distance())
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_F) && !m_isScriptOn)
		{
			m_pScriptUI->Set_Rend(true);
			if(m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("첫번째 텍스트"));
			m_isScriptOn = true;
		}
	}

	if (m_isScriptOn)
		m_pScriptUI->Late_Tick(fTimeDelta);
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

_bool CNPC_Rlya::Check_Distance()
{
	_vector vBetween = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector4Length(vBetween));

	return ACTIVATE_DISTANCE >= fDistance;
}

void CNPC_Rlya::Key_Input()
{
	// 엔터를 칠 때마다 Cnt가 하나씩 감소하고 0이 되면 종료되는 것으로?
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		switch (m_iDialogCnt)
		{
		case 3:
		{
			m_pScriptUI->Set_DialogText(TEXT("두번째 텍스트"));
			--m_iDialogCnt;
			break;
		}
		case 2:
		{
			m_pScriptUI->Set_DialogText(TEXT("세번째 텍스트"));
			--m_iDialogCnt;
			break;
		}
		case 1:
		{
			m_pScriptUI->Set_DialogText(TEXT("이미 종료된 이벤트"));
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			--m_iDialogCnt;
			break;
		}
		case 0:
		{
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			break;
		}
		default:
			break;
		}
	}
}

HRESULT CNPC_Rlya::Create_Script()
{
	CUIGroup::UIGROUP_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	m_pScriptUI = dynamic_cast<CUIGroup_Script*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Script"), &pDesc));
	if (nullptr == m_pScriptUI)
		return E_FAIL;

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

	Safe_Release(m_pBody);
}

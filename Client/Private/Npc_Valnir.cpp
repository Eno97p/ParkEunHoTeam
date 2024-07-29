#include "Npc_Valnir.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "Body_Valnir.h"
#include "Player.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"
#include "ItemData.h"

CNpc_Valnir::CNpc_Valnir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc{ pDevice, pContext }
{
}

CNpc_Valnir::CNpc_Valnir(const CNpc_Valnir& rhs)
	: CNpc{ rhs }
{
}

HRESULT CNpc_Valnir::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNpc_Valnir::Initialize(void* pArg)
{
	NPC_DESC Desc;

	Desc.eLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(75.f, 521.f, 98.f, 1.f)); // Test

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_iDialogCnt = 3; // 로직 생각해보아야 함

	Create_Script();

	return S_OK;
}

void CNpc_Valnir::Priority_Tick(_float fTimeDelta)
{
	m_pBody->Priority_Tick(fTimeDelta);
}

void CNpc_Valnir::Tick(_float fTimeDelta)
{
	m_pBody->Tick(fTimeDelta);

	m_pActivateUI->Tick(fTimeDelta);

	if (m_isScriptOn)
	{
		m_pScriptUI->Tick(fTimeDelta);

		Key_Input();
	}
}

void CNpc_Valnir::Late_Tick(_float fTimeDelta)
{
	m_pBody->Late_Tick(fTimeDelta);

	if (Check_Distance())
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (m_pGameInstance->Key_Down(DIK_F) && !m_isScriptOn)
		{
			m_pScriptUI->Set_Rend(true);
			if (m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("첫번째 스크립트"));
			m_isScriptOn = true;
		}
	}

	if (m_isScriptOn)
		m_pScriptUI->Late_Tick(fTimeDelta);
}

HRESULT CNpc_Valnir::Render()
{
	return S_OK;
}

HRESULT CNpc_Valnir::Add_PartObjects()
{
	// Body
	CPartObject::PARTOBJ_DESC pDesc{};
	pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	pDesc.eLevel = m_eLevel;

	m_pBody = dynamic_cast<CBody_Valnir*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Valnir"), &pDesc));

	return S_OK;
}

HRESULT CNpc_Valnir::Create_Script()
{
	CUIGroup_Script::UIGROUP_SCRIPT_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eNpcType = CUIGroup_Script::NPC_VALNIR;

	m_pScriptUI = dynamic_cast<CUIGroup_Script*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Script"), &pDesc));
	if (nullptr == m_pScriptUI)
		return E_FAIL;

	return S_OK;
}

_bool CNpc_Valnir::Check_Distance()
{
	_vector vBetween = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector4Length(vBetween));

	return ACTIVATE_DISTANCE >= fDistance;
}

void CNpc_Valnir::Key_Input()
{
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		switch (m_iDialogCnt)
		{
		case 3:
		{
			m_pScriptUI->Set_DialogText(TEXT("기억해라, 방황하는 자여.\n빛이 어둠을 물리치듯 너 또한 네 앞에 놓인 모든 어둠을 헤쳐나갈 수 있을 거다."));
			--m_iDialogCnt;
			break;
		}
		case 2:
		{
			m_pScriptUI->Set_DialogText(TEXT("자, 받아라. 그리고 이 빛을 따라 나아가라."));
			--m_iDialogCnt;
			break;
		}
		case 1:
		{
			m_pScriptUI->Set_DialogText(TEXT("FireFly를 사용하면 길을 밝힐 수 있어."));
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			--m_iDialogCnt;

			CInventory::GetInstance()->Add_Item(CItemData::ITEMNAME_FIREFLY); // Inventory에 Firefly 추가

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

CNpc_Valnir* CNpc_Valnir::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNpc_Valnir* pInstance = new CNpc_Valnir(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNpc_Valnir");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNpc_Valnir::Clone(void* pArg)
{
	CNpc_Valnir* pInstance = new CNpc_Valnir(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNpc_Valnir");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNpc_Valnir::Free()
{
	__super::Free();

	Safe_Release(m_pBody);
}

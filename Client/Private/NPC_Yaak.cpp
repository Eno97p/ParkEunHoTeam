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

	m_iDialogCnt = 4;

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
				m_pScriptUI->Set_DialogText(TEXT("방랑하는 망령을 쓰러뜨렸구나. 새로운 여정의 시작이라고 볼 수도 있겠지."));
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
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		switch (m_iDialogCnt)
		{
		case 4:
		{
			m_pScriptUI->Set_DialogText(TEXT("네가 과연 어디까지 도달할 수 있을지 궁금한걸. 도움을 주지."));
			--m_iDialogCnt;
			break;
		}
		case 3:
		{
			m_pScriptUI->Set_DialogText(TEXT("이 검은 네가 마주할 모든 어둠을 밝히는 불꽃이 될 것이고 그 불꽃은 네가 지닌 용기를\n시험할 것이다."));
			--m_iDialogCnt;
			break;
		}
		case 2:
		{
			m_pScriptUI->Set_DialogText(TEXT("왕국의 운명을 짊어진 자여, 행운을 빈다. 네 여정이 불꽃처럼 밝게 빛나기를."));
			--m_iDialogCnt;
			break;
		}
		case 1:
		{
			m_pScriptUI->Set_DialogText(TEXT("잊지 마라. 좋은 무기는 오직 진정한 용사만이 다룰 수 있다는 것을."));
			m_isScriptOn = false;
			m_pScriptUI->Set_Rend(false);
			--m_iDialogCnt;

			CInventory::GetInstance()->Add_Weapon(CItemData::ITEMNAME_DURGASWORD);


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

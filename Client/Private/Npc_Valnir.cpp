#include "Npc_Valnir.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"

#include "Body_Valnir.h"
#include "Player.h"
#include "UI_Activate.h"
#include "UIGroup_Script.h"
#include "UIGroup_Shop.h"
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

	m_iDialogCnt = 0; // 로직 생각해보아야 함

	Create_Script();
	Create_Shop();

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
		m_pShopUI->Tick(fTimeDelta);

		// 마우스 위치가 Select 객체의 어디와 충돌하고 있느냐에 따라 스크립트의 대사가 바뀔 것임
		// 그 정보를 ShopUI(Group)이 가지고 있도록 하자

		Set_DialogText();
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
			// 여기도 다르게 처리해야함
			/*if (m_iDialogCnt != 0)
				m_pScriptUI->Set_DialogText(TEXT("첫번째 스크립트"));*/
			m_isScriptOn = true;

			m_pShopUI->Set_Rend(true);
		}
	}

	if (m_isScriptOn)
	{
		m_pScriptUI->Late_Tick(fTimeDelta);
		m_pShopUI->Late_Tick(fTimeDelta);
	}
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

HRESULT CNpc_Valnir::Create_Shop()
{
	CUIGroup_Shop::UIGROUP_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;

	m_pShopUI = dynamic_cast<CUIGroup_Shop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Shop"), &pDesc));
	if (nullptr == m_pShopUI)
		return E_FAIL;

	return S_OK;
}

_bool CNpc_Valnir::Check_Distance()
{
	_vector vBetween = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector4Length(vBetween));

	return ACTIVATE_DISTANCE >= fDistance;
}

void CNpc_Valnir::Set_DialogText()
{
	_int iSelectIdx = m_pShopUI->Get_SelectIdx();

	switch (iSelectIdx)
	{
	case -1:
	{
		m_pScriptUI->Set_DialogText(TEXT("원하는 상품을 골라. 값을 치루면 너에게 주도록 하지."));
		break;
	}
	case 0:
	{
		m_pScriptUI->Set_DialogText(TEXT("[System] 섬세하게 조각된 반투명 수정 조각\n\t풍부한 천상 에너지를 발산"));
		break;
	}
	case 1:
	{
		m_pScriptUI->Set_DialogText(TEXT("[System] 희미한 빛의 수정 조각에 장착된 수수께끼의 봉인구\n\t저항력을 일시적으로 강화"));
		break;
	}
	case 2:
	{
		m_pScriptUI->Set_DialogText(TEXT("[System] Astyr 신체 대부분을 구성하는 잠재적 물질\n\t무기 업그레이드 시 사용 가능"));
		break;
	}
	default:
		break;
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

	Safe_Release(m_pShopUI);
	Safe_Release(m_pBody);
}

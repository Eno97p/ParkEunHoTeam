#include "ItemData.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "UI_Manager.h"

#include "UIGroup_Inventory.h"
#include "UIGroup_InvSub.h"
#include "UIGroup_Quick.h"
#include "UIGroup_WeaponSlot.h"

CItemData::CItemData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{pDevice, pContext}
{
}

CItemData::CItemData(const CItemData& rhs)
	: CGameObject{rhs}
{
}

HRESULT CItemData::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemData::Initialize(void* pArg)
{
	ITEMDATA_DESC* pDesc = static_cast<ITEMDATA_DESC*>(pArg);

	if (pDesc->isDropTem)
		Set_DropItem_Data(pDesc->eDropItemName);
	else
	{
		m_eItemName = pDesc->eItemName;
		Set_Item_Data();
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iCount = 1;

	return S_OK;
}

void CItemData::Priority_Tick(_float fTimeDelta)
{
}

void CItemData::Tick(_float fTimeDelta)
{
}

void CItemData::Late_Tick(_float fTimeDelta)
{
}

HRESULT CItemData::Render()
{
	return S_OK;
}

void CItemData::Use_Item(_uint iInvenIdx)
{
	// 호버보드나 Firefly 같은 특정 아이템들은 사용 시 Inventory 포함한 메뉴들이 싹 꺼져야 할 거 같음
	// 나머지는 그냥 써지는(적용되는) 식으로


	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_CATALYST:
	{
		//Apply_UseCount(iInvenIdx);
		// 얘는 0이 되어도 사라지지 않기



		break;
	}
	case Client::CItemData::ITEMNAME_HOVERBOARD:
		break;
	case Client::CItemData::ITEMNAME_FIREFLY:
		break;
	case Client::CItemData::ITEMNAME_WHISPERER:
		break;
	case Client::CItemData::ITEMNAME_BUFF1:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_BUFF2:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_BUFF3:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_BUFF4:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_ESSENCE:
	{
		_uint iRand = rand() % 300 + 100;
		CInventory::GetInstance()->Calcul_Soul(iRand);
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Rend_Calcul(iRand);

		Apply_UseCount(iInvenIdx);
		break;
	}
	case Client::CItemData::ITEMNAME_ETHER:
	{
		Apply_UseCount(iInvenIdx);
		break;
	}
	default:
		break;
	}


}

void CItemData::Apply_UseCount(_uint iInvenIdx)
{
	if (m_iCount > 1)
	{
		m_iCount -= 1;
	}
	else // 하나 남은 경우 삭제 필요
	{
		// UI Inventory의 Sub Quick에서 삭제
		dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Delete_InvSub_QuickSlot_ToInvIdx(iInvenIdx);

		// Quick의 Slot에서 삭제
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_QuickSlot_Delete(iInvenIdx);

		// UI Inventory에서도 삭제
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory_Delete(iInvenIdx);

		// Quick SubInv에서도 삭제
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_Delete(iInvenIdx);

		// HUD에서도 삭제   
		dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_QUICK);

		CInventory::GetInstance()->Delete_Item(this);

		// Slot들 땡겨오기
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory(iInvenIdx);
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_Inventory(iInvenIdx);
	}
}

void CItemData::Set_DropItem_Data(CItem::ITEM_NAME eItemName)
{
	switch (eItemName)
	{
	case Client::CItem::ITEM_BUFF1:
	{
		m_eItemType = ITEMTYPE_USABLE; //  ITEMTYPE_BUFF
		m_eItemName = ITEMNAME_BUFF1;
		m_wszItemName = TEXT("SIGIL OF STRENGTH");
		m_wszItemExplain = TEXT("금속 조각에 큰 홈으로 새겨진\n기하학적 봉인구\n무기의 공격 능력을\n일시적으로 강화");
		m_wszItemExplain_quick = TEXT("금속 조각에 큰 홈을 새기는 기하학적 봉인구\n무기의 공격 능력을 일시적으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("유리 구슬에 장착된\n원형 봉인구\n갑옷의 방어 능력을\n일시적으로 강화");
		m_wszItemExplain_quick = TEXT("유리 구슬에 장착된 원형 봉인구\n갑옷의 방어 능력을 일시적으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("암석에 조각된\n정교한 봉인구\n체력 회복을 일시적으로 증가");
		m_wszItemExplain_quick = TEXT("암석에 조각된 정교한 봉인구\n체력 회복을 일시적으로 증가");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("희미한 빛의 수정 조각에 장착된\n수수께끼의 봉인구\n저항력을 일시적으로 강화");
		m_wszItemExplain_quick = TEXT("희미한 빛의 수정 조각에 장착된 수수께끼의 봉인구\n저항력을 일시정으로 강화");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_ESSENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ESSENCE;
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Essence");
		m_wszItemName = TEXT("GRACEFUL ESSENCE");
		m_wszItemExplain = TEXT("안정적인 데이터 [컴파일링 완료]\n보류중인 메모리 분석");
		m_wszItemExplain_quick = TEXT("안정적인 데이터 [컴파일링 완료]\n보류중인 메모리 분석");
		break;
	}
	case Client::CItem::ITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ETHER;
		m_wszItemName = TEXT("RADIANT ETHER");
		m_wszItemExplain = TEXT("섬세하게 조각된 반투명 수정 조각\n풍부한 천상 에너지를 발산");
		m_wszItemExplain_quick = TEXT("섬세하게 조각된 반투명 수정 조각\n풍부한 천상 에너지를 발산");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItem::ITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE1;
		m_wszItemName = TEXT("HADRONITE");
		m_wszItemExplain = TEXT("Astyr 신체 대부분을 구성하는\n잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszItemExplain_quick = TEXT("Astyr 신체 대부분을 구성하는 잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0");
		break;
	}
	case Client::CItem::ITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE2;
		m_wszItemName = TEXT("GRACEFUL HADRONITE");
		m_wszItemExplain = TEXT("Astyr 신체 대부분을 구성하는\n잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszItemExplain_quick = TEXT("Astyr 신체 대부분을 구성하는 잠재적 물질\n무기 업그레이드 시 사용 가능");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade1");
		break;
	}
	default:
		break;
	}
}

void CItemData::Set_Item_Data()
{
	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_DURGASWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("DURGA'S CLAYMORE");
		m_wszItemExplain = TEXT("추기경 Durga의 검\n심화 네트워크 접속의 중요 단서\n신의 핵심 일부 복제품이 포함됨\n결합된 모든 특권을 제공한다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Durgas_Claymore");
		break;
	}
	case Client::CItemData::ITEMNAME_PRETORIANSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("PRETORIAN SWORD");
		m_wszItemExplain = TEXT("히프노스와 나라야나를 보호하기 위해\n전적으로 헌신하는 안티 기계들,\n프리토리아 근위대의 검\n그들이 지나간 자리에는\n침묵의 폐허만이 남는다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Pretorian");
		break;
	}
	case Client::CItemData::ITEMNAME_RADAMANTHESWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("RADAMANTHE'S DAGGER");
		m_wszItemExplain = TEXT("추기경 Radamanthe의 검\n심화 네트워크 접속의 중요 단서\n신의 핵심 일부 복제품이 포함됨\n결합된 모든 특권을 제공한다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Radamanthes");
		break;
	}
	case Client::CItemData::ITEMNAME_WHISPERSWORD:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("ELISH");
		m_wszItemExplain = TEXT("손상된 데이터. 읽기 오류 발생.\n전체 격리 권장");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Elish");
		break;
	}
	case Client::CItemData::ITEMNAME_OPH:
		break;
	case Client::CItemData::ITEMNAME_ETHERBOLT:
	{
		m_eItemType = ITEMTYPE_SKILL;
		m_wszItemName = TEXT("ETHERBOLT");
		m_wszItemExplain = TEXT("어둠 속에서도 빛나는\n정화의 상징\n강력한 에테르의 힘으로\n멀리까지 날아간다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItemData::ITEMNAME_AEGIS:
		break;
	case Client::CItemData::ITEMNAME_CATALYST:
		break;
	case Client::CItemData::ITEMNAME_HOVERBOARD:
		break;
	case Client::CItemData::ITEMNAME_FIREFLY:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("FIREFLY");
		m_wszItemExplain = TEXT("모험가의 앞길을 밝혀주는\n희망의 상징\n아무리 어둥이 짙어도\n빛은 항상 길을 찾아낸다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Firefly");
		break;
	}
	case Client::CItemData::ITEMNAME_WHISPERER:
		break;
	default:
		break;
	}
}

CItemData* CItemData::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemData* pInstance = new CItemData(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItemData");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemData::Clone(void* pArg)
{
	CItemData* pInstance = new CItemData(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItemData");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemData::Free()
{
	__super::Free();
}

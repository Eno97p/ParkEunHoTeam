#include "ItemData.h"

#include "GameInstance.h"

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

void CItemData::Set_DropItem_Data(CItem::ITEM_NAME eItemName)
{
	switch (eItemName)
	{
	case Client::CItem::ITEM_BUFF1:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF1;
		m_wszItemName = TEXT("SIGIL OF STRENGTH");
		m_wszItemExplain = TEXT("버프 1");
		m_wszItemExplain_quick = TEXT("버프 1");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("버프 2");
		m_wszItemExplain_quick = TEXT("버프 2");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("버프 3");
		m_wszItemExplain_quick = TEXT("버프 3");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("버프 4");
		m_wszItemExplain_quick = TEXT("버프 4");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_SOUL:
	{
		m_eItemType = ITEMTYPE_ETC; // 먹자마자 소모되어야 하므로 >> 인벤에 들어가는 일 없음
		m_eItemName = ITEMNAME_SOUL;
		m_wszItemName = TEXT("SOUL");
		m_wszItemExplain = TEXT("인벤토리에 출력하지 않을 예정인 아이템");
		m_wszTexture = TEXT("Prototype_Component_Texture_HUD_StateSoul");
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
	case Client::CItemData::ITEMNAME_CATHARSIS:
	{
		m_eItemType = ITEMTYPE_WEAPON;
		m_wszItemName = TEXT("Catharsis"); // CATHARSIS
		m_wszItemExplain = TEXT("순수한 영혼과 강력한 마력의\n결합체\n그 자체로 살아있는 존재처럼\n사용자의 의지에 반응한다.");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Catharsis");
		break;
	}
	case Client::CItemData::ITEMNAME_CENDRES:
		break;
	case Client::CItemData::ITEMNAME_CORRUPTEDSWORD:
		break;
	case Client::CItemData::ITEMNAME_DURGASWORD:
		break;
	case Client::CItemData::ITEMNAME_ICEBLADE:
		break;
	case Client::CItemData::ITEMNAME_NARUEHSWORD:
		break;
	case Client::CItemData::ITEMNAME_PRETORIANSWORD:
		break;
	case Client::CItemData::ITEMNAME_RADAMANTHESWORD:
		break;
	case Client::CItemData::ITEMNAME_SITRASWORD:
		break;
	case Client::CItemData::ITEMNAME_VALNIRSWORD:
		break;
	case Client::CItemData::ITEMNAME_VEILLEURSWORD:
		break;
	case Client::CItemData::ITEMNAME_WHISPERSWORD:
		break;
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

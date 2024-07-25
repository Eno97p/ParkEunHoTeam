#include "Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"
#include "ItemData.h"

#include "UIGroup_DropItem.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Weapon.h"
#include "UIGroup_UpGPage.h"
#include "UIGroup_State.h"

IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CItemData* CInventory::Get_ItemData(_uint iSlotIdx)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < iSlotIdx; ++i)
		++item;

	return (*item);
}

HRESULT CInventory::Initialize()
{
	if (FAILED(Initialize_DefaultItem()))
		return E_FAIL;

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
}

HRESULT CInventory::Initialize_DefaultItem()
{
	// 게임 처음 시작 시 기본적으로 가지고 있는 아이템 
	// Weapon에 추가
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_CATHARSIS;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Equip Sign 활성화
	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	Add_EquipWeapon((*weapon), 0);
	(*weapon)->Set_isEquip(true);
	dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true);


	// Skill에 추가
	pDesc.eItemName = CItemData::ITEMNAME_ETHERBOLT;
	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI의 경우에는 Tab 누르면 변환되면서 보여줘야함
	//CUI_Manager::GetInstance()->Update_Skill_Add();

	// Upgrade Page에도 weapon 추가
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	//test
	Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_BUFF1);

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	if (CItem::ITEM_SOUL == eItemType)
	{
		_uint iRand = rand() % 300 + 100;
		CInventory::GetInstance()->Calcul_Soul(iRand);

		// UI 출력 관련 함수를 호출
		dynamic_cast<CUIGroup_State*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_State"))->Rend_Calcul(iRand);
	}
	else
	{
		// 동일한 템이 존재하는 경우 해당 슬롯의 개수를 늘리는 식으로 들어가야 함 !
		// 아이템 개수 정보는 ItemData가 가지고 있는 것으로 

		// Inventory를 순회하며 동일한 ItemData가 있는지 검사 -> 없으면 기존처럼 추가 -> 있으면 해당 ItemData의 개수를 늘리는 것으로
		if (!Check_Overlab(eItemType)) // 중복 체크
		{
			// Inventory에 ItemData 추가
			CItemData::ITEMDATA_DESC pDesc{};

			pDesc.isDropTem = true;
			pDesc.eDropItemName = eItemType;
			m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

			// UI 출력
			CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
			pUIDesc.eLevel = LEVEL_STATIC;

			vector<CItemData*>::iterator item = m_vecItem.begin();
			for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
				++item;
			pUIDesc.eItemName = (*item)->Get_ItemName();
			pUIDesc.wszTextureName = (*item)->Get_TextureName();
			m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

			CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

			// Quick의 InvSlot에도 ItemIcon 출력해주어야 함
			CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);
		}
	}

	return S_OK;
}

HRESULT CInventory::Add_Item(CItemData::ITEM_NAME eItemName)
{
	// Inventory에 ItemData추가
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));
	
	// UI 출력
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
		++item;
	pUIDesc.eItemName = (*item)->Get_ItemName();
	pUIDesc.wszTextureName = (*item)->Get_TextureName();
	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	// UI도
	CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

	// Quick의 InvSlot에도 ItemIcon 출력해주어야 함
	CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData, _int iInvenIdx)
{
	// Inventory에서 현재 선택한 아이템을 QuickAccess에 등록
	m_mapQuickAccess.emplace(iInvenIdx, pItemData);

	// InvSub QuickAccess Slot UI에 출력하기
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add(iInvenIdx);

	// QuickAccess에도 출력 필요
	CUI_Manager::GetInstance()->Update_Quick_Add(pItemData, iInvenIdx);

	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_QuickSlot(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Add_EquipWeapon(CItemData* pItemData, _uint iEquipSlotIdx)
{
	// Weapon의 Slot에서 현재 선택한 아이템을 EquipWeapon에 넣어주기
	m_arrEquipWeapon[iEquipSlotIdx] = pItemData;

	// Weapon Equip Slot UI에 출력
	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx);

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_WEAPON);

	return S_OK;
}

HRESULT CInventory::Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx)
{
	// ItemData의 Equp 여부 설정
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < iInvenIdx; ++i)
		++item;

	(*item)->Set_isEquip(false);

	m_mapQuickAccess.erase(iInvenIdx);

	return S_OK;
}

HRESULT CInventory::Delete_EquipWeapon(_uint iEquipSlotIdx)
{
	// 인자로 들어온 Slot의 정보를 제거
	m_arrEquipWeapon[iEquipSlotIdx] = nullptr;

	// Weapon Equip Slot UI에서 제거
	CUI_Manager::GetInstance()->Update_EquipWeapon_Delete(iEquipSlotIdx);

	// HUD에서도 출력 제거 
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_WEAPON);

	return S_OK;
}

HRESULT CInventory::Add_EquipSkill(CItemData* pItemData, _uint iEquipSlotIdx)
{
	m_arrEquipSkill[iEquipSlotIdx] = pItemData;

	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx);

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_SKILL);

	return S_OK;
}

HRESULT CInventory::Delete_EquipSkill(_uint iEquipSlotIdx)
{
	m_arrEquipSkill[iEquipSlotIdx] = nullptr;

	CUI_Manager::GetInstance()->Update_EquipSkill_Delete(iEquipSlotIdx);

	// HUD 관련 코드도 추가해야함
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_SKILL);

	return S_OK;
}

_bool CInventory::Check_Overlab(CItem::ITEM_NAME eItemType)
{
	CItemData::ITEM_NAME eItemName = { CItemData::ITEMNAME_END };
	switch (eItemType)
	{
	case Client::CItem::ITEM_BUFF1:
		eItemName = CItemData::ITEMNAME_BUFF1;
		break;
	case Client::CItem::ITEM_BUFF2:
		eItemName = CItemData::ITEMNAME_BUFF2;
		break;
	case Client::CItem::ITEM_BUFF3:
		eItemName = CItemData::ITEMNAME_BUFF3;
		break;
	case Client::CItem::ITEM_BUFF4:
		eItemName = CItemData::ITEMNAME_BUFF4;
		break;
	case Client::CItem::ITEM_ESSENCE:
		eItemName = CItemData::ITEMNAME_ESSENCE;
		break;
	case Client::CItem::ITEM_ETHER:
		eItemName = CItemData::ITEMNAME_ETHER;
		break;
	case Client::CItem::ITEM_UPGRADE1:
		eItemName = CItemData::ITEMNAME_UPGRADE1;
		break;
	case Client::CItem::ITEM_UPGRADE2:
		eItemName = CItemData::ITEMNAME_UPGRADE2;
		break;
	default:
		break;
	}

	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (eItemName == (*item)->Get_ItemName()) // 이름이 일치하는 것이 있다면
		{
			// 여기서 해당 ItemData에 중복 처리를 해주는 것이?
			(*item)->Set_Count(1);

			return true;
		}
		else
		{
			++item;
		}
	}

	return false;
}

void CInventory::Free()
{
	for (auto& pItemData : m_vecItem)
		Safe_Release(pItemData);

	for (auto& pWeaponItem : m_vecWeapon)
		Safe_Release(pWeaponItem);

	for (auto& pSkillItem : m_vecSkill)
		Safe_Release(pSkillItem);

	for (auto& pArtefact : m_vecArtefact)
		Safe_Release(pArtefact);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
}

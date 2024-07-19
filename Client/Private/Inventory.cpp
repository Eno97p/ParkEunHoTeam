#include "Inventory.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"
#include "ItemData.h"
#include "UIGroup_DropItem.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Weapon.h"

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

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
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

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData)
{
	// Inventory에서 현재 선택한 아이템을 QuickAccess에 등록
	// 현재 m_vecItem에 들어가 있는, 선택한 그 아이템을 m_vecQuickAccess에 넣어주기
	m_vecQuickAccess.emplace_back(pItemData);

	// InvSub QuickAccess Slot UI에 출력하기
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add();

	// QuickAccess에도 출력 필요
	CUI_Manager::GetInstance()->Update_Quick_Add(pItemData);

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
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Delete_EquipWeapon(_uint iEquipSlotIdx)
{
	// 인자로 들어온 Slot의 정보를 제거
	m_arrEquipWeapon[iEquipSlotIdx] = nullptr;

	// Weapon Equip Slot UI에서 제거
	CUI_Manager::GetInstance()->Update_EquipWeapon_Delete(iEquipSlotIdx);

	// HUD에서도 출력 제거 
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture();

	return S_OK;
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

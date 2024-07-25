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
	// ���� ó�� ���� �� �⺻������ ������ �ִ� ������ 
	// Weapon�� �߰�
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_CATHARSIS;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI ���
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Equip Sign Ȱ��ȭ
	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	Add_EquipWeapon((*weapon), 0);
	(*weapon)->Set_isEquip(true);
	dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true);


	// Skill�� �߰�
	pDesc.eItemName = CItemData::ITEMNAME_ETHERBOLT;
	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI�� ��쿡�� Tab ������ ��ȯ�Ǹ鼭 ���������
	//CUI_Manager::GetInstance()->Update_Skill_Add();

	// Upgrade Page���� weapon �߰�
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	//test
	/*Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_ESSENCE);*/

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	// Soul�� ��쿡�� �б� ó�� ���� �� ��
	if (CItem::ITEM_SOUL == eItemType)
	{
		// Inventory�� ���°� �ƴ϶� Soul�� ������ �� + UI ���?
		// ������ ���� �����ֱ�
		_uint iRand = rand() % 300;
		CInventory::GetInstance()->Calcul_Soul(iRand);

		// UI ��� ���� �Լ��� ȣ��
		dynamic_cast<CUIGroup_State*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_State"))->Rend_Calcul(iRand);
	}
	else
	{
		// Inventory�� ItemData �߰�
		CItemData::ITEMDATA_DESC pDesc{};

		pDesc.isDropTem = true;
		pDesc.eDropItemName = eItemType;
		m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

		// UI ���
		CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
		pUIDesc.eLevel = LEVEL_STATIC;

		vector<CItemData*>::iterator item = m_vecItem.begin();
		for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
			++item;
		pUIDesc.eItemName = (*item)->Get_ItemName();
		pUIDesc.wszTextureName = (*item)->Get_TextureName();
		m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

		CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

		// Quick�� InvSlot���� ItemIcon ������־�� ��
		CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);
	}

	return S_OK;
}

HRESULT CInventory::Add_Item(CItemData::ITEM_NAME eItemName)
{
	// Inventory�� ItemData�߰�
	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));
	
	// UI ���
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
		++item;
	pUIDesc.eItemName = (*item)->Get_ItemName();
	pUIDesc.wszTextureName = (*item)->Get_TextureName();
	m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

	// UI��
	CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

	// Quick�� InvSlot���� ItemIcon ������־�� ��
	CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData, _int iInvenIdx)
{
	// Inventory���� ���� ������ �������� QuickAccess�� ���
	m_mapQuickAccess.emplace(iInvenIdx, pItemData);

	// InvSub QuickAccess Slot UI�� ����ϱ�
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add(iInvenIdx);

	// QuickAccess���� ��� �ʿ�
	CUI_Manager::GetInstance()->Update_Quick_Add(pItemData, iInvenIdx);

	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_QuickSlot(pItemData->Get_TextureName());

	return S_OK;
}

HRESULT CInventory::Add_EquipWeapon(CItemData* pItemData, _uint iEquipSlotIdx)
{
	// Weapon�� Slot���� ���� ������ �������� EquipWeapon�� �־��ֱ�
	m_arrEquipWeapon[iEquipSlotIdx] = pItemData;

	// Weapon Equip Slot UI�� ���
	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx);

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_WEAPON);

	return S_OK;
}

HRESULT CInventory::Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx)
{
	// ItemData�� Equp ���� ����
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < iInvenIdx; ++i)
		++item;

	(*item)->Set_isEquip(false);

	m_mapQuickAccess.erase(iInvenIdx);

	return S_OK;
}

HRESULT CInventory::Delete_EquipWeapon(_uint iEquipSlotIdx)
{
	// ���ڷ� ���� Slot�� ������ ����
	m_arrEquipWeapon[iEquipSlotIdx] = nullptr;

	// Weapon Equip Slot UI���� ����
	CUI_Manager::GetInstance()->Update_EquipWeapon_Delete(iEquipSlotIdx);

	// HUD������ ��� ���� 
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

	// HUD ���� �ڵ嵵 �߰��ؾ���
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_SKILL);

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

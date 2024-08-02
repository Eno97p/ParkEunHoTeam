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
	pDesc.eItemName = CItemData::ITEMNAME_WHISPERSWORD;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));


	// ���� ���Ƿ� �̷��� �߰������� ���߿� ������ �߰��� ���� <<Upgrade�� Slot�� �߰��ϴ� ����>>�� �ʿ�
	// test >> ���� ����
	// UI ���
	//CUI_Manager::GetInstance()->Update_Weapon_Add();

	//pDesc.isDropTem = false;
	//pDesc.eItemName = CItemData::ITEMNAME_DURGASWORD;
	//m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	//// UI ���
	//CUI_Manager::GetInstance()->Update_Weapon_Add();

	//pDesc.isDropTem = false;
	//pDesc.eItemName = CItemData::ITEMNAME_PRETORIANSWORD;
	//m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	//// UI ���
	//CUI_Manager::GetInstance()->Update_Weapon_Add();

	//pDesc.isDropTem = false;
	//pDesc.eItemName = CItemData::ITEMNAME_RADAMANTHESWORD;
	//m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));




	// UI ���
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Equip Sign Ȱ��ȭ
	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	Add_EquipWeapon((*weapon), 0);
	(*weapon)->Set_isEquip(true);
	dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true);


	// Skill�� �߰�
	pDesc.eItemName = CItemData::ITEMNAME_OPH;
	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI�� ��쿡�� Tab ������ ��ȯ�Ǹ鼭 ���������
	//CUI_Manager::GetInstance()->Update_Skill_Add();

	// Upgrade Page���� weapon �߰�
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	//test
	Add_DropItem(CItem::ITEM_ESSENCE);
	Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_UPGRADE1);
	Add_DropItem(CItem::ITEM_UPGRADE1);

	return S_OK;
}

HRESULT CInventory::Add_DropItem(CItem::ITEM_NAME eItemType)
{
	if (CItem::ITEM_SOUL == eItemType)
	{
		_uint iRand = rand() % 300 + 100;
		CInventory::GetInstance()->Calcul_Soul(iRand);

		// UI ��� ���� �Լ��� ȣ��
		dynamic_cast<CUIGroup_State*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_State"))->Rend_Calcul(iRand);
	}
	else
	{
		if (!Check_Overlab(eItemType)) // �ߺ� üũ
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

HRESULT CInventory::Add_Weapon(CItemData::ITEM_NAME eItemName)
{
	// ���� ȹ�� �Լ�

	// Inventory���� �־�����ϰ� / UI Weapon���� �־���� �ϰ� / 

	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI ���
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Upgrade Page���� weapon �߰�
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	return S_OK;
}

HRESULT CInventory::Add_QuickAccess(CItemData* pItemData, _int iInvenIdx)
{
	// Inventory���� ���� ������ �������� QuickAccess�� ���
	m_mapQuickAccess.emplace(iInvenIdx, pItemData);

	// InvSub QuickAccess Slot UI�� ����ϱ�
	CUI_Manager::GetInstance()->Update_InvSub_Quick_Add(iInvenIdx); // !!!! ���⼭ Inventory Idx���� ����� Ȱ�� ���� �� ����

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

HRESULT CInventory::Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx, wstring wstrItemName)
{
	// ItemData�� Equp ���� ����
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (wstrItemName == (*item)->Get_ItemNameText())
		{
			(*item)->Set_isEquip(false);
			break;
		}
		else
			++item;
	}

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

HRESULT CInventory::Delete_Item(CItemData* pItemData)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (pItemData == (*item))
		{
			// Quick�� ���� ���̾��ٸ� �� ���� �������־�� ��
			if ((*item)->Get_isEquip())
			{
				// i �� ���� Inventory ���� �ε����϶���? �ƴϾ��� �� ����; �߰��� ���� �� �ֵ��� ������� ��.
				map<_uint, CItemData*>::iterator quickItem = m_mapQuickAccess.begin();
				for (size_t j = 0; j < m_mapQuickAccess.size(); ++j)
				{
					if ((*quickItem).second == (*item))
					{
						m_mapQuickAccess.erase(quickItem);
						break;
					}
					else
						++quickItem;
				}
			}

			Safe_Release((*item));
			m_vecItem.erase(item);
			break;
		}
		else
		{
			++item;
		}
	}

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
		if (eItemName == (*item)->Get_ItemName()) // �̸��� ��ġ�ϴ� ���� �ִٸ�
		{
			// ���⼭ �ش� ItemData�� �ߺ� ó���� ���ִ� ����?
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

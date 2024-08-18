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

_uint CInventory::Get_EquipWeaponSize()
{
	// 장착 슬롯에 값이 들어간 개수가 몇 개인지 반환하는 함수
	_uint iCount = { 0 };

	for (size_t i = 0; i < 3; ++i)
	{
		if (m_arrEquipWeapon[i] != nullptr)
			++iCount;
	}

	return iCount;
}

_uint CInventory::Get_EquipSkillSize()
{
	_uint iCount = { 0 };

	for (size_t i = 0; i < 3; ++i)
	{
		if (m_arrEquipSkill[i] != nullptr)
			++iCount;
	}

	return iCount;
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
	pDesc.eItemName = CItemData::ITEMNAME_WHISPERSWORD;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));


	// 지금 임의로 이렇게 추가하지만 나중에 실제로 추가할 때는 <<Upgrade의 Slot에 추가하는 로직>>도 필요
	// test >> 추후 삭제
	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_DURGASWORD;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_PRETORIANSWORD;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	pDesc.isDropTem = false;
	pDesc.eItemName = CItemData::ITEMNAME_RADAMANTHESWORD;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(false); // Test


	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Equip Sign 활성화
	vector<CItemData*>::iterator weapon = m_vecWeapon.begin();
	Add_EquipWeapon((*weapon), 0);
	(*weapon)->Set_isEquip(true);
	dynamic_cast<CUIGroup_Weapon*>(CUI_Manager::GetInstance()->Get_UIGroup("Weapon"))->Update_Slot_EquipSign(true);


	// Skill에 추가
	pDesc.eItemName = CItemData::ITEMNAME_OPH;
	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// test
	pDesc.eItemName = CItemData::ITEMNAME_AKSHA;
	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));


	// UI의 경우에는 Tab 누르면 변환되면서 보여줘야함
	//CUI_Manager::GetInstance()->Update_Skill_Add();

	// Upgrade Page에도 weapon 추가
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	//test
	Add_DropItem(CItem::ITEM_ESSENCE);
	Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_BUFF1);
	Add_DropItem(CItem::ITEM_BUFF2);
	Add_DropItem(CItem::ITEM_BUFF3);
	Add_DropItem(CItem::ITEM_BUFF4);
	Add_DropItem(CItem::ITEM_UPGRADE1);
	Add_DropItem(CItem::ITEM_UPGRADE1);
	Add_DropItem(CItem::ITEM_ESSENCE);
	Add_DropItem(CItem::ITEM_ETHER);
	Add_DropItem(CItem::ITEM_UPGRADE2);

	Add_Item(CItemData::ITEMNAME_CATALYST);
	Add_Item(CItemData::ITEMNAME_CATALYST);
	Add_Item(CItemData::ITEMNAME_CATALYST);

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
		// RedDot 추가
		CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(true); // Menu Btn
		

		if (!Check_Overlab(eItemType)) // 중복 체크
		{
			// Inventory에 ItemData 추가
			CItemData::ITEMDATA_DESC pDesc{};

			pDesc.isDropTem = true;
			pDesc.eDropItemName = eItemType;
			m_vecItem.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));


			CUI_Manager::GetInstance()->Update_Inventory_Add(m_vecItem.size() - 1);

			// Quick의 InvSlot에도 ItemIcon 출력해주어야 함
			CUI_Manager::GetInstance()->Update_Quick_InvSlot_Add(m_vecItem.size() - 1);

			// UI DropItem 출력
			CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
			pUIDesc.eLevel = LEVEL_STATIC;

			vector<CItemData*>::iterator item = m_vecItem.begin();
			for (size_t i = 0; i < m_vecItem.size() - 1; ++i)
				++item;
			pUIDesc.eItemName = (*item)->Get_ItemName();
			pUIDesc.wszTextureName = (*item)->Get_TextureName();
			m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);
		
			// Slot에 RedDot 생성
			CUI_Manager::GetInstance()->Create_RedDot_Slot(true, m_vecItem.size() - 1);
		}
	}

	return S_OK;
}

HRESULT CInventory::Add_Item(CItemData::ITEM_NAME eItemName)
{
	if (eItemName == CItemData::ITEMNAME_CATALYST)
	{
		CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pDropItemDesc{};
		pDropItemDesc.eLevel = LEVEL_STATIC;

		vector<CItemData*>::iterator item = m_vecItem.begin();

		for (size_t i = 0; i < m_vecItem.size(); ++i)
		{
			if (CItemData::ITEMNAME_CATALYST == (*item)->Get_ItemName())
			{
				(*item)->Set_Count(1);

				pDropItemDesc.eItemName = (*item)->Get_ItemName();
				pDropItemDesc.wszTextureName = (*item)->Get_TextureName();
				m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pDropItemDesc);

				return S_OK;
			}
			else
				++item;
		}
	}

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

HRESULT CInventory::Add_Weapon(CItemData::ITEM_NAME eItemName)
{
	// RedDot
	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(false);

	// Weapon인가 Skill인가에 따라 다르게 들어가야 하는데...
	// Slot을 기준으로 하니까 문제가 생기네. ItemIcon을 기준으로 했어야 했나...?
	// ㄴㄴ 할 수 있을 거임 해보자

	// 이 함수에서는 Weapon만 추가하는 거 같은데 Skill은? 함수 없는듯?

	// ItemIcon이 출력하는 걸로 바꾸는 게 맞을 거 같은데............... 현재 추가하는 로직은 크게 바뀌지 않아도 되고
	// Slot에 접근하면 그 Slot이 가지는 게 아니라 그 Slot이 가지고 있는 ItemData가 가지도록 변경을 좀 해줘야 할 거 같음
	// 

	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;
	m_vecWeapon.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	// UI 출력
	CUI_Manager::GetInstance()->Update_Weapon_Add();

	// Upgrade Page에도 weapon 추가
	dynamic_cast<CUIGroup_UpGPage*>(CUI_Manager::GetInstance()->Get_UIGroup("UpGPage"))->Add_WeaponList(m_vecWeapon.size() - 1);

	return S_OK;
}

HRESULT CInventory::Add_Skill(CItemData::ITEM_NAME eItemName)
{
	// RedDot
	CUI_Manager::GetInstance()->Create_RedDot_MenuBtn(false);

	CItemData::ITEMDATA_DESC pDesc{};

	pDesc.isDropTem = false;
	pDesc.eItemName = eItemName;

	m_vecSkill.emplace_back(dynamic_cast<CItemData*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemData"), &pDesc)));

	CUI_Manager::GetInstance()->Update_Skill_Add(); // 음 이거 조심해야겠는데? Tab이 Weapon인 상태인데 거기에다가 갱신해버릴거가틈

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
	CUI_Manager::GetInstance()->Update_EquipWeapon_Add(iEquipSlotIdx); // 여기서 지워지넹

	// HUD
	dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Update_WeaponSlot(pItemData->Get_TextureName(), CUIGroup_WeaponSlot::SLOT_WEAPON);

	return S_OK;
}

HRESULT CInventory::Delete_QuickAccess(_uint iInvenIdx, _uint iQuickIdx, wstring wstrItemName)
{
	// ItemData의 Equp 여부 설정
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

HRESULT CInventory::Delete_Item(CItemData* pItemData)
{
	vector<CItemData*>::iterator item = m_vecItem.begin();
	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (pItemData == (*item))
		{
			// Quick에 장착 중이었다면 그 또한 제거해주어야 함
			if ((*item)->Get_isEquip())
			{
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

	// UI 출력
	CUIGroup_DropItem::UIGROUP_DROPITEM_DESC pUIDesc{};
	pUIDesc.eLevel = LEVEL_STATIC;

	vector<CItemData*>::iterator item = m_vecItem.begin();

	for (size_t i = 0; i < m_vecItem.size(); ++i)
	{
		if (eItemName == (*item)->Get_ItemName()) // 이름이 일치하는 것이 있다면
		{
			// 여기서 해당 ItemData에 중복 처리
			(*item)->Set_Count(1);

			// UI DropItem 출력
			pUIDesc.eItemName = (*item)->Get_ItemName();
			pUIDesc.wszTextureName = (*item)->Get_TextureName();
			m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UIGroup_DropItem"), &pUIDesc);

			// Slot에 RedDot 생성
			CUI_Manager::GetInstance()->Create_RedDot_Slot(true, i);

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

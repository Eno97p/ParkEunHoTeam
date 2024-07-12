#include "UI_Manager.h"

#include "GameInstance.h"
#include "UIGroup_Logo.h"
#include "UIGroup_Loading.h"
#include "UIGroup_State.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Menu.h"
#include "UIGroup_Quick.h"
#include "UIGroup_Character.h"
#include "UIGroup_Inventory.h"
#include "UIGroup_Weapon.h"
#include "UIGroup_InvSub.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	: m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);

//	Initialize();
}

void CUI_Manager::Set_MenuPage(_bool isOpen, string PageKey)
{
	// render true false  조절
	map<string, CUIGroup*>::iterator iter = m_mapUIGroup.find(PageKey);
	(*iter).second->Set_Rend(isOpen);
	(*iter).second->Set_RenderOnAnim(true);
}

_bool CUI_Manager::Get_MenuPageState()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	return dynamic_cast<CUIGroup_Menu*>((*menu).second)->Get_MenuPageState();
}

void CUI_Manager::Set_MenuPageOpen()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	dynamic_cast<CUIGroup_Menu*>((*menu).second)->Set_MenuPageState(true);
}

CUIGroup* CUI_Manager::Get_UIGroup(string strKey)
{
	map<string, CUIGroup*>::iterator group = m_mapUIGroup.find(strKey);
	return (*group).second;
}

void CUI_Manager::Tick(_float fTimeDelta)
{
	for (auto& pGroup : m_mapUIGroup)
		pGroup.second->Tick(fTimeDelta);
}

void CUI_Manager::Late_Tick(_float fTimeDelta)
{
	Key_Input();

	for (auto& pGroup : m_mapUIGroup)
		pGroup.second->Late_Tick(fTimeDelta);
}

void CUI_Manager::Render_UIGroup(_bool isRender, string strKey)
{
	map<string, CUIGroup*>::iterator uigroup = m_mapUIGroup.find(strKey);
	(*uigroup).second->Set_Rend(isRender);
}

void CUI_Manager::Update_Inventory_Add(_uint iSlotIdx)
{
	map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
	// Update_Inventory_Add
	dynamic_cast<CUIGroup_Inventory*>((*inventory).second)->Update_Inventory_Add(iSlotIdx);

}

HRESULT CUI_Manager::Initialize()
{
	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Create_UI()
{
	CUIGroup::UIGROUP_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// Logo
	m_mapUIGroup.emplace("Logo", dynamic_cast<CUIGroup_Logo*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Logo"), &pDesc)));

	// Loading
	m_mapUIGroup.emplace("Loading", dynamic_cast<CUIGroup_Loading*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Loading"), &pDesc)));

	// State
	m_mapUIGroup.emplace("HUD_State", dynamic_cast<CUIGroup_State*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_State"), &pDesc)));

	// WeaponSlot
	m_mapUIGroup.emplace("HUD_WeaponSlot", dynamic_cast<CUIGroup_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_WeaponSlot"), &pDesc)));

	// Menu
	m_mapUIGroup.emplace("Menu", dynamic_cast<CUIGroup_Menu*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Menu"), &pDesc)));

	// Quick 
	m_mapUIGroup.emplace("Quick", dynamic_cast<CUIGroup_Quick*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Quick"), &pDesc)));

	// Character 
	m_mapUIGroup.emplace("Menu_Ch", dynamic_cast<CUIGroup_Character*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Character"), &pDesc)));

	// Inventory
	m_mapUIGroup.emplace("Inventory", dynamic_cast<CUIGroup_Inventory*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Inventory"), &pDesc)));

	// Weapon 
	m_mapUIGroup.emplace("Weapon", dynamic_cast<CUIGroup_Weapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_Weapon"), &pDesc)));

	// Inv Sub 
	m_mapUIGroup.emplace("InvSub", dynamic_cast<CUIGroup_InvSub*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_InvSub"), &pDesc)));

	return S_OK;
}

void CUI_Manager::Key_Input()
{
	// 게임 플레이 레벨에서만 키보드 먹도록 하는 예외 처리 필요

	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	map<string, CUIGroup*>::iterator invsub = m_mapUIGroup.find("InvSub");
	_bool isMenuOpen = (*menu).second->Get_Rend();
	_bool isQuickOpen = (*quick).second->Get_Rend();
	_bool isInvSubOpen = (*invsub).second->Get_Rend();

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		map<string, CUIGroup*>::iterator character = m_mapUIGroup.find("Menu_Ch");
		map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
		map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");

		_bool isChOpen = (*character).second->Get_Rend();
		_bool isInvOpen = (*inventory).second->Get_Rend();
		_bool isWeaponOpen = (*weapon).second->Get_Rend();

		if (isMenuOpen)
		{
			if (!isInvSubOpen)
			{
				if (isChOpen)
				{
					(*character).second->Set_RenderOnAnim(false);
				}
				else if (isInvOpen)
				{
					(*inventory).second->Set_RenderOnAnim(false);
				}
				else if (isWeaponOpen)
				{
					(*weapon).second->Set_RenderOnAnim(false);
				}
				else
				{
					(*menu).second->Set_RenderOnAnim(false);
				}
				dynamic_cast<CUIGroup_Menu*>((*menu).second)->Set_MenuPageState(false);
			}
			else
			{
				(*invsub).second->Set_RenderOnAnim(false);
			}
		}
		else
		{
			if (!isQuickOpen)
			{
				(*menu).second->Set_Rend(true);
				(*menu).second->Set_RenderOnAnim(true);
			}
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_I))
	{
		if (!isMenuOpen)
		{
			(*quick).second->Set_AnimFinished(false);

			if (isQuickOpen) // 퀵슬롯이 켜져 있을 때 > 꺼지게
			{
				(*quick).second->Set_RenderOnAnim(false);
			}
			else // 꺼져있을 때 > 켜지게
			{
				(*quick).second->Set_Rend(true);
				(*quick).second->Set_RenderOnAnim(true);
			}
		}
	}
}

void CUI_Manager::Free()
{
	for (auto& pair : m_mapUIGroup)
	{
		Safe_Release(pair.second);
	}
	m_mapUIGroup.clear();

	Safe_Release(m_pGameInstance);
}

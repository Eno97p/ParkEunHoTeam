#include "UI_Manager.h"

#include "GameInstance.h"
#include "UIGroup_State.h"
#include "UIGroup_WeaponSlot.h"
#include "UIGroup_Menu.h"
#include "UIGroup_Quick.h"
#include "UIGroup_Character.h"
#include "UIGroup_Inventory.h"
#include "UIGroup_Weapon.h"

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

HRESULT CUI_Manager::Initialize()
{
	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::Create_UI()
{
	CUIGroup::UIGROUP_DESC pDesc{};


	//m_mapUIGroup.clear();
	//for(auto& pair : m_mapUIGroup)
	//{
	//	
	//}

	// State
	pDesc.eLevel = LEVEL_STATIC;
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

	return S_OK;
}

void CUI_Manager::Key_Input()
{
	map<string, CUIGroup*>::iterator menu = m_mapUIGroup.find("Menu");
	map<string, CUIGroup*>::iterator quick = m_mapUIGroup.find("Quick");
	_bool isMenuOpen = (*menu).second->Get_Rend();
	_bool isQuickOpen = (*quick).second->Get_Rend();

	if (m_pGameInstance->Key_Down(DIK_ESCAPE)) // 다른 메뉴 실행중에는 켜지면 안 됨. 예외 처리 필요 
	{
		map<string, CUIGroup*>::iterator character = m_mapUIGroup.find("Menu_Ch");
		map<string, CUIGroup*>::iterator inventory = m_mapUIGroup.find("Inventory");
		map<string, CUIGroup*>::iterator weapon = m_mapUIGroup.find("Weapon");

		_bool isChOpen = (*character).second->Get_Rend();
		_bool isInvOpen = (*inventory).second->Get_Rend();
		_bool isWeaponOpen = (*weapon).second->Get_Rend();

		if (isMenuOpen)
		{
			if (isChOpen)
			{
				(*character).second->Set_Rend(false);
			}
			else if (isInvOpen)
			{
				(*inventory).second->Set_Rend(false);
			}
			else if (isWeaponOpen)
			{
				(*weapon).second->Set_Rend(false);
			}
			else
			{
				(*menu).second->Set_Rend(!isMenuOpen);
			}
		}
		else
		{
			if (!isQuickOpen)
			{
				(*menu).second->Set_Rend(!isMenuOpen);
			}
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_I))
	{
		if (!isMenuOpen)
		{
			if (isQuickOpen) // 퀵슬롯이 켜져 있을 때 > 꺼지게
			{
				// 여기서 Render를 바로 false로 만들면 꺼져버리기 때문에 다른 방식으로 접근 필요
				// > 꺼지는 애니메이션 On
				(*quick).second->Set_AnimFinished(false);
				(*quick).second->Set_RenderOnAnim(false);
			}
			else // 꺼져있을 때 > 켜지게
			{
				(*quick).second->Set_Rend(true); // 퀵슬롯의 Render를 true로
			}
			// (*quick).second->Set_Rend(!isQuickOpen);
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

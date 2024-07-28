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
	// ȣ�����峪 Firefly ���� Ư�� �����۵��� ��� �� Inventory ������ �޴����� �� ������ �� �� ����
	// �������� �׳� ������(����Ǵ�) ������


	switch (m_eItemName)
	{
	case Client::CItemData::ITEMNAME_CATALYST:
	{
		//Apply_UseCount(iInvenIdx);
		// ��� 0�� �Ǿ ������� �ʱ�



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
	else // �ϳ� ���� ��� ���� �ʿ�
	{
		// UI Inventory������ ����
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory_Delete(iInvenIdx);

		// UI Inventory�� Sub QuickSlot������ ���� �ʿ���
		dynamic_cast<CUIGroup_InvSub*>(CUI_Manager::GetInstance()->Get_UIGroup("InvSub"))->Delete_InvSub_QuickSlot_ToInvIdx(iInvenIdx);

		// Quick SubInv������ ����
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_InvSlot_Delete(iInvenIdx);

		// Quick�� Slot������ ����
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_QuickSlot_Delete(iInvenIdx);

		// HUD������ ����   
		dynamic_cast<CUIGroup_WeaponSlot*>(CUI_Manager::GetInstance()->Get_UIGroup("HUD_WeaponSlot"))->Reset_SlotTexture(CUIGroup_WeaponSlot::SLOT_QUICK);

		CInventory::GetInstance()->Delete_Item(this);

		// Slot�� ���ܿ��� >> ���⿡ InvenIdx�� �־��ִ� �� ������?		
		dynamic_cast<CUIGroup_Inventory*>(CUI_Manager::GetInstance()->Get_UIGroup("Inventory"))->Update_Inventory(iInvenIdx);
		dynamic_cast<CUIGroup_Quick*>(CUI_Manager::GetInstance()->Get_UIGroup("Quick"))->Update_Inventory(iInvenIdx);
	
		// Inventory�� Quick(map)������ �������־�� �ϴ� �� �ƴ���!

	
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
		m_wszItemExplain = TEXT("�ݼ� ������ ū Ȩ���� ������\n�������� ���α�\n������ ���� �ɷ���\n�Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("�ݼ� ������ ū Ȩ�� ����� �������� ���α�\n������ ���� �ɷ��� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("���� ������ ������\n���� ���α�\n������ ��� �ɷ���\n�Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("���� ������ ������ ���� ���α�\n������ ��� �ɷ��� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("�ϼ��� ������\n������ ���α�\nü�� ȸ���� �Ͻ������� ����");
		m_wszItemExplain_quick = TEXT("�ϼ��� ������ ������ ���α�\nü�� ȸ���� �Ͻ������� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("����� ���� ���� ������ ������\n���������� ���α�\n���׷��� �Ͻ������� ��ȭ");
		m_wszItemExplain_quick = TEXT("����� ���� ���� ������ ������ ���������� ���α�\n���׷��� �Ͻ������� ��ȭ");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_ESSENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ESSENCE;
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Essence");
		m_wszItemName = TEXT("GRACEFUL ESSENCE");
		m_wszItemExplain = TEXT("�������� ������ [�����ϸ� �Ϸ�]\n�������� �޸� �м�");
		m_wszItemExplain_quick = TEXT("�������� ������ [�����ϸ� �Ϸ�]\n�������� �޸� �м�");
		break;
	}
	case Client::CItem::ITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ETHER;
		m_wszItemName = TEXT("RADIANT ETHER");
		m_wszItemExplain = TEXT("�����ϰ� ������ ������ ���� ����\nǳ���� õ�� �������� �߻�");
		m_wszItemExplain_quick = TEXT("�����ϰ� ������ ������ ���� ����\nǳ���� õ�� �������� �߻�");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItem::ITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE1;
		m_wszItemName = TEXT("HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszItemExplain_quick = TEXT("Astyr ��ü ��κ��� �����ϴ� ������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0");
		break;
	}
	case Client::CItem::ITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE2;
		m_wszItemName = TEXT("GRACEFUL HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszItemExplain_quick = TEXT("Astyr ��ü ��κ��� �����ϴ� ������ ����\n���� ���׷��̵� �� ��� ����");
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
		m_wszItemExplain = TEXT("������ ��ȥ�� ������ ������\n����ü\n�� ��ü�� ����ִ� ����ó��\n������� ������ �����Ѵ�.");
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
		m_wszItemExplain = TEXT("��� �ӿ����� ������\n��ȭ�� ��¡\n������ ���׸��� ������\n�ָ����� ���ư���.");
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
		m_wszItemExplain = TEXT("���谡�� �ձ��� �����ִ�\n����� ��¡\n�ƹ��� ����� £�\n���� �׻� ���� ã�Ƴ���.");
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

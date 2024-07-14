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

	Set_DropItem_Data(pDesc->eItemName);

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
		m_wszItemExplain = TEXT("���� 1");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff0");
		break;
	}
	case Client::CItem::ITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF2;
		m_wszItemName = TEXT("SIGIL OF PROTECTION");
		m_wszItemExplain = TEXT("���� 2");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff1");
		break;
	}
	case Client::CItem::ITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF3;
		m_wszItemName = TEXT("SIGIL OF RECOVERY");
		m_wszItemExplain = TEXT("���� 3");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff2");
		break;
	}
	case Client::CItem::ITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_BUFF;
		m_eItemName = ITEMNAME_BUFF4;
		m_wszItemName = TEXT("SIGIL OF ETHER");
		m_wszItemExplain = TEXT("���� 4");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Buff3");
		break;
	}
	case Client::CItem::ITEM_SOUL:
	{
		m_eItemType = ITEMTYPE_ETC; // ���ڸ��� �Ҹ�Ǿ�� �ϹǷ� >> �κ��� ���� �� ����
		m_eItemName = ITEMNAME_SOUL;
		m_wszItemName = TEXT("SOUL");
		m_wszItemExplain = TEXT("�κ��丮�� ������� ���� ������ ������");
		m_wszTexture = TEXT("Prototype_Component_Texture_HUD_StateSoul");
		break;
	}
	case Client::CItem::ITEM_ESSENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ESSENCE;
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Essence");
		m_wszItemName = TEXT("GRACEFUL ESSENCE");
		m_wszItemExplain = TEXT("�������� ������ [�����ϸ� �Ϸ�]\n�������� �޸� �м�");
		break;
	}
	case Client::CItem::ITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		m_eItemName = ITEMNAME_ETHER;
		m_wszItemName = TEXT("RADIANT ETHER");
		m_wszItemExplain = TEXT("�����ϰ� ������ ������ ���� ����\nǳ���� õ�� �������� �߻�");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Ether");
		break;
	}
	case Client::CItem::ITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE1;
		m_wszItemName = TEXT("HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade0");
		break;
	}
	case Client::CItem::ITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		m_eItemName = ITEMNAME_UPGRADE2;
		m_wszItemName = TEXT("GRACEFUL HADRONITE");
		m_wszItemExplain = TEXT("Astyr ��ü ��κ��� �����ϴ�\n������ ����\n���� ���׷��̵� �� ��� ����");
		m_wszTexture = TEXT("Prototype_Component_Texture_Icon_Item_Upgrade1");
		break;
	}
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

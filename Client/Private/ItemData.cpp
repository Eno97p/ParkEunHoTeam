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

	if (pDesc->isDropTem) // ��� ���� ���
	{
		m_eDropItemName = static_cast<DROPITEM_NAME>(rand() % DROPITEM_END); // �������� ���� ����
		Set_DropItem_Type(); 
	}
	else
	{

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

void CItemData::Set_DropItem_Type()
{
	switch (m_eDropItemName)
	{
	case Client::CItemData::DROPITEM_BUFF1:
	{
		m_eItemType = ITEMTYPE_BUFF;
		break;
	}
	case Client::CItemData::DROPITEM_BUFF2:
	{
		m_eItemType = ITEMTYPE_BUFF;
		break;
	}
	case Client::CItemData::DROPITEM_BUFF3:
	{
		m_eItemType = ITEMTYPE_BUFF;
		break;
	}
	case Client::CItemData::DROPITEM_BUFF4:
	{
		m_eItemType = ITEMTYPE_BUFF;
		break;
	}
	case Client::CItemData::DROPITEM_SOUL:
	{
		m_eItemType = ITEMTYPE_ETC; // ���ڸ��� �Ҹ�Ǿ�� �ϹǷ� >> �κ��� ���� �� ����
		break;
	}
	case Client::CItemData::DROPITEM_ESEENCE:
	{
		m_eItemType = ITEMTYPE_USABLE;
		break;
	}
	case Client::CItemData::DROPITEM_ETHER:
	{
		m_eItemType = ITEMTYPE_USABLE;
		break;
	}
	case Client::CItemData::DROPITEM_UPGRADE1:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
		break;
	}
	case Client::CItemData::DROPITEM_UPGRADE2:
	{
		m_eItemType = ITEMTYPE_UPGRADE;
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

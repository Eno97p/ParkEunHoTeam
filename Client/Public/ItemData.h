#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#include "Item.h"

BEGIN(Engine)

END

// Item에 대한 정보를 가지는 가상의 클래스(실체가 X) > Item 습득 시 생성되며 Inv가 가지고 있도록 > 해당 클래스가 가지는 데이터에 따라 Texture 출력 등 상호작용 발생하도록

BEGIN(Client)

class CItemData final : public CGameObject
{
public:
	enum ITEM_TYPE{ ITEMTYPE_WEAPON, ITEMTYPE_BUFF, ITEMTYPE_SKILL, ITEMTYPE_USABLE, ITEMTYPE_UPGRADE, ITEMTYPE_ETC, ITEMTYPE_END };
	enum ITEM_NAME{ 
		ITEMNAME_CATHARSIS, ITEMNAME_CENDRES, ITEMNAME_CORRUPTEDSWORD, ITEMNAME_DURGASWORD, ITEMNAME_ICEBLADE, ITEMNAME_NARUEHSWORD, ITEMNAME_PRETORIANSWORD, ITEMNAME_RADAMANTHESWORD, ITEMNAME_SITRASWORD, ITEMNAME_VALNIRSWORD, ITEMNAME_VEILLEURSWORD, ITEMNAME_WHISPERSWORD,
		ITEMNAME_OPH, ITEMNAME_ETHERBOLT, ITEMNAME_AEGIS,
		ITEMNAME_CATALYST, 
		ITEMNAME_HOVERBOARD, ITEMNAME_FIREFLY, ITEMNAME_WHISPERER,
		ITEMNAME_BUFF1, ITEMNAME_BUFF2, ITEMNAME_BUFF3, ITEMNAME_BUFF4, ITEMNAME_SOUL, ITEMNAME_ESSENCE, ITEMNAME_ETHER, ITEMNAME_UPGRADE1, ITEMNAME_UPGRADE2,
		ITEMNAME_END };

	typedef struct ItemData_Desc : public GAMEOBJECT_DESC 
	{
		_bool					isDropTem; // 드탑템 여부
		CItem::ITEM_NAME		eDropItemName;
		ITEM_NAME				eItemName;
		
	}ITEMDATA_DESC;

private:
	CItemData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemData(const CItemData& rhs);
	virtual ~CItemData() = default;

public:
	wstring			Get_TextureName() { return m_wszTexture; }
	wstring			Get_ItemNameText() { return m_wszItemName; }
	wstring			Get_ItemExplainText() { return m_wszItemExplain; }

	_bool			Get_isEquip() { return m_isEquip; }
	void			Set_isEquip(_bool isEquip) { m_isEquip = isEquip; }

	ITEM_NAME		Get_ItemName() { return m_eItemName; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool					m_isEquip = { false };

	wstring					m_wszTexture = TEXT("");
	wstring					m_wszItemName = TEXT("");
	wstring					m_wszItemExplain = TEXT("");

	ITEM_TYPE				m_eItemType = { ITEMTYPE_END };
	ITEM_NAME				m_eItemName = { ITEMNAME_END };

private:
	void					Set_DropItem_Data(CItem::ITEM_NAME eItemName);
	void					Set_Item_Data();

public:
	static CItemData*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
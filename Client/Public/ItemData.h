#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

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
		ITEMNAME_END }; // 아이템 이름 >>> 안쓸듯

	enum DROPITEM_NAME{ DROPITEM_BUFF1 = 0, DROPITEM_BUFF2, DROPITEM_BUFF3, DROPITEM_BUFF4, DROPITEM_SOUL, DROPITEM_ESEENCE, DROPITEM_ETHER, DROPITEM_UPGRADE1, DROPITEM_UPGRADE2, DROPITEM_END };

	// 드랍되는 것의 경우에만 랜덤으로 종류를 결정해야함
	typedef struct ItemData_Desc : public GAMEOBJECT_DESC 
	{
		_bool			isDropTem; // 드탑템 여부
	}ITEMDATA_DESC;

	typedef struct DropItem_Desc : public ITEMDATA_DESC
	{
		//DROPITEM_NAME	eDropItemName;
	}DROPITEM_DESC;

private:
	CItemData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemData(const CItemData& rhs);
	virtual ~CItemData() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 플레이어에게 상호작용 하는 함수? 종류에 따라 다른 작용하도록 구현 필요
	ITEM_TYPE				m_eItemType = { ITEMTYPE_END };
	ITEM_NAME				m_eItemName = { ITEMNAME_END };
	DROPITEM_NAME			m_eDropItemName = { DROPITEM_END };

private:
	void					Set_DropItem_Type(); // 드랍 아이템의 종류를 결정

public:
	static CItemData*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
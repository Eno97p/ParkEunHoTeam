#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

END

// Item�� ���� ������ ������ ������ Ŭ����(��ü�� X) > Item ���� �� �����Ǹ� Inv�� ������ �ֵ��� > �ش� Ŭ������ ������ �����Ϳ� ���� Texture ��� �� ��ȣ�ۿ� �߻��ϵ���

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
		ITEMNAME_END }; // ������ �̸� >>> �Ⱦ���

	enum DROPITEM_NAME{ DROPITEM_BUFF1 = 0, DROPITEM_BUFF2, DROPITEM_BUFF3, DROPITEM_BUFF4, DROPITEM_SOUL, DROPITEM_ESEENCE, DROPITEM_ETHER, DROPITEM_UPGRADE1, DROPITEM_UPGRADE2, DROPITEM_END };

	// ����Ǵ� ���� ��쿡�� �������� ������ �����ؾ���
	typedef struct ItemData_Desc : public GAMEOBJECT_DESC 
	{
		_bool			isDropTem; // ��ž�� ����
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
	// �÷��̾�� ��ȣ�ۿ� �ϴ� �Լ�? ������ ���� �ٸ� �ۿ��ϵ��� ���� �ʿ�
	ITEM_TYPE				m_eItemType = { ITEMTYPE_END };
	ITEM_NAME				m_eItemName = { ITEMNAME_END };
	DROPITEM_NAME			m_eDropItemName = { DROPITEM_END };

private:
	void					Set_DropItem_Type(); // ��� �������� ������ ����

public:
	static CItemData*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_UpGPage final : public CUIGroup
{
private:
	CUIGroup_UpGPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_UpGPage(const CUIGroup_UpGPage& rhs);
	virtual ~CUIGroup_UpGPage() = default;

public:
	void			Set_CurSlotIdx(_uint iSlotIdx) { m_iCurSlotIdx = iSlotIdx; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Add_WeaponList(_uint iWeaponIdx);

private:
	_uint						m_iCurSlotIdx = { 0 }; // 현재 선택된 Slot의 인덱스
	
	vector<class CUI*>			m_vecUI;
	vector<class CUI*>			m_vecSlot;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();

	void					Update_CurSlot();

public:
	static CUIGroup_UpGPage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;

class CUIGroup_Inventory final : public CUIGroup
{
private:
	CUIGroup_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Inventory(const CUIGroup_Inventory& rhs);
	virtual ~CUIGroup_Inventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Slot();

public:
	static CUIGroup_Inventory*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
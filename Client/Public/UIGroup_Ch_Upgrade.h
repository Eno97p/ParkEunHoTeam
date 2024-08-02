#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_Ch_Upgrade final : public CUIGroup
{
private:
	CUIGroup_Ch_Upgrade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Ch_Upgrade(const CUIGroup_Ch_Upgrade& rhs);
	virtual ~CUIGroup_Ch_Upgrade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CUI*>		m_vecUI;
	vector<class CUI*>		m_vecBtn;

private:
	HRESULT					Create_UI();
	HRESULT					Create_Btn();

public:
	static CUIGroup_Ch_Upgrade*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END
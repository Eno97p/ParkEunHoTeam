#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Shop final : public CUIGroup
{
private:
	CUIGroup_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Shop(const CUIGroup_Shop& rhs);
	virtual ~CUIGroup_Shop() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<class CUI*>			m_vecUI;
	vector<class CUI*>			m_vecSlot;

private:
	HRESULT			Create_UI();
	HRESULT			Create_Slot();

public:
	static CUIGroup_Shop*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
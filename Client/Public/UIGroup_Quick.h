#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_Quick final : public CUIGroup
{
private:
	CUIGroup_Quick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Quick(const CUIGroup_Quick& rhs);
	virtual ~CUIGroup_Quick() = default;

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
	HRESULT					Create_InvSlot();

public:
	static CUIGroup_Quick*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
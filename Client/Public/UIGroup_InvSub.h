#pragma once

#include "UIGroup.h"

BEGIN(Client)
class CUI;

class CUIGroup_InvSub final : public CUIGroup
{
private:
	CUIGroup_InvSub(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_InvSub(const CUIGroup_InvSub& rhs);
	virtual ~CUIGroup_InvSub() = default;

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
	HRESULT					Create_Btn();

public:
	static CUIGroup_InvSub*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
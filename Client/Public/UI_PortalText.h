#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_PortalText final : public CUI
{
private:
	CUI_PortalText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PortalText(const CUI_PortalText& rhs);
	virtual ~CUI_PortalText() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isRend = { false };
	_float			m_fFontX = { 0.f };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	wstring	Setting_Text();

public:
	static CUI_PortalText*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Btn final : public CUI
{
public:
	typedef struct UI_Qte_Btn_Desc : public UI_DESC
	{
		_uint iBtnIndex;
	}UI_QTE_BTN_DESC;
private:
	CUI_QTE_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Btn(const CUI_QTE_Btn& rhs);
	virtual ~CUI_QTE_Btn() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint			m_iBtnNum = { 0 };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_QTE_Btn*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
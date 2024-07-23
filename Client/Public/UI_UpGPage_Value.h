#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_UpGPage_Value final : public CUI
{
public:
	enum VALUE_TYPE { VALUE_SOUL, VALUE_MATERIAL, VALUE_END };
	typedef struct UI_Value_Desc : public UI_DESC
	{
		VALUE_TYPE	eValueType;
	}UI_VALUE_DESC;

private:
	CUI_UpGPage_Value(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPage_Value(const CUI_UpGPage_Value& rhs);
	virtual ~CUI_UpGPage_Value() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	VALUE_TYPE		m_eValueType = { VALUE_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_UpGPage_Value*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
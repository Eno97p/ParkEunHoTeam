#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_ShopSelect final : public CUI
{
public:
	typedef struct UI_ShopSelect_Desc : public UI_DESC
	{
		_uint		iSlotIdx;
	}UI_SHOPSELECT_DESC;

private:
	CUI_ShopSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ShopSelect(const CUI_ShopSelect& rhs);
	virtual ~CUI_ShopSelect() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint					m_iSlotIdx = { 0 };

	// 필요한 변수 : ItemIcon / ItemName / Price / SoulIcon / RemainCnt / RemainIcon
	class CUI_ItemIcon*		m_pItemIcon = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_UI();

public:
	static CUI_ShopSelect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
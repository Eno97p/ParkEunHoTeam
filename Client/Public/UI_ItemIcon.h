#pragma once

#include "UI.h"
#include "ItemData.h"

BEGIN(Client)

class CUI_ItemIcon final : public CUI
{
public:
	typedef struct UI_ItemIcon_Desc : public UI_DESC
	{
		UISORT_PRIORITY	eUISort;
		wstring			wszTexture;
	}UI_ITEMICON_DESC;

private:
	CUI_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemIcon(const CUI_ItemIcon& rhs);
	virtual ~CUI_ItemIcon() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	UISORT_PRIORITY	m_eUISort = { SORT_END };

	wstring			m_wszTexture;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_ItemIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};
END

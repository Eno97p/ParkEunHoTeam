#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateSoul final : public CUI
{
public:
	typedef struct UI_Sous_Desc : public UI_DESC
	{
		UISORT_PRIORITY			eUISort;
	}UI_SOUL_DESC;
private:
	CUI_StateSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateSoul(const CUI_StateSoul& rhs);
	virtual ~CUI_StateSoul() = default;

public:
	void			Set_isSoulCntRend(_bool isRend) { m_isSoulCntRend = isRend; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Rend_Calcul(_int iSoul);

private:
	_bool						m_isCalculRend = { false };
	_bool						m_isSoulCntRend = { false };
	_float						m_fCalculTimer = { 0.f };

	wstring						m_wstrCalculText;

	UISORT_PRIORITY				m_eUISort = { SORT_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_StateSoul*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
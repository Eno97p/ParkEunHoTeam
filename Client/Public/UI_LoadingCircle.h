#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LoadingCircle final : public CUI
{
public:
	enum CIRCLE_TYPE { CIRCLE_ONE = 0, CIRCLE_TWO, CIRCLE_END };
	typedef struct UI_Circle_Desc : public UI_DESC
	{
		CIRCLE_TYPE		eCircleType;
	}UI_CIRCLE_DESC;

private:
	CUI_LoadingCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LoadingCircle(const CUI_LoadingCircle& rhs);
	virtual ~CUI_LoadingCircle() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_float			m_fRotationAngle = { 0.f };
	_matrix			m_RotationMatrix;

	CIRCLE_TYPE		m_eCircleType = { CIRCLE_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Turn_Animation(_float fTimeDelta);

public:
	static CUI_LoadingCircle*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END
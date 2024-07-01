#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
public:
	typedef struct UI_Desc : public GAMEOBJECT_DESC
	{
		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
		LEVEL	eLevel;
	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	LEVEL				m_eLevel = { LEVEL_END };

protected:
	void		Setting_Position();

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END
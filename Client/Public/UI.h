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
	_bool			Get_RenderOnAnim() { return m_isRenderOffAnim; }
	void			Set_RenderOnAnim(_bool isRenderOffAnim) { m_isRenderOffAnim = isRenderOffAnim; }

	_bool			Get_RenderAnimFinished() { return m_isRenderAnimFinished; }
	void			Set_RenderAnimFinised(_bool isRenderAnimFinished) { m_isRenderAnimFinished = isRenderAnimFinished; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Render_Animation(_float fTimeDelta);
	void			Resset_Animation();
	_bool			isRender_End();

protected:
	_bool				m_isRenderOffAnim = { false }; // ������ �ִϸ��̼����� ������ �ִϸ��̼�����
	_bool				m_isRenderAnimFinished = { false }; // �ִϸ��̼� ���� ����
	_float				m_fRenderTimer = { 0.f };

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
#pragma once

#include "Effect_Define.h"
#include "BlendObject.h"
#include "VIBuffer_SwordTrail.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Effect)

class CSTrail final : public CBlendObject
{
public:
	typedef struct STRAIL_DESC
	{
		CVIBuffer_SwordTrail::SwordTrailDesc			traildesc;
		wstring								Texture = TEXT("");
		wstring								TexturePath = TEXT("");
	};

private:
	CSTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSTrail(const CSTrail& rhs);
	virtual ~CSTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };	
	CVIBuffer_SwordTrail*			m_pVIBufferCom = { nullptr };
	shared_ptr<STRAIL_DESC>			m_pTrailDesc = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
public:
	static CSTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
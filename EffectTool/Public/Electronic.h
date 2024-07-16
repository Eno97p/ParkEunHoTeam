#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
#include "VIBuffer_Lightning.h"
BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Effect)
class CElectronic final : public CBlendObject
{
public:
	typedef struct ELECTRONICDESC
	{
		CVIBuffer_Lightning::LIGHTNINGDESC		 BufferDesc;
		_float4									 vStartPos = { 0.f,0.f,0.f,1.f };
		_int									 iNumNoise = 0;
		_float3									 vColor = { 0.f,0.f,0.f };
		wstring									 Texture = TEXT("");
		wstring									 TexturePath = TEXT("");
	};

private:
	CElectronic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectronic(const CElectronic& rhs);
	virtual ~CElectronic() = default;
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	virtual HRESULT Render_Blur() override;



private:
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pNoiseTex = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Lightning* m_pVIBufferCom = { nullptr };
	shared_ptr<ELECTRONICDESC>	OwnDesc;
	_float						fTime = 0.f;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_BlurResources();

public:
	static CElectronic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
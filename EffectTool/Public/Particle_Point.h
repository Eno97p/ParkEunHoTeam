#pragma once

#include "Effect_Define.h"
#include "Particle.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Instance_Point;
END

BEGIN(Effect)

class CParticle_Point final : public CParticle
{
public:
	typedef struct PARTICLEPOINT : public CParticle::PARTICLEDESC
	{
		wstring									 Texture;
		wstring									 TexturePath;
		_bool									 isColored = false;	//true 면 색깔 입히기, false 면 원래 텍스쳐 색깔로
	};
private:
	CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Point(const CParticle_Point& rhs);
	virtual ~CParticle_Point() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Instance_Point*		m_pVIBufferCom = { nullptr };	
	_bool m_IsColored = false;

private:
	HRESULT Add_Components(const wstring& Texcom);
	HRESULT Bind_ShaderResources();
public:
	static CParticle_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Particle_Trail.h"

BEGIN(Client)
class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)
private:
	CEffectManager();
	virtual ~CEffectManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Generate_Trail(const _int iIndex, const _float4x4* BindMat);

private:
	HRESULT Load_Trails();
	HRESULT Ready_GameObjects();
	HRESULT	Add_Texture_Prototype(const wstring& path, const wstring& name);
	void Dynamic_Deallocation();

private:
	vector<pair<PARTICLETYPE, void*>>		m_Particles;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> Trailes;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};
END

#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Particle_Trail.h"
#include "Particle_STrail.h"

BEGIN(Client)
class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)
private:
	CEffectManager();
	virtual ~CEffectManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Generate_Trail(const _int iIndex, const _float4x4* BindMat);	//1. 인덱스, 2.대상의 행렬
	HRESULT Generate_SwordTrail(const _int iIndex, const _float4x4* Swordmat);  //1. 인덱스, 2. 칼 행렬
	HRESULT Generate_Particle(const _int iIndex, const _float4 vStartpos,  //1. 인덱스, 2. 위치, 3. 타겟, 4. 회전축, 5.회전각도
		CGameObject* pTarget = nullptr,
		const _vector vAxis = XMVectorZero(),
		const _float fRadians = 0.f,
		const _vector vLook = XMVectorZero());
private:
	HRESULT Load_Trails();
	HRESULT Load_SwordTrails();
	HRESULT Load_Particles();

	HRESULT Ready_GameObjects();
	HRESULT	Add_Texture_Prototype(const wstring& path, const wstring& name);
	void Dynamic_Deallocation();

private:
	vector<pair<PARTICLETYPE, void*>>		m_Particles;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> Trailes;
	vector<shared_ptr<CSTrail::STRAIL_DESC>> m_pSwordTrailes;
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
public:
	virtual void Free() override;
};
END

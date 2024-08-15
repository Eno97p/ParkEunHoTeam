#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CPhysXParticle : public CVIBuffer
{
public:
	typedef struct PhysX_Particle_Desc
	{
		_uint			iNumInstance = 0;
		_float3			Range = { 0.f,0.f,0.f };
		_float3			Pivot = { 0.f,0.f,0.f };
		_float3			Offset = { 0.f,0.f,0.f };
		_float2			Size = { 0.f,0.f };
		_float2			Velocity = { 0.f,0.f };
		_float2			LifeTime = { 0.f,0.f };
		_float			Energy = { 0.01f };
		_float			AirDrag = 0.f;
		_float			Threshold = 300.f;
		_float			BubbleDrag = 0.9f;
		_float			Buoyancy = 0.9f;
		_float			PressureWeight = 1.0f;
		_float			DivergenceWeight = 1.0f;
	};

protected:
	CPhysXParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXParticle(const CPhysXParticle& rhs);
	virtual ~CPhysXParticle() = default;

public:
	virtual HRESULT Initialize_Prototype(class CMesh* m_Meshes, const PhysX_Particle_Desc& InstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
	void	Tick(_float fTimeDelta);


private:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	_uint						m_iMaterialIndex = { 0 };
	unique_ptr<PhysX_Particle_Desc>				m_Owndesc{};
	_float* m_Size = nullptr;
private:
	PxScene*					g_Scene = nullptr;
	PxCudaContextManager*		g_CudaContext = nullptr;
	PxPhysics*					g_PhysXs = nullptr;
	PxPBDParticleSystem*		g_ParticleSystem = nullptr;
	PxParticleAndDiffuseBuffer* g_ParticleBuffer = nullptr;

private:
	HRESULT Compute_RandomNumbers();
	HRESULT Init_Mesh_InstanceBuffer(class CMesh* m_Meshes);
	HRESULT Init_Particle_System(VTXPARTICLE* Buffer);
	void PxDmaDataToDevice(PxCudaContextManager* cudaContextManager, PxParticleBuffer* particleBuffer, const PxParticleBufferDesc& desc);
	PxParticleAndDiffuseBuffer* PxCreateAndPopulateParticleAndDiffuseBuffer(const PxParticleAndDiffuseBufferDesc& desc, PxCudaContextManager* cudaContextManager);
protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;


public:
	static CPhysXParticle* Create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext,
		class CMesh* m_Meshes,
		const PhysX_Particle_Desc& InstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END
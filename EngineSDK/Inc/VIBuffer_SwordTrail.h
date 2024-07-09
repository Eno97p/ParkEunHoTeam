#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_SwordTrail final : public CVIBuffer
{
public:
	typedef struct SwordTrailDesc
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float3			vSize;
		_float			fLifeTime;
		const _float4x4* ParentMat;
	};
private:
	CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs);
	virtual ~CVIBuffer_SwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	void Tick(_float fDelta);

private:
	XMVECTOR CatmullRom(const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3, float t);

private:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

private:
	deque<SwordTrailVertex>		m_pTrailVertex;

	shared_ptr<SwordTrailDesc>	m_pDesc;
public:
	static CVIBuffer_SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
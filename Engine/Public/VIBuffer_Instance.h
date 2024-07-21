#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct INSTANCE_DESC
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float3			vPivotPos;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_float2			vGravity;
		_bool			isLoop;

		//For Map Elements
		//vector<_float4x4*>		WorldMats;
	}INSTANCE_DESC;

	typedef struct INSTANCE_MAP_DESC
	{
		_uint			iNumInstance = { 0 };
		_float3			vOffsetPos;
		_float3			vPivotPos;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;
		_float2			vLifeTime;
		_float2			vGravity;
		_bool			isLoop;

		//For Map Elements
		vector<_float4x4*>		WorldMats;
	}INSTANCE_MAP_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC& InstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;
	INSTANCE_DESC* Get_Instance_Desc() { return &m_InstanceDesc; }



public:
	virtual void Spread(_float fTimeDelta);	//������ �������� �ڵ� ( �߷�, ȸ�� �߰� )
	virtual void Drop(_float fTimeDelta);
	virtual void GrowOut(_float fTimeDelta); //���ǵ� ���� ���� ���� Ŀ���� �Լ�
	virtual void Spread_Size_Up(_float fTimeDelta);	//���ǵ忡 ����ؼ� ����� Ŀ��
	virtual void Spread_Non_Rotation(_float fTimeDelta);	//ȸ�� ���� ��������
	virtual void CreateSwirlEffect(_float fTimeDelta);	//ȸ���� ����Ʈ y�� �����Ҽ��� �ݰ��� Ŀ��
	virtual void Spread_Speed_Down(_float fTimeDelta);	//���������ε� ���ǵ尡 ���� ������ �߷°��� ���
	virtual void SlashEffect(_float fTimeDelta);
	virtual void Spread_Speed_Down_SizeUp(_float fTimeDelta);
	virtual void Gather(_float fTimeDelta);
	virtual void Extinction(_float fTimeDelta);
	virtual void GrowOutY(_float fTimeDelta);
	virtual void GrowOut_Speed_Down(_float fTimeDelta);
	virtual void GrowOut_Speed_Down_Texture(_float fTimeDelta);

	virtual void Leaf_Fall(_float fTimeDelta);
	

	void Initial_RotateY();

	void Excute_Trail(_float fTimeDelta);


	_bool Check_Instance_Dead() { return m_bInstanceDead; }	//IsLoop�� false�� ��� ������ �������� Ȯ�����ִ� �Լ�

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	_float* m_pSpeeds = { nullptr };
	_float* m_pOriginalSpeed = { nullptr };
	_float3* m_pOriginalPositions = { nullptr };
	_float* m_pOriginalGravity = { nullptr };
	_float* m_pSize = { nullptr };
	_float* m_pOriginalSize = { nullptr };


	_bool						m_bInstanceDead = false;
	INSTANCE_DESC				m_InstanceDesc = {};
	INSTANCE_MAP_DESC				m_InstanceMapDesc = {};


protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
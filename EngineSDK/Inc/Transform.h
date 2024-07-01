#pragma once

#include "Component.h"

/* 객체의 월드 변환 행렬을 보관한다. */
/* 월드 변환 상태를 제어하기위한 기능. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct TRANSFORM_DESC
	{
		_float4x4 mWorldMatrix = 
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		_float		fSpeedPerSec;
		_float		fRotationPerSec;

	}TRANSFORM_DESC;

	typedef struct TransformDesc : CComponent::ComponentDesc
	{
		//수정 가능하게 할 항목만 넣기
		_float4x4* pWorldMatrix = nullptr;


	}TransformDesc;



private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrixInverse() { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }

	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() {
		return _float3(XMVector3Length(Get_State(STATE_RIGHT)).m128_f32[0], 
			XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
			XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]);
	}

	const _float4x4* Get_WorldFloat4x4() {
		return &m_WorldMatrix;
	}
	_float4x4* Get_WorldFloat4x4Ref() {
		return &m_WorldMatrix;
	}
	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_WorldMatrix(const _matrix matWorld) { XMStoreFloat4x4(&m_WorldMatrix, matWorld); }

public:
	void Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void* GetData() override { return &m_OutDesc; }

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);
	HRESULT Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	HRESULT Go_Backward(_float fTimeDelta);
	HRESULT Go_Left(_float fTimeDelta);
	HRESULT Go_Right(_float fTimeDelta);
	HRESULT LookAt(_fvector vTargetPosition);
	HRESULT LookAt_For_LandObject(_fvector vTargetPosition);
	HRESULT Turn(_fvector vAxis, _float fTimeDelta);
	HRESULT Rotation(_fvector vAxis, _float fRadian);
	void TurnToTarget(_float fTimeDelta, _fvector vTargetPosition);
	void Speed_Scaling(_float fRatio) { m_fSpeedPerSec *= fRatio; }
	void Set_Speed(_float fSpeed) { m_fSpeedPerSec = fSpeed; }

private:
	//void Update_WorldMatrix();


private:
	/* row major */
	_float4x4				m_WorldMatrix;

	_float4x4               m_QuternionMatrix;
	_float					m_fSpeedPerSec = { 0.0f };
	_float					m_fRotationPerSec = { 0.0f };

	TransformDesc			m_OutDesc;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
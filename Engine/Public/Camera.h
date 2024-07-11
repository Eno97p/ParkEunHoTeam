#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	struct CameraKeyFrame
	{
		_float fTime;
		_float4x4 matWorld;
		_float fFovy;
		_float fNear;
		_float fFar;
		vector<tuple<_float, _float, _float>> speedChanges; // 상대적인 시간 (0.0 ~ 1.0), 해당 시점에서의 속도 배수, 보간 시간
	};

	typedef struct CAMERA_DESC : CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;
	}CAMERA_DESC;
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Activate() { m_bCamActivated = true; }
	void Inactivate() { m_bCamActivated = false; }

	_vector Get_CamLook();
	_bool Get_Activate() { return m_bCamActivated; }

	void Set_CamDesc(CAMERA_DESC* desc)
	{
		m_fFovy = desc->fFovy;
		m_fAspect = desc->fAspect;
		m_fNear = desc->fNear;
		m_fFar = desc->fFar;
	}

	void Set_Fovy(_float fFov) { m_fFovy = fFov; }

protected:
	_float				m_fFovy = { 0.f };
	_float				m_fAspect = { 0.f };
	_float				m_fNear = { 0.f };
	_float				m_fFar = { 0.f };


protected:
	_bool				m_bCamActivated = { true };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
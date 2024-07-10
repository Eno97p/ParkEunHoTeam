#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "GameObject.h"

BEGIN(Client)

class CThirdPersonCamera final : public CCamera
{
public:
	enum class CAMERA_PHASE { CAMERA_FOLLOW = 0, CAMERA_REVOLUTION, CAMERA_RETURN, CAMERA_ZOOMIN, CAMERA_END };

public:
	typedef struct THIRDPERSONCAMERA_DESC : public CCamera::CAMERA_DESC
	{
		_float		fSensor = { 0.f };
		CTransform* pPlayerTrans = { nullptr };
	}THIRDPERSONCAMERA_DESC;
private:
	CThirdPersonCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThirdPersonCamera(const CThirdPersonCamera& rhs);
	virtual ~CThirdPersonCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	//
	void Get_LockOnCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition);
	void Get_ThirdCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition);
	void Update_LockOnCam(_float fTimeDelta);
	void Update_ThirdCam(_float fTimeDelta);
	void Update_TransitionCam(_float fTimeDelta);




public:
	void RotateCamera(float fDeltaX, float fDeltaY);
	void Mouse_Move(_float fTimeDelta);

	//�����
	void ZoomIn(_float fTimeDelta);
	void Revolution360(_float fTime);
	void TiltAdjust(_float fAngle);
	void Set_Camera_Phase(CAMERA_PHASE ePhase)
	{
		m_eCameraPhase = ePhase;
		if (ePhase == CAMERA_PHASE::CAMERA_FOLLOW)
		{
			m_bFirstMove = true;
			m_fHeight = 0.f;
			m_fZoomDistance = 0.f;
		}

	}
	void Set_Revolution_Time(float fTime)
	{
		m_fRevolutionTime = fTime;
	}


public:
	void Set_Height(float fHeight) { m_fHeight = fHeight; }
	void Set_ZoomSpeed(float fZoomSpeed) { m_fZoomSpeed = fZoomSpeed; }
	void Set_MaxZoomDistance(float fMaxZoomDistance) { m_fMaxZoomDistance = fMaxZoomDistance; }

//���� ���
public:
	void TargetLock_On(_vector vTargetPos);
	void TargetLock_Off();
	void TargetLockView(_float fTimeDelta);

	_float DistancePointLine(_vector point, _vector lineStart, _vector lineEnd);
	_vector ProjectPointLine(_vector point, _vector lineStart, _vector lineEnd);

private:
	_float4 m_vLockedTargetPos = { 0.f, 10.f, 0.f, 1.f };
	_bool m_bIsTargetLocked = false;
	_float m_fLerpFactor = 8.0f;  // ���� �ӵ� ���� ����
	float m_fFollowSpeed = 10.0f; // ���ο� �ȷο� �ӵ� ���� (�⺻�� 10.0f)


	_float4 m_vLastLockedPosition;
	_float m_fTransitionProgress = 0.0f;
	const _float m_fTransitionDuration = 1.0f; // ��ȯ�� �ɸ��� �ð� (��)

private:
	_float		m_fSensor = { 0.0f };
	CTransform* m_pPlayerTrans = { nullptr };
private:
	const _vector m_HeightOffset = { 0.f, 10.f, 0.f, 0.f };
	_bool		m_bActivatePick = false;
	_vector		m_vLerpPos = { 0.f, };
	class CGameObject* m_pTarget = nullptr;			//Ÿ��


	_float4 m_vCameraPosition;
	_float4 m_vLookAtPosition;


private:
	float m_fYaw = 0.0f;  // ī�޶��� ���� ȸ�� ����
	float m_fPitch = 0.0f;  // ī�޶��� ���� ����
	float m_fDistance = 4.5f;  // ī�޶�� �÷��̾� ������ �Ÿ�
	float m_fMinPitch = -60.f;  // �ּ� ���� ����
	float m_fMaxPitch = 60.f;  // �ִ� ���� ����
	float m_fMinDistance = 3.0f;  // �ּ� �Ÿ�
	float m_fMaxDistance = 6.5f;  // �ִ� �Ÿ� (8.5���� 6.5�� ����)
	float m_fHeightOffset = 1.8f;  // ī�޶� ���� ������
	float m_fSensorX = 0.1f;  // ���� ���콺 ����
	float m_fSensorY = 0.05f;  // ���� ���콺 ���� (�������� ����)

	//�ڿ������� ���� ��ȯ for ����
	float m_fLockOnTransitionSpeed = 3.0f; // LockOn ��ȯ �ӵ� (�⺻�� 3.0f)
	bool m_bIsFirstUpdate = true;

	bool m_bIsTransitioning = false;
	float m_fTransitionSpeed = 2.0f;  // ��ȯ �ӵ� ���� factor
	float m_fTransitionThreshold = 0.1f;  // ��ȯ �Ϸ� �Ǵ� �Ӱ谪
	_float4 m_vTransitionEndPos;
	_float4 m_vTransitionEndLookAt;

private:
	_float4 vEye;
	_float4 vDir;
	_float	fSpeed = 0.f;
	_float4 vAt;
	_float  fDistance = 20.f;
	_float	m_fHeight = 0.f;
	_float	m_fAngle = 0.f;
	_bool m_bIsMove = false;

private:
	_vector LerpAt;
	_bool IsNear = false;

private:
	bool m_bIsShaking = false;
	float m_fShakeDuration = 0.5f;  // ����ŷ ���� �ð� (��)
	float m_fShakeTimer = 0.0f;
	float m_fIntensity = 0.1f;  // ����ŷ ����
	bool m_bLockWidth = false;  // ���� ���� ����ŷ ��� ����
	float m_fShakeInterval = 0.05f;  // ����ŷ ���� ���� (��)
	float m_fShakeIntervalTimer = 0.0f;
	_float4 m_vShakeStart = { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vShakeTarget = { 0.f, 0.f, 0.f, 1.f };

private:
	_float m_fZoomDistance = 0.f;
	_float m_fRevolutionTime = 3.f;
	_float m_fRevolutionAccTime = 0.f;
	_float m_fRollAngle = 30.f;
	_float m_fTargetHeight = 6.f;
	_float m_fZoomSpeed = 5.f;
	_float m_fHeightChangeSpeed = 2.f;
	_float m_fZoomAccTime = 0.f;
	_float m_fMaxZoomDistance = 10.f;
	_bool m_bZoomIn = false;
	_bool m_bRevolution360 = false;
	_bool m_bTiltAdjust = false;
	_bool m_bHeightChange = false;
	_bool m_bFirstMove = true;

	_float m_fZoomDuration = 3.f;
	CAMERA_PHASE m_eCameraPhase = CAMERA_PHASE::CAMERA_FOLLOW;

	_float m_fZoomDelay = 0.f;
	_bool m_bZoomNow = false;

public:
	static CThirdPersonCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	void Shaking();
	void Key_Input(_float fTimeDelta);
};

END
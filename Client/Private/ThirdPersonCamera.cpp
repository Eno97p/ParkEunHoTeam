#include "..\Public\ThirdPersonCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

CThirdPersonCamera::CThirdPersonCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CThirdPersonCamera::CThirdPersonCamera(const CThirdPersonCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CThirdPersonCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThirdPersonCamera::Initialize(void* pArg)
{
    THIRDPERSONCAMERA_DESC* pDesc = (THIRDPERSONCAMERA_DESC*)pArg;
    m_fSensor = pDesc->fSensor;
    m_pPlayerTrans = pDesc->pPlayerTrans;
    Safe_AddRef(m_pPlayerTrans);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //__super::Activate();

    // 초기 카메라 위치 설정
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vInitialCameraPosition, vInitialLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vInitialCameraPosition, &vInitialLookAtPosition);

    m_vCameraPosition = vInitialCameraPosition;
    m_vLookAtPosition = vInitialLookAtPosition;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    return S_OK;
}


void CThirdPersonCamera::Priority_Tick(_float fTimeDelta)
{
    if (m_pGameInstance->Mouse_Down(DIM_MB))
    {
        if (!m_bIsTargetLocked)
        {
            _vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
            _vector vCamLookNormalized = XMVector3Normalize(vCamLook);

            // 카메라 레이의 끝점 (예: 카메라 앞 100 유닛)
            _vector vRayEnd = XMVectorAdd(vCamPos, XMVectorScale(vCamLookNormalized, 100.0f));

            list<CGameObject*> monsters = (m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Monster")));

            float closestDistance = FLT_MAX;
            CTransform* closestMonsterTransform = nullptr;

            for (auto& monster : monsters)
            {
                CTransform* pMonsterTransform = dynamic_cast<CTransform*>(monster->Get_Component(TEXT("Com_Transform")));
                if (pMonsterTransform == nullptr)
                    continue;

                _vector vMonsterPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);

                // 몬스터가 화면 내에 있는지 확인
                if (m_pGameInstance->isIn_WorldFrustum(vMonsterPos, 5.f))
                {
                    // 카메라 레이에 대한 몬스터의 투영점 계산
                    _vector projectedPoint = ProjectPointLine(vMonsterPos, vCamPos, vRayEnd);

                    // 투영점과 몬스터 사이의 거리 계산
                    float distToRay = XMVectorGetX(XMVector3Length(vMonsterPos - projectedPoint));

                    // 카메라와 투영점 사이의 거리 계산 (깊이)
                    float depthOnRay = XMVectorGetX(XMVector3Length(projectedPoint - vCamPos));

                    // 거리에 가중치를 주어 깊이도 고려 (예: 깊이에 0.5 가중치)
                    float weightedDistance = distToRay + depthOnRay * 0.5f;

                    if (weightedDistance < closestDistance)
                    {
                        closestDistance = weightedDistance;
                        closestMonsterTransform = pMonsterTransform;
                    }
                }
            }

            // 가장 가까운 몬스터까지의 거리가 10 이하인 경우에만 타겟 설정
            if (closestMonsterTransform != nullptr && closestDistance <= 40.0f)
            {
                // 이전 타겟이 있다면 레퍼런스 카운트 감소
                if (m_pTargetTrans != nullptr)
                    Safe_Release(m_pTargetTrans);

                // 새로운 타겟의 Transform 컴포넌트 설정 및 레퍼런스 카운트 증가
                m_pTargetTrans = closestMonsterTransform;
                Safe_AddRef(m_pTargetTrans);

                // 타겟 락온
                _vector vTargetPos = m_pTargetTrans->Get_State(CTransform::STATE_POSITION);
                TargetLock_On(vTargetPos);
            }
            else
            {
                // 가장 가까운 몬스터가 없거나 거리가 10보다 큰 경우
                if (m_pTargetTrans != nullptr)
                {
                    Safe_Release(m_pTargetTrans);
                    m_pTargetTrans = nullptr;
                }
                TargetLock_Off();
            }
        }
        else
        {
            // 이미 타겟이 락온된 상태에서 다시 클릭한 경우
            if (m_pTargetTrans != nullptr)
            {
                Safe_Release(m_pTargetTrans);
                m_pTargetTrans = nullptr;
            }
            TargetLock_Off();
        }
    }

    //if (m_pGameInstance->Mouse_Down(DIM_RB))
    if (m_pGameInstance->Key_Down(DIK_M))
    {
        Shake_Camera(false, 0.5f);
        m_bIsShaking = true;
        m_fShakeTimer = 0.f;
        m_fShakeIntervalTimer = 0.f;
        m_fShakeInterval = m_fInitialShakeInterval;
        m_fShakeAmount = m_fInitialShakeAmount; // 초기 셰이크 강도로 리셋
        XMStoreFloat4(&m_qShakeRotation, XMQuaternionIdentity());
    }

}

void CThirdPersonCamera::Tick(_float fTimeDelta)
{
    if (m_pPlayerTrans == nullptr)
        return;
    if (m_bIsFirstUpdate)
    {
        m_bIsFirstUpdate = false;
        return;
    }

    // 줌인 상태 처리
    if (m_bZoomIn)
    {
        ParryingZoomIn(fTimeDelta);
    }
    // 줌아웃 상태 처리
    else if (m_bZoomOut)
    {
        ParryingZoomOut(fTimeDelta);
    }
    // 타겟 락온 상태 처리
    else if (m_bIsTargetLocked)
    {
        if (m_pTargetTrans != nullptr)
        {
           _vector vTP = m_pTargetTrans->Get_State(CTransform::STATE_POSITION);
           if (40.f < XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vCameraPosition) - vTP)))
           {
               Safe_Release(m_pTargetTrans);
               m_pTargetTrans = nullptr;
               TargetLock_Off();
           }
           else
           {
               XMStoreFloat4(&m_vLockedTargetPos, vTP);
           }
        }
        TargetLockView(fTimeDelta);
    }
    // 일반 상태 처리
    else
    {
        Update_ThirdCam(fTimeDelta);
    }
   
  

    if (m_bIsShaking)
    {
        m_fShakeTimer += fTimeDelta;
        m_fShakeIntervalTimer += fTimeDelta;

        // 감쇠 효과 (원래 셰이크 강도의 일정 비율 이하로 떨어지지 않도록 함)
        m_fShakeAmount = max(m_fShakeAmount * (1.0f - fTimeDelta * 2.0f), m_fInitialShakeAmount * 0.1f);

        // 주파수 변조
        float frequency = 1.0f + sin(m_fShakeTimer * 5.0f) * 0.5f;

        m_fShakeInterval = min(0.4f, m_fInitialShakeInterval + m_fShakeTimer * 0.05f);

        if (m_fShakeIntervalTimer >= m_fShakeInterval)
        {
            m_fShakeIntervalTimer = 0.f;
            m_vShakeTargetPosition = m_vCameraPosition;

            // 3축 셰이크
            float noiseX = PerlinNoise(m_fShakeTimer * m_fShakeSpeed * frequency, 0, 10, 0.5f);
            float noiseY = PerlinNoise(0, m_fShakeTimer * m_fShakeSpeed * frequency, 10, 0.8f);
            float noiseZ = PerlinNoise(m_fShakeTimer * m_fShakeSpeed * frequency, m_fShakeTimer * m_fShakeSpeed * frequency, 10, 0.5f);

            XMVECTOR shake = XMVectorSet(noiseX, noiseY, noiseZ, 0) * m_fShakeAmount;
            XMStoreFloat4(&m_vShakeTargetPosition, XMLoadFloat4(&m_vShakeTargetPosition) + shake);

            // 회전 셰이크
            XMVECTOR rotationShake = XMQuaternionRotationRollPitchYaw(noiseX * 0.1f, noiseY * 0.1f, noiseZ * 0.1f);
            XMStoreFloat4(&m_qShakeRotation, XMLoadFloat4(&m_qShakeRotation) * rotationShake);
        }

        // 부드러운 전환
        float t = m_fShakeIntervalTimer / m_fShakeInterval;
        t = t * t * (3.0f - 2.0f * t); // Smoothstep
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vShakeTargetPosition), t));

        // Look 벡터 계산 및 회전 셰이크 적용
        XMVECTOR lookVector = XMVector3Normalize(XMLoadFloat4(&m_vLookAtPosition) - XMLoadFloat4(&m_vCameraPosition));
        lookVector = XMVector3Rotate(lookVector, XMLoadFloat4(&m_qShakeRotation));
        XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&m_vCameraPosition) + lookVector);

        if (m_fShakeTimer > m_fShakeDuration)
        {
            m_bIsShaking = false;
            m_fShakeTimer = 0.f;
            m_fShakeIntervalTimer = 0.f;
            m_fShakeInterval = m_fInitialShakeInterval;
            m_fShakeAmount = m_fInitialShakeAmount;
            XMStoreFloat4(&m_qShakeRotation, XMQuaternionIdentity());
        }
    }

  

    // 카메라 위치 설정
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));
    __super::Tick(fTimeDelta);
}

void CThirdPersonCamera::Get_LockOnCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    // 플레이어와 타겟 사이의 방향 벡터 계산
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // 플레이어와 타겟 사이의 거리 계산
    float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // 카메라 거리 조정 (플레이어-타겟 거리에 따라)
    float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.5f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // 플레이어 뒤쪽 방향 계산
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // 카메라 위치 계산 (플레이어 뒤쪽)
    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);
    pOutCameraPosition->y += m_fHeightOffset * 1.7f;

    // LookAt 위치 계산 (플레이어와 타겟의 중간점)
    if (m_bZoomIn)
    {
        XMStoreFloat4(pOutLookAtPosition, XMLoadFloat4(&m_vLockedTargetPos));
    }
    else
    {
        XMStoreFloat4(pOutLookAtPosition, (XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&m_vLockedTargetPos)) * 0.5f);
    }
}

void CThirdPersonCamera::Get_ThirdCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    _float4 vCameraOffset;
    XMStoreFloat4(&vCameraOffset, XMVectorSet(
        -m_fDistance * sinf(XMConvertToRadians(m_fYaw)) * cosf(XMConvertToRadians(m_fPitch)),
        m_fDistance * sinf(XMConvertToRadians(m_fPitch)) + m_fHeightOffset,
        -m_fDistance * cosf(XMConvertToRadians(m_fYaw)) * cosf(XMConvertToRadians(m_fPitch)),
        0.f
    ));

    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vCameraOffset));
    *pOutLookAtPosition = vPlayerPosition;
    pOutLookAtPosition->y += m_fHeightOffset;
}

void CThirdPersonCamera::Update_LockOnCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    float fLerpFactor = 1.0f - exp(-m_fLockOnTransitionSpeed * fTimeDelta);

    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        fLerpFactor
    ));
}

void CThirdPersonCamera::Update_ThirdCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    // 보간 factor 계산
    float fLerpFactor = 1.0f - exp(-m_fFollowSpeed * fTimeDelta);

    // 카메라 위치 보간
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    // 시선 위치 보간
    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        fLerpFactor
    ));
}

void CThirdPersonCamera::Update_TransitionCam(_float fTimeDelta)
{
    m_fTransitionProgress += fTimeDelta;
    float t = min(m_fTransitionProgress / m_fTransitionDuration, 1.0f);
    t = t * t * (3.0f - 2.0f * t); // Smoothstep interpolation

    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLastLockedPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        t
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        t
    ));
}

void CThirdPersonCamera::ParryingZoomIn(_float fTimeDelta)
{
    // 거리와 FOV 조정
    m_fDistance -= 30.f * fTimeDelta;
    m_fFovy -= XMConvertToRadians(1.f);

    // 최소값 제한
    m_fDistance = max(m_fDistance, 4.5f);
    m_fFovy = max(m_fFovy, XMConvertToRadians(45.f));

    _float4 vPlayerPosition;

    if (!m_bIsTargetLocked)
    {
        // 플레이어 위치와 Look 벡터 가져오기
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // 플레이어 뒤쪽 방향으로 Yaw 조정
        _float3 vPlayerLookFloat3;
        XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
        m_fYaw = XMConvertToDegrees(atan2f(vPlayerLookFloat3.x, vPlayerLookFloat3.z)); // 180도 추가하여 뒤쪽을 바라보도록 함

        // Get_ThirdCamPos 함수를 사용하여 목표 카메라 위치 계산
        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        // 현재 카메라 위치에서 목표 위치로 보간
        float fLerpFactor = 5.f * fTimeDelta; // 보간 속도 조절
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&vTargetCameraPosition), fLerpFactor));

        // LookAt 위치 업데이트
        XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&vTargetLookAtPosition));
    }
    else
    {
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);
            // 플레이어 위치와 Look 벡터 가져오기
            _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

            // 플레이어 뒤쪽 방향으로 카메라 위치 조정
            _float3 vPlayerLookFloat3;
            XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
            _float4 vPlayerBackward;
            XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat3(&vPlayerLookFloat3) * -1));
            XMStoreFloat4(&vTargetCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * m_fDistance);
            vTargetCameraPosition.y += m_fHeightOffset;


            float fLerpFactor = 1.0f - exp(-m_fLockOnTransitionSpeed * fTimeDelta * 5.f);

            XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
                XMLoadFloat4(&m_vCameraPosition),
                XMLoadFloat4(&vTargetCameraPosition),
                fLerpFactor
            ));

            XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
                XMLoadFloat4(&m_vLookAtPosition),
                XMLoadFloat4(&vTargetLookAtPosition),
                fLerpFactor
            ));
        
    }


    // 줌인 완료 체크
    if (m_fDistance <= 4.5f && m_fFovy <= XMConvertToRadians(45.f))
    {
        m_bZoomIn = false;
    }
}

void CThirdPersonCamera::ParryingZoomOut(_float fTimeDelta)
{
    bool bDistanceReached = false;
    bool bFovyReached = false;

    m_fDistance += 6.f * fTimeDelta;
    m_fFovy += XMConvertToRadians(1.f);

    if (m_fDistance > 5.3f)
    {
        m_fDistance = 5.3f;
        bDistanceReached = true;
    }

    if (m_fFovy > XMConvertToRadians(60.f))
    {
        m_fFovy = XMConvertToRadians(60.f);
        bFovyReached = true;
    }

    // 두 조건이 모두 만족되면 m_bZoomOut을 false로 설정
    if (bDistanceReached && bFovyReached)
    {
        m_bZoomOut = false;
        m_fShakeDuration = 0.4f;
    }
}



void CThirdPersonCamera::Late_Tick(_float fTimeDelta)
{
    if (!m_bCamActivated) return;


   

    Mouse_Move(fTimeDelta);
    Key_Input(fTimeDelta);
}

HRESULT CThirdPersonCamera::Render()
{
    return S_OK;
}

void CThirdPersonCamera::Revolution360(_float fTimeDelta)
{
    if (m_fRevolutionAccTime < m_fRevolutionTime)
    {
        m_fRevolutionAccTime += fTimeDelta;
        float fRevolutionAngle = m_fRevolutionAccTime / m_fRevolutionTime * XM_2PI;

        vEye.x = vAt.x + cosf(fRevolutionAngle) * fDistance;
        vEye.y = vAt.y + m_fHeight;
        vEye.z = vAt.z + sinf(fRevolutionAngle) * fDistance;
        vEye.w = 1.f;
    }
}

void CThirdPersonCamera::TiltAdjust(_float fTimeDelta)
{
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Cross(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), vUp);
    vUp = XMVector3Cross(vRight, XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));

    _matrix matRoll = XMMatrixRotationAxis(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), XMConvertToRadians(m_fRollAngle));
    vUp = XMVector3TransformNormal(vUp, matRoll);

    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
}

void CThirdPersonCamera::TargetLock_On(_vector vTargetPos)
{
    XMStoreFloat4(&m_vLockedTargetPos, vTargetPos);
    m_bIsTargetLocked = true;
    // LockOn 시작 시 전환 속도를 느리게 설정
    m_fLockOnTransitionSpeed = 3.0f;
}

void CThirdPersonCamera::TargetLock_Off()
{
    m_bIsTargetLocked = false;
    m_fTransitionProgress = 0.0f;
    m_vLastLockedPosition = m_vCameraPosition;
}

float CThirdPersonCamera::DistancePointLine(_vector point, _vector lineStart, _vector lineEnd)
{
    _vector projection = ProjectPointLine(point, lineStart, lineEnd);
    return XMVectorGetX(XMVector3Length(projection - point));
}

_vector CThirdPersonCamera::ProjectPointLine(_vector point, _vector lineStart, _vector lineEnd)
{
    _vector lineDirection = lineEnd - lineStart;
    float lineLengthSq = XMVectorGetX(XMVector3LengthSq(lineDirection));

    if (lineLengthSq < 1e-12f)  // 라인의 길이가 거의 0인 경우
    {
        return lineStart;
    }

    _vector toPoint = point - lineStart;
    float t = XMVectorGetX(XMVector3Dot(toPoint, lineDirection)) / lineLengthSq;

    // t를 0과 1 사이로 제한 (선분에 대한 투영을 위해)
    t = max(0.f, min(1.f, t));

    return XMVectorAdd(lineStart, XMVectorScale(lineDirection, t));
}

void CThirdPersonCamera::TargetLockView(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // 플레이어와 타겟 사이의 방향 벡터 계산
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // 플레이어와 타겟 사이의 거리 계산
    float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // 카메라 거리 조정 (플레이어-타겟 거리에 따라)
    float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.7f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // 플레이어 뒤쪽 방향 계산
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // 카메라 위치 계산 (플레이어 뒤쪽)
    _float4 vIdealCameraPos;
    XMStoreFloat4(&vIdealCameraPos, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);

    // 카메라 높이 조정
    vIdealCameraPos.y += m_fHeightOffset;

    // 현재 카메라 위치에서 이상적인 위치로 부드럽게 이동
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vIdealCameraPos),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // LookAt 위치 계산 (타겟 위치)
    XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&m_vLockedTargetPos));
}

void CThirdPersonCamera::Shaking()
{
    _vector vLook;
    _vector vRight;
    _vector vUp;

    vLook = XMVector3Normalize(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));
    vRight = XMVector3Cross(vLook, m_pTransformCom->Get_State(CTransform::STATE_UP));
    vUp = XMVector3Cross(vLook, vRight);

    // -1.5f ~ 1.5f 사이의값 구함
    float offsetY = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
    _vector eye = XMLoadFloat4(&vEye);
    _vector at = XMLoadFloat4(&vAt);
    eye += vUp * offsetY;
    at += vUp * offsetY;

    if (!m_bLockWidth)
    {
        float offsetX = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
        eye += vRight * offsetX;
        at += vRight * offsetX;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, eye);
    m_pTransformCom->LookAt(at);

}

float CThirdPersonCamera::PerlinNoise(float x, float y, int octaves, float persistence)
{
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;

    for (int i = 0; i < octaves; i++)
    {
        total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    // 정규화된 값을 -1에서 1 사이로 매핑
    float normalizedValue = total / maxValue;
    return normalizedValue;
}

float CThirdPersonCamera::InterpolatedNoise(float x, float y)
{
    int intX = static_cast<int>(floor(x));
    float fracX = x - intX;
    int intY = static_cast<int>(floor(y));
    float fracY = y - intY;

    float v1 = SmoothNoise(intX, intY);
    float v2 = SmoothNoise(intX + 1, intY);
    float v3 = SmoothNoise(intX, intY + 1);
    float v4 = SmoothNoise(intX + 1, intY + 1);

    float i1 = CosineInterpolate(v1, v2, fracX);
    float i2 = CosineInterpolate(v3, v4, fracX);

    return CosineInterpolate(i1, i2, fracY);
}

float CThirdPersonCamera::SmoothNoise(int x, int y)
{
    float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16.0f;
    float sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8.0f;
    float center = Noise(x, y) / 4.0f;
    return corners + sides + center;
}

float CThirdPersonCamera::Noise(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float CThirdPersonCamera::CosineInterpolate(float a, float b, float t)
{
    float ft = t * 3.1415927f;
    float f = (1 - cos(ft)) * 0.5f;
    return a * (1 - f) + b * f;
}

void CThirdPersonCamera::Key_Input(_float fTimeDelta)
{


 /*    카메라 회전
    if (m_pGameInstance->Key_Pressing(DIK_LEFT))
        RotateCamera(-90.0f * fTimeDelta);
    else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
        RotateCamera(90.0f * fTimeDelta);

     거리 조절 (선택적)
    if (m_pGameInstance->Key_Pressing(DIK_UP))
        m_fDistance = max(2.0f, m_fDistance - 5.0f * fTimeDelta);
    else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
        m_fDistance = min(10.0f, m_fDistance + 5.0f * fTimeDelta);*/
}

// ThirdPersonCamera.cpp 수정
void CThirdPersonCamera::Mouse_Move(_float fTimeDelta)
{
    _long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
    _long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

    if (MouseMoveX != 0 || MouseMoveY != 0)
    {
        RotateCamera(MouseMoveX * m_fSensorX, MouseMoveY * m_fSensorY);

        // 마우스를 화면 중앙으로 고정
        POINT ptCenter = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
        ClientToScreen(g_hWnd, &ptCenter);
        SetCursorPos(ptCenter.x, ptCenter.y);
    }
}

void CThirdPersonCamera::Shake_Camera(_bool bSlowMo, _float fDuration)
{
    if (bSlowMo)
    {
        fDuration *= 1.5f;
    }

    m_fShakeDuration =  fDuration;

    //m_fFovy = XMConvertToRadians(60.f);

    m_bIsShaking = true;
    m_fShakeTimer = 0.f;

    // 펄린 노이즈를 이용한 초기 셰이크 타겟 위치 계산
    float noiseX = PerlinNoise(0, 0, 10, 0.5f);
    float noiseY = PerlinNoise(0, 0, 10, 0.5f);
    XMVECTOR shake = XMVectorSet(noiseX, noiseY, noiseY, 0) * m_fShakeAmount;
    XMStoreFloat4(&m_vShakeTargetPosition, XMLoadFloat4(&m_vCameraPosition) + shake);
}

void CThirdPersonCamera::RotateCamera(float fDeltaX, float fDeltaY)
{
    m_fYaw += fDeltaX;
    m_fPitch += fDeltaY;

    // 각도를 0-360 범위 내로 유지
    if (m_fYaw > 360.0f)
        m_fYaw -= 360.0f;
    else if (m_fYaw < 0.0f)
        m_fYaw += 360.0f;

    // 수직 각도 제한
    m_fPitch = max(m_fMinPitch, min(m_fMaxPitch, m_fPitch));

    // 수직 각도에 따른 거리 조정 (보간 함수 사용)
    float fPitchRatio = (m_fPitch - m_fMinPitch) / (m_fMaxPitch - m_fMinPitch);
    m_fDistance = m_fMinDistance + (m_fMaxDistance - m_fMinDistance) * (1.0f - fPitchRatio * fPitchRatio);
}



CThirdPersonCamera* CThirdPersonCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CThirdPersonCamera* pInstance = new CThirdPersonCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CThirdPersonCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CThirdPersonCamera::Clone(void* pArg)
{
    CThirdPersonCamera* pInstance = new CThirdPersonCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CThirdPersonCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThirdPersonCamera::Free()
{
    Safe_Release(m_pPlayerTrans);
    Safe_Release(m_pTargetTrans);

    __super::Free();
}




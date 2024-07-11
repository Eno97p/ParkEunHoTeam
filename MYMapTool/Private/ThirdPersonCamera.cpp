#include "..\Public\ThirdPersonCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "ToolObj.h"

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

    __super::Activate();

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
            _vector closestMonsterPos = XMVectorZero();

            for (auto& monster : monsters)
            {
                _vector vMonsterPos = dynamic_cast<CMonster*>(monster)->Get_MonsterPos();

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
                        closestMonsterPos = vMonsterPos;
                    }
                }
            }

            if (closestDistance != FLT_MAX)
            {
                TargetLock_On(closestMonsterPos);
            }
        }
        else
        {
            TargetLock_Off();
        }
    }

    if (m_pGameInstance->Mouse_Down(DIM_LB))
    {
       // m_bIsShaking = true;
        //m_fShakeTimer = 0.0f;
    }

    if (m_pGameInstance->Key_Pressing(DIK_UP))
    {
        m_vLockedTargetPos.x += 1.f;
        m_vLockedTargetPos.z += 1.f;
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

    // 플레이어 위치 얻기
    _vector vPlayerPos = m_pPlayerTrans->Get_State(CTransform::STATE_POSITION);
    _vector vCamPos = m_pGameInstance->Get_CamPosition();

    // 맵 오브젝트 리스트 얻기
    list<CGameObject*> mapObjects = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Obj"));
    if (!mapObjects.empty())
    {
        for (auto& obj : mapObjects)
        {
            CToolObj* mapObj = dynamic_cast<CToolObj*>(obj);
            if (mapObj == nullptr) continue;

            _vector vMapObjPos = mapObj->Get_Position();

            // 플레이어, 카메라, 맵 오브젝트 위치를 Float3로 변환
            _float3 f3PlayerPos, f3CamPos, f3MapObjPos;
            XMStoreFloat3(&f3PlayerPos, vPlayerPos);
            XMStoreFloat3(&f3CamPos, vCamPos);
            XMStoreFloat3(&f3MapObjPos, vMapObjPos);

            // 플레이어와 맵 오브젝트 사이의 거리 계산
            _float fDistToPlayer = XMVectorGetX(XMVector3Length(vMapObjPos - vPlayerPos));

            // 카메라와 맵 오브젝트 사이의 거리 계산
            _float fDistToCam = XMVectorGetX(XMVector3Length(vMapObjPos - vCamPos));

            // 플레이어와 카메라 사이의 거리 계산
            _float fDistPlayerToCam = XMVectorGetX(XMVector3Length(vPlayerPos - vCamPos));

            // 맵 오브젝트가 플레이어와 카메라 사이에 있는지 판단
            if (fDistToCam < fDistPlayerToCam && fDistToPlayer < fDistPlayerToCam)
            {
                // 맵 오브젝트가 플레이어와 카메라 사이에 있다면 알파 블렌딩 적용
                _float fAlpha = fDistToCam / fDistPlayerToCam; // 카메라와의 거리에 따른 알파 값 계산
                mapObj->Set_AlphaBlendOn(); // 맵 오브젝트의 알파 값 설정
            }
            else
            {
                // 그 외의 경우 알파 값을 1로 설정하여 불투명하게 렌더링
                mapObj->Set_AlphaBlendOff();
            }
        }
    }
   

    if (m_bIsTargetLocked)
    {
        Update_LockOnCam(fTimeDelta);
    }
    else
    {
        Update_ThirdCam(fTimeDelta);
    }

    // 셰이킹 적용
    if (m_bIsShaking)
    {
        m_fShakeTimer += fTimeDelta;
        m_fShakeIntervalTimer += fTimeDelta;

        if (m_fShakeIntervalTimer >= m_fShakeInterval)
        {
            m_fShakeIntervalTimer = 0.0f;
            m_vShakeStart = m_vCameraPosition;
            Shaking();
        }

        // 보간을 사용하여 카메라 위치 업데이트
        float t = m_fShakeIntervalTimer / m_fShakeInterval;
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
            XMLoadFloat4(&m_vShakeStart),
            XMLoadFloat4(&m_vShakeTarget),
            t
        ));

        if (m_fShakeTimer > m_fShakeDuration)
        {
            m_bIsShaking = false;
        }
    }

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
    pOutCameraPosition->y += m_fHeightOffset;

    // LookAt 위치 계산 (플레이어와 타겟의 중간점)
    XMStoreFloat4(pOutLookAtPosition, (XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&m_vLockedTargetPos)) * 0.5f);
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

void CThirdPersonCamera::ZoomIn(_float fTimeDelta)
{
    m_fZoomDistance += m_fZoomSpeed * fTimeDelta;

    _vector CurAt = XMLoadFloat4(&vAt);
    _vector vZoomDir = XMVector3Normalize(CurAt - XMLoadFloat4(&vEye));
    vEye.x = vAt.x - XMVectorGetX(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.y = vAt.y - XMVectorGetY(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.z = vAt.z - XMVectorGetZ(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.w = 1.f;
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

    // LookAt 위치 계산 (플레이어와 타겟의 중간점)
    _float4 vIdealLookAt;
    XMStoreFloat4(&vIdealLookAt, (XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&m_vLockedTargetPos)) * 0.5f);
    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vIdealLookAt),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // 플레이어와 타겟이 화면에 모두 담기도록 조정
    _float4 vScreenEdge1, vScreenEdge2;
    XMStoreFloat4(&vScreenEdge1, m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.8f);  // 화면 너비의 80%
    XMStoreFloat4(&vScreenEdge2, m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -0.8f);

    float distPlayer = DistancePointLine(XMLoadFloat4(&vPlayerPosition), XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vLookAtPosition));
    float distTarget = DistancePointLine(XMLoadFloat4(&m_vLockedTargetPos), XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vLookAtPosition));

    if (distPlayer > XMVectorGetX(XMVector3Length(XMLoadFloat4(&vScreenEdge1))) ||
        distTarget > XMVectorGetX(XMVector3Length(XMLoadFloat4(&vScreenEdge1))))
    {
        // 화면 밖으로 벗어난 경우, 카메라를 더 멀리 이동
        fAdjustedDistance *= 1.1f;
        XMStoreFloat4(&vIdealCameraPos, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);
        vIdealCameraPos.y += m_fHeightOffset;
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
            XMLoadFloat4(&m_vCameraPosition),
            XMLoadFloat4(&vIdealCameraPos),
            1.0f - exp(-m_fLerpFactor * 0.5f * fTimeDelta)
        ));
    }
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


void CThirdPersonCamera::Key_Input(_float fTimeDelta)
{

    if (m_pGameInstance->Key_Down(DIK_TAB))
    {
        m_pGameInstance->Set_MainCamera(0);
    }
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

    __super::Free();
}




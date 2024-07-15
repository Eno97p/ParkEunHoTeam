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

    // �ʱ� ī�޶� ��ġ ����
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

            // ī�޶� ������ ���� (��: ī�޶� �� 100 ����)
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

                // ���Ͱ� ȭ�� ���� �ִ��� Ȯ��
                if (m_pGameInstance->isIn_WorldFrustum(vMonsterPos, 5.f))
                {
                    // ī�޶� ���̿� ���� ������ ������ ���
                    _vector projectedPoint = ProjectPointLine(vMonsterPos, vCamPos, vRayEnd);

                    // �������� ���� ������ �Ÿ� ���
                    float distToRay = XMVectorGetX(XMVector3Length(vMonsterPos - projectedPoint));

                    // ī�޶�� ������ ������ �Ÿ� ��� (����)
                    float depthOnRay = XMVectorGetX(XMVector3Length(projectedPoint - vCamPos));

                    // �Ÿ��� ����ġ�� �־� ���̵� ��� (��: ���̿� 0.5 ����ġ)
                    float weightedDistance = distToRay + depthOnRay * 0.5f;

                    if (weightedDistance < closestDistance)
                    {
                        closestDistance = weightedDistance;
                        closestMonsterTransform = pMonsterTransform;
                    }
                }
            }

            // ���� ����� ���ͱ����� �Ÿ��� 10 ������ ��쿡�� Ÿ�� ����
            if (closestMonsterTransform != nullptr && closestDistance <= 40.0f)
            {
                // ���� Ÿ���� �ִٸ� ���۷��� ī��Ʈ ����
                if (m_pTargetTrans != nullptr)
                    Safe_Release(m_pTargetTrans);

                // ���ο� Ÿ���� Transform ������Ʈ ���� �� ���۷��� ī��Ʈ ����
                m_pTargetTrans = closestMonsterTransform;
                Safe_AddRef(m_pTargetTrans);

                // Ÿ�� ����
                _vector vTargetPos = m_pTargetTrans->Get_State(CTransform::STATE_POSITION);
                TargetLock_On(vTargetPos);
            }
            else
            {
                // ���� ����� ���Ͱ� ���ų� �Ÿ��� 10���� ū ���
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
            // �̹� Ÿ���� ���µ� ���¿��� �ٽ� Ŭ���� ���
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
        m_fShakeAmount = m_fInitialShakeAmount; // �ʱ� ����ũ ������ ����
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

    // ���� ���� ó��
    if (m_bZoomIn)
    {
        ParryingZoomIn(fTimeDelta);
    }
    // �ܾƿ� ���� ó��
    else if (m_bZoomOut)
    {
        ParryingZoomOut(fTimeDelta);
    }
    // Ÿ�� ���� ���� ó��
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
    // �Ϲ� ���� ó��
    else
    {
        Update_ThirdCam(fTimeDelta);
    }
   
  

    if (m_bIsShaking)
    {
        m_fShakeTimer += fTimeDelta;
        m_fShakeIntervalTimer += fTimeDelta;

        // ���� ȿ�� (���� ����ũ ������ ���� ���� ���Ϸ� �������� �ʵ��� ��)
        m_fShakeAmount = max(m_fShakeAmount * (1.0f - fTimeDelta * 2.0f), m_fInitialShakeAmount * 0.1f);

        // ���ļ� ����
        float frequency = 1.0f + sin(m_fShakeTimer * 5.0f) * 0.5f;

        m_fShakeInterval = min(0.4f, m_fInitialShakeInterval + m_fShakeTimer * 0.05f);

        if (m_fShakeIntervalTimer >= m_fShakeInterval)
        {
            m_fShakeIntervalTimer = 0.f;
            m_vShakeTargetPosition = m_vCameraPosition;

            // 3�� ����ũ
            float noiseX = PerlinNoise(m_fShakeTimer * m_fShakeSpeed * frequency, 0, 10, 0.5f);
            float noiseY = PerlinNoise(0, m_fShakeTimer * m_fShakeSpeed * frequency, 10, 0.8f);
            float noiseZ = PerlinNoise(m_fShakeTimer * m_fShakeSpeed * frequency, m_fShakeTimer * m_fShakeSpeed * frequency, 10, 0.5f);

            XMVECTOR shake = XMVectorSet(noiseX, noiseY, noiseZ, 0) * m_fShakeAmount;
            XMStoreFloat4(&m_vShakeTargetPosition, XMLoadFloat4(&m_vShakeTargetPosition) + shake);

            // ȸ�� ����ũ
            XMVECTOR rotationShake = XMQuaternionRotationRollPitchYaw(noiseX * 0.1f, noiseY * 0.1f, noiseZ * 0.1f);
            XMStoreFloat4(&m_qShakeRotation, XMLoadFloat4(&m_qShakeRotation) * rotationShake);
        }

        // �ε巯�� ��ȯ
        float t = m_fShakeIntervalTimer / m_fShakeInterval;
        t = t * t * (3.0f - 2.0f * t); // Smoothstep
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vShakeTargetPosition), t));

        // Look ���� ��� �� ȸ�� ����ũ ����
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

  

    // ī�޶� ��ġ ����
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));
    __super::Tick(fTimeDelta);
}

void CThirdPersonCamera::Get_LockOnCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    // �÷��̾�� Ÿ�� ������ ���� ���� ���
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // �÷��̾�� Ÿ�� ������ �Ÿ� ���
    float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // ī�޶� �Ÿ� ���� (�÷��̾�-Ÿ�� �Ÿ��� ����)
    float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.5f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // �÷��̾� ���� ���� ���
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // ī�޶� ��ġ ��� (�÷��̾� ����)
    XMStoreFloat4(pOutCameraPosition, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);
    pOutCameraPosition->y += m_fHeightOffset * 1.7f;

    // LookAt ��ġ ��� (�÷��̾�� Ÿ���� �߰���)
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

    // ���� factor ���
    float fLerpFactor = 1.0f - exp(-m_fFollowSpeed * fTimeDelta);

    // ī�޶� ��ġ ����
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        fLerpFactor
    ));

    // �ü� ��ġ ����
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
    // �Ÿ��� FOV ����
    m_fDistance -= 30.f * fTimeDelta;
    m_fFovy -= XMConvertToRadians(1.f);

    // �ּҰ� ����
    m_fDistance = max(m_fDistance, 4.5f);
    m_fFovy = max(m_fFovy, XMConvertToRadians(45.f));

    _float4 vPlayerPosition;

    if (!m_bIsTargetLocked)
    {
        // �÷��̾� ��ġ�� Look ���� ��������
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));
        _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

        // �÷��̾� ���� �������� Yaw ����
        _float3 vPlayerLookFloat3;
        XMStoreFloat3(&vPlayerLookFloat3, vPlayerLook);
        m_fYaw = XMConvertToDegrees(atan2f(vPlayerLookFloat3.x, vPlayerLookFloat3.z)); // 180�� �߰��Ͽ� ������ �ٶ󺸵��� ��

        // Get_ThirdCamPos �Լ��� ����Ͽ� ��ǥ ī�޶� ��ġ ���
        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_ThirdCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

        // ���� ī�޶� ��ġ���� ��ǥ ��ġ�� ����
        float fLerpFactor = 5.f * fTimeDelta; // ���� �ӵ� ����
        XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&vTargetCameraPosition), fLerpFactor));

        // LookAt ��ġ ������Ʈ
        XMStoreFloat4(&m_vLookAtPosition, XMLoadFloat4(&vTargetLookAtPosition));
    }
    else
    {
        XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

        _float4 vTargetCameraPosition, vTargetLookAtPosition;
        Get_LockOnCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);
            // �÷��̾� ��ġ�� Look ���� ��������
            _vector vPlayerLook = m_pPlayerTrans->Get_State(CTransform::STATE_LOOK);

            // �÷��̾� ���� �������� ī�޶� ��ġ ����
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


    // ���� �Ϸ� üũ
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

    // �� ������ ��� �����Ǹ� m_bZoomOut�� false�� ����
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
    // LockOn ���� �� ��ȯ �ӵ��� ������ ����
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

    if (lineLengthSq < 1e-12f)  // ������ ���̰� ���� 0�� ���
    {
        return lineStart;
    }

    _vector toPoint = point - lineStart;
    float t = XMVectorGetX(XMVector3Dot(toPoint, lineDirection)) / lineLengthSq;

    // t�� 0�� 1 ���̷� ���� (���п� ���� ������ ����)
    t = max(0.f, min(1.f, t));

    return XMVectorAdd(lineStart, XMVectorScale(lineDirection, t));
}

void CThirdPersonCamera::TargetLockView(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    // �÷��̾�� Ÿ�� ������ ���� ���� ���
    _float4 vPlayerToTarget;
    XMStoreFloat4(&vPlayerToTarget, XMLoadFloat4(&m_vLockedTargetPos) - XMLoadFloat4(&vPlayerPosition));

    // �÷��̾�� Ÿ�� ������ �Ÿ� ���
    float fDistancePlayerToTarget = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vPlayerToTarget)));

    // ī�޶� �Ÿ� ���� (�÷��̾�-Ÿ�� �Ÿ��� ����)
    float fAdjustedDistance = max(m_fDistance, fDistancePlayerToTarget * 0.7f);
    fAdjustedDistance = min(fAdjustedDistance, m_fMaxDistance);

    // �÷��̾� ���� ���� ���
    _float4 vPlayerBackward;
    XMStoreFloat4(&vPlayerBackward, XMVector3Normalize(XMLoadFloat4(&vPlayerToTarget) * -1));

    // ī�޶� ��ġ ��� (�÷��̾� ����)
    _float4 vIdealCameraPos;
    XMStoreFloat4(&vIdealCameraPos, XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&vPlayerBackward) * fAdjustedDistance);

    // ī�޶� ���� ����
    vIdealCameraPos.y += m_fHeightOffset;

    // ���� ī�޶� ��ġ���� �̻����� ��ġ�� �ε巴�� �̵�
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vIdealCameraPos),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // LookAt ��ġ ��� (Ÿ�� ��ġ)
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

    // -1.5f ~ 1.5f �����ǰ� ����
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

    // ����ȭ�� ���� -1���� 1 ���̷� ����
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


 /*    ī�޶� ȸ��
    if (m_pGameInstance->Key_Pressing(DIK_LEFT))
        RotateCamera(-90.0f * fTimeDelta);
    else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
        RotateCamera(90.0f * fTimeDelta);

     �Ÿ� ���� (������)
    if (m_pGameInstance->Key_Pressing(DIK_UP))
        m_fDistance = max(2.0f, m_fDistance - 5.0f * fTimeDelta);
    else if (m_pGameInstance->Key_Pressing(DIK_DOWN))
        m_fDistance = min(10.0f, m_fDistance + 5.0f * fTimeDelta);*/
}

// ThirdPersonCamera.cpp ����
void CThirdPersonCamera::Mouse_Move(_float fTimeDelta)
{
    _long MouseMoveX = m_pGameInstance->Get_DIMouseMove(DIMS_X);
    _long MouseMoveY = m_pGameInstance->Get_DIMouseMove(DIMS_Y);

    if (MouseMoveX != 0 || MouseMoveY != 0)
    {
        RotateCamera(MouseMoveX * m_fSensorX, MouseMoveY * m_fSensorY);

        // ���콺�� ȭ�� �߾����� ����
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

    // �޸� ����� �̿��� �ʱ� ����ũ Ÿ�� ��ġ ���
    float noiseX = PerlinNoise(0, 0, 10, 0.5f);
    float noiseY = PerlinNoise(0, 0, 10, 0.5f);
    XMVECTOR shake = XMVectorSet(noiseX, noiseY, noiseY, 0) * m_fShakeAmount;
    XMStoreFloat4(&m_vShakeTargetPosition, XMLoadFloat4(&m_vCameraPosition) + shake);
}

void CThirdPersonCamera::RotateCamera(float fDeltaX, float fDeltaY)
{
    m_fYaw += fDeltaX;
    m_fPitch += fDeltaY;

    // ������ 0-360 ���� ���� ����
    if (m_fYaw > 360.0f)
        m_fYaw -= 360.0f;
    else if (m_fYaw < 0.0f)
        m_fYaw += 360.0f;

    // ���� ���� ����
    m_fPitch = max(m_fMinPitch, min(m_fMaxPitch, m_fPitch));

    // ���� ������ ���� �Ÿ� ���� (���� �Լ� ���)
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




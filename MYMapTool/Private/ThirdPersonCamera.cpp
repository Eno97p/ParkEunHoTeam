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
            _vector closestMonsterPos = XMVectorZero();

            for (auto& monster : monsters)
            {
                _vector vMonsterPos = dynamic_cast<CMonster*>(monster)->Get_MonsterPos();

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

    // �÷��̾� ��ġ ���
    _vector vPlayerPos = m_pPlayerTrans->Get_State(CTransform::STATE_POSITION);
    _vector vCamPos = m_pGameInstance->Get_CamPosition();

    // �� ������Ʈ ����Ʈ ���
    list<CGameObject*> mapObjects = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_Obj"));
    if (!mapObjects.empty())
    {
        for (auto& obj : mapObjects)
        {
            CToolObj* mapObj = dynamic_cast<CToolObj*>(obj);
            if (mapObj == nullptr) continue;

            _vector vMapObjPos = mapObj->Get_Position();

            // �÷��̾�, ī�޶�, �� ������Ʈ ��ġ�� Float3�� ��ȯ
            _float3 f3PlayerPos, f3CamPos, f3MapObjPos;
            XMStoreFloat3(&f3PlayerPos, vPlayerPos);
            XMStoreFloat3(&f3CamPos, vCamPos);
            XMStoreFloat3(&f3MapObjPos, vMapObjPos);

            // �÷��̾�� �� ������Ʈ ������ �Ÿ� ���
            _float fDistToPlayer = XMVectorGetX(XMVector3Length(vMapObjPos - vPlayerPos));

            // ī�޶�� �� ������Ʈ ������ �Ÿ� ���
            _float fDistToCam = XMVectorGetX(XMVector3Length(vMapObjPos - vCamPos));

            // �÷��̾�� ī�޶� ������ �Ÿ� ���
            _float fDistPlayerToCam = XMVectorGetX(XMVector3Length(vPlayerPos - vCamPos));

            // �� ������Ʈ�� �÷��̾�� ī�޶� ���̿� �ִ��� �Ǵ�
            if (fDistToCam < fDistPlayerToCam && fDistToPlayer < fDistPlayerToCam)
            {
                // �� ������Ʈ�� �÷��̾�� ī�޶� ���̿� �ִٸ� ���� ���� ����
                _float fAlpha = fDistToCam / fDistPlayerToCam; // ī�޶���� �Ÿ��� ���� ���� �� ���
                mapObj->Set_AlphaBlendOn(); // �� ������Ʈ�� ���� �� ����
            }
            else
            {
                // �� ���� ��� ���� ���� 1�� �����Ͽ� �������ϰ� ������
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

    // ����ŷ ����
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

        // ������ ����Ͽ� ī�޶� ��ġ ������Ʈ
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
    pOutCameraPosition->y += m_fHeightOffset;

    // LookAt ��ġ ��� (�÷��̾�� Ÿ���� �߰���)
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

    // LookAt ��ġ ��� (�÷��̾�� Ÿ���� �߰���)
    _float4 vIdealLookAt;
    XMStoreFloat4(&vIdealLookAt, (XMLoadFloat4(&vPlayerPosition) + XMLoadFloat4(&m_vLockedTargetPos)) * 0.5f);
    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vIdealLookAt),
        1.0f - exp(-m_fLerpFactor * fTimeDelta)
    ));

    // �÷��̾�� Ÿ���� ȭ�鿡 ��� ��⵵�� ����
    _float4 vScreenEdge1, vScreenEdge2;
    XMStoreFloat4(&vScreenEdge1, m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.8f);  // ȭ�� �ʺ��� 80%
    XMStoreFloat4(&vScreenEdge2, m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * -0.8f);

    float distPlayer = DistancePointLine(XMLoadFloat4(&vPlayerPosition), XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vLookAtPosition));
    float distTarget = DistancePointLine(XMLoadFloat4(&m_vLockedTargetPos), XMLoadFloat4(&m_vCameraPosition), XMLoadFloat4(&m_vLookAtPosition));

    if (distPlayer > XMVectorGetX(XMVector3Length(XMLoadFloat4(&vScreenEdge1))) ||
        distTarget > XMVectorGetX(XMVector3Length(XMLoadFloat4(&vScreenEdge1))))
    {
        // ȭ�� ������ ��� ���, ī�޶� �� �ָ� �̵�
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


void CThirdPersonCamera::Key_Input(_float fTimeDelta)
{

    if (m_pGameInstance->Key_Down(DIK_TAB))
    {
        m_pGameInstance->Set_MainCamera(0);
    }
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

    __super::Free();
}




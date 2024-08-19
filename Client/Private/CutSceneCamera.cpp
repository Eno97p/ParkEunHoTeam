#include "CutSceneCamera.h"
#include "GameInstance.h"
#include "TransitionCamera.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
    CUTSCENECAMERA_DESC* pDesc = (CUTSCENECAMERA_DESC*)pArg;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_KeyFrames = pDesc->KeyFrames;

   Load_CameraKeyFrames();
   Set_CutSceneIdx(0);

    return S_OK;
}

void CCutSceneCamera::Priority_Tick(_float fTimeDelta)
{

}
void CCutSceneCamera::Tick(_float fTimeDelta)
{
    if (!m_bCamActivated || m_AllCutScenes.empty() || m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        return;

    vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[m_iCurrentCutSceneIdx];

    if (currentCutScene.empty())
        return;

    if (m_bAnimationFinished || m_bPaused)
    {
       // m_pGameInstance->Set_MainCamera(CAM_THIRDPERSON);

        //컷씬 트렌지션
        CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

        pTCDesc.fFovy = XMConvertToRadians(60.f);
        pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
        pTCDesc.fNear = 0.1f;
        pTCDesc.fFar = 3000.f;

        pTCDesc.fSpeedPerSec = 40.f;
        pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

        pTCDesc.iStartCam = CAM_CUTSCENE;
        pTCDesc.iEndCam = CAM_THIRDPERSON;
        pTCDesc.fTransitionTime = 1.f;
        if (FAILED(m_pGameInstance->Add_Camera(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
        {
            MSG_BOX("FAILED");
            return;
        }

        return;
    }

    if (!m_bPaused)
    {
        if (m_iCurrentKeyFrame + 1 < currentCutScene.size())
        {
            CameraKeyFrame& currentKeyFrame = currentCutScene[m_iCurrentKeyFrame];
            CameraKeyFrame& nextKeyFrame = currentCutScene[m_iCurrentKeyFrame + 1];
            float t = m_fKeyFrameTime / (nextKeyFrame.fTime - currentKeyFrame.fTime);

            // 현재 키프레임 구간에서의 속도 변화 확인 및 적용
            float speedMultiplier = 1.0f;
            float prevSpeedMultiplier = 1.0f;

            for (const auto& [changeTime, changeSpeed, smoothOffset] : currentKeyFrame.speedChanges)
            {
                if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
                {
                    // 속도 변화 구간에서는 이전 속도와 현재 속도를 보간
                    float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
                    speedMultiplier = prevSpeedMultiplier + (changeSpeed - prevSpeedMultiplier) * lerpT;
                    break;
                }
                else if (t >= changeTime + smoothOffset)
                {
                    prevSpeedMultiplier = changeSpeed;
                    speedMultiplier = changeSpeed;
                }
            }

            m_fKeyFrameTime += fTimeDelta * speedMultiplier;

            // 현재 월드 행렬과 목표 월드 행렬 가져오기
            _matrix matCurrent = XMLoadFloat4x4(&currentKeyFrame.matWorld);
            _matrix matTarget = XMLoadFloat4x4(&nextKeyFrame.matWorld);

            if (t >= 1.0f)
            {
                m_iCurrentKeyFrame++;
                m_fKeyFrameTime = 0.0f;
                m_pTransformCom->Set_WorldMatrix(matTarget);
            }
            else
            {
                // 행렬 구면 선형 보간
                _matrix matLerpedWorld = XMMatrixSlerp(matCurrent, matTarget, t);
                // 보간된 월드 행렬 설정
                m_pTransformCom->Set_WorldMatrix(matLerpedWorld);
            }

            // 보간된 시야각, 근평면, 원평면 설정
            _float fLerpedFovy = XMConvertToRadians(XMConvertToDegrees(currentKeyFrame.fFovy) + (XMConvertToDegrees(nextKeyFrame.fFovy) - XMConvertToDegrees(currentKeyFrame.fFovy)) * t);
            _float fLerpedNear = currentKeyFrame.fNear + (nextKeyFrame.fNear - currentKeyFrame.fNear) * t;
            _float fLerpedFar = currentKeyFrame.fFar + (nextKeyFrame.fFar - currentKeyFrame.fFar) * t;
            Set_Fovy(fLerpedFovy);
            // Set_Near(fLerpedNear);
            // Set_Far(fLerpedFar);
        }
        else
        {
            m_bAnimationFinished = true;
        }
    }

    __super::Tick(fTimeDelta);
}
void CCutSceneCamera::Late_Tick(_float fTimeDelta)
{
    Key_Input(fTimeDelta);
}

HRESULT CCutSceneCamera::Render()
{
    return S_OK;
}


void CCutSceneCamera::Load_CameraKeyFrames()
{
    wstring filePath = L"../Bin/CutSceneData/CutSceneKeyFrames.dat";

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed to open file for camera keyframes.");
        return;
    }

    DWORD dwByte = 0;

    // 컷씬 개수 로드
    _uint iCutSceneCount = 0;
    ReadFile(hFile, &iCutSceneCount, sizeof(_uint), &dwByte, nullptr);

    // 기존 컷씬 클리어
    Clear_CutScenes();

    // 각 컷씬의 키프레임 로드
    for (_uint i = 0; i < iCutSceneCount; ++i)
    {
        vector<CCamera::CameraKeyFrame> cutScene;

        // 키프레임 개수 로드
        _uint iKeyFrameCount = 0;
        ReadFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

        // 각 키프레임 정보 로드
        for (_uint j = 0; j < iKeyFrameCount; ++j)
        {
            CCamera::CameraKeyFrame keyFrame;
            ReadFile(hFile, &keyFrame.fTime, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.matWorld, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFovy, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fNear, sizeof(float), &dwByte, nullptr);
            ReadFile(hFile, &keyFrame.fFar, sizeof(float), &dwByte, nullptr);

            // 속도 변화 정보 로드
            _uint iSpeedChangeCount = 0;
            ReadFile(hFile, &iSpeedChangeCount, sizeof(_uint), &dwByte, nullptr);
            for (_uint k = 0; k < iSpeedChangeCount; ++k)
            {
                float time, speed, smoothOffset;
                ReadFile(hFile, &time, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &speed, sizeof(float), &dwByte, nullptr);
                ReadFile(hFile, &smoothOffset, sizeof(float), &dwByte, nullptr);
                keyFrame.speedChanges.emplace_back(time, speed, smoothOffset);
            }

            cutScene.push_back(keyFrame);
        }

        Add_CutScene(cutScene);
    }

    CloseHandle(hFile);
    MSG_BOX("Camera KeyFrames Data Loaded");

}

void CCutSceneCamera::Set_CutSceneIdx(_uint idx)
{
    if (idx >= m_AllCutScenes.size())
        return;

    m_iCurrentCutSceneIdx = idx;
    m_iCurrentKeyFrame = 0;
    m_fKeyFrameTime = 0.0f;
    m_bAnimationFinished = false;
    
    vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[idx];

    if (currentCutScene.size() < 2)  // 최소 2개의 키프레임이 필요합니다.
        return;

    CameraKeyFrame& currentKeyFrame = currentCutScene[0];
    CameraKeyFrame& nextKeyFrame = currentCutScene[1];

    // 0.01초 동안의 보간 수행
    float fTimeDelta = 0.01f;
    float t = fTimeDelta / (nextKeyFrame.fTime - currentKeyFrame.fTime);

    // 속도 변화 적용 (Tick 함수의 로직과 유사)
    float speedMultiplier = 1.0f;
    for (const auto& [changeTime, changeSpeed, smoothOffset] : currentKeyFrame.speedChanges)
    {
        if (t >= changeTime - smoothOffset && t < changeTime + smoothOffset)
        {
            float lerpT = (t - (changeTime - smoothOffset)) / (2.0f * smoothOffset);
            speedMultiplier = 1.0f + (changeSpeed - 1.0f) * lerpT;
            break;
        }
        else if (t >= changeTime + smoothOffset)
        {
            speedMultiplier = changeSpeed;
        }
    }

    // 월드 행렬 보간
    _matrix matCurrent = XMLoadFloat4x4(&currentKeyFrame.matWorld);
    _matrix matTarget = XMLoadFloat4x4(&nextKeyFrame.matWorld);
    _matrix matLerpedWorld = XMMatrixSlerp(matCurrent, matTarget, t * speedMultiplier);

    // 보간된 월드 행렬 설정
    m_pTransformCom->Set_WorldMatrix(matLerpedWorld);

    // FOV, Near, Far 보간
    float fLerpedFovy = XMConvertToRadians(
        XMConvertToDegrees(currentKeyFrame.fFovy) +
        (XMConvertToDegrees(nextKeyFrame.fFovy) - XMConvertToDegrees(currentKeyFrame.fFovy)) * t * speedMultiplier
    );
    Set_Fovy(fLerpedFovy);

    // Near와 Far 값 설정이 필요한 경우 주석 해제
    // float fLerpedNear = currentKeyFrame.fNear + (nextKeyFrame.fNear - currentKeyFrame.fNear) * t * speedMultiplier;
    // float fLerpedFar = currentKeyFrame.fFar + (nextKeyFrame.fFar - currentKeyFrame.fFar) * t * speedMultiplier;
    // Set_Near(fLerpedNear);
    // Set_Far(fLerpedFar);

    // 키프레임 시간 업데이트
    m_fKeyFrameTime = fTimeDelta * speedMultiplier;
}
void CCutSceneCamera::ZoomIn(_float fTimeDelta)
{
    m_fZoomDistance += m_fZoomSpeed * fTimeDelta;

    _vector CurAt = XMLoadFloat4(&vAt);
    _vector vZoomDir = XMVector3Normalize(CurAt - XMLoadFloat4(&vEye));
    vEye.x = vAt.x - XMVectorGetX(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.y = vAt.y - XMVectorGetY(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.z = vAt.z - XMVectorGetZ(vZoomDir) * (fDistance - m_fZoomDistance);
    vEye.w = 1.f;
}

void CCutSceneCamera::Revolution360(_float fTimeDelta)
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

void CCutSceneCamera::TiltAdjust(_float fTimeDelta)
{
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Cross(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), vUp);
    vUp = XMVector3Cross(vRight, XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));

    _matrix matRoll = XMMatrixRotationAxis(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye), XMConvertToRadians(m_fRollAngle));
    vUp = XMVector3TransformNormal(vUp, matRoll);

    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
}

void CCutSceneCamera::HeightChange(_float fTimeDelta)
{
    if (m_fHeight < m_fTargetHeight)
        m_fHeight = min(m_fHeight + m_fHeightChangeSpeed * fTimeDelta, m_fTargetHeight);
    else if (m_fHeight > m_fTargetHeight)
        m_fHeight = max(m_fHeight - m_fHeightChangeSpeed * fTimeDelta, m_fTargetHeight);

    vEye.y = vAt.y + m_fHeight;
}

float CCutSceneCamera::Get_AnimationProgress() const
{
    if (m_AllCutScenes.empty() || m_iCurrentCutSceneIdx >= m_AllCutScenes.size())
        return 0.0f;

    const vector<CameraKeyFrame>& currentCutScene = m_AllCutScenes[m_iCurrentCutSceneIdx];

    if (currentCutScene.empty())
        return 0.0f;

    if (m_bAnimationFinished)
        return 1.0f;

    if (m_iCurrentKeyFrame >= currentCutScene.size() - 1)
        return 1.0f;

    float totalDuration = currentCutScene.back().fTime - currentCutScene.front().fTime;
    float currentTime = currentCutScene[m_iCurrentKeyFrame].fTime + m_fKeyFrameTime;

    return (currentTime - currentCutScene.front().fTime) / totalDuration;
}

const vector<CCutSceneCamera::CameraKeyFrame>& CCutSceneCamera::Get_CutScene(_uint iIndex) const
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // 잘못된 인덱스에 대한 예외 처리
        static vector<CameraKeyFrame> emptyKeyFrames;
        return emptyKeyFrames;
    }
    return m_AllCutScenes[iIndex];
}

void CCutSceneCamera::Set_CutScene(_uint iIndex, const vector<CameraKeyFrame>& keyFrames)
{
    if (iIndex >= m_AllCutScenes.size())
    {
        // 잘못된 인덱스에 대한 예외 처리
        return;
    }
    m_AllCutScenes[iIndex] = keyFrames;
}

void CCutSceneCamera::Clear_CutScenes()
{
    m_AllCutScenes.clear();
    m_iCurrentCutSceneIdx = 0;
    m_iCurrentKeyFrame = 0;
    m_fKeyFrameTime = 0.0f;
    m_bAnimationFinished = true;
    m_bPaused = true;
}

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCutSceneCamera::Free()
{
    Safe_Release(m_pPlayerTrans);

    __super::Free();
}

void CCutSceneCamera::Shaking()
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


void CCutSceneCamera::Key_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_W))
        fDistance -= 10.f * fTimeDelta;
    else if (m_pGameInstance->Key_Pressing(DIK_S))
        fDistance += 10.f * fTimeDelta;


    if (m_pGameInstance->Key_Pressing(DIK_O))
    {
        m_fAngle += 5.f;
    }


    if (m_pGameInstance->Key_Pressing(DIK_P))
    {
        m_fAngle -= 5.f;
    }

    if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        m_bPaused = !m_bPaused;
    }

    /*if (m_pGameInstance->Key_Down(DIK_UP))
{
    m_fDebugRevolveTime += 0.5f;
}
else if (m_pGameInstance->Key_Down(DIK_DOWN))
{
    m_fDebugRevolveTime -= 0.5f;
}*/
    ////줌인
    //if (m_pGameInstance->Key_Down(DIK_Z))
    //    m_bZoomIn = true;
    //else
    //    m_bZoomIn = false;

    ////공전
    //if (m_pGameInstance->Key_Down(DIK_X))
    //    m_bRevolution360 = true;
    //else
    //    m_bRevolution360 = false;

    ////각도
    //if (m_pGameInstance->Key_Down(DIK_C))
    //    m_bTiltAdjust = true;
    //else
    //    m_bTiltAdjust = false;

    ////높이체인지
    //if (m_pGameInstance->Key_Down(DIK_V))
    //    m_bHeightChange = true;
    //else
    //    m_bHeightChange = false;

}

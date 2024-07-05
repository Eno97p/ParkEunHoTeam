
#include "Loader.h"

#include "GameInstance.h"
#include "QuarterCamera.h"
#include "Map_Flat.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"
#include "Particle_Rect.h"
#include "Particle_Trail.h"
#include "Player.h"
#include "PlayerHead.h"
#include "TextureFrame.h"
#include "WhisperSword_Anim.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader*		pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{


	EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
#pragma region TEXTURE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/T_SwordTrail01.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail_Rotated"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Texture/SwordTrail_Rotated.png"), 1))))
		return E_FAIL;
#pragma endregion TEXTURE
	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩 중 입니다."));
#pragma region COMPONENT
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion COMPONENT
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
#pragma region MODEL
	_matrix		PreTransformMatrix;
	//Slash
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/InstanceModel/Slash.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Cube
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/InstanceModel/WhiteCube.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Circle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/InstanceModel/ParticleCircle.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Flat
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Flat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Model/Flat/NewFlat.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Mantari Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MantariSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/Mantari/MatariSword.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion MODEL
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
#pragma region SHADER
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstanceRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Trail.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstanceMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Mesh.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstancePoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;


#pragma endregion SHADER
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));
#pragma region PROTOTYPE_CLASS
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CQuarterCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlatMap"),
		CFlatMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleMesh"),
		CParticleMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticlePoint"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleRect"),
		CParticle_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail"),
		CParticle_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerDummy"),
		CPlayerDummy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHead"),
		CPlayerHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword"),
		CWhisperSword_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FrameTexture"),
		CTextureFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion PROTOTYPE_CLASS
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;
	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

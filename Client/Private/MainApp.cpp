#include "stdafx.h"
#include "..\Public\MainApp.h"

//new

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Level_Loading.h"
#include "UI_Manager.h"
#include "BackGround.h"
#include "CMouse.h"

#include "Shader.h"
#include"CImGuiMgr.h"

#pragma region UI

#pragma region State
#include "UI_StateBG.h"
#include "UI_StateBar.h"
#include "UI_StateHP.h"
#include "UI_StateEnergy.h"
#include "UI_StateEther.h"
#include "UI_StateSoul.h"
#include "UIGroup_State.h"
#pragma endregion State

#pragma region WeaponSlot
#include "UI_WeaponSlotBG.h"
#include "UI_WeaponSlot.h"
#include "UIGroup_WeaponSlot.h"
#pragma endregion WeaponSlot

#pragma region Menu
#include "UI_MenuBG.h"
#include "UI_MenuFontaine.h"
#include "UI_MenuBtn.h"
#include "UI_Menu_SelectFrame.h"
#include "UIGroup_Menu.h"
#pragma endregion Menu

#pragma region Quick
#include "UI_QuickBG.h"
#include "UI_QuickTop.h"
#include "UI_QuickExplain.h"
#include "UI_QuickInvBG.h"
#include "UIGroup_Quick.h"
#pragma endregion Quick

#pragma region Character
#include "UI_CharacterBG.h"
#include "UI_CharacterTop.h"
#include "UIGroup_Character.h"
#pragma endregion Character

#pragma region Weapon
#include "UI_WeaponRTop.h"
#include "UI_WPEquipSlot.h"
#include "UI_WPEquipNone.h"
#include "UI_WPFontaine.h"
#include "UI_WeaponTab.h"

#include "UIGroup_Weapon.h"
#pragma endregion Weapon

#include "UI_MenuPageBG.h"
#include "UI_MenuPageTop.h"
#include "UI_Slot.h"
#include "UI_Slot_Frame.h"
#include "UIGroup_Inventory.h"


#pragma endregion UI


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pUI_Manager{CUI_Manager::GetInstance()}
#ifdef _DEBUG
	,m_pImGuiMgr{CImGuiMgr::GetInstance()}
#endif // _DEBUG
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUI_Manager);

#ifdef _DEBUG
	Safe_AddRef(m_pImGuiMgr);
#endif // _DEBUG

	/*D3D11_RASTERIZER_DESC		RSDesc;
	ID3D11RasterizerState*		pRSState;
	m_pDevice->CreateRasterizerState();
	m_pContext->RSSetState();

	D3D11_DEPTH_STENCIL_DESC	DSState;

	ID3D11DepthStencilState*	pDSState;
	m_pDevice->CreateDepthStencilState();
	m_pContext->OMSetDepthStencilState();

	D3D11_BLEND_DESC*			pBState;
	ID3D11BlendState*			pBlendState;
	m_pContext->OMSetBlendState();*/

	


	//D3DLIGHT9;
	//m_pDevice->SetLight();

	//D3DMATERIAL9;
	//m_pDevice->SetMaterial();

	//m_pDevice->LightEnble(0, true);

}

HRESULT CMainApp::Initialize()
{

	//LPCWSTR applicationName = L"C:\\Program Files (x86)\\NVIDIA Corporation\\PhysX Visual Debugger 3\\PVD3.exe";
	//CreatePhysXProcess(applicationName);

	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	/* 엔진 초기화과정을 거친다. ( 그래픽디바이스 초기화과정 + 레벨매니져를 사용할 준비를 한다. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;	

	// Font
	/*if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/NotoSansR.spritefont"))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Cardo"), TEXT("../Bin/Resources/Fonts/Cardo_Regular_12.spritefont"))))
		return E_FAIL;



	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->Ready_ImGui(m_pDevice, m_pContext, m_pGameInstance)))return E_FAIL;);
		//if (FAILED(m_pImGuiMgr->Ready_ImGui(m_pDevice, m_pContext, m_pGameInstance)))
		//	return E_FAIL;


	

	

	return S_OK;
}

void CMainApp::Tick(float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;


	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);
		//Terst
		m_fTimeAcc = 0.f;
		m_iNumRender = 0;
	}


	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->Render_ImGui())) return E_FAIL;);
	//if (FAILED(m_pImGuiMgr->Render_ImGui())) return E_FAIL;



	/* 그린다. */
	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	IMGUI_EXEC(if (FAILED(m_pImGuiMgr->End_ImGui())) return E_FAIL;);
	//if (FAILED(m_pImGuiMgr->End_ImGui()))
	//	return E_FAIL;


	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 142.spritefont
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/142ex.spritefont"))))
		return E_FAIL;

	/*_float3		vPoints[3] = {};

	_ulong		dwByte = {};
	HANDLE		hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.f, 0.f, 0.0f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

	ID3D11Texture2D*		pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint*			pPixel = new _uint[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_uint) * TextureDesc.Width * TextureDesc.Height);

	D3D11_SUBRESOURCE_DATA		InitialData{};

	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = 256 * 4;
	
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	for (size_t i = 0; i < 256; i++)
	{
		for (size_t j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;
			
			if(j < 10)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	pPixel[0] = D3DCOLOR_ARGB(255, 0, 0, 255);

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pPixel, sizeof(_uint) * 256 * 256);

	m_pContext->Unmap(pTexture2D, 0);

	SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"));

	Safe_Delete_Array(pPixel);

	Safe_Release(pTexture2D);*/

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), 
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_StartMenu_BG.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_UI()
{
	// UI Texture
	if (FAILED(Ready_Texture_UI()))
		return E_FAIL;

	// UI 원형
	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, TEXT("Layer_Mouse"), TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	// UI 생성
	if (FAILED(m_pUI_Manager->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Texture_UI()
{
#pragma region UI_Texture

#pragma region Logo
	/* Prototype_Component_Texture_LogoBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/T_StartMenu_Banner.png"), 1))))
		return E_FAIL;


#pragma endregion Logo

#pragma region HUD
#pragma region State
	/* Prototype_Component_Texture_HUD_StateBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_StateBG.png"), 1)))) // dds 이슈?
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_Bar.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_HP.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateEnergy */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateEnergy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_Energy.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateEther */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateEther"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HDU_Ether.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_HUD_StateSoul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateSoul"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/State/HUD_Soul.png"), 1))))
		return E_FAIL;
#pragma endregion State

#pragma region WeaponSlot
	/* Prototype_Component_Texture_HUD_WeaponSlotBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_WeaponSlotBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD/WeaponSlot/HUD_WeaponSlotBG.png"), 1))))
		return E_FAIL;
#pragma endregion WeaponSlot
#pragma endregion HUD

#pragma region Icon
#pragma region Weapon
	/* Prototype_Component_Texture_Icon_LimboBlade */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_LimboBlade"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_LimboBlade.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_NaruehsGlaive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_NaruehsGlaive"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_NaruehsGlaive.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Icon_EtherBolt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_EtherBolt"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_EtherBolt.png"), 1))))
		return E_FAIL;
#pragma endregion Weapon

#pragma region Item
	/* Prototype_Component_Texture_Icon_Whisperer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Icon_Whisperer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_Item_Whisperer.png"), 1))))
		return E_FAIL;
#pragma endregion Item

#pragma endregion Icon

#pragma region Menu

	/* Prototype_Component_Texture_MenuBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/T_IGGOUI_BG.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuFontaine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuFontaine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/T_IGGOUI_Fontaine.png"), 1))))
		return E_FAIL;

#pragma region Btn
	/* Prototype_Component_Texture_MenuBtn_Ch */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Ch"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Ch_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Cod */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Cod"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Cod_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Inv */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Inv"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Inv_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Map"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Map_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_Set */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_Set"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_Set_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_MenuBtn_WP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBtn_WP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBtn_WP_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Menu_SelectFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_SelectFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Menu_SelectFrame.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Menu_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Menu_Mask.png"), 1))))
		return E_FAIL;


#pragma endregion Btn

#pragma endregion Menu

#pragma region Quick
	/* Prototype_Component_Texture_UI_QuickBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/T_Weapons_Overlay.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/T_Character_TopImage.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickInvBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickInvBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/QuickInvBG.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_QuickExplain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickExplain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/QuickAccess/QuickExplain.png"), 1))))
		return E_FAIL;
#pragma endregion Quick

#pragma region Inventory

#pragma endregion Inventory

#pragma region Character
	/* Prototype_Component_Texture_UI_CharacterBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/T_background_character_sheet.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_CharacterTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Character/Ch_Top.png"), 1))))
		return E_FAIL;
#pragma endregion Character

#pragma region Slot
	/* Prototype_Component_Texture_UI_Slot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Slot.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Slot_SelectFrame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot_SelectFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Slot_SelectFrame.png"), 1))))
		return E_FAIL;
#pragma endregion Slot

#pragma region MenuPage
	/* Prototype_Component_Texture_UI_MenuPageBG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MenuPageBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Options_BG.png"), 1))))
		return E_FAIL;
#pragma endregion MenuPage

#pragma region Weapon
	/* Prototype_Component_Texture_UI_WeaponRTop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponRTop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/RightTop.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WeaponEquipSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponEquipSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/EquipSlot.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WPFontaine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WPFontaine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/Fontaine_%d.png"), 3))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WPEquipNone */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WPEquipNone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/EquipSlot_None.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_WeaponTab */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponTab"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Weapon/Tab.png"), 1))))
		return E_FAIL;


#pragma endregion Weapon

	/* Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ShatteredCursor.png"), 1))))
		return E_FAIL;

#pragma endregion UI_Texture

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_UI()
{

	/* For.Prototype_Component_Shader_VtxUITex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUITex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

#pragma region UI_Obj
	// UI 객체 원형 생성

	// Mouse
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"), CMouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region HUD

#pragma region State
	/* For.Prototype_GameObject_UI_StateBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateBG"),
		CUI_StateBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateBar"),
		CUI_StateBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateHP*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateHP"),
		CUI_StateHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateEnergy*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateEnergy"),
		CUI_StateEnergy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateEther*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateEther"),
		CUI_StateEther::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_StateSoul*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StateSoul"),
		CUI_StateSoul::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_State*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_State"),
		CUIGroup_State::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion State

#pragma region WeaponSlot
	/* For.Prototype_GameObject_UI_WeaponSlotBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponSlotBG"),
		CUI_WeaponSlotBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WeaponSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponSlot"),
		CUI_WeaponSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_WeaponSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_WeaponSlot"),
		CUIGroup_WeaponSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion WeaponSlot

#pragma endregion HUD

#pragma region Menu
	/* For.Prototype_GameObject_UI_MenuBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuBG"),
		CUI_MenuBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuFontaine*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuFontaine"),
		CUI_MenuFontaine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuBtn*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuBtn"),
		CUI_MenuBtn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Menu_SelectFrame*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Menu_SelectFrame"),
		CUI_Menu_SelectFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UIGroup_Menu*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Menu"),
		CUIGroup_Menu::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Menu

#pragma region Quick
	/* For.Prototype_GameObject_UI_QuickBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickBG"),
		CUI_QuickBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickTop"),
		CUI_QuickTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickExplain*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickExplain"),
		CUI_QuickExplain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_QuickInvBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_QuickInvBG"),
		CUI_QuickInvBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Quick*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Quick"),
		CUIGroup_Quick::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Quick

#pragma region Character
	/* For.Prototype_GameObject_UIGroup_CharacterBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_CharacterBG"),
		CUI_CharacterBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_CharacterTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_CharacterTop"),
		CUI_CharacterTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Character*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Character"),
		CUIGroup_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Character

#pragma region Slot
	/* For.Prototype_GameObject_UI_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Slot"),
		CUI_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Slot_Frame*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Slot_Frame"),
		CUI_Slot_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Slot

#pragma region MenuPage
	/* For.Prototype_GameObject_UI_MenuPageBG*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuPageBG"),
		CUI_MenuPageBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_MenuPageTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MenuPageTop"),
		CUI_MenuPageTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion MenuPage

#pragma region Weapon
	/* For.Prototype_GameObject_UI_WeaponRTop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponRTop"),
		CUI_WeaponRTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPEquipSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPEquipSlot"),
		CUI_WPEquipSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPEquipNone*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPEquipNone"),
		CUI_WPEquipNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WPFontaine*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WPFontaine"),
		CUI_WPFontaine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_WeaponTab*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WeaponTab"),
		CUI_WeaponTab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIGroup_Weapon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Weapon"),
		CUIGroup_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Weapon

	/* For.Prototype_GameObject_UIGroup_Inventory*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIGroup_Inventory"),
		CUIGroup_Inventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion UI_Obj

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	
	Safe_Release(m_pUI_Manager);
	CUI_Manager::GetInstance()->DestroyInstance();
	//CUI_Manager::DestroyInstance();
	/* 레퍼런스 카운트를 0으로만든다. */





#ifdef _DEBUG

	m_pImGuiMgr->DestroyInstance();
	CImGuiMgr::DestroyInstance();
#endif // _DEBUG

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();



}

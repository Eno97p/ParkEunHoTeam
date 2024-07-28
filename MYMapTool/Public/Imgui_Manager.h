#pragma once

#include "Base.h"
#include "MYMapTool_Defines.h"
#include "..\Public\imgui.h"
#include "GameInstance.h"
#include "CutSceneCamera.h"

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	enum LIST_LAYER { LAYER_MONSTER, LAYER_PASSIVE_ELEMENT, LAYER_ACTIVE_ELEMENT, LAYER_TRIGGER, LAYER_ENVEFFECTS,
		LAYER_TRAP , LAYER_DECAL, LAYER_VEGETATION, LAYER_END };

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	_int	Get_ObjIdx() { return m_iObjIdx; }
	_int	Get_StageIdx() { return m_iStageIdx; }
	_int	Get_LayerIdx() { return m_iLayerIdx; }
	_int	Get_CellIdx() { return m_iCreateCellIdx; }

	_bool	Get_IsSave() { return m_IsSave; }
	void	Set_DontSave() { m_IsSave = false; }

	_bool	Get_IsPhysXSave() { return m_IsPhysXSave; }
	void	Set_DontPhysXSave() { m_IsPhysXSave = false; }

	_bool	Get_IsPhysXLoad() { return m_IsPhysXLoad; }
	void	Set_DontPhysXLoad() { m_IsPhysXLoad = false; }

	_bool	Get_IsDecalSave() { return m_IsDecalSave; }
	void	Set_DontDecalSave() { m_IsDecalSave = false; }

	_bool	Get_IsDecalLoad() { return m_IsDecalLoad; }
	void	Set_DontDecalLoad() { m_IsDecalLoad = false; }

	_bool	Get_IsLoad() { return m_IsLoad; }
	void	Set_DontLoad() { m_IsLoad = false; }

	_bool	Get_IsEffectsSave() { return m_IsEffectsSave; }
	void	Set_DontEffectsSave() { m_IsEffectsSave = false; }

	_bool	Get_IsEffectsLoad() { return m_IsEffectsLoad; }
	void	Set_DontEffectsLoad() { m_IsEffectsLoad = false; }

	_bool	Get_IsReLoad() { return m_IsTerrainReLoad; }
	void	Set_DontReLoad() { m_IsTerrainReLoad = false; }

	_int	Get_TerrainX() { return m_iTerrainX; }
	_int	Get_TerrainZ() { return m_iTerrainZ; }

	_bool	Get_IsNaviMode() { return m_IsNaviMode; }
	_bool	Get_IsNaviClear() { return m_IsNaviClear; }
	void	Set_IsNaviClear() { m_IsNaviClear = false; }

	_bool	Get_IsNaviDelete() { return m_isNaviDelete; }
	void	Set_IsNaviDelete() { m_isNaviDelete = false; }

	_float Get_TrapTimeOffset() { return m_fTrapTimeOffset; }
	_bool isAnimModel() { return m_bIsAnimModel; }
	_bool isDecoObject() { return m_bIsDecoObject; }


	_float3 Get_GlobalWindDir() { return m_GlobalWindDir; }
	_float Get_GlobalWindStrenth() { return m_fGlobalWindStrength; }


	_float3 Get_GrassTopCol() { return m_TopCol; }
	_float3 Get_GrassBotCol() { return m_BotCol; }
	_float3 Get_LeafCol() { return m_LeafCol; }
	_bool Get_TreeBloom() { return m_bTreeBloom; }

	void	Add_vecCreateObj(_char* pName) { m_vecCreateObj.emplace_back(pName); }
	void	Add_vecCreateCell(_int iIndex)
	{
		_char szName[MAX_PATH] = "";
		sprintf(szName, "%d", iIndex);
		m_vecCreateCell.emplace_back(szName);
	}
	void	Add_vecCreateCellSize();

	void	Clear_vecCell();


public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Render();

	void	EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
	void Add_ToolObj(_vector vPosition, _vector vColor);

private:
	void Light_Editor();
	ImTextureID DirectXTextureToImTextureID(_uint iIdx);

public:
	void Save_Lights();
	void Load_Lights();
	void Reset_Lights();

private:
	void Shadow_Editor();

	void Camera_Editor();

	void Terrain_Editor();
	void GlobalWind_Editor();
	void Fog_Editor();

private:
	wstring m_LightsDataPath = L""; //Lights 저장 경로

private:
	_bool	m_bShowWindow;
	_bool	m_bShowAnotherWindow;
	_bool	m_bLightWindow = false;
	_bool	m_bShadowWindow = false;
	_bool	m_bCameraWindow = false;
	_bool	m_bTerrainWindow = false;
	_bool	m_bGlobalWindWindow = false;
	_bool	m_bFogWindow = false;
	
	_bool	m_bShowDecalTextureWindow = false;

	_bool	m_IsSave = { false };
	_bool	m_IsLoad = { false };

	_bool	m_IsPhysXSave = { false };
	_bool	m_IsPhysXLoad = { false };

	_bool	m_IsEffectsSave = { false };
	_bool	m_IsEffectsLoad = { false };

	_bool	m_IsDecalSave = { false };
	_bool	m_IsDecalLoad = { false };

	_bool	m_IsTerrainReLoad = { false };

	_bool	m_IsNaviMode = { false };
	_bool	m_IsNaviClear = { false };
	_bool	m_isNaviDelete = { false };

private:
	_bool m_bAddToolObj = false;
	_bool g_bIsHovered = false;
	_bool m_bIsAnimModel = false;
	_bool m_bIsDecoObject = false;

	_float4 m_ClickedMousePos = { 0.f, 0.f, 0.f, 1.f };
	_int	m_selectedOption = 0; // 선택된 옵션을 저장할 변수

	_bool m_bTreeBloom = false;
private:

	// Imguizmo
	_bool	useWindow = { false };
	_int	gizmoCount = 1;

	_int		m_iObjIdx; // Obj Index
	_int		m_iStageIdx; // 스테이지 Index
	_int		m_iLayerIdx; // Layer Index
	_int		m_iCreateObjIdx = { 0 }; // 생성된 Obj Index
	_int		m_iCreateCellIdx = { 0 }; // 생성된 Cell Index

	_int		m_iTerrainX = { 512 };
	_int		m_iTerrainZ = { 512 };

	vector<_char*>	m_vecCreateObj;			// 생성된 ToolObj 벡터
	vector<_char*>	m_vecCreateCell;		// 생성된 Cell 벡터

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };


	CTexture* m_pDecalTexs = { nullptr };
private:
	_float m_fTrapTimeOffset = 0.f;
private:
	// Imgui_Manager 클래스에 멤버 변수 추가
	XMFLOAT4 m_DirectionalLightDirection{ 0.f, -1.f, 0.0f, 0.0f };
	XMFLOAT4 m_SpotlightDirection{ 0.f, -1.f, 0.0f, 0.0f };

private:
	_float3				m_TopCol = { 0.f, 1.f, 0.f };
	_float3				m_BotCol = { 0.f, 0.f, 0.f };
	_float3				m_LeafCol = { 0.f, 0.f, 0.f };

	_float3 m_GlobalWindDir = _float3(1.0f, 0.0f, 0.5f);
	_float m_fGlobalWindStrength = 1.f;
	_float m_fGlobalWindFrequency = 1.f;

private:
	vector<CCamera::CameraKeyFrame>	m_vCameraKeyFrames;
	_float m_fKeyFrameTime = 0.f;
	_int m_iSelectedKeyFrame = 0;
private:
	void	Setting_ObjListBox(_int iLayerIdx);



	void	Setting_CreateObj_ListBox();
	void	Delete_Obj();



public:
	virtual void Free() override;
};


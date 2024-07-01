#include"stdafx.h"

#include "GameInstance.h"
#include"Object_Manager.h"
#include"CImGuiMgr.h"

#pragma region LEVEL_HEADER
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "Level_GamePlay.h"
#pragma endregion LEVEL_HEADER

#ifdef _DEBUG
IMPLEMENT_SINGLETON(CImGuiMgr)

CImGuiMgr::CImGuiMgr()
{
}


HRESULT CImGuiMgr::Ready_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.MousePos;


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);


	

	return S_OK;
}

HRESULT CImGuiMgr::Render_ImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	Update_ImGui();

	ImGui::Render();

	return S_OK;
}

HRESULT CImGuiMgr::Update_ImGui()
{
	ImGui::Begin("Property", nullptr, ImGuiWindowFlags_HorizontalScrollbar/*| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize*/);
	ImGui::Text("Frame:%f", ImGui::GetIO().Framerate);
	Render_MainMenu();















	ImGui::End();
	return S_OK;
}

HRESULT CImGuiMgr::End_ImGui()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	return S_OK;
}

void CImGuiMgr::Render_MainMenu()
{
	LEVEL NowLevel = (LEVEL)m_pGameInstance->Get_CurrentLevel();

	//m_pGameInstance->Get_Layer(NowLevel);


	unordered_map<LEVEL, const char*> LevelName = []() {
		unordered_map<LEVEL, const char*> map;
		for (int i = 0; i < sizeof(Client::LevelNames) / sizeof(Client::LevelNames[0]); ++i)
		{
			map[static_cast<LEVEL>(i)] = Client::LevelNames[i];
		}
		return map;
		}();


	vector<const char*> level_names;
	for (const auto& level : LevelName)
	{
		level_names.push_back(level.second);
	}
	int current_level_index = 0;
	for (const auto& level : LevelName)
	{
		if (level.first == NowLevel)
		{
			break;
		}
		current_level_index++;
		m_iCurrentLevel = current_level_index;
	}
	
	

	//list<CGameObject*> ObjectLis;


	//m_pGameInstance->Get_GameObjects_Ref(current_level_index, m_LayerTags[0], &ObjectLis);
		
	
	if (ImGui::Combo("Levels", &current_level_index, level_names.data(), level_names.size())) 
	{
		
		
		auto it = LevelName.begin();
		std::advance(it, current_level_index);
		// �����ϸ� �ش� ���� �ε����� ���ͷ����͸� �̵�
		LEVEL selected_level = it->first;
		if (NowLevel != selected_level)
		{
			// ���� ���� ����
			
			switch (selected_level)
			{
			case LEVEL_STATIC:
	
				break;
			case LEVEL_LOADING:
				break;
			case LEVEL_LOGO:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				break;
				
			case LEVEL_GAMEPLAY:
				if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
				{
					MSG_BOX("IMGUI::Failed to Open Level");
					return;
				}
				
				break;
			case LEVEL_END:
				
				break;
			default:
				break;
			}
			NowLevel = selected_level;
			return;
			
		}
	}

	m_pGameInstance->Get_LayerTags_String(current_level_index, &m_LayerTags);
	
	Render_Layer();





}

void CImGuiMgr::Render_Layer()
{
	list<CGameObject*> ObjectLis;
	//m_LayerTags


	ImGui::Combo("Layers", &m_iCurrentLayer, m_LayerTags.data(), m_LayerTags.size());
	//if (ImGui::Combo("Layers", &m_iCurrentLayer,m_LayerTags.data(), m_LayerTags.size()))
	{
		
		if (m_iCurrentLayer >= 0 && m_iCurrentLayer < m_LayerTags.size())
		{

		}
	}
	if(m_LayerTags.empty())
		return;

	m_wstrLayerTag = Client::const_char_to_wstring(m_LayerTags[m_iCurrentLayer]);
	ObjectLis = m_pGameInstance->Get_GameObjects_Ref(m_iCurrentLevel, m_wstrLayerTag);
	Render_Object(ObjectLis);





}

void CImGuiMgr::Render_Object(list<CGameObject*>& listGameObject)
{
	size_t iObjectCount = listGameObject.size();
	auto iter = listGameObject.begin();
	
	


	if(m_LayerTags.empty())
		return;

	vector<string> ObjectNamesString;


	for (size_t i = 0; i < iObjectCount; ++i, ++iter)
	{
		const std::type_info& info = typeid((**iter));
		const char* szClassname = info.name();
		const char* szClassCutName= strstr(szClassname, "class");
		if (szClassCutName)
			szClassname = szClassCutName + 6;
		else
			szClassname = szClassCutName;

		string FullClassName = string(szClassname) + " :" + to_string(i);

		ObjectNamesString.push_back(FullClassName);

	}
	vector<const char*> ObjectNames;

	ObjectNames.reserve(ObjectNamesString.size());
	for (auto& ObjectName : ObjectNamesString)
	{
		ObjectNames.push_back(ObjectName.c_str());
	}


	
	//CGameObject* pGameObject = nullptr;
	ImGui::Combo("Objects", &m_iCurrentObject, ObjectNames.data(), ObjectNames.size());

	iter = listGameObject.begin();

	if (m_iCurrentObject >= listGameObject.size())
	{
		m_iCurrentObject = 0;
	}


	std::advance(iter, m_iCurrentObject);
	m_pSelectedObject = *iter;




		//if()

	Render_Component(m_pSelectedObject);




}

void CImGuiMgr::Render_Component(CGameObject* pGameObject)
{
	vector<const char*> ComponentNames;
	

	if (nullptr == pGameObject)
		return;

	map<const wstring, CComponent*> Components;
	Components = pGameObject->Get_Components();


	m_ComponentNames.clear();
	m_ComponentNames.reserve(Components.size());

	for (auto& Component : Components)
	{
		string ComponentName = wstring_to_string(Component.first);
		m_ComponentNames.push_back(ComponentName);
		ComponentNames.push_back(m_ComponentNames.back().c_str());

	}

	
	if (ImGui::Combo("Components", &m_iCurrentComponent, ComponentNames.data(), ComponentNames.size()))
	{

		wstring ComponentTag = Client::const_char_to_wstring(ComponentNames[m_iCurrentComponent]);
		m_bComponentPanel = true;
		//Render_Component_Properties(Components[ComponentTag]);
	}
	if(m_iCurrentComponent >= ComponentNames.size())
	{
		m_iCurrentComponent = 0;
	}

	Render_Component_Properties(Components[Client::const_char_to_wstring(ComponentNames[m_iCurrentComponent])], ComponentNames[m_iCurrentComponent]);

}

void CImGuiMgr::Render_Component_Properties(CComponent* pComponent, const char* szComponentName)
{

	//������Ʈ���� �ٸ� �����͸� ��ȯ�ϰ� ����
	//���� �����Ϳ� �´� UI�� �׷������


	if (m_bComponentPanel)
	{
		string ComponentName = string(szComponentName)+ " Properties";

		ImGui::Begin(ComponentName.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::Button("Close"))
		{
			m_bComponentPanel = false;
		}
		ImGui::End();
	}

	//Test

	//void* pComponentData = nullptr;

//pComponentData=	Components[ComponentTag]->GetData();


//if(nullptr!=pComponentData)
//{
//	if (ComponentTag == TEXT("Com_Transform"))
//	{
//		CTransform::TransformDesc* tDesc = static_cast<CTransform::TransformDesc*>(pComponentData);
//		//tDesc
//		//_float4x4* pWorldMatrix = static_cast<_float4x4*>(pComponentData);
//		int tmep = 0;

//	}
//	
//}

//auto iter = Components.begin();
//std::advance(iter, iCurrentComponent);
//CComponent* pComponent = iter->second;






}


void CImGuiMgr::Free()
{
	Free_LayerTags_String(&m_LayerTags);
	Free_String(&m_wstrLayerTag);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();


	ImGui::DestroyContext();



}

#endif // _DEBUG
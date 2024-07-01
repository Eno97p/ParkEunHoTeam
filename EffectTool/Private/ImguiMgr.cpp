#pragma once
#include "ImguiMgr.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameInstance.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"
#include "Particle_Trail.h"
#include "PartObject.h"
#include "Player.h"




CImguiMgr::CImguiMgr()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

wstring CImguiMgr::utf8_to_wstring(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

HRESULT CImguiMgr::Add_Texture_Prototype(const wstring& path, const wstring& name)
{
	if (m_pGameInstance->IsPrototype(m_pGameInstance->Get_CurrentLevel(), name) == true)
	{
		m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), name));
		return S_OK;
	}
	else
	{
		if (FAILED(m_pGameInstance->Add_Prototype(m_pGameInstance->Get_CurrentLevel(), name,
			CTexture::Create(m_pDevice, m_pContext, path, 1))))
			return E_FAIL;
		m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), name));
	}
	return S_OK;
}

HRESULT CImguiMgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::StyleColorsDark();

	m_pDevice = pDevice;
	m_pContext = pContext;

	return S_OK;
}

ImTextureID CImguiMgr::DirectXTextureToImTextureID()
{
	ID3D11ShaderResourceView* SRV = m_pTextureCom->Get_Texture();
	return reinterpret_cast<ImTextureID>(SRV);
}

PARTICLETYPE CImguiMgr::EffectListBox(PARTICLETYPE type, void** vDesc)
{
	ImGui::Begin("Effect_List");
	ImVec2 list_box_size = ImVec2(-1, 200);
	static bool HeaderOpen[3] = { false,false,false };
	static int current_item[3] = { 0,0,0 };

#pragma region POINT
	if (ImGui::CollapsingHeader("Point_Lists", HeaderOpen[0]))
	{
		HeaderOpen[1] = false;
		HeaderOpen[2] = false;
		if (PointEffects.size() != PointEffectsNames.size())
		{
			MSG_BOX("Vector Size Error");
			HeaderOpen[0] = false;
		}

		if (ImGui::BeginListBox("Point List Box", list_box_size))
		{
			for (int i = 0; i < PointEffectsNames.size(); ++i)
			{
				const bool is_selected = (current_item[0] == i);
				if (ImGui::Selectable(PointEffectsNames[i].c_str(), is_selected))
				{
					current_item[0] = i;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (current_item[0] >= 0 && current_item[0] < PointEffects.size())
		{
			if (ImGui::Button("Add", ImVec2(100, 30)))
			{
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticlePoint"), PointEffects[current_item[0]]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load this", ImVec2(100, 30)))
			{
				*vDesc = PointEffects[current_item[0]];
				ChangedDesc = true;
				ImGui::End();
				return PART_POINT;
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit", ImVec2(100, 30)))
			{
				PointEffects[current_item[0]] = *vDesc;
			}
			
			if (ImGui::Button("Erase", ImVec2(100, 30)))
			{
				
				Safe_Delete(PointEffects[current_item[0]]);
				PointEffects.erase(PointEffects.begin() + current_item[0]);
				PointEffectsNames.erase(PointEffectsNames.begin() + current_item[0]);

				if (current_item[0] >= PointEffects.size())
					current_item[0] = PointEffects.size() - 1;

			}
			ImGui::SameLine();
			
			if (ImGui::Button("Erase All", ImVec2(100, 30)))
			{
				for (auto& iter : PointEffects)
					Safe_Delete(iter);
				PointEffects.clear();
				PointEffectsNames.clear();
				current_item[0] = 0;
			}

		}


	}
#pragma endregion POINT
#pragma region MESH
	if (ImGui::CollapsingHeader("Mesh_Lists", HeaderOpen[1]))
	{
		if (MeshEffects.size() != MeshEffectsNames.size())
		{
			MSG_BOX("Vector Size Error");
			HeaderOpen[1] = false;
		}

		if (ImGui::BeginListBox("Mesh List Box", list_box_size))
		{
			for (int i = 0; i < MeshEffectsNames.size(); ++i)
			{
				const bool is_selected = (current_item[1] == i);
				if (ImGui::Selectable(MeshEffectsNames[i].c_str(), is_selected))
				{
					current_item[1] = i;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (current_item[1] >= 0 && current_item[1] < MeshEffects.size())
		{
			if (ImGui::Button("Add", ImVec2(100, 30)))
			{
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticleMesh"), MeshEffects[current_item[1]]);
			}
			if (ImGui::Button("Load this", ImVec2(100, 30)))
			{
				*vDesc = MeshEffects[current_item[1]];
				ChangedDesc = true;
				ImGui::End();
				return PART_MESH;
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit", ImVec2(100, 30)))
			{
				MeshEffects[current_item[1]] = *vDesc;
			}
		
			if (ImGui::Button("Erase", ImVec2(100, 30)))
			{

				Safe_Delete(MeshEffects[current_item[1]]);
				MeshEffects.erase(MeshEffects.begin() + current_item[1]);
				MeshEffectsNames.erase(MeshEffectsNames.begin() + current_item[1]);

				if (current_item[1] >= MeshEffects.size())
					current_item[1] = MeshEffects.size() - 1;

			}
			ImGui::SameLine();

			if (ImGui::Button("Erase All", ImVec2(100, 30)))
			{
				for (auto& iter : MeshEffects)
					Safe_Delete(iter);
				MeshEffects.clear();
				MeshEffectsNames.clear();
				current_item[1] = 0;
			}

		}
		HeaderOpen[0] = false;
		HeaderOpen[2] = false;
	}
#pragma endregion MESH
#pragma region RECT
	if (ImGui::CollapsingHeader("Rect_Lists", HeaderOpen[2]))
	{
		if (RectEffects.size() != RectEffectsNames.size())
		{
			MSG_BOX("Vector Size Error");
			HeaderOpen[2] = false;
		}

		HeaderOpen[0] = false;
		HeaderOpen[1] = false;
	}
#pragma endregion RECT
	ImGui::End();

	return type;
}

HRESULT CImguiMgr::Store_Effects(char* Name, PARTICLETYPE type, void* Arg)
{
	string sName = Name;

	switch (type)
	{
	case Effect::PART_POINT:
	{
		CParticle_Point::PARTICLEPOINT* point = new CParticle_Point::PARTICLEPOINT;
		*point = *((CParticle_Point::PARTICLEPOINT*)Arg);
		PointEffects.emplace_back(point);
		PointEffectsNames.emplace_back(sName);
		break;
	}
	case Effect::PART_MESH:
	{
		CParticleMesh::PARTICLEMESH* mesh = new CParticleMesh::PARTICLEMESH;
		*mesh = *((CParticleMesh::PARTICLEMESH*)Arg);
		MeshEffects.emplace_back(mesh);
		MeshEffectsNames.emplace_back(sName);
		break;
	}
	case Effect::PART_END:
	{
		break;
	}
	default:
		return E_FAIL;
		break;
	}


	return S_OK;
}

HRESULT CImguiMgr::Save_PointsList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Points.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = PointEffects.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : PointEffects)
	{
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->eType, sizeof(EFFECTTYPE));
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		save_wstring_to_stream(((CParticle_Point::PARTICLEPOINT*)iter)->Texture, file);
		save_wstring_to_stream(((CParticle_Point::PARTICLEPOINT*)iter)->TexturePath, file);
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->vStartColor, sizeof(_float3));
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->vEndColor, sizeof(_float3));
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->isColored, sizeof(_bool));
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->IsBlur, sizeof(_bool));
		file.write((char*)&((CParticle_Point::PARTICLEPOINT*)iter)->Desolve, sizeof(_bool));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Points.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : PointEffectsNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Points.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < PointEffectsNames.size(); ++i)
	{
		NumberFile << i << ". " << PointEffectsNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_PointsList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Points.Bin";
	ifstream inFile(finalPath, std::ios::binary);

	if (!inFile.good())	
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : PointEffects)
		Safe_Delete(iter);
	PointEffects.clear();
	PointEffectsNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CParticle_Point::PARTICLEPOINT* readFile = new CParticle_Point::PARTICLEPOINT;
		inFile.read((char*)&readFile->eType, sizeof(EFFECTTYPE));
		inFile.read((char*)&readFile->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);

		if (!m_pGameInstance->IsPrototype(m_pGameInstance->Get_CurrentLevel(), readFile->Texture))
			Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);

		inFile.read((char*)&readFile->vStartColor, sizeof(_float3));
		inFile.read((char*)&readFile->vEndColor, sizeof(_float3));
		inFile.read((char*)&readFile->isColored, sizeof(_bool));
		inFile.read((char*)&readFile->IsBlur, sizeof(_bool));
		inFile.read((char*)&readFile->Desolve, sizeof(_bool));
		readFile->vStartPos = _float4(0.f, 0.f, 0.f, 1.f);
		PointEffects.emplace_back(readFile);
	}
	inFile.close();
	

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Points.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		PointEffectsNames.emplace_back(str);
	}
	NameFile.close();
	

	return S_OK;
}

HRESULT CImguiMgr::Save_MeshList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Mesh.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = MeshEffects.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : MeshEffects)
	{
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->eType, sizeof(EFFECTTYPE));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->vStartColor, sizeof(_float3));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->vEndColor, sizeof(_float3));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->eModelType, sizeof(EFFECTTYPE));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->IsBlur, sizeof(_bool));
		file.write((char*)&((CParticleMesh::PARTICLEMESH*)iter)->Desolve, sizeof(_bool));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Mesh.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : MeshEffectsNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Mesh.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < MeshEffectsNames.size(); ++i)
	{
		NumberFile << i << ". " << MeshEffectsNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_MeshList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Mesh.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : MeshEffects)
		Safe_Delete(iter);
	MeshEffects.clear();
	MeshEffectsNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CParticleMesh::PARTICLEMESH* readFile = new CParticleMesh::PARTICLEMESH;
		inFile.read((char*)&readFile->eType, sizeof(EFFECTTYPE));
		inFile.read((char*)&readFile->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		inFile.read((char*)&readFile->vStartColor, sizeof(_float3));
		inFile.read((char*)&readFile->vEndColor, sizeof(_float3));
		inFile.read((char*)&readFile->eModelType, sizeof(EFFECTTYPE));
		inFile.read((char*)&readFile->IsBlur, sizeof(_bool));
		inFile.read((char*)&readFile->Desolve, sizeof(_bool));
		readFile->vStartPos = _float4(0.f, 0.f, 0.f, 1.f);

		MeshEffects.emplace_back(readFile);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Mesh.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		MeshEffectsNames.emplace_back(str);
	}
	NameFile.close();


	return S_OK;
}

void CImguiMgr::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Visible_Data();
	//코드
	//EffectTool();


	ImGui::Render();


	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImguiMgr::Tick(_float fTimiedelta)
{
}

void CImguiMgr::Visible_Data()
{
	ImGui::Begin("DATA");
	ImGui::Text("Frame : %f", ImGui::GetIO().Framerate);

	static _bool bShow[3] = { false,false,false };
	ImGui::Checkbox("Texture_FileSystem", &bShow[0]);
	if (bShow[0] == true)
		Load_Texture();
	ImGui::Checkbox("Effect_Tool", &bShow[1]);
	if (bShow[1] == true)
		EffectTool();
	ImGui::Checkbox("Trail_Tool", &bShow[2]);
	if (bShow[2] == true)
	{
		if (TrailMat == nullptr)
		{
			MSG_BOX("행렬을 먼저 대입해주세요");
			bShow[2] = false;
		}
		else
		{
			Trail_Tool();
		}
	}
	if (ImGui::Button("Bind_Sword_Matrix"))
	{
		CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
		TrailMat = pPlayer->Get_WeaponMat();
	}
	if (ImGui::Button("Bind_Player_Matrix"))
	{
		CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
		TrailMat = pPlayer->Get_WorldMat();
	}
	ImGui::End();
}

void CImguiMgr::EffectTool()
{
	ImGui::Begin("Effect_Editor");
	static CVIBuffer_Instance::INSTANCE_DESC instDesc = {};
	static CParticleMesh::PARTICLEMESH classDesc = {};
	static CParticle_Point::PARTICLEPOINT pointDesc = {};

#pragma region FuncType
	static EFFECTTYPE FuncType = SPREAD;
	static _float vStartColor[3] = { 1.0f, 1.0f, 1.0f };
	static _float vEndColor[3] = { 1.0f, 1.0f, 1.0f };
	static PARTICLETYPE PartType = PART_POINT;
	static void* voidDesc = nullptr;

	if (ChangedDesc == true)
	{
		switch (PartType)
		{
		case Effect::PART_POINT:
			pointDesc = *((CParticle_Point::PARTICLEPOINT*)voidDesc);
			FuncType = pointDesc.eType;
			instDesc = pointDesc.InstanceDesc;
			vStartColor[0] = pointDesc.vStartColor.x;
			vStartColor[1] = pointDesc.vStartColor.y;
			vStartColor[2] = pointDesc.vStartColor.z;
			vEndColor[0] = pointDesc.vEndColor.x;
			vEndColor[1] = pointDesc.vEndColor.y;
			vEndColor[2] = pointDesc.vEndColor.z;
			m_pTextureCom = static_cast<CTexture*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), m_pTextureProtoName));
			break;
		case Effect::PART_MESH:
			classDesc = *((CParticleMesh::PARTICLEMESH*)voidDesc);
			FuncType = classDesc.eType;
			instDesc = classDesc.InstanceDesc;
			vStartColor[0] = classDesc.vStartColor.x;
			vStartColor[1] = classDesc.vStartColor.y;
			vStartColor[2] = classDesc.vStartColor.z;
			vEndColor[0] = classDesc.vEndColor.x;
			vEndColor[1] = classDesc.vEndColor.y;
			vEndColor[2] = classDesc.vEndColor.z;
			break;
		case Effect::PART_END:
			break;
		default:
			break;
		}
		ChangedDesc = false;
	}



	if (ImGui::RadioButton("Spread", FuncType == SPREAD))
	{
		FuncType = SPREAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Drop", FuncType == DROP))
	{
		FuncType = DROP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOut", FuncType == GROWOUT))
	{
		FuncType = GROWOUT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Tornado", FuncType == TORNADO))
	{
		FuncType = TORNADO;
	}
	if (ImGui::RadioButton("Spread_SizeUp", FuncType == SPREAD_SIZEUP))
	{
		FuncType = SPREAD_SIZEUP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_NonRotation", FuncType == SPREAD_NONROTATION))
	{
		FuncType = SPREAD_NONROTATION;
	}


	if (ImGui::RadioButton("Spread_SpeedDown", FuncType == SPREAD_SPEED_DOWN))
	{
		FuncType = SPREAD_SPEED_DOWN;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("SlashEffect", FuncType == SLASH_EFFECT))
	{
		FuncType = SLASH_EFFECT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_SpeedDown_SizeUp", FuncType == SPREAD_SPEED_DOWN_SIZE_UP))
	{
		FuncType = SPREAD_SPEED_DOWN_SIZE_UP;
	}
	if (ImGui::RadioButton("Gather", FuncType == GATHER))
	{
		FuncType = GATHER;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Extinction", FuncType == EXTINCTION))
	{
		FuncType = EXTINCTION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOutY", FuncType == GROWOUTY))
	{
		FuncType = GROWOUTY;
	}
#pragma endregion FuncType

#pragma region PARTICLETYPE

	if (ImGui::RadioButton("Point", PartType == PART_POINT))
	{
		PartType = PART_POINT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Mesh", PartType == PART_MESH))
	{
		PartType = PART_MESH;
	}
	ImGui::Text("FunctionType :");


	if (PartType == PART_POINT)
	{
		if (ImGui::CollapsingHeader("Point Settings"))
		{
			// Add your point settings UI elements here
			ImGui::Text("Point specific settings");
			if (m_pTextureProtoName == TEXT(""))
				MSG_BOX("텍스쳐를 먼저 선택해주세요");
			else
			{
				pointDesc.Texture = m_pTextureProtoName;
				pointDesc.TexturePath = m_pTextureFilePath;
			}
			ImGui::Checkbox("Blur", &pointDesc.IsBlur);
			ImGui::Checkbox("Desolve", &pointDesc.Desolve);
			

			ImGui::Checkbox("ColorMap", &pointDesc.isColored);
			if (pointDesc.isColored == true)
			{
				if (ImGui::ColorEdit3("Start_Color", vStartColor))
				{
					pointDesc.vStartColor = _float3(vStartColor[0], vStartColor[1], vStartColor[2]);
				}
				
				if (ImGui::ColorEdit3("End_Color", vEndColor))
				{
					pointDesc.vEndColor = _float3(vEndColor[0], vEndColor[1], vEndColor[2]);
				}
			}
			pointDesc.eType = FuncType;
		}
	}
	else if (PartType == PART_MESH)
	{
		if (ImGui::CollapsingHeader("Mesh Settings"))
		{
			ImGui::Text("Mesh specific settings");
			ImGui::Text("MeshType :");
			ImGui::SameLine();
			if (ImGui::RadioButton("Cube", classDesc.eModelType == CUBE))
			{
				classDesc.eModelType = CUBE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Circle", classDesc.eModelType == CIRCLE))
			{
				classDesc.eModelType = CIRCLE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Slash", classDesc.eModelType == SLASH))
			{
				classDesc.eModelType = SLASH;
			}



			
			if (ImGui::ColorEdit3("Start_Color", vStartColor))
			{
				classDesc.vStartColor = _float3(vStartColor[0], vStartColor[1], vStartColor[2]);
			}
			if (ImGui::ColorEdit3("End_Color", vEndColor))
			{
				classDesc.vEndColor = _float3(vEndColor[0], vEndColor[1], vEndColor[2]);
			}

			ImGui::Checkbox("Blur", &classDesc.IsBlur);
			ImGui::Checkbox("Desolve", &classDesc.Desolve);
			if (ImGui::InputInt("DesolveTexNum", &classDesc.DesolveNum))
			{
				if (classDesc.DesolveNum < 0)
					classDesc.DesolveNum = 0;
				if (classDesc.DesolveNum > 15)
					classDesc.DesolveNum = 15;
			}
			classDesc.eType = FuncType;
		}
	}

#pragma endregion PARTICLETYPE

#pragma region INSTANCEDESC
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &instDesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&instDesc.vOffsetPos));
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&instDesc.vPivotPos));
	ImGui::InputFloat3("Range", reinterpret_cast<float*>(&instDesc.vRange));
	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&instDesc.vSize));
	ImGui::InputFloat2("Speed", reinterpret_cast<float*>(&instDesc.vSpeed));
	ImGui::InputFloat2("LifeTime", reinterpret_cast<float*>(&instDesc.vLifeTime));
	ImGui::InputFloat2("Gravity", reinterpret_cast<float*>(&instDesc.vGravity));
	static _float4 fStartPos = { 0.f,0.f,0.f,1.f };
	ImGui::InputFloat4("StartPosition", reinterpret_cast<float*>(&fStartPos));
	ImGui::Checkbox("IsLoop", &instDesc.isLoop);
#pragma endregion INSTANCEDESC


	classDesc.InstanceDesc = instDesc;	//메쉬
	pointDesc.InstanceDesc = instDesc;

	classDesc.vStartPos = fStartPos;
	pointDesc.vStartPos = fStartPos;


#pragma region BUTTONS
	if (ImGui::Button("Add", ImVec2(100, 30)))	//이펙트 생성버튼
	{
		switch (PartType)
		{
		case Effect::PART_POINT:
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticlePoint"), &pointDesc);
			break;
		}
		case Effect::PART_MESH:
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticleMesh"), &classDesc);
			break;
		case Effect::PART_END:
			break;
		default:
			break;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("EraseEffects", ImVec2(100, 30)))		//이펙트 레이어 삭제
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Effect"));
	}
	
	
	switch (PartType)
	{
	case Effect::PART_POINT:
		voidDesc = &pointDesc;
		break;
	case Effect::PART_MESH:
		voidDesc = &classDesc;
		break;
	case Effect::PART_END:
		break;
	default:
		break;
	}


	static char effectname[256] = "";
	//ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name :");
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));

	if (ImGui::Button("Store", ImVec2(50, 30)))		//현재 설정되어 있는 이펙트를 리스트에 추가
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Effects(effectname, PartType, voidDesc);
		}
	}

	if (ImGui::Button("Save", ImVec2(100, 30)))
	{
		HRESULT hr;
		hr = Save_PointsList();
		if (FAILED(hr))
			MSG_BOX("Failed Save Effects");
		hr = Save_MeshList();
		if (FAILED(hr))
			MSG_BOX("Failed Save Effects");

		if(hr == S_OK)
			MSG_BOX("Succeed Save Effects");

	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(100, 30)))
	{
		HRESULT hr;
		hr = Load_PointsList();
		if (FAILED(hr))
			MSG_BOX("Failed Load Effects");
		hr = Load_MeshList();
		if (FAILED(hr))
			MSG_BOX("Failed Load Effects");

		if (hr == S_OK)
			MSG_BOX("Succeed Load Effects");

	}

#pragma endregion BUTTONS

	PartType = EffectListBox(PartType, &voidDesc);
	ImGui::End();
}

void CImguiMgr::Load_Texture()
{
	ImGui::Begin("File Browser");

	static std::string currentPath = "../../Client/Bin/Resources/Textures/";
	static std::string selectedFile = "";
	static std::string fullPath = "";

	std::vector<std::string> files = GetFilesInDirectory(currentPath);

	if (ImGui::Button("..")) {
		currentPath = std::filesystem::path(currentPath).parent_path().string();
	}

	if (ImGui::BeginListBox("##file_list", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
		for (const auto& file : files) {
			std::string fileFullPath = currentPath + "/" + file;
			if (std::filesystem::is_directory(fileFullPath)) {
				if (ImGui::Selectable((file + "/").c_str())) {
					currentPath = fileFullPath;
				}
			}
			else {
				if (ImGui::Selectable(file.c_str())) {
					selectedFile = file;
					fullPath = fileFullPath;
				}
			}
		}
		ImGui::EndListBox();
	}

	if (IsPNGFile(selectedFile)) {
		string selectedFilePath = fullPath;
		wstring wPath = utf8_to_wstring(selectedFilePath);
		wstring wName = utf8_to_wstring(selectedFile);
		Add_Texture_Prototype(wPath, wName);
		if (m_pTextureCom != nullptr)
		{
			if (ImGui::ImageButton(DirectXTextureToImTextureID(), ImVec2(300.f, 300.f), ImVec2(0.f, 0.f)))
			{
				m_pTextureProtoName = wName;
				m_pTextureFilePath = wPath;
			}
		}

	}

	ImGui::End();
}

void CImguiMgr::Trail_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	
	ImGui::Begin("Trail_Effect_Editor");
	static CParticle_Trail::TRAIL_DESC classdesc{};
	static CVIBuffer_Trail::TRAILDESC traildesc{};

	if (ChangedTrail == true)
	{
		traildesc = classdesc.traildesc;
		traildesc.ParentMat = TrailMat;
		ChangedTrail = false;
	}


	if (ImGui::RadioButton("SwordTrail", classdesc.eFuncType == TRAIL_EXTINCT))
		classdesc.eFuncType = TRAIL_EXTINCT;
	ImGui::SameLine();
	if (ImGui::RadioButton("EternalTrail", classdesc.eFuncType == TRAIL_ETERNAL))
		classdesc.eFuncType = TRAIL_ETERNAL;
	
	if (ImGui::RadioButton("Usage_Sword", classdesc.eUsage == USAGE_SWORD))
		classdesc.eUsage = USAGE_SWORD;
	ImGui::SameLine();
	if (ImGui::RadioButton("Other", classdesc.eUsage == USAGE_EYE))
		classdesc.eUsage = USAGE_EYE;
	
	ImGui::ColorEdit3("Start_Color", reinterpret_cast<float*>(&classdesc.vStartColor));
	ImGui::ColorEdit3("End_Color", reinterpret_cast<float*>(&classdesc.vEndColor));
	traildesc.ParentMat = TrailMat;
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &traildesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&traildesc.vPivotPos));
	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&traildesc.vSize));
	ImGui::InputFloat("Speed", &traildesc.vSpeed);
	ImGui::InputFloat("lifetime", &traildesc.fLifeTime);
	ImGui::Checkbox("IsLoop", &traildesc.IsLoop);
	ImGui::Checkbox("Alpha", &classdesc.Alpha);
	ImGui::Checkbox("Desolve", &classdesc.Desolve);
	if (classdesc.Desolve == true)
	{
		ImGui::InputInt("DesolveTextureNum", &classdesc.DesolveNum);
	}
	ImGui::Checkbox("Blur", &classdesc.Blur);
	if (classdesc.Blur == true)
	{
		ImGui::InputFloat("BloomPower", &classdesc.fBloompower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&classdesc.vBloomColor));
	}
	classdesc.traildesc = traildesc;

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Trail"),
			TEXT("Prototype_GameObject_Trail"), &classdesc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Trail"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ButtonSize))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Trails(effectname, &classdesc);
		}
	}
	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_TrailList()))
		{
			MSG_BOX("Failed_Save_Trail");
		}
		else
			MSG_BOX("Succeed_Save_Trail");
	}
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_TrailList()))
			MSG_BOX("Failed_Load_Trail");
		else
			MSG_BOX("Succeed_Load_Trail");
	}
	Trail_ListBox(&classdesc);

	ImGui::End();
}

void CImguiMgr::Trail_ListBox(CParticle_Trail::TRAIL_DESC* vDesc)
{
	if (TrailEffects.size() < 1)
		return;

	if (TrailEffects.size() != TrailEffectsNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Trail_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Trail_List", list_box_size))
	{
		for (int i = 0; i < TrailEffectsNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(TrailEffectsNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < TrailEffects.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			CParticle_Trail::TRAIL_DESC* traildesc = TrailEffects[current_item].get();
			traildesc->traildesc.ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail"), traildesc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*vDesc = *TrailEffects[current_item].get();
			ChangedTrail = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			*TrailEffects[current_item] = *((CParticle_Trail::TRAIL_DESC*)vDesc);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			TrailEffects[current_item].reset();
			TrailEffects.erase(TrailEffects.begin() + current_item);
			TrailEffectsNames.erase(TrailEffectsNames.begin() + current_item);

			if (current_item >= TrailEffects.size())
				current_item = TrailEffects.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : TrailEffects)
				iter.reset();
			TrailEffects.clear();
			TrailEffectsNames.clear();
			current_item = 0;
		}

	}

	ImGui::End();
}

HRESULT CImguiMgr::Store_Trails(char* Name, void* Value)
{
	string sName = Name;
	CParticle_Trail::TRAIL_DESC* rawPtr = reinterpret_cast<CParticle_Trail::TRAIL_DESC*>(Value);
	shared_ptr<CParticle_Trail::TRAIL_DESC> traildesc = make_shared<CParticle_Trail::TRAIL_DESC>(*rawPtr);
	TrailEffects.emplace_back(traildesc);
	TrailEffectsNames.emplace_back(sName);
	return S_OK;
}

HRESULT CImguiMgr::Save_TrailList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Trail.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = TrailEffects.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& iter : TrailEffects)
	{
		file.write((char*)&iter->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		file.write((char*)&iter->vStartColor, sizeof(_float3));
		file.write((char*)&iter->vEndColor, sizeof(_float3));
		file.write((char*)&iter->vBloomColor, sizeof(_float3));
		file.write((char*)&iter->fBloompower, sizeof(_float));
		file.write((char*)&iter->Desolve, sizeof(_bool));
		file.write((char*)&iter->Blur, sizeof(_bool));
		file.write((char*)&iter->Alpha, sizeof(_bool));
		file.write((char*)&iter->eFuncType, sizeof(TRAILFUNCTYPE));
		file.write((char*)&iter->eUsage, sizeof(TRAIL_USAGE));
		file.write((char*)&iter->DesolveNum, sizeof(_int));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : TrailEffectsNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < TrailEffectsNames.size(); ++i)
	{
		NumberFile << i << ". " << TrailEffectsNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_TrailList()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Trail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : TrailEffects)
		iter.reset();
	TrailEffects.clear();
	TrailEffectsNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CParticle_Trail::TRAIL_DESC> readFile = make_shared<CParticle_Trail::TRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		inFile.read((char*)&readFile->vStartColor, sizeof(_float3));
		inFile.read((char*)&readFile->vEndColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloompower, sizeof(_float));
		inFile.read((char*)&readFile->Desolve, sizeof(_bool));
		inFile.read((char*)&readFile->Blur, sizeof(_bool));
		inFile.read((char*)&readFile->Alpha, sizeof(_bool));
		inFile.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
		inFile.read((char*)&readFile->eUsage, sizeof(TRAIL_USAGE));
		inFile.read((char*)&readFile->DesolveNum, sizeof(_int));

		TrailEffects.emplace_back(readFile);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Trail.bin";
	ifstream NameFile(TexPath);
	if (!NameFile.good())
		return E_FAIL;
	if (!NameFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		NameFile.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		NameFile.read(&str[0], length);
		TrailEffectsNames.emplace_back(str);
	}
	NameFile.close();
	return S_OK;
}

vector<string> CImguiMgr::GetFilesInDirectory(const string& path)
{
	vector<std::string> files;
	for (const auto& entry : filesystem::directory_iterator(path)) {
		files.push_back(entry.path().filename().string());
	}
	return files;
}

CImguiMgr* CImguiMgr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImguiMgr* pInstance = new CImguiMgr();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed To Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImguiMgr::Free()
{
	for (auto& mesh : MeshEffects)
	{
		Safe_Delete(mesh);
	}
	for (auto& point : PointEffects)
	{
		Safe_Delete(point);
	}
	for (auto& rect : RectEffects)
	{
		Safe_Delete(rect);
	}
	TrailEffects.clear();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pGameInstance);
}

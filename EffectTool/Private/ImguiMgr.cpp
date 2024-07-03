#pragma once
#include "ImguiMgr.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameInstance.h"
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

	static _bool bShow[4] = { false,false,false,false };
	ImGui::Checkbox("Texture_FileSystem", &bShow[0]);
	if (bShow[0] == true)
		Load_Texture();
	ImGui::Checkbox("Particle_Tool", &bShow[1]);
	if (bShow[1] == true)
	{
		EffectTool_Rework();
	}
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
	ImGui::Checkbox("FrameTextureTool", &bShow[3]);
	if (bShow[3] == true)
	{
		FrameTextureTool();
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
	if (ImGui::Button("Bind_Player_Head"))
	{
		CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
		TrailMat = pPlayer->Get_HeadMAt();
	}
	ImGui::End();
}

void CImguiMgr::EffectTool_Rework()
{
	ImGui::Begin("Effect_Editor");

	static CParticle::PARTICLEDESC parentsDesc{};
	static CParticleMesh::PARTICLEMESH MeshDesc{};
	static CParticle_Point::PARTICLEPOINT PointDesc{};
	static CParticle_Rect::PARTICLERECT RectDesc{};
	static PARTICLETYPE eParticleType = PART_POINT;
	

	if (ChangedDesc)
	{
		switch (eParticleType)
		{
		case Effect::PART_POINT:
			PointDesc = *((CParticle_Point::PARTICLEPOINT*)Variants);
			parentsDesc = PointDesc.SuperDesc;
			break;
		case Effect::PART_MESH:
			MeshDesc = *((CParticleMesh::PARTICLEMESH*)Variants);
			parentsDesc = MeshDesc.SuperDesc;
			break;
		case Effect::PART_RECT:
			RectDesc = *((CParticle_Rect::PARTICLERECT*)Variants);
			parentsDesc = RectDesc.SuperDesc;
			break;
		default:
			break;
		}
		ChangedDesc = false;
	}




	MeshDesc.particleType = PART_MESH;
	PointDesc.particleType = PART_POINT;
	RectDesc.particleType = PART_RECT;

#pragma region PARTICLEDESC

	if (ImGui::RadioButton("Point", eParticleType == PART_POINT))
		eParticleType = PART_POINT;
	ImGui::SameLine();
	if (ImGui::RadioButton("Mesh", eParticleType == PART_MESH))
		eParticleType = PART_MESH;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rect", eParticleType == PART_RECT))
		eParticleType = PART_RECT;

	if (eParticleType == PART_MESH)
	{
		CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Mesh_Type");
		if (ImGui::RadioButton("Cube", MeshDesc.eModelType == EFFECTMODELTYPE::CUBE))
			MeshDesc.eModelType = EFFECTMODELTYPE::CUBE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Circle", MeshDesc.eModelType == EFFECTMODELTYPE::CIRCLE))
			MeshDesc.eModelType = EFFECTMODELTYPE::CIRCLE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Slash", MeshDesc.eModelType == EFFECTMODELTYPE::SLASH))
			MeshDesc.eModelType = EFFECTMODELTYPE::SLASH;
	}

	ImGui::Checkbox("Bloom", &parentsDesc.IsBlur);
	if (parentsDesc.IsBlur == true)
	{
		ImGui::InputFloat("BloomPower", &parentsDesc.fBlurPower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&parentsDesc.vBloomColor));
	}
	ImGui::Checkbox("Desolve", &parentsDesc.Desolve);
	if (parentsDesc.Desolve == true)
	{
		ImGui::InputInt("DesolveNumber", &parentsDesc.DesolveNum);
		ImGui::ColorEdit3("DesolveColor", reinterpret_cast<float*>(&parentsDesc.vDesolveColor));
		ImGui::InputFloat("DesolveLength", &parentsDesc.fDesolveLength);
	}
	ImGui::Checkbox("ColorMapping", &parentsDesc.IsColor);
	if (parentsDesc.IsColor == true)
	{
		ImGui::ColorEdit3("Start_Color", reinterpret_cast<float*>(&parentsDesc.vStartColor));
		ImGui::ColorEdit3("End_Color", reinterpret_cast<float*>(&parentsDesc.vEndColor));
	}
	ImGui::Checkbox("AlphaLerp", &parentsDesc.IsAlpha);
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&parentsDesc.vStartPos));

#pragma region FUNCTYPE
	if (ImGui::RadioButton("Spread", parentsDesc.eType == SPREAD))
	{
		parentsDesc.eType = SPREAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Drop", parentsDesc.eType == DROP))
	{
		parentsDesc.eType = DROP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOut", parentsDesc.eType == GROWOUT))
	{
		parentsDesc.eType = GROWOUT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Tornado", parentsDesc.eType == TORNADO))
	{
		parentsDesc.eType = TORNADO;
	}
	if (ImGui::RadioButton("Spread_SizeUp", parentsDesc.eType == SPREAD_SIZEUP))
	{
		parentsDesc.eType = SPREAD_SIZEUP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_NonRotation", parentsDesc.eType == SPREAD_NONROTATION))
	{
		parentsDesc.eType = SPREAD_NONROTATION;
	}


	if (ImGui::RadioButton("Spread_SpeedDown", parentsDesc.eType == SPREAD_SPEED_DOWN))
	{
		parentsDesc.eType = SPREAD_SPEED_DOWN;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("SlashEffect", parentsDesc.eType == SLASH_EFFECT))
	{
		parentsDesc.eType = SLASH_EFFECT;
	}
	if (ImGui::RadioButton("Spread_SpeedDown_SizeUp", parentsDesc.eType == SPREAD_SPEED_DOWN_SIZE_UP))
	{
		parentsDesc.eType = SPREAD_SPEED_DOWN_SIZE_UP;
	}
	if (ImGui::RadioButton("Gather", parentsDesc.eType == GATHER))
	{
		parentsDesc.eType = GATHER;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Extinction", parentsDesc.eType == EXTINCTION))
	{
		parentsDesc.eType = EXTINCTION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOutY", parentsDesc.eType == GROWOUTY))
	{
		parentsDesc.eType = GROWOUTY;
	}
#pragma endregion FUNCTYPE
	
#pragma endregion PARTICLEDESC

#pragma region INSTANCEDESC
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &parentsDesc.InstanceDesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vOffsetPos));
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vPivotPos));
	ImGui::InputFloat3("Range", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vRange));
	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vSize));
	ImGui::InputFloat2("Speed", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vSpeed));
	ImGui::InputFloat2("LifeTime", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vLifeTime));
	ImGui::InputFloat2("Gravity", reinterpret_cast<float*>(&parentsDesc.InstanceDesc.vGravity));
	ImGui::Checkbox("IsLoop", &parentsDesc.InstanceDesc.isLoop);
#pragma endregion INSTANCEDESC

	void* value2 = nullptr;
	switch (eParticleType)
	{
	case PARTICLETYPE::PART_POINT:
		PointDesc.Texture = m_pTextureProtoName;
		PointDesc.TexturePath = m_pTextureFilePath;
		PointDesc.SuperDesc = parentsDesc;
		value2 = &PointDesc;
		break;
	case PARTICLETYPE::PART_MESH:
		MeshDesc.SuperDesc = parentsDesc;
		value2 = &MeshDesc;
		break;
	case PARTICLETYPE::PART_RECT:
		RectDesc.Texture = m_pTextureProtoName;
		RectDesc.TexturePath = m_pTextureFilePath;
		RectDesc.SuperDesc = parentsDesc;
		value2 = &RectDesc;
		break;
	default:
		break;
	}

#pragma region BUTTON
	if (ImGui::Button("Add", ImVec2(100, 30)))
	{
		switch (eParticleType)
		{
		case PARTICLETYPE::PART_POINT:
		{
			if (PointDesc.Texture == TEXT("") || PointDesc.TexturePath == TEXT(""))
				MSG_BOX("텍스쳐를 먼저 선택해주세요");
			else
			{
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticlePoint"), &PointDesc);
			}
			break;
		}
		case PARTICLETYPE::PART_MESH:
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
				TEXT("Prototype_GameObject_ParticleMesh"), &MeshDesc);
			break;
		case PARTICLETYPE::PART_RECT:
		{
			if (RectDesc.Texture == TEXT("") || RectDesc.TexturePath == TEXT(""))
				MSG_BOX("텍스쳐를 먼저 선택해주세요");
			else
			{
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleRect"), &RectDesc);
			}
			break;
		}
		default:
			break;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", ImVec2(100, 30)))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"));
	}

	static char ParticleNames[256] = "";
	ImGui::InputText("Name", ParticleNames, IM_ARRAYSIZE(ParticleNames));

	if (ImGui::Button("PushBack_List", ImVec2(200, 30)))		
	{
		if (ParticleNames[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			switch (eParticleType)
			{
			case Effect::PART_POINT:
				Store_Particles(ParticleNames, eParticleType, &PointDesc);
				break;
			case Effect::PART_MESH:
				Store_Particles(ParticleNames, eParticleType, &MeshDesc);
				break;
			case Effect::PART_RECT:
				Store_Particles(ParticleNames, eParticleType, &RectDesc);
				break;
			default:
				MSG_BOX("형식이 잘못되었습니다.");
				break;
			}
		}
	}

	if (ImGui::Button("Save", ImVec2(200, 30)))
	{
		if (FAILED(Save_Particles()))
			MSG_BOX("Failed_Save");
		else
			MSG_BOX("Succeed_Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(200, 30)))
	{
		if (FAILED(Load_Particles()))
			MSG_BOX("Failed_Load");
		else
			MSG_BOX("Succeed_Load");
	}
#pragma endregion BUTTON

	
	eParticleType = ParticleListBox(eParticleType, &Variants, value2);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Particles(char* Name, PARTICLETYPE type, void* pValue)
{
	string sName = Name;

	switch (type)
	{
	case Effect::PART_POINT:
	{
		CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pValue);
		shared_ptr<CParticle_Point::PARTICLEPOINT> PointDesc = make_shared<CParticle_Point::PARTICLEPOINT>(val);
		//CParticle_Point::PARTICLEPOINT* PointDesc =  new CParticle_Point::PARTICLEPOINT(val);
		m_Particles.insert(make_pair(PART_POINT, PointDesc));
		break;
	}
	case Effect::PART_MESH:
	{
		CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pValue);
		shared_ptr<CParticleMesh::PARTICLEMESH> MeshDesc = make_shared<CParticleMesh::PARTICLEMESH>(val);
		//CParticleMesh::PARTICLEMESH* MeshDesc = new CParticleMesh::PARTICLEMESH(val);
		m_Particles.insert(make_pair(PART_MESH, MeshDesc));
		break;
	}
	case Effect::PART_RECT:
	{
		CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pValue);
		shared_ptr<CParticle_Rect::PARTICLERECT> RectDesc = make_shared<CParticle_Rect::PARTICLERECT>(val);
		//CParticle_Rect::PARTICLERECT* RectDesc = new CParticle_Rect::PARTICLERECT(val);
		m_Particles.insert(make_pair(PART_RECT, RectDesc));
		break;
	}
	default:
		break;
	}
	ParticleNames.emplace_back(sName);

	return S_OK;
}

PARTICLETYPE CImguiMgr::ParticleListBox(PARTICLETYPE type, void** pValue, void* pValue2)
{
	if (m_Particles.size() < 1)
		return type;

	if (m_Particles.size() != ParticleNames.size())
	{
		MSG_BOX("Size Error");
		return type;
	}
	ImGui::Begin("Particle_List_Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
	if (ImGui::BeginListBox("Particle", list_box_size))
	{
		for (int i = 0; i < ParticleNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(ParticleNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < m_Particles.size())
	{
		auto it = m_Particles.begin();
		advance(it, current_item);
		if (it == m_Particles.end())
		{
			ImGui::End();
			return type;
		}

		if (ImGui::Button("Add", ButtonSize))
		{	
			switch (it->first)
			{
			case PART_POINT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticlePoint"), it->second.get());
				break;
			case PART_MESH:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleMesh"), it->second.get());
				break;
			case PART_RECT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleRect"), it->second.get());
				break;
			default:
				break;
			}
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*pValue = it->second.get();
			ChangedDesc = true;
			ImGui::End();
			return it->first;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			if (it->first != type)
				MSG_BOX("다른 타입은 수정할 수 없습니다.");
			else
			{
				switch (type)
				{
				case Effect::PART_POINT:
				{
					it->second.reset();
					CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pValue2);
					it->second = make_shared<CParticle_Point::PARTICLEPOINT>(val);
					break;
				}
				case Effect::PART_MESH:
				{
					it->second.reset();
					CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pValue2);
					it->second = make_shared<CParticleMesh::PARTICLEMESH>(val);
					break;
				}
				case Effect::PART_RECT:
				{
					it->second.reset();
					CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pValue2);
					it->second = make_shared<CParticle_Rect::PARTICLERECT>(val);
					break;
				}
				default:
					break;
				}
			}
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			it->second.reset();
			m_Particles.erase(it);
			ParticleNames.erase(ParticleNames.begin() + current_item);

			if (current_item >= m_Particles.size())
				current_item = m_Particles.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& pair : m_Particles)
				pair.second.reset();

			m_Particles.clear();
			ParticleNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();

	return type;
}

HRESULT CImguiMgr::Save_Particles()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Particles.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Particles.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& pair : m_Particles)
	{
		PARTICLETYPE type = pair.first;
		file.write((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case Effect::PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pair.second.get());
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			save_wstring_to_stream(val.Texture, file);
			save_wstring_to_stream(val.TexturePath, file);
			break;
		}
		case Effect::PART_MESH:
		{
			CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pair.second.get());
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			file.write((char*)&val.eModelType, sizeof(EFFECTMODELTYPE));
			break;
		}
		case Effect::PART_RECT:
		{
			CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pair.second.get());
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			save_wstring_to_stream(val.Texture, file);
			save_wstring_to_stream(val.TexturePath, file);
			break;
		}
		}
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Particle.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : ParticleNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/ParticleIndex.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < ParticleNames.size(); ++i)
	{
		NumberFile << i << ". " << ParticleNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Particles()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Particles.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& pair : m_Particles)
		pair.second.reset();
	m_Particles.clear();
	ParticleNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		PARTICLETYPE type;
		inFile.read((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT val;
			inFile.read((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			val.Texture = load_wstring_from_stream(inFile);
			val.TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(val.TexturePath, val.Texture);
			val.particleType = PART_POINT;
			shared_ptr<CParticle_Point::PARTICLEPOINT> Arg = make_shared<CParticle_Point::PARTICLEPOINT>(val);
			m_Particles.insert(make_pair(PART_POINT, Arg));
			break;
		}
		case PART_MESH:
		{
			CParticleMesh::PARTICLEMESH val;
			inFile.read((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			inFile.read((char*)&val.eModelType, sizeof(EFFECTMODELTYPE));
			val.particleType = PART_MESH;
			shared_ptr<CParticleMesh::PARTICLEMESH> Arg = make_shared<CParticleMesh::PARTICLEMESH>(val);
			m_Particles.insert(make_pair(PART_MESH, Arg));
			break;
		}
		case PART_RECT:
		{
			CParticle_Rect::PARTICLERECT val;
			inFile.read((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			val.Texture = load_wstring_from_stream(inFile);
			val.TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(val.TexturePath, val.Texture);
			val.particleType = PART_RECT;
			shared_ptr<CParticle_Rect::PARTICLERECT> Arg = make_shared<CParticle_Rect::PARTICLERECT>(val);
			m_Particles.insert(make_pair(PART_RECT, Arg));
			break;
		}
		}
	}

	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Particle.bin";
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
		ParticleNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
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

void CImguiMgr::FrameTextureTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("FrameTexture_Editor");

	static CTextureFrame::TEXFRAMEDESC classdesc{};

	ImGui::InputInt("Division", &classdesc.iDivision);
	classdesc.Texture = m_pTextureProtoName;
	classdesc.TexturePath = m_pTextureFilePath;
	ImGui::Checkbox("Color", &classdesc.State[0]);
	if (classdesc.State[0] == true)
	{
		ImGui::ColorEdit3("StartColor", reinterpret_cast<float*>(&classdesc.Color[0]));
		ImGui::ColorEdit3("EndColor", reinterpret_cast<float*>(&classdesc.Color[1]));
	}
	ImGui::Checkbox("Desolve", &classdesc.State[1]);
	if (classdesc.State[1] == true)
	{
		if (ImGui::InputInt("DesolveNum", &classdesc.iNumDesolve))
		{
			if (classdesc.iNumDesolve < 0)
				classdesc.iNumDesolve = 0;
			else if (classdesc.iNumDesolve > 15)
				classdesc.iNumDesolve = 15;
		}
	}
	ImGui::Checkbox("Blur", &classdesc.State[2]);
	if (classdesc.State[2] == true)
	{
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&classdesc.Color[2]));
		ImGui::InputFloat("Bloompower", &classdesc.Frame[2]);
	}
	ImGui::Checkbox("Billboard", &classdesc.State[3]);

	ImGui::InputFloat("MaxFrame", &classdesc.Frame[0]);
	ImGui::InputFloat("FrameSpeed", &classdesc.Frame[1]);

	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&classdesc.vSize));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&classdesc.vStartPos));

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (classdesc.Texture == TEXT("") || classdesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐를 먼저 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FrameTex"),
				TEXT("Prototype_GameObject_FrameTexture"), &classdesc);
		}
	}
	ImGui::End();
}

void CImguiMgr::CenteredTextColored(const ImVec4& color, const char* text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text).x;
	float textIndent = (windowWidth - textWidth) / 2.0f;
	if (textIndent < 0.0f)
		textIndent = 0.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textIndent);
	ImGui::TextColored(color, "%s", text);
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
	m_Particles.clear();

	TrailEffects.clear();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pGameInstance);
}

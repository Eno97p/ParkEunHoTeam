#pragma once
#include "ImguiMgr.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "Particle_STrail.h"

#include "Andras.h"
#include "AndrasLazer_Base.h"
#include "AndrasLazer_Cylinder.h"
#include "AndrasScrew.h"
#include "AndrasRain.h"
#include "ElectricCylinder.h"
#include "AndrasLazer.h"


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

	ModelName.emplace_back("Wander");
	ModelName.emplace_back("Andras");

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

void CImguiMgr::Model_Change()
{
	ImGui::Begin("Model_List");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
	if (ImGui::BeginListBox("Models", list_box_size))
	{
		for (int i = 0; i < ModelName.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(ModelName[i].c_str(), is_selected))
			{
				current_item = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < ModelName.size())
	{
		if (ImGui::Button("Change_Model", ButtonSize))
		{
			m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"));
			switch (current_item)
			{
			case 0:		//Wander
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_PlayerDummy"));
				break;
			case 1:		//Andras
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy"), TEXT("Prototype_GameObject_Andras"));
				break;
			default:
				break;
			}
			CurModel = current_item;
			TrailMat = nullptr;
		}
	}

	ImGui::End();
}

void CImguiMgr::Visible_Data()
{
	ImGui::Begin("DATA");
	ImGui::Text("Frame : %f", ImGui::GetIO().Framerate);
	static _bool bShow[13] = { false,false,false,false,false,false,false,false,false,false,false,false,false};
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

	ImGui::Checkbox("SwordTrailTool", &bShow[4]);
	if(bShow[4] == true)
		SwordTrail_Tool();

	ImGui::Checkbox("DistortionTool", &bShow[5]);
	if (bShow[5] == true)
		Distortion_Tool();

	ImGui::Checkbox("ElectronicTool", &bShow[6]);
	if (bShow[6] == true)
		Electron_Tool();

	ImGui::Checkbox("Fire Tool", &bShow[7]);
	if (bShow[7] == true)
		FireTool();

	ImGui::Checkbox("Lazer_Tool", &bShow[8]);
	if (bShow[8] == true)
		Lazer_Tool();

	ImGui::Checkbox("Tornado_Tool", &bShow[9]);
	if (bShow[9] == true)
		Tornado_Tool();

	ImGui::Checkbox("FirePillar_Tool", &bShow[10]);
	if (bShow[10] == true)
		FirePillarTool();

	ImGui::Checkbox("Heal_Tool", &bShow[11]);
	if (bShow[11] == true)
		HealEffectTool();

	ImGui::Checkbox("Model_Change", &bShow[12]);
	if (bShow[12] == true)
		Model_Change();

	if (ImGui::Button("Bind_Sword_Matrix"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_WeaponMat();
			break;
		}
		case 1:
		{
			break;
		}
		default:
			break;
		}

	}
	if (ImGui::Button("Bind_Body_Matrix"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_WorldMat();
			break;
		}
		case 1:
		{
			CAndras* Andras = static_cast<CAndras*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = Andras->Get_WorldMat();
			break;
		}
		default:
			break;
		}

	}
	if (ImGui::Button("Bind_Head"))
	{
		switch (CurModel)
		{
		case 0:
		{
			CPlayerDummy* pPlayer = static_cast<CPlayerDummy*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = pPlayer->Get_HeadMAt();
			break;
		}
		case 1:
		{
			CAndras* Andras = static_cast<CAndras*>(m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerDummy")));
			TrailMat = Andras->Get_HeadMat();
			break;
		}
		default:
			break;
		}

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
	else
	{
		PointDesc.Texture = m_pTextureProtoName;
		PointDesc.TexturePath = m_pTextureFilePath;
		RectDesc.Texture = m_pTextureProtoName;
		RectDesc.TexturePath = m_pTextureFilePath;
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
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf0", MeshDesc.eModelType == EFFECTMODELTYPE::LEAF0))
			MeshDesc.eModelType = EFFECTMODELTYPE::LEAF0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Leaf1", MeshDesc.eModelType == EFFECTMODELTYPE::LEAF1))
			MeshDesc.eModelType = EFFECTMODELTYPE::LEAF1;

		if (ImGui::RadioButton("Blade", MeshDesc.eModelType == EFFECTMODELTYPE::BLADE_SLASH))
			MeshDesc.eModelType = EFFECTMODELTYPE::BLADE_SLASH;
		ImGui::SameLine();
		if (ImGui::RadioButton("Blade_Long", MeshDesc.eModelType == EFFECTMODELTYPE::BLADE_SLASH_LONG))
			MeshDesc.eModelType = EFFECTMODELTYPE::BLADE_SLASH_LONG;
		ImGui::SameLine();
		if (ImGui::RadioButton("Grass", MeshDesc.eModelType == EFFECTMODELTYPE::GRASS))
			MeshDesc.eModelType = EFFECTMODELTYPE::GRASS;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock1", MeshDesc.eModelType == EFFECTMODELTYPE::ROCK0))
			MeshDesc.eModelType = EFFECTMODELTYPE::ROCK0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rock2", MeshDesc.eModelType == EFFECTMODELTYPE::ROCK1))
			MeshDesc.eModelType = EFFECTMODELTYPE::ROCK1;
	}

	ImGui::Checkbox("Bloom", &parentsDesc.IsBloom);
	ImGui::SameLine();
	ImGui::Checkbox("Blur", &parentsDesc.IsBlur);
	if (parentsDesc.IsBloom == true)
	{
		ImGui::InputFloat("BloomPower", &parentsDesc.fBlurPower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&parentsDesc.vBloomColor));
	}
	ImGui::Checkbox("Desolve", &parentsDesc.Desolve);
	if (parentsDesc.Desolve == true)
	{
		if (ImGui::InputInt("DesolveNumber", &parentsDesc.DesolveNum))
		{
			if (parentsDesc.DesolveNum < 0)
				parentsDesc.DesolveNum = 0;
			if (parentsDesc.DesolveNum > 42)
				parentsDesc.DesolveNum = 42;
		}
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
	if (ImGui::RadioButton("GrowOutSpeedDown", parentsDesc.eType == GROWOUT_SPEEDDOWN))
	{
		parentsDesc.eType = GROWOUT_SPEEDDOWN;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Leaf_Fall", parentsDesc.eType == LEAF_FALL))
	{
		parentsDesc.eType = LEAF_FALL;
	}

	if (ImGui::RadioButton("Spiral_Gather", parentsDesc.eType == SPIRAL_EXTINCTION))
	{
		parentsDesc.eType = SPIRAL_EXTINCTION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spiral_Spread", parentsDesc.eType == SPIRAL_SPERAD))
	{
		parentsDesc.eType = SPIRAL_SPERAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("LenzFlare", parentsDesc.eType == LENZ_FLARE))
	{
		parentsDesc.eType = LENZ_FLARE;
	}
	if (ImGui::RadioButton("Blow", parentsDesc.eType == BLOW))
	{
		parentsDesc.eType = BLOW;
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
		PointDesc.SuperDesc = parentsDesc;
		value2 = &PointDesc;
		break;
	case PARTICLETYPE::PART_MESH:
		MeshDesc.SuperDesc = parentsDesc;
		value2 = &MeshDesc;
		break;
	case PARTICLETYPE::PART_RECT:
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
		CParticle_Point::PARTICLEPOINT* PointDesc = new CParticle_Point::PARTICLEPOINT;
		*PointDesc = *((CParticle_Point::PARTICLEPOINT*)pValue);
		m_Types.emplace_back(make_pair(PART_POINT, PointDesc));
		break;
	}
	case Effect::PART_MESH:
	{
		CParticleMesh::PARTICLEMESH* MeshDesc = new CParticleMesh::PARTICLEMESH;
		*MeshDesc = *((CParticleMesh::PARTICLEMESH*)pValue);
		m_Types.emplace_back(make_pair(PART_MESH, MeshDesc));
		break;
	}
	case Effect::PART_RECT:
	{
		CParticle_Rect::PARTICLERECT* RectDesc = new CParticle_Rect::PARTICLERECT;
		*RectDesc = *((CParticle_Rect::PARTICLERECT*)pValue);
		m_Types.emplace_back(make_pair(PART_RECT, RectDesc));
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
	//if (m_Particles.size() < 1)
	//	return type;
	if (m_Types.size() < 1)
		return type;


	if (m_Types.size() != ParticleNames.size())
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

	if (current_item >= 0 && current_item < m_Types.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{	
			switch (m_Types[current_item].first)
			{
			case PART_POINT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticlePoint"), m_Types[current_item].second);
				break;
			case PART_MESH:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleMesh"), m_Types[current_item].second);
				break;
			case PART_RECT:
				m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Particle"),
					TEXT("Prototype_GameObject_ParticleRect"), m_Types[current_item].second);
				break;
			default:
				break;
			}
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*pValue = m_Types[current_item].second;
			ChangedDesc = true;
			ImGui::End();
			return m_Types[current_item].first;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			if (m_Types[current_item].first != type)
				MSG_BOX("다른 타입은 수정할 수 없습니다.");
			else
			{
				switch (type)
				{
				case Effect::PART_POINT:
				{
					Erase_Particle(current_item);
					CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pValue2);
					m_Types[current_item].second = new CParticle_Point::PARTICLEPOINT(val);
					break;
				}
				case Effect::PART_MESH:
				{
					Erase_Particle(current_item);
					CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pValue2);
					m_Types[current_item].second = new CParticleMesh::PARTICLEMESH(val);
					break;
				}
				case Effect::PART_RECT:
				{
					Erase_Particle(current_item);
					CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pValue2);
					m_Types[current_item].second = new CParticle_Rect::PARTICLERECT(val);
					break;
				}
				default:
					break;
				}
			}
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			Erase_Particle(current_item);
			m_Types.erase(m_Types.begin() + current_item);
			ParticleNames.erase(ParticleNames.begin() + current_item);

			if (current_item >= m_Types.size())
				current_item = m_Types.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (int i = 0; i < m_Types.size(); ++i)
				Erase_Particle(i);

			m_Types.clear();
			//m_Particles.clear();
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
	_uint iSize = m_Types.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& pair : m_Types)
	{
		PARTICLETYPE type = pair.first;
		file.write((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case Effect::PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT val = *((CParticle_Point::PARTICLEPOINT*)pair.second);
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			save_wstring_to_stream(val.Texture, file);
			save_wstring_to_stream(val.TexturePath, file);
			break;
		}
		case Effect::PART_MESH:
		{
			CParticleMesh::PARTICLEMESH val = *((CParticleMesh::PARTICLEMESH*)pair.second);
			file.write((char*)&val.SuperDesc, sizeof(CParticle::PARTICLEDESC));
			file.write((char*)&val.eModelType, sizeof(EFFECTMODELTYPE));
			break;
		}
		case Effect::PART_RECT:
		{
			CParticle_Rect::PARTICLERECT val = *((CParticle_Rect::PARTICLERECT*)pair.second);
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
	for (int i = 0; i < m_Types.size(); ++i)
		Erase_Particle(i);
	m_Types.clear();
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
			CParticle_Point::PARTICLEPOINT* Arg = new CParticle_Point::PARTICLEPOINT;
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_POINT;
			m_Types.emplace_back(make_pair(PART_POINT, Arg));
			
			break;
		}
		case PART_MESH:
		{
			CParticleMesh::PARTICLEMESH* Arg = new CParticleMesh::PARTICLEMESH();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			inFile.read((char*)&Arg->eModelType, sizeof(EFFECTMODELTYPE));
			Arg->particleType = PART_MESH;
			m_Types.emplace_back(make_pair(PART_MESH, Arg));
			break;
		}
		case PART_RECT:
		{
			CParticle_Rect::PARTICLERECT* Arg = new CParticle_Rect::PARTICLERECT();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_RECT;
			m_Types.emplace_back(make_pair(PART_RECT, Arg));
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

	if (ImGui::Button("..", ImVec2(50,30))) {
		currentPath = std::filesystem::path(currentPath).parent_path().string();
	}
	ImGui::SameLine();
	if (ImGui::Button("Convert_To_DDS_This_Folder", ImVec2(200.f, 30.f)))
	{
		string outputDirectory = "../../Client/Bin/Resources/Textures/DDS_Storage/";
		vector<string> imageFiles = GetFilesTexture(currentPath);
		for (const auto& file : imageFiles) {
			ConvertToDDSWithMipmap(file, outputDirectory);
		}
		MSG_BOX("DDS 추출 완료");
	}



	//()


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

	if (IsPNGFile(selectedFile) || IsDDSFile(selectedFile) || ISTex(selectedFile)) {
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

	if (ImGui::Button("Convert_To_DDS_Here", ImVec2(200.f, 30.f)))
	{
		vector<string> imageFiles = GetFilesTexture(currentPath);
		for (const auto& file : imageFiles) {
			ConvertToDDSWithMipmap_PathHere(file);
		}
		MSG_BOX("DDS 추출 완료");
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
		m_pTextureProtoName = classdesc.Texture;
		m_pTextureFilePath = classdesc.TexturePath;
	}


	if (ImGui::RadioButton("SwordTrail", classdesc.eFuncType == TRAIL_EXTINCT))
		classdesc.eFuncType = TRAIL_EXTINCT;
	ImGui::SameLine();
	if (ImGui::RadioButton("EternalTrail", classdesc.eFuncType == TRAIL_ETERNAL))
		classdesc.eFuncType = TRAIL_ETERNAL;
	
	
	ImGui::ColorEdit3("Start_Color", reinterpret_cast<float*>(&classdesc.vStartColor));
	ImGui::ColorEdit3("End_Color", reinterpret_cast<float*>(&classdesc.vEndColor));
	traildesc.ParentMat = TrailMat;
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &traildesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&traildesc.vPivotPos));
	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&traildesc.vSize));
	ImGui::InputFloat("Speed", &traildesc.vSpeed);
	ImGui::InputFloat("lifetime", &traildesc.fLifeTime);
	ImGui::Checkbox("Desolve", &classdesc.Desolve);
	if (classdesc.Desolve == true)
	{
		ImGui::InputInt("DesolveTextureNum", &classdesc.DesolveNum);
	}
	ImGui::Checkbox("Blur", &classdesc.Blur);
	ImGui::Checkbox("Bloom", &classdesc.Bloom);
	if (classdesc.Bloom == true)
	{
		ImGui::InputFloat("BloomPower", &classdesc.fBloompower);
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&classdesc.vBloomColor));
	}
	classdesc.traildesc = traildesc;
	classdesc.Texture = m_pTextureProtoName;
	classdesc.TexturePath = m_pTextureFilePath;

	if (ImGui::Button("Generate", ButtonSize))
	{
		if (classdesc.Texture == TEXT("") || classdesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐 파일을 선택해 주세요.");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Trail"),
				TEXT("Prototype_GameObject_Trail"), &classdesc);
		}
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
		file.write((char*)&iter->eFuncType, sizeof(TRAILFUNCTYPE));
		file.write((char*)&iter->DesolveNum, sizeof(_int));
		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
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
		inFile.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
		inFile.read((char*)&readFile->DesolveNum, sizeof(_int));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
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

void CImguiMgr::SwordTrail_Tool()
{
	ImGui::Begin("SwordTrail_Tool");

	ImVec2 ButtonSize = { 100.f,30.f };

	static CSTrail::STRAIL_DESC StaticDesc = {};

	if (ChangedSwordTrail != true)
	{
		StaticDesc.traildesc.ParentMat = TrailMat;
		StaticDesc.Texture = m_pTextureProtoName;
		StaticDesc.TexturePath = m_pTextureFilePath;
	}
	else
	{
		StaticDesc.traildesc.ParentMat = TrailMat;
		ChangedSwordTrail = false;
	}
	
	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &StaticDesc.traildesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&StaticDesc.traildesc.vOffsetPos));
	ImGui::InputFloat("Size", reinterpret_cast<float*>(&StaticDesc.traildesc.vSize));
	ImGui::InputFloat("LifeTime", &StaticDesc.traildesc.fLifeTime);
	ImGui::InputFloat("MovingTime", &StaticDesc.traildesc.fMaxTime);
	ImGui::InputInt("DesolveNum", &StaticDesc.iDesolveNum);
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&StaticDesc.vColor));

	ImGui::Checkbox("Bloom", &StaticDesc.isBloom);
	if (StaticDesc.isBloom == true)
	{
		ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&StaticDesc.vBloomColor));
		ImGui::InputFloat("BloomPower", &StaticDesc.fBloomPower);
	}
	ImGui::Checkbox("Distortion", &StaticDesc.isDestortion);

#pragma region BUTTONS
	if (ImGui::Button("Add", ButtonSize))
	{
		if (StaticDesc.Texture == TEXT("") || StaticDesc.TexturePath == TEXT("") || StaticDesc.traildesc.ParentMat == nullptr)
			MSG_BOX("텍스쳐와 행렬을 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"),
				TEXT("Prototype_GameObject_Sword_Trail"), &StaticDesc);
		}
	}

	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"));
	}

	static char Names[256] = "";
	ImGui::InputText("Name", Names, IM_ARRAYSIZE(Names));
	if (ImGui::Button("Store", ButtonSize))
	{
		if (Names[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreSwordTrail(Names, StaticDesc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_SwordTrail()))
		{
			MSG_BOX("Failed_SaveTrail");
		}
		else
			MSG_BOX("Save Success");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_SwordTrail()))
		{
			MSG_BOX("Failed_LoadTrail");
		}
		else
			MSG_BOX("Load Success");
	}
#pragma endregion BUTTONS

	SwordTrailListBox(&StaticDesc);


	ImGui::End();
}

HRESULT CImguiMgr::StoreSwordTrail(char* Name, CSTrail::STRAIL_DESC trail)
{
	string sName = Name;
	shared_ptr<CSTrail::STRAIL_DESC> RawPtr = make_shared<CSTrail::STRAIL_DESC>(trail);
	m_SwordTrails.emplace_back(RawPtr);
	SwordTrailNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::SwordTrailListBox(CSTrail::STRAIL_DESC* trail)
{
	if (m_SwordTrails.size() < 1)
		return;

	if (m_SwordTrails.size() != SwordTrailNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Sword_Trail_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("SwordTrailList", list_box_size))
	{
		for (int i = 0; i < SwordTrailNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(SwordTrailNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < m_SwordTrails.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			CSTrail::STRAIL_DESC* traildesc = m_SwordTrails[current_item].get();
			traildesc->traildesc.ParentMat = TrailMat;
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_Sword_Trail"), traildesc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*trail = *m_SwordTrails[current_item].get();
			ChangedSwordTrail = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			shared_ptr<CSTrail::STRAIL_DESC> newItem = make_shared<CSTrail::STRAIL_DESC>(*trail);
			m_SwordTrails[current_item] = newItem;
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_SwordTrails[current_item].reset();
			m_SwordTrails.erase(m_SwordTrails.begin() + current_item);
			SwordTrailNames.erase(SwordTrailNames.begin() + current_item);

			if (current_item >= m_SwordTrails.size())
				current_item = m_SwordTrails.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_SwordTrails)
				iter.reset();
			m_SwordTrails.clear();
			SwordTrailNames.clear();
			current_item = 0;
		}

	}

	ImGui::End();

}

HRESULT CImguiMgr::Save_SwordTrail()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/SwordTrail.Bin";

	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_SwordTrails.size();
	file.write((char*)&iSize, sizeof(_uint));

	for (auto& iter : m_SwordTrails)
	{
		file.write((char*)&iter->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		file.write((char*)&iter->isBloom, sizeof(_bool));
		file.write((char*)&iter->isDestortion, sizeof(_bool));
		file.write((char*)&iter->iDesolveNum, sizeof(_int));
		file.write((char*)&iter->vColor, sizeof(_float3));
		file.write((char*)&iter->vBloomColor, sizeof(_float3));
		file.write((char*)&iter->fBloomPower, sizeof(_float));

		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : SwordTrailNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < SwordTrailNames.size(); ++i)
	{
		NumberFile << i << ". " << SwordTrailNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_SwordTrail()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/SwordTrail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	for (auto& iter : m_SwordTrails)
		iter.reset();
	m_SwordTrails.clear();
	SwordTrailNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		//file.write((char*)&iter->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		//file.write((char*)&iter->isBloom, sizeof(_bool));
		//file.write((char*)&iter->isDestortion, sizeof(_bool));
		//file.write((char*)&iter->iDesolveNum, sizeof(_int));
		//file.write((char*)&iter->vColor, sizeof(_float3));
		//file.write((char*)&iter->vBloomColor, sizeof(_float3));
		//file.write((char*)&iter->fBloomPower, sizeof(_float));




		shared_ptr<CSTrail::STRAIL_DESC> readFile = make_shared<CSTrail::STRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		readFile->traildesc.ParentMat = nullptr;
		inFile.read((char*)&readFile->isBloom, sizeof(_bool));
		inFile.read((char*)&readFile->isDestortion, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloomPower, sizeof(_float));

		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_SwordTrails.emplace_back(readFile);
	}
	inFile.close();


	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/SwordTrail.bin";
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
		SwordTrailNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::Distortion_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Distortion_Editor");
	static CDistortionEffect::DISTORTIONEFFECT DisDesc{};
	if (!ChangedDistortion)
	{
		DisDesc.Texture = m_pTextureProtoName;
		DisDesc.TexturePath = m_pTextureFilePath;
	}
	else
	{
		DisDesc.vStartpos = { 0.f,0.f,0.f,1.f };
		ChangedDistortion = false;
	}

	ImGui::Checkbox("Extinct&Grow", &DisDesc.bFuncType);
	ImGui::Checkbox("Desolve", &DisDesc.bDisolve);
	if (DisDesc.bDisolve == true)
	{
		ImGui::InputInt("DesolveNum", &DisDesc.iDesolveNum);
	}
	ImGui::InputFloat2("StartScale", reinterpret_cast<float*>(&DisDesc.vStartScale));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&DisDesc.vStartpos));
	ImGui::InputFloat("Speed", &DisDesc.fSpeed);
	ImGui::InputFloat("LifeTime", &DisDesc.fLifeTime);

#pragma region BUTTONS
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (DisDesc.Texture == TEXT("") || DisDesc.TexturePath == TEXT(""))
			MSG_BOX("텍스쳐를 선택해주세요");
		else
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"),
				TEXT("Prototype_GameObject_Distortion_Effect"), &DisDesc);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"));
	}

	static char Names[256] = "";
	ImGui::InputText("Name", Names, IM_ARRAYSIZE(Names));
	if (ImGui::Button("Store", ImVec2(200,30)))
	{
		if (Names[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreDistortion(Names, DisDesc);
		}
	}

	if (ImGui::Button("Save", ImVec2(100, 30)))
	{
		if (FAILED(Save_Distortion()))
			MSG_BOX("Failed_Save");
		else
			MSG_BOX("Succeed_Save");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(100, 30)))
	{
		if (FAILED(Load_Distortion()))
			MSG_BOX("Failed_Load");
		else
			MSG_BOX("Succeed_Load");
	}

#pragma endregion BUTTONS
	Distortion_ListBox(&DisDesc);
	ImGui::End();
}

HRESULT CImguiMgr::StoreDistortion(char* Name, CDistortionEffect::DISTORTIONEFFECT distortion)
{
	string sName = Name;
	shared_ptr<CDistortionEffect::DISTORTIONEFFECT> RawPtr = make_shared<CDistortionEffect::DISTORTIONEFFECT>(distortion);
	m_Distortions.emplace_back(RawPtr);
	DistortionNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Distortion_ListBox(CDistortionEffect::DISTORTIONEFFECT* distortion)
{
	if (m_Distortions.size() < 1)
		return;
	if (m_Distortions.size() != DistortionNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}
	ImGui::Begin("Distortion_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Distortion_List", list_box_size))
	{
		for (int i = 0; i < DistortionNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(DistortionNames[i].c_str(), is_selected))
			{
				current_item = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
#pragma region BUTTONS
	if (current_item >= 0 && current_item < m_Distortions.size())
	{
		if (ImGui::Button("Add", ButtonSize))
		{
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Distortion"),
				TEXT("Prototype_GameObject_Distortion_Effect"), m_Distortions[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*distortion = *m_Distortions[current_item].get();
			ChangedDistortion = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			shared_ptr<CDistortionEffect::DISTORTIONEFFECT> newItem = make_shared<CDistortionEffect::DISTORTIONEFFECT>(*distortion);
			m_Distortions[current_item] = newItem;
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Distortions[current_item].reset();
			m_Distortions.erase(m_Distortions.begin() + current_item);
			DistortionNames.erase(DistortionNames.begin() + current_item);

			if (current_item >= m_Distortions.size())
				current_item = m_Distortions.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Distortions)
				iter.reset();
			m_Distortions.clear();
			DistortionNames.clear();
			current_item = 0;
		}


	}
#pragma endregion BUTTONS
	ImGui::End();
}

HRESULT CImguiMgr::Save_Distortion()
{
	//CDistortionEffect::DISTORTIONEFFECT
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Distortion.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Distortions.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Distortions)
	{
		file.write((char*)&iter->bFuncType, sizeof(_bool));
		file.write((char*)&iter->bDisolve, sizeof(_bool));
		file.write((char*)&iter->iDesolveNum, sizeof(_int));
		file.write((char*)&iter->vStartScale, sizeof(_float2));
		file.write((char*)&iter->vStartpos, sizeof(_float4));
		file.write((char*)&iter->fSpeed, sizeof(_float));
		file.write((char*)&iter->fLifeTime, sizeof(_float));
		save_wstring_to_stream(iter->Texture, file);
		save_wstring_to_stream(iter->TexturePath, file);
	}
	file.close();


	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : DistortionNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < DistortionNames.size(); ++i)
	{
		NumberFile << i << ". " << DistortionNames[i] << std::endl;
	}
	NumberFile.close();
	return S_OK;
}

HRESULT CImguiMgr::Load_Distortion()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Distortion.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Distortions)
		iter.reset();
	m_Distortions.clear();
	DistortionNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CDistortionEffect::DISTORTIONEFFECT> readFile = make_shared<CDistortionEffect::DISTORTIONEFFECT>();
		inFile.read((char*)&readFile->bFuncType, sizeof(_bool));
		inFile.read((char*)&readFile->bDisolve, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vStartScale, sizeof(_float2));
		inFile.read((char*)&readFile->vStartpos, sizeof(_float4));
		inFile.read((char*)&readFile->fSpeed, sizeof(_float));
		inFile.read((char*)&readFile->fLifeTime, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_Distortions.emplace_back(readFile);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Distortion.bin";
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
		DistortionNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::FireTool()
{
	ImGui::Begin("Fire_Editor");
	ImVec2 ButtonSize = { 100.f,30.f };

	static CFireEffect::FIREEFFECTDESC FireDesc{};

	ImGui::InputFloat3("ScrollSpeeds", reinterpret_cast<float*>(&FireDesc.ScrollSpeeds));
	ImGui::InputFloat3("FireScales", reinterpret_cast<float*>(&FireDesc.Scales));
	ImGui::InputFloat2("Distortion1", reinterpret_cast<float*>(&FireDesc.distortion1));
	ImGui::InputFloat2("Distortion2", reinterpret_cast<float*>(&FireDesc.distortion2));
	ImGui::InputFloat2("Distortion3", reinterpret_cast<float*>(&FireDesc.distortion3));
	ImGui::InputFloat("DistortionScale", &FireDesc.distortionScale);
	ImGui::InputFloat("DistortionBias", &FireDesc.distortionBias);

	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&FireDesc.vStartPos));
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&FireDesc.vOffsetPos));
	ImGui::InputFloat2("StartScale", reinterpret_cast<float*>(&FireDesc.vStartScale));


	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Fire"), TEXT("Prototype_GameObject_Fire_Effect"), &FireDesc);
	}

	if (ImGui::Button("Clear", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Fire"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f,30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			StoreFires(effectname, FireDesc);
		}
	}


	FireListBox(&FireDesc);

	ImGui::End();
}

HRESULT CImguiMgr::StoreFires(char* Name, CFireEffect::FIREEFFECTDESC Fire)
{
	string sName = Name;
	shared_ptr<CFireEffect::FIREEFFECTDESC> StockValue = make_shared<CFireEffect::FIREEFFECTDESC>(Fire);
	m_Fires.emplace_back(StockValue);
	FireNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::FireListBox(CFireEffect::FIREEFFECTDESC* Fire)
{
	if (m_Fires.size() < 1)
		return;

	if (m_Fires.size() != FireNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Fire_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("Fire_List", list_box_size))
	{
		for (int i = 0; i < FireNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(FireNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < FireNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Fire"), TEXT("Prototype_GameObject_Fire_Effect"), m_Fires[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Fire = *m_Fires[current_item].get();
			ChangedFire = true;
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Fires[current_item] = make_shared<CFireEffect::FIREEFFECTDESC>(*Fire);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Fires[current_item].reset();
			m_Fires.erase(m_Fires.begin() + current_item);
			FireNames.erase(FireNames.begin() + current_item);

			if (current_item >= m_Fires.size())
				current_item = m_Fires.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Fires)
				iter.reset();
			m_Fires.clear();
			FireNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();
}

void CImguiMgr::Electron_Tool()
{
	ImGui::Begin("Electric_Editor");
	ImVec2 ButtonSize = { 100.f,30.f };

	static CElectronic::ELECTRONICDESC Desc{};
	ImGui::InputFloat3("Size", reinterpret_cast<float*>(&Desc.vSize));
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));
	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	ImGui::InputFloat("LifeTime", &Desc.fMaxLifeTime);
	ImGui::InputFloat("BloomPower", &Desc.fBloomPower);
	ImGui::InputFloat("UVSpeed", &Desc.fUVSpeed);

	ImGui::ColorEdit3("BaseColor", reinterpret_cast<float*>(&Desc.vColor));
	ImGui::ColorEdit3("BloomColor", reinterpret_cast<float*>(&Desc.vBloomColor));
	ImGui::InputInt("Particle", &Desc.ParticleIndex);

	if (ImGui::Button("Generate", ButtonSize))
	{
		//Prototype_GameObject_Electronic_Effect
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Electronic"), TEXT("Prototype_GameObject_Electronic_Effect"), &Desc);
	}

	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Electronic"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Lightnings(effectname, Desc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Lightning()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Lightning()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	Electron_ListBox(&Desc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Lightnings(char* Name, CElectronic::ELECTRONICDESC Lightning)
{
	string sName = Name;
	shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(Lightning);
	m_Lightnings.emplace_back(StockValue);
	LightningNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Electron_ListBox(CElectronic::ELECTRONICDESC* Lightning)
{
	if (m_Lightnings.size() < 1)
		return;

	if (m_Lightnings.size() != LightningNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Lightning_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;

	if (ImGui::BeginListBox("LightningList", list_box_size))
	{
		for (int i = 0; i < LightningNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(LightningNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < LightningNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Electronic"), TEXT("Prototype_GameObject_Electronic_Effect"), m_Lightnings[current_item].get());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Lightning = *m_Lightnings[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Lightnings[current_item] = make_shared<CElectronic::ELECTRONICDESC>(*Lightning);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Lightnings[current_item].reset();
			m_Lightnings.erase(m_Lightnings.begin() + current_item);
			LightningNames.erase(LightningNames.begin() + current_item);

			if (current_item >= m_Lightnings.size())
				current_item = m_Lightnings.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Lightnings)
				iter.reset();
			m_Lightnings.clear();
			LightningNames.clear();
			current_item = 0;
		}
	}
	ImGui::End();
}

HRESULT CImguiMgr::Save_Lightning()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lightning.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Lightnings.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Lightnings)
	{
		file.write((char*)iter.get(), sizeof(CElectronic::ELECTRONICDESC));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightning.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : LightningNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightnings.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < LightningNames.size(); ++i)
	{
		NumberFile << i << ". " << LightningNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Lightning()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lightning.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Lightnings)
		iter.reset();
	m_Lightnings.clear();
	LightningNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CElectronic::ELECTRONICDESC readFile{};
		//shared_ptr<CElectronic::ELECTRONICDESC> readFile = make_shared<CElectronic::ELECTRONICDESC>();
		inFile.read((char*)&readFile, sizeof(CElectronic::ELECTRONICDESC));
		shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(readFile);
		m_Lightnings.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Lightning.bin";
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
		LightningNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::Lazer_Tool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Lazer_Edditor");

	static CAndrasLazer::ANDRAS_LAZER_TOTALDESC TotalDesc{};

	TotalDesc.ShooterMat = TrailMat;
	ImGui::InputFloat("LifeTime", &TotalDesc.fLifeTime);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Base_Lazer");

	ImGui::InputFloat3("B_MaxSize", reinterpret_cast<float*>(&TotalDesc.BaseDesc.vMaxSize));
	ImGui::InputFloat3("B_Offset", reinterpret_cast<float*>(&TotalDesc.BaseDesc.vOffset));
	ImGui::InputFloat("B_RotSpeed", &TotalDesc.BaseDesc.fRotationSpeed);
	ImGui::InputFloat("B_BloomPower", &TotalDesc.BaseDesc.fBloomPower);
	ImGui::InputFloat("B_DistortionPower", &TotalDesc.BaseDesc.fDistortionPower);
	ImGui::InputFloat("B_UVSpeed", &TotalDesc.BaseDesc.fUVSpeed);
	ImGui::InputInt("B_NumDesolve", &TotalDesc.BaseDesc.NumDesolve);
	ImGui::ColorEdit3("B_Color", reinterpret_cast<float*>(&TotalDesc.BaseDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Cylinder");

	ImGui::InputFloat3("C_MaxSize", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.vMaxSize));
	ImGui::InputFloat3("C_Offset", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.vOffset));
	ImGui::InputFloat("C_RotSpeed", &TotalDesc.CylinderDesc.fRotationSpeed);
	ImGui::InputFloat("C_BloomPower", &TotalDesc.CylinderDesc.fBloomPower);
	ImGui::InputFloat("C_DistortionPower", &TotalDesc.CylinderDesc.fDistortionPower);
	ImGui::InputFloat("C_UVSpeed", &TotalDesc.CylinderDesc.fUVSpeed);
	ImGui::InputInt("C_NumDesolve", &TotalDesc.CylinderDesc.NumDesolve);
	ImGui::ColorEdit3("C_Color", reinterpret_cast<float*>(&TotalDesc.CylinderDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Screw");

	ImGui::InputFloat3("S_MaxSize", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.vMaxSize));
	ImGui::InputFloat3("S_Offset", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.vOffset));
	ImGui::InputFloat("S_RotSpeed", &TotalDesc.ScrewDesc.fRotationSpeed);
	ImGui::InputFloat("S_BloomPower", &TotalDesc.ScrewDesc.fBloomPower);
	ImGui::InputFloat("S_DistortionPower", &TotalDesc.ScrewDesc.fDistortionPower);
	ImGui::InputFloat("S_UVSpeed", &TotalDesc.ScrewDesc.fUVSpeed);
	ImGui::InputInt("S_NumDesolve", &TotalDesc.ScrewDesc.NumDesolve);
	ImGui::ColorEdit3("S_Color", reinterpret_cast<float*>(&TotalDesc.ScrewDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Electron");

	ImGui::InputFloat("E_Interval", &TotalDesc.ElectricInterval);
	ImGui::InputFloat3("E_MaxSize", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.vMaxSize));
	ImGui::InputFloat3("E_Offset", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.vOffset));
	ImGui::InputFloat("E_LifeTime", &TotalDesc.ElectricDesc.fMaxLifeTime);
	ImGui::InputFloat("E_UVSpeed", &TotalDesc.ElectricDesc.fUVSpeed);
	ImGui::ColorEdit3("E_Color", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.fColor));
	ImGui::ColorEdit3("E_BloomColor", reinterpret_cast<float*>(&TotalDesc.ElectricDesc.fBloomColor));
	ImGui::InputFloat("E_BloomPower", &TotalDesc.ElectricDesc.fBloomPower);
	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Rain");

	ImGui::InputFloat3("R_MaxSize", reinterpret_cast<float*>(&TotalDesc.RainDesc.vMaxSize));
	ImGui::InputFloat3("R_Offset", reinterpret_cast<float*>(&TotalDesc.RainDesc.vOffset));
	ImGui::InputFloat("R_RotSpeed", &TotalDesc.RainDesc.fRotationSpeed);
	ImGui::InputFloat("R_BloomPower", &TotalDesc.RainDesc.fBloomPower);
	ImGui::InputFloat("R_DistortionPower", &TotalDesc.RainDesc.fDistortionPower);
	ImGui::InputFloat("R_UVSpeed", &TotalDesc.RainDesc.fUVSpeed);
	ImGui::InputInt("R_NumDesolve", &TotalDesc.RainDesc.NumDesolve);
	ImGui::ColorEdit3("R_Color", reinterpret_cast<float*>(&TotalDesc.RainDesc.fColor));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Shield");

	ImGui::InputFloat("SH_Interval", &TotalDesc.ShieldInterval);
	ImGui::InputFloat("SH_LifeTime", &TotalDesc.ShieldDesc.fMaxLifeTime);
	ImGui::InputFloat3("SH_MaxSize", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.vMaxSize));
	ImGui::InputFloat3("SH_Offset", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.vOffset));
	ImGui::InputFloat("SH_BloomPower", &TotalDesc.ShieldDesc.fBloomPower);
	ImGui::InputFloat("SH_UVSpeed", &TotalDesc.ShieldDesc.fUVSpeed);
	ImGui::ColorEdit3("SH_Color", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.fColor));
	ImGui::ColorEdit3("SH_BloomColor", reinterpret_cast<float*>(&TotalDesc.ShieldDesc.fBloomColor));


	/*static CAndrasLazerBase::ANDRAS_LASER_BASE_DESC Desc{};
	static CAndrasLazerCylinder::ANDRAS_LASER_CYLINDER_DESC CDesc{};
	static CAndrasScrew::ANDRAS_SCREW_DESC SDesc{};
	static CElectricCylinder::ANDRAS_ELECTRIC_DESC EDesc{};
	static CAndrasRain::ANDRAS_RAIN_DESC RDesc{};

	ImGui::InputFloat3("MaxSize", reinterpret_cast<float*>(&Desc.vMaxSize));
	ImGui::InputFloat3("CMaxSize", reinterpret_cast<float*>(&CDesc.vMaxSize));
	ImGui::InputFloat3("SMaxSize", reinterpret_cast<float*>(&SDesc.vMaxSize));
	ImGui::InputFloat3("EMaxSize", reinterpret_cast<float*>(&EDesc.vMaxSize));
	ImGui::InputFloat3("RMaxSize", reinterpret_cast<float*>(&RDesc.vMaxSize));

	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&Desc.vOffset));
	ImGui::InputFloat3("COffsetPos", reinterpret_cast<float*>(&CDesc.vOffset));
	RDesc.vOffset = CDesc.vOffset;
	ImGui::InputFloat3("SOffsetPos", reinterpret_cast<float*>(&SDesc.vOffset));
	ImGui::InputFloat3("EOffsetPos", reinterpret_cast<float*>(&EDesc.vOffset));

	ImGui::InputFloat("RotationSpeed", &Desc.fRotationSpeed);
	ImGui::InputFloat("CRotationSpeed", &CDesc.fRotationSpeed);
	ImGui::InputFloat("SRotationSpeed", &SDesc.fRotationSpeed);
	ImGui::InputFloat("ERotationSpeed", &EDesc.fRotationSpeed);

	ImGui::InputFloat("BloomPower", &Desc.fBloomPower);
	ImGui::InputFloat("CBloomPower", &CDesc.fBloomPower);
	ImGui::InputFloat("SBloomPower", &SDesc.fBloomPower);

	ImGui::InputFloat("DistortionPower", &Desc.fDistortionPower);
	ImGui::InputFloat("CDistortionPower", &CDesc.fDistortionPower);
	ImGui::InputFloat("SDistortionPower", &SDesc.fDistortionPower);

	ImGui::InputFloat("MaxLifeTime", &Desc.fMaxLifeTime);
	ImGui::InputFloat("CMaxLifeTime", &CDesc.fMaxLifeTime);
	ImGui::InputFloat("SMaxLifeTime", &SDesc.fMaxLifeTime);
	ImGui::InputFloat("EMaxLifeTime", &EDesc.fMaxLifeTime);
	RDesc.fMaxLifeTime = CDesc.fMaxLifeTime;

	ImGui::InputFloat("UVSpeed", &Desc.fUVSpeed);
	ImGui::InputFloat("CUVSpeed", &CDesc.fUVSpeed);
	ImGui::InputFloat("SUVSpeed", &SDesc.fUVSpeed);
	ImGui::InputFloat("RUVSpeed", &RDesc.fUVSpeed);
	ImGui::InputFloat("EFrameSpeed", &EDesc.frameSpeed);

	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&Desc.fColor));
	ImGui::ColorEdit3("CColor", reinterpret_cast<float*>(&CDesc.fColor));
	ImGui::ColorEdit3("SColor", reinterpret_cast<float*>(&SDesc.fColor));
	ImGui::ColorEdit3("RColor", reinterpret_cast<float*>(&RDesc.fColor));
	ImGui::ColorEdit3("EColor", reinterpret_cast<float*>(&EDesc.fColor));


	Desc.ParentMatrix = TrailMat;
	CDesc.ParentMatrix = TrailMat;
	SDesc.ParentMatrix = TrailMat;
	EDesc.ParentMatrix = TrailMat;
	RDesc.ParentMatrix = TrailMat;*/
	
	if (ImGui::Button("Generate", ButtonSize))
	{
		if (TotalDesc.ShooterMat == nullptr)
			MSG_BOX("행렬을 대입해주세요");
		else
		{
			m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerLazer"),
				TEXT("Prototype_GameObject_AndrasLazerSpawner"), &TotalDesc);
		}
	}
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("LayerLazer"));
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AndrasLazer"));
	}
	ImGui::End();
}

void CImguiMgr::Tornado_Tool()
{
#pragma region VALUES
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Tornado_Editor");

	static CTornadoEffect::TORNADODESC TDesc{};
	ImGui::InputFloat("Speed", &TDesc.fSpeed);
	ImGui::InputFloat("Rot_Speed", &TDesc.fRotationSpeed);
	ImGui::InputFloat("LifeTime", &TDesc.fLifeTime);
	ImGui::InputInt("NumParticle", &TDesc.NumParticle);

	ImGui::InputFloat4("StartPos", reinterpret_cast<float*>(&TDesc.vStartPos));

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind");

	ImGui::InputFloat3("WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc.vMaxSize));
	ImGui::InputFloat3("WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc.vOffset));
	ImGui::ColorEdit3("WindColor", reinterpret_cast<float*>(&TDesc.WindDesc.fColor));
	ImGui::ColorEdit3("WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc.fColor2));


	ImGui::InputFloat("WindRotSpeed", &TDesc.WindDesc.fRotationSpeed);
	ImGui::InputFloat("WindUVSpeed", &TDesc.WindDesc.fUVSpeed);
	ImGui::InputFloat("WindGrowSpeed", &TDesc.WindDesc.fGrowSpeed);
	ImGui::InputFloat("WindBloomPower", &TDesc.WindDesc.fBloomPower);
	ImGui::InputInt("WindNumDesolve", &TDesc.WindDesc.NumDesolve);

	ImGui::InputFloat("RadialStrength", &TDesc.WindDesc.RadicalStrength);
	ImGui::Checkbox("Distortion", &TDesc.WindDesc.IsDistortion);
	ImGui::Checkbox("Opacity", &TDesc.WindDesc.Opacity);
	if (TDesc.WindDesc.Opacity == true)
		ImGui::InputFloat("OpacityStrength", &TDesc.WindDesc.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind2");

	ImGui::InputFloat3("2WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc2.vMaxSize));
	ImGui::InputFloat3("2WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc2.vOffset));
	ImGui::ColorEdit3("2WindColor", reinterpret_cast<float*>(&TDesc.WindDesc2.fColor));
	ImGui::ColorEdit3("2WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc2.fColor2));


	ImGui::InputFloat("2WindRotSpeed", &TDesc.WindDesc2.fRotationSpeed);
	ImGui::InputFloat("2WindUVSpeed", &TDesc.WindDesc2.fUVSpeed);
	ImGui::InputFloat("2WindGrowSpeed", &TDesc.WindDesc2.fGrowSpeed);
	ImGui::InputFloat("2WindBloomPower", &TDesc.WindDesc2.fBloomPower);
	ImGui::InputInt("2WindNumDesolve", &TDesc.WindDesc2.NumDesolve);

	ImGui::InputFloat("2RadialStrength", &TDesc.WindDesc2.RadicalStrength);
	ImGui::Checkbox("2Distortion", &TDesc.WindDesc2.IsDistortion);
	ImGui::Checkbox("2Opacity", &TDesc.WindDesc2.Opacity);
	if (TDesc.WindDesc2.Opacity == true)
		ImGui::InputFloat("2OpacityStrength", &TDesc.WindDesc2.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Wind3");

	ImGui::InputFloat3("3WindMaxSize", reinterpret_cast<float*>(&TDesc.WindDesc3.vMaxSize));
	ImGui::InputFloat3("3WindOffset", reinterpret_cast<float*>(&TDesc.WindDesc3.vOffset));
	ImGui::ColorEdit3("3WindColor", reinterpret_cast<float*>(&TDesc.WindDesc3.fColor));
	ImGui::ColorEdit3("3WindColor2", reinterpret_cast<float*>(&TDesc.WindDesc3.fColor2));


	ImGui::InputFloat("3WindRotSpeed", &TDesc.WindDesc3.fRotationSpeed);
	ImGui::InputFloat("3WindUVSpeed", &TDesc.WindDesc3.fUVSpeed);
	ImGui::InputFloat("3WindGrowSpeed", &TDesc.WindDesc3.fGrowSpeed);
	ImGui::InputFloat("3WindBloomPower", &TDesc.WindDesc3.fBloomPower);
	ImGui::InputInt("3WindNumDesolve", &TDesc.WindDesc3.NumDesolve);

	ImGui::InputFloat("3RadialStrength", &TDesc.WindDesc3.RadicalStrength);
	ImGui::Checkbox("3Distortion", &TDesc.WindDesc3.IsDistortion);
	ImGui::Checkbox("3Opacity", &TDesc.WindDesc3.Opacity);
	if (TDesc.WindDesc3.Opacity == true)
		ImGui::InputFloat("3OpacityStrength", &TDesc.WindDesc3.OpacityPower);


	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Root");

	ImGui::InputFloat3("RootMaxSize", reinterpret_cast<float*>(&TDesc.RootDesc.vMaxSize));
	ImGui::InputFloat3("RootOffset", reinterpret_cast<float*>(&TDesc.RootDesc.vOffset));
	ImGui::ColorEdit3("RootColor", reinterpret_cast<float*>(&TDesc.RootDesc.fColor));


	ImGui::InputFloat("Interval", &TDesc.fRootInterval);
	ImGui::InputFloat("StartYScale", &TDesc.RootDesc.StartYScale);
	ImGui::InputFloat("RootRotSpeed", &TDesc.RootDesc.fRotationSpeed);
	ImGui::InputFloat("RootBloompower", &TDesc.RootDesc.fBloomPower);
	ImGui::InputFloat("RootLifeTime", &TDesc.RootDesc.fMaxLifeTime);
	ImGui::InputFloat("RootUVSpeed", &TDesc.RootDesc.fUVSpeed);
	ImGui::InputFloat("RootGrowSpeed", &TDesc.RootDesc.fGrowSpeed);
	ImGui::InputInt("RootNumDesolve", &TDesc.RootDesc.NumDesolve);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Ring1");

	ImGui::InputFloat3("RingMaxSize", reinterpret_cast<float*>(&TDesc.RingDesc1.vMaxSize));
	ImGui::InputFloat3("RingOffset", reinterpret_cast<float*>(&TDesc.RingDesc1.vOffset));
	ImGui::ColorEdit3("RingColor", reinterpret_cast<float*>(&TDesc.RingDesc1.fColor));
	ImGui::ColorEdit3("RingColor2", reinterpret_cast<float*>(&TDesc.RingDesc1.fColor2));


	ImGui::InputFloat("RingRotSpeed", &TDesc.RingDesc1.fRotationSpeed);
	ImGui::InputFloat("RingUVSpeed", &TDesc.RingDesc1.fUVSpeed);
	ImGui::InputFloat("RingGrowSpeed", &TDesc.RingDesc1.fGrowSpeed);
	ImGui::InputFloat("RingBloomPower", &TDesc.RingDesc1.fBloomPower);
	ImGui::InputInt("RingNumDesolve", &TDesc.RingDesc1.NumDesolve);

	ImGui::InputFloat("Ring_RadialStrength", &TDesc.RingDesc1.RadicalStrength);
	ImGui::Checkbox("Ring_Distortion", &TDesc.RingDesc1.IsDistortion);
	ImGui::Checkbox("Ring_Opacity", &TDesc.RingDesc1.Opacity);
	if (TDesc.RingDesc1.Opacity == true)
		ImGui::InputFloat("Ring_OpacityStrength", &TDesc.RingDesc1.OpacityPower);

	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Ring2");

	ImGui::InputFloat3("2RingMaxSize", reinterpret_cast<float*>(&TDesc.RingDesc2.vMaxSize));
	ImGui::InputFloat3("2RingOffset", reinterpret_cast<float*>(&TDesc.RingDesc2.vOffset));
	ImGui::ColorEdit3("2RingColor", reinterpret_cast<float*>(&TDesc.RingDesc2.fColor));
	ImGui::ColorEdit3("2RingColor2", reinterpret_cast<float*>(&TDesc.RingDesc2.fColor2));


	ImGui::InputFloat("2RingRotSpeed", &TDesc.RingDesc2.fRotationSpeed);
	ImGui::InputFloat("2RingUVSpeed", &TDesc.RingDesc2.fUVSpeed);
	ImGui::InputFloat("2RingGrowSpeed", &TDesc.RingDesc2.fGrowSpeed);
	ImGui::InputFloat("2RingBloomPower", &TDesc.RingDesc2.fBloomPower);
	ImGui::InputInt("2RingNumDesolve", &TDesc.RingDesc2.NumDesolve);

	ImGui::InputFloat("2Ring_RadialStrength", &TDesc.RingDesc2.RadicalStrength);
	ImGui::Checkbox("2Ring_Distortion", &TDesc.RingDesc2.IsDistortion);
	ImGui::Checkbox("2Ring_Opacity", &TDesc.RingDesc2.Opacity);
	if (TDesc.RingDesc2.Opacity == true)
		ImGui::InputFloat("2Ring_OpacityStrength", &TDesc.RingDesc2.OpacityPower);
#pragma endregion VALUES

	TDesc.pTarget = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(),
		TEXT("LayerDummy"));

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Tornado"), TEXT("Prototype_GameObject_Tornado"), &TDesc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Tornado"));
	}

	static char effectname[256] = "";
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Name", effectname, IM_ARRAYSIZE(effectname));
	ImGui::SameLine();
	if (ImGui::Button("Store", ImVec2(50.f, 30.f)))
	{
		if (effectname[0] == '\0')
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Tornados(effectname, TDesc);
		}
	}

	if (ImGui::Button("Save", ButtonSize))
	{
		if (FAILED(Save_Tornado()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ButtonSize))
	{
		if (FAILED(Load_Tornado()))
			MSG_BOX("FAILED");
		else
			MSG_BOX("SUCCEED");
	}


	Tornado_ListBox(&TDesc);

	ImGui::End();
}

HRESULT CImguiMgr::Store_Tornados(char* Name, CTornadoEffect::TORNADODESC desc)
{
	string sName = Name;
	shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(desc);
	m_Tornados.emplace_back(StockValue);
	TornadoNames.emplace_back(sName);
	return S_OK;
}

void CImguiMgr::Tornado_ListBox(CTornadoEffect::TORNADODESC* Tornado)
{
#pragma region exception
	if (m_Tornados.size() < 1)
		return;

	if (m_Tornados.size() != TornadoNames.size())
	{
		MSG_BOX("Size Error");
		return;
	}

	ImGui::Begin("Tornado_List Box Header");
	ImVec2 list_box_size = ImVec2(-1, 200);
	ImVec2 ButtonSize = { 100,30 };
	static int current_item = 0;
#pragma endregion exception

	if (ImGui::BeginListBox("Tornado_List", list_box_size))
	{
		for (int i = 0; i < TornadoNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(TornadoNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (current_item >= 0 && current_item < TornadoNames.size())
	{
		if (ImGui::Button("Generate", ButtonSize))
		{
			CTornadoEffect::TORNADODESC* Desc = m_Tornados[current_item].get();
			Desc->pTarget = m_pGameInstance->Get_Object(m_pGameInstance->Get_CurrentLevel(),
				TEXT("LayerDummy"));
			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(),
				TEXT("Layer_Tornado"), TEXT("Prototype_GameObject_Tornado"), Desc);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load this", ButtonSize))
		{
			*Tornado = *m_Tornados[current_item].get();
			ImGui::End();
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ButtonSize))
		{
			m_Tornados[current_item] = make_shared<CTornadoEffect::TORNADODESC>(*Tornado);
		}

		if (ImGui::Button("Erase", ButtonSize))
		{
			m_Tornados[current_item].reset();
			m_Tornados.erase(m_Tornados.begin() + current_item);
			TornadoNames.erase(TornadoNames.begin() + current_item);

			if (current_item >= m_Tornados.size())
				current_item = m_Tornados.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Erase All", ButtonSize))
		{
			for (auto& iter : m_Tornados)
				iter.reset();
			m_Tornados.clear();
			TornadoNames.clear();
			current_item = 0;
		}
	}

	ImGui::End();
}

HRESULT CImguiMgr::Save_Tornado()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Tornados.Bin";
	ofstream file(finalPath, ios::out | ios::binary);
	_uint iSize = m_Tornados.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_Tornados)
	{
		file.write((char*)iter.get(), sizeof(CTornadoEffect::TORNADODESC));
	}
	file.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.bin";
	ofstream Text(TexPath, ios::out);
	for (auto& iter : TornadoNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string IndexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.txt";
	std::ofstream NumberFile(IndexPath);
	for (size_t i = 0; i < TornadoNames.size(); ++i)
	{
		NumberFile << i << ". " << TornadoNames[i] << std::endl;
	}
	NumberFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Load_Tornado()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Tornados.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	for (auto& iter : m_Tornados)
		iter.reset();
	m_Tornados.clear();
	TornadoNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		CTornadoEffect::TORNADODESC readFile{};
		inFile.read((char*)&readFile, sizeof(CTornadoEffect::TORNADODESC));
		readFile.pTarget = nullptr;
		shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(readFile);
		m_Tornados.emplace_back(StockValue);
	}
	inFile.close();

	string TexPath = "../../Client/Bin/BinaryFile/Effect/EffectsIndex/Tornados.bin";
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
		TornadoNames.emplace_back(str);
	}
	NameFile.close();

	return S_OK;
}

void CImguiMgr::FirePillarTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Tornado_Editor");

	static CFirePillar::FIREPILLAR Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat("SizeInterval", &Desc.Interval);
	ImGui::InputFloat4("vStartPos", reinterpret_cast<float*>(&Desc.vStartPos));
	ImGui::InputFloat3("vParentScale", reinterpret_cast<float*>(&Desc.vScale));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Pillar1");

	ImGui::InputFloat3("P1_vMaxSize", reinterpret_cast<float*>(&Desc.pillar1.vMaxSize));
	ImGui::InputFloat("P1_BloomPower", &Desc.pillar1.fBloomPower);
	ImGui::InputFloat("P1_UVSpeed", &Desc.pillar1.fUVSpeed);
	ImGui::InputFloat("P1_RadicalStrength", &Desc.pillar1.RadicalStrength);
	ImGui::InputFloat("P1_GrowSpeed", &Desc.pillar1.fGrowSpeed);
	ImGui::Checkbox("P1_Distortion", &Desc.pillar1.IsDistortion);
	ImGui::Checkbox("P1_Opacity", &Desc.pillar1.Opacity);
	if(Desc.pillar1.Opacity == true)
		ImGui::InputFloat("P1_OpPower", &Desc.pillar1.OpacityPower);

	ImGui::InputInt("P1_NumDesolve", &Desc.pillar1.NumDesolve);

	ImGui::ColorEdit3("P1_Color1", reinterpret_cast<float*>(&Desc.pillar1.fColor));
	ImGui::ColorEdit3("P1_Color2", reinterpret_cast<float*>(&Desc.pillar1.fColor2));


	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Pillar2");

	ImGui::InputFloat3("P2_vMaxSize", reinterpret_cast<float*>(&Desc.pillar2.vMaxSize));
	ImGui::InputFloat("P2_BloomPower", &Desc.pillar2.fBloomPower);
	ImGui::InputFloat("P2_UVSpeed", &Desc.pillar2.fUVSpeed);
	ImGui::InputFloat("P2_RadicalStrength", &Desc.pillar2.RadicalStrength);
	ImGui::InputFloat("P2_GrowSpeed", &Desc.pillar2.fGrowSpeed);
	ImGui::Checkbox("P2_Distortion", &Desc.pillar2.IsDistortion);
	ImGui::Checkbox("P2_Opacity", &Desc.pillar2.Opacity);
	if (Desc.pillar2.Opacity == true)
		ImGui::InputFloat("P2_OpPower", &Desc.pillar2.OpacityPower);

	ImGui::InputInt("P2_NumDesolve", &Desc.pillar2.NumDesolve);

	ImGui::ColorEdit3("P2_Color1", reinterpret_cast<float*>(&Desc.pillar2.fColor));
	ImGui::ColorEdit3("P2_Color2", reinterpret_cast<float*>(&Desc.pillar2.fColor2));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Pillar3");

	ImGui::InputFloat3("P3_vMaxSize", reinterpret_cast<float*>(&Desc.pillar3.vMaxSize));
	ImGui::InputFloat("P3_BloomPower", &Desc.pillar3.fBloomPower);
	ImGui::InputFloat("P3_UVSpeed", &Desc.pillar3.fUVSpeed);
	ImGui::InputFloat("P3_RadicalStrength", &Desc.pillar3.RadicalStrength);
	ImGui::InputFloat("P3_GrowSpeed", &Desc.pillar3.fGrowSpeed);
	ImGui::Checkbox("P3_Distortion", &Desc.pillar3.IsDistortion);
	ImGui::Checkbox("P3_Opacity", &Desc.pillar3.Opacity);
	if (Desc.pillar3.Opacity == true)
		ImGui::InputFloat("P3_OpPower", &Desc.pillar3.OpacityPower);

	ImGui::InputInt("P3_NumDesolve", &Desc.pillar3.NumDesolve);

	ImGui::ColorEdit3("P3_Color1", reinterpret_cast<float*>(&Desc.pillar3.fColor));
	ImGui::ColorEdit3("P3_Color2", reinterpret_cast<float*>(&Desc.pillar3.fColor2));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Pillar4");

	ImGui::InputFloat3("P4_vMaxSize", reinterpret_cast<float*>(&Desc.pillar4.vMaxSize));
	ImGui::InputFloat("P4_BloomPower", &Desc.pillar4.fBloomPower);
	ImGui::InputFloat("P4_UVSpeed", &Desc.pillar4.fUVSpeed);
	ImGui::InputFloat("P4_RadicalStrength", &Desc.pillar4.RadicalStrength);
	ImGui::InputFloat("P4_GrowSpeed", &Desc.pillar4.fGrowSpeed);
	ImGui::Checkbox("P4_Opacity", &Desc.pillar4.Opacity);
	if (Desc.pillar4.Opacity == true)
		ImGui::InputFloat("P4_OpPower", &Desc.pillar4.OpacityPower);

	ImGui::InputInt("P4_NumDesolve", &Desc.pillar4.NumDesolve);

	ImGui::ColorEdit3("P4_Color1", reinterpret_cast<float*>(&Desc.pillar4.fColor));
	ImGui::ColorEdit3("P4_Color2", reinterpret_cast<float*>(&Desc.pillar4.fColor2));


	CenteredTextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Bottom");

	ImGui::InputFloat3("Bottom_Size", reinterpret_cast<float*>(&Desc.Bottom.vMaxSize));
	ImGui::InputFloat("Bottom_BloomPower", &Desc.Bottom.fBloomPower);

	ImGui::InputFloat("Bottom_UVSpeed", &Desc.Bottom.fUVSpeed);
	ImGui::InputFloat("Bottom_RadicalStrength", &Desc.Bottom.RadicalStrength);
	ImGui::InputFloat("Bottom_GrowSpeed", &Desc.Bottom.fGrowSpeed);
	ImGui::Checkbox("Bottom_Distortion", &Desc.Bottom.IsDistortion);
	ImGui::Checkbox("Bottom_Opacity", &Desc.Bottom.Opacity);
	if (Desc.Bottom.Opacity == true)
		ImGui::InputFloat("Bottom_OpPower", &Desc.Bottom.OpacityPower);

	ImGui::InputInt("Bottom_NumDesolve", &Desc.Bottom.NumDesolve);

	ImGui::ColorEdit3("Bottom_Color1", reinterpret_cast<float*>(&Desc.Bottom.fColor));
	ImGui::ColorEdit3("Bottom_Color2", reinterpret_cast<float*>(&Desc.Bottom.fColor2));

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_FirePillar"), TEXT("Prototype_GameObject_FirePillar"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_FirePillar"));
	}

	ImGui::End();
}

void CImguiMgr::HealEffectTool()
{
	ImVec2 ButtonSize = { 100.f,30.f };
	ImGui::Begin("Heal_Effect_Editor");
	static CHealEffect::HEALEFFECT Desc{};

	ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
	ImGui::InputFloat3("Offset", reinterpret_cast<float*>(&Desc.vOffset));

	CenteredTextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Ribbon");
	ImGui::InputFloat3("RibbonSize", reinterpret_cast<float*>(&Desc.RibbonDesc.vSize));
	ImGui::ColorEdit3("RibbonColor", reinterpret_cast<float*>(&Desc.RibbonDesc.fColor));
	ImGui::ColorEdit3("RibbonBloomColor", reinterpret_cast<float*>(&Desc.RibbonDesc.BloomColor));
	ImGui::InputFloat("RibbonBloomPower", &Desc.RibbonDesc.fBloomPower);
	ImGui::InputFloat("RibbonLifeTime", &Desc.RibbonDesc.fMaxLifeTime);

	Desc.ParentMat = TrailMat;

	if (ImGui::Button("Generate", ButtonSize))
	{
		m_pGameInstance->CreateObject(m_pGameInstance->Get_CurrentLevel(),
			TEXT("Layer_Heal"), TEXT("Prototype_GameObject_HealEffect"), &Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase", ButtonSize))
	{
		m_pGameInstance->Clear_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Heal"));
	}

	ImGui::End();
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

HRESULT CImguiMgr::ConvertToDDSWithMipmap(const string& inputFilePath, const string& outputFilePath)
{
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	HRESULT hr;

	// 이미지 로드
	hr = DirectX::LoadFromWICFile(utf8_to_wstring(inputFilePath).c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		MSG_BOX("Failed to load image: ");
		return hr;
	}

	// mipmap 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		MSG_BOX("Failed to generate mipmaps, Proceeding with original image. ");
		mipChain = std::move(image);
	}

	// 새로운 파일명 생성: 기존 확장자 .png 제거 후 .dds 추가
	std::filesystem::path outputPath = std::filesystem::path(outputFilePath) / std::filesystem::path(inputFilePath).filename().replace_extension(".dds");

	// DDS 파일로 저장
	hr = DirectX::SaveToDDSFile(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DirectX::DDS_FLAGS_NONE, outputPath.c_str());
	if (FAILED(hr)) {
		MSG_BOX("Failed to save DDS file: ");
		return hr;
	}

	return S_OK;
}

HRESULT CImguiMgr::ConvertToDDSWithMipmap_PathHere(const string& inputFilePath)
{
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	HRESULT hr;

	// 이미지 로드
	hr = DirectX::LoadFromWICFile(utf8_to_wstring(inputFilePath).c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr)) {
		MSG_BOX("Failed to load image");
		return hr;
	}

	// mipmap 생성
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
	if (FAILED(hr)) {
		MSG_BOX("Failed to generate mipmaps, Proceeding with original image.");
		mipChain = std::move(image);
	}

	// 출력 경로 설정
	std::filesystem::path inputPath(inputFilePath);
	std::filesystem::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + ".dds");

	// DDS 파일로 저장
	hr = DirectX::SaveToDDSFile(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DirectX::DDS_FLAGS_NONE, outputPath.c_str());
	if (FAILED(hr)) {
		MSG_BOX("Failed to save DDS file: ");
		return hr;
	}
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

vector<string> CImguiMgr::GetFilesTexture(const string& path)
{
	vector<string> imageFiles;
	for (const auto& entry : filesystem::directory_iterator(path)) {
		if (entry.is_regular_file())
		{
			string filePath = entry.path().string();
			if (IsTextureFile(filePath))
			{
				imageFiles.push_back(filePath);
			}
		}
	}
	return imageFiles;
}

void CImguiMgr::Erase_Particle(_int index)
{
	if (index >= m_Types.size())
		return;
	switch (m_Types[index].first)
	{
	case PART_POINT:
		delete ((CParticle_Point::PARTICLEPOINT*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	case PART_MESH:
		delete ((CParticleMesh::PARTICLEMESH*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	case PART_RECT:
		delete ((CParticle_Rect::PARTICLERECT*)m_Types[index].second);
		m_Types[index].second = nullptr;
		break;
	default:
		break;
	}
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
	for (int i = 0; i < m_Types.size(); ++i)
		Erase_Particle(i);

	m_Types.clear();

	TrailEffects.clear();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pGameInstance);
}

#pragma once
#include "Base.h"
#include "Effect_Define.h"
#include "imgui.h"
#include "Particle_Trail.h"
#include "Particle_Rect.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"
#include "TextureFrame.h"
#include "Particle_STrail.h"
#include "Distortion_Effect.h"
#include "FireEffect.h"
#include "Electronic.h"
#include "TornadoEffect.h"
#include "FirePillar.h"
#include "HealEffect.h"
#include "SwingEffect.h"
#include "AndrasLazer.h"
#include "Meteor.h"
#include "Particle_PhysX.h"

BEGIN(Engine)
class CGameInstance;
class CTexture;
END

BEGIN(Effect)
class CImguiMgr :  public CBase
{

private:
	CImguiMgr();
	virtual ~CImguiMgr() = default;
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	void Visible_Data();
	void Render();
	void Tick(_float fTimiedelta);
private:
	void Model_Change();
private:
	void Sound_Tool(_bool* Open);

private:	//for FileSystem
	void Load_Texture();
	vector<string> GetFilesInDirectory(const string& path);
	vector<string> GetFilesTexture(const string& path);
	bool IsPNGFile(const std::string& filename) {
		return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".png";
	}
	bool IsTextureFile(const string& filename)
	{
		return (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".png") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".jpg") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".jpeg") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".tga");
	}

	bool IsDDSFile(const std::string& filename) {
		return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dds";
	}
	bool ISTex(const std::string& filename) {
		return (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".jpg") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".JPG") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".bmp");
	}
	bool Is_SoundFile(const string& filename)
	{
		return (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".mp3") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".OGG") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".wav") ||
			(filename.size() >= 4 && filename.substr(filename.size() - 4) == ".ogg");
	}


	wstring utf8_to_wstring(const std::string& str);
	HRESULT Add_Texture_Prototype(const wstring& path, const wstring& name);
	ImTextureID DirectXTextureToImTextureID();

private: //for Particle
	void EffectTool_Rework();
	HRESULT Store_Particles(char* Name, PARTICLETYPE type, void* pValue);
	PARTICLETYPE ParticleListBox(PARTICLETYPE type, void** pValue, void* pValue2);
	HRESULT Save_Particles();
	HRESULT Load_Particles();

private:	//for Trail
	void Trail_Tool();
	void Trail_ListBox(CParticle_Trail::TRAIL_DESC* vDesc);
	HRESULT Store_Trails(char* Name , void* Value);
	HRESULT Save_TrailList();
	HRESULT Load_TrailList();

	

	void Set_TrailMatrix(const _float4x4* mat)
	{
		TrailMat = mat;
	}

private:	//for FrameTexture
	void FrameTextureTool();


private:	//for SwordTrail
	void SwordTrail_Tool();
	HRESULT StoreSwordTrail(char* Name, CSTrail::STRAIL_DESC trail);
	void SwordTrailListBox(CSTrail::STRAIL_DESC* trail);
	HRESULT Save_SwordTrail();
	HRESULT Load_SwordTrail();

private:	//for Distortion
	void Distortion_Tool();
	HRESULT StoreDistortion(char* Name, CDistortionEffect::DISTORTIONEFFECT distortion);
	void Distortion_ListBox(CDistortionEffect::DISTORTIONEFFECT* distortion);
	HRESULT Save_Distortion();
	HRESULT Load_Distortion();

private:
	void FireTool();
	HRESULT StoreFires(char* Name, CFireEffect::FIREEFFECTDESC Fire);
	void FireListBox(CFireEffect::FIREEFFECTDESC* Fire);

private:	//Lightning
	void Electron_Tool();
	HRESULT Store_Lightnings(char* Name, CElectronic::ELECTRONICDESC Lightning);
	void Electron_ListBox(CElectronic::ELECTRONICDESC* Lightning);
	HRESULT Save_Lightning();
	HRESULT Load_Lightning();
private:
	void Lazer_Tool();
	HRESULT Store_Lazer(char* Name, CAndrasLazer::ANDRAS_LAZER_TOTALDESC Lazer);
	void Lazer_ListBox(CAndrasLazer::ANDRAS_LAZER_TOTALDESC* Lazer);
	HRESULT Save_Lazer();
	HRESULT Load_Lazer();

private:
	void Tornado_Tool();
	HRESULT Store_Tornados(char* Name, CTornadoEffect::TORNADODESC desc);
	void Tornado_ListBox(CTornadoEffect::TORNADODESC* Tornado);
	HRESULT Save_Tornado();
	HRESULT Load_Tornado();

private:
	void FirePillarTool();
	HRESULT Save_FirePillar(CFirePillar::FIREPILLAR* FirePillar);
	HRESULT Load_FirePillar(CFirePillar::FIREPILLAR* FirePillar);

private:
	void HealEffectTool();
	HRESULT Store_Heal(char* Name, CHealEffect::HEALEFFECT desc);
	void Heal_ListBox(CHealEffect::HEALEFFECT* Tornado);
	HRESULT Save_Heal();
	HRESULT Load_Heal();

private:
	void SwingEffectTool();
	HRESULT Store_Swing(char* Name, CSwingEffect::SWINGEFFECT desc);
	void Swing_ListBox(CSwingEffect::SWINGEFFECT* Swing);
	HRESULT Save_Swing();
	HRESULT Load_Swing();

private:
	void Meteor_Tool(_bool* Open);
	HRESULT Save_Meteor(CMeteor::METEOR_DESC* pMeteor);
	HRESULT Load_Meteor(CMeteor::METEOR_DESC* pMeteor);

private:
	void PhysX_Particle_Tool(_bool* Open);

private:
	void CenteredTextColored(const ImVec4& color, const char* text);


private:
	HRESULT ConvertToDDSWithMipmap(const string& inputFilePath, const string& outputFilePath);
	HRESULT ConvertToDDSWithMipmap_PathHere(const string& inputFilePath);

private:
	class CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CTexture* m_pTextureCom = { nullptr };		//선택한 텍스쳐의 프로토타입 포인터
	wstring		m_pTextureProtoName = TEXT("");	//프로토타입 이름
	wstring		m_pTextureFilePath = TEXT("");
	_bool ChangedDesc = false;
	void* Variants = nullptr;
	_bool ChangedTrail = false;
	_bool ChangedSwordTrail = false;
	_bool ChangedDistortion = false;
	_bool ChangedFire = false;
	const _float4x4* TrailMat = nullptr;

private:
	void Erase_Particle(_int index);



private:
	vector<pair<PARTICLETYPE, void*>>		m_Types;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> TrailEffects;
	vector<shared_ptr<CSTrail::STRAIL_DESC>>	m_SwordTrails;
	vector<shared_ptr<CDistortionEffect::DISTORTIONEFFECT>> m_Distortions;
	vector<shared_ptr<CFireEffect::FIREEFFECTDESC>>			m_Fires;
	vector<shared_ptr<CElectronic::ELECTRONICDESC>>			m_Lightnings;
	vector<shared_ptr<CTornadoEffect::TORNADODESC>>			m_Tornados;
	vector<shared_ptr<CHealEffect::HEALEFFECT>>				m_Heals;
	vector<shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>> m_Lazers;
	vector<shared_ptr<CSwingEffect::SWINGEFFECT>>			m_Swings;

	vector<string> ParticleNames;
	vector<string> TrailEffectsNames;
	vector<string> SwordTrailNames;
	vector<string> DistortionNames;
	vector<string> FireNames;
	vector<string> LightningNames;
	vector<string> TornadoNames;
	vector<string> HealNames;
	vector<string> LazerNames;
	vector<string> SwingNames;


private:
	vector<string> ModelName;
	_int CurModel = 0;

	_bool bShow[20];
public:
	static CImguiMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
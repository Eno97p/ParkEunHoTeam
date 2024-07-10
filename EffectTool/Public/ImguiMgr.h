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

private:
	void CenteredTextColored(const ImVec4& color, const char* text);

private:


private:
	HRESULT ConvertToDDSWithMipmap(const string& inputFilePath, const string& outputFilePath);

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
	const _float4x4* TrailMat = nullptr;

private:
	void Erase_Particle(_int index);

private:
	vector<pair<PARTICLETYPE, void*>>		m_Types;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> TrailEffects;
	vector<shared_ptr<CSTrail::STRAIL_DESC>>	m_SwordTrails;

	vector<string> ParticleNames;
	vector<string> TrailEffectsNames;
	vector<string> SwordTrailNames;
public:
	static CImguiMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
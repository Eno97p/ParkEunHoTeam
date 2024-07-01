#pragma once
#include "Base.h"
#include "Effect_Define.h"
#include "imgui.h"
#include "Particle_Trail.h"

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
	bool IsPNGFile(const std::string& filename) {
		return filename.size() >= 4 && filename.substr(filename.size() - 4) == ".png";
	}
	wstring utf8_to_wstring(const std::string& str);
	HRESULT Add_Texture_Prototype(const wstring& path, const wstring& name);
	ImTextureID DirectXTextureToImTextureID();

private: //for Particle
	void EffectTool();
	PARTICLETYPE EffectListBox(PARTICLETYPE type, void** vDesc);
	HRESULT Store_Effects(char* Name, PARTICLETYPE type, void* Arg);
	HRESULT Save_PointsList();
	HRESULT Load_PointsList();
	HRESULT Save_MeshList();
	HRESULT Load_MeshList();

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
private:
	class CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CTexture* m_pTextureCom = { nullptr };		//선택한 텍스쳐의 프로토타입 포인터
	wstring		m_pTextureProtoName = TEXT("");	//프로토타입 이름
	wstring		m_pTextureFilePath = TEXT("");
	_bool ChangedDesc = false;
	_bool ChangedTrail = false;
	const _float4x4* TrailMat = nullptr;

private:
	vector<void*> MeshEffects;		//메쉬이펙트 저장하는 벡터
	vector<void*> PointEffects;		//포인트 이펙트 저장하는 벡터
	vector<void*> RectEffects;		//렉트 이펙트 저장하는 벡터
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> TrailEffects;

	vector<string> MeshEffectsNames;	//메쉬이펙트 이름 리스트
	vector<string> PointEffectsNames;	//포인트이펙트 이름 리스트
	vector<string> RectEffectsNames;	//렉트이펙트 이름 리스트
	vector<string> TrailEffectsNames;
public:
	static CImguiMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
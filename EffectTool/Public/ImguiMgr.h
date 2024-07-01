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
	CTexture* m_pTextureCom = { nullptr };		//������ �ؽ����� ������Ÿ�� ������
	wstring		m_pTextureProtoName = TEXT("");	//������Ÿ�� �̸�
	wstring		m_pTextureFilePath = TEXT("");
	_bool ChangedDesc = false;
	_bool ChangedTrail = false;
	const _float4x4* TrailMat = nullptr;

private:
	vector<void*> MeshEffects;		//�޽�����Ʈ �����ϴ� ����
	vector<void*> PointEffects;		//����Ʈ ����Ʈ �����ϴ� ����
	vector<void*> RectEffects;		//��Ʈ ����Ʈ �����ϴ� ����
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> TrailEffects;

	vector<string> MeshEffectsNames;	//�޽�����Ʈ �̸� ����Ʈ
	vector<string> PointEffectsNames;	//����Ʈ����Ʈ �̸� ����Ʈ
	vector<string> RectEffectsNames;	//��Ʈ����Ʈ �̸� ����Ʈ
	vector<string> TrailEffectsNames;
public:
	static CImguiMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
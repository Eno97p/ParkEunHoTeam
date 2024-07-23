#pragma once

#include "Base.h"

/* 실제 객체들을 그리는 순서를 관리해야한다. */
/* 대부분의 객체들은 깊이 테스트를 통해서 알아서 차폐관리가 되고 있다. */
/* 깊이버퍼의 사용을 통해 알아서 깊이관리가된다 굉장히 큰 장점이긴한데. */
/* 이로 인해서 생기는 부작용이 존재하낟. */
/* 부작용 1. 스카이박스. : 스카이박스는 실제로 작게 그리낟.  */
/* 하늘 : 무조건 다른 객체들에게 덮여야한다(가장먼저그린다) + 하늘의 깊이는 다른객체들과 비교연산이 일어나서는 안된다.(하늘의 깊이를 깊이버퍼에 기록하지 않는다.) */
/* 부작용 2. 알파블렌드 :   */
/* 블렌드 : 내가 그릴려고하는 색과 이전에 그려져있던 색을 섞는다. */
/* 부작용 3. UI.  */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOWOBJ, RENDER_NONBLEND, RENDER_DECAL, RENDER_NONLIGHT, RENDER_BLEND, RENDER_MIRROR, RENDER_REFLECTION, RENDER_BLUR, RENDER_BLOOM, RENDER_DISTORTION, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

	void Clear();
	void Draw();




#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderGroup[RENDER_END];

private:
	class CShader* m_pShader = { nullptr };
	//class CComputeShader_Texture* m_pBloomComputeShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	_float4x4							m_WorldMatrix = {};
	_float4x4							m_ViewMatrix = {};
	_float4x4							m_ProjMatrix = {};
	_float4x4 m_Mat = { 1.f, 0.f, 0.f, 0.f,
					  0.f, 1.f, 0.f, 0.f,
					  0.f, 0.f, 1.f, 0.f,
					  -155.f, -522.f, -113.f, 1.f };

	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
	ID3D11DepthStencilView* m_pReflectionDepthStencilView = { nullptr };
	_float	m_fTime = 0.f;
	_float m_fValue = 2.2f;

#pragma region MINYOUNG
	//이민영 추가 240621 1423PM
public:
	void Set_ShadowEyeFocus(_vector vEye, _vector vFocus, _float fThreshold)
	{
		m_vShadowEye = vEye;
		m_vShadowFocus = vFocus;
		m_fShadowThreshold = fThreshold;
	}

	_vector Get_ShadowEye() { return m_vShadowEye; };
	_vector Get_ShadowFocus() { return m_vShadowFocus; };

	ID3D11Texture2D* Get_PrevDepthTex() { return m_pPrevDepthTexture; }
	_float Sample_HZB(_float2 uv, UINT mipLevel);

	//이민영 추가 240711 2002PM
private:
	ID3D11Texture2D* m_pPrevDepthTexture = nullptr;
	ID3D11ShaderResourceView* m_pPrevDepthSRV = { nullptr };
	class CComputeShader_Texture* m_pHZBComputeShader = { nullptr };

	static const UINT MAX_MIP_LEVELS = 16;  // 예상되는 최대 밉맵 레벨 수
	ID3D11ShaderResourceView* m_pHZBSRV[MAX_MIP_LEVELS] = { nullptr };
	ID3D11UnorderedAccessView* m_pHZBUAV[MAX_MIP_LEVELS] = { nullptr };
	ID3D11Texture2D* m_pHZBTexture[MAX_MIP_LEVELS] = { nullptr };


	UINT m_HZBMipLevels = 0;

#pragma endregion





#pragma region PARKJAEWON
public:
	void ProcessByThread(DWORD dwThreadIndex);
	void ProcessRenderQueue(DWORD dwThreadIndex, ID3D11DeviceContext* pDeferredContext, RENDERGROUP eRenderGroup, const wchar_t* mrtTarget);
private:
	HRESULT InitRenderThreadPool(DWORD dwThreadCount);
	void ClearRenderThreadPool();
public:
	struct RENDER_THREAD_DESC
	{
		HANDLE hEventList[RENDER_THREAD_EVENT_TYPE_COUNT];
		CRenderer* pRenderer;
		DWORD dwThreadIndex;
		HANDLE hThread;
	};
private:
	RENDER_THREAD_DESC* m_pThreadDescList = nullptr;
	HANDLE m_hCompleteEvent = nullptr;
	LONG m_lActiveThreadCount = 0;
	DWORD m_dwThreadCount = 0;
	vector<ID3D11DeviceContext*> m_DeferredContexts;
	vector<ID3D11CommandList*> m_CommandLists;
	const UINT MAX_OBJECTS_PER_COMMANDLIST = 400;
#pragma endregion

private:
	//LUT TEXTURE
	class CTexture* m_pLUTTex = { nullptr };
	class CTexture* m_pDistortionTex = { nullptr };
	_float								m_fLUTOffset = 0.f;
	vector<_matrix> m_vDecalMatricesInv;
	class CTexture* m_pDecalTex = { nullptr };

	
	//Shadow
private:
	_vector								m_vShadowEye = XMVectorSet(0.f, 10.f, -10.f, 1.f);
	_vector								m_vShadowFocus = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float								m_fShadowThreshold = 0.5f;

private:
	void Render_Priority();
	void Render_ShadowObjects();
	void Render_NonBlend();
	void Render_Decal();
	void Render_LightAcc();
	void Render_DeferredResult();
	void Render_NonLight();
	void Render_Blend();
	void Render_Reflection();
	void Render_Blur();
	void Render_Bloom();
	void Render_Distortion();
	void Render_Final();
	void Compute_HDR();

	void Render_UI();

	void Update_HZB();

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;

private:
	void Render_Debug();
#endif


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
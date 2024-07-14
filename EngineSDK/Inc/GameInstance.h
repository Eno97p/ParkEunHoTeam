#pragma once

#include"CProfileScope.h"
#include"CProfiler.h"


#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

	//������ ���� PSW
	eKEY_STATE GetKeyState(_ubyte byKeyID);
	eKEY_STATE GetMouseState(MOUSEKEYSTATE eMouseState);

public:
	_bool Key_Pressing(_ubyte byKeyID);
	_bool Key_Down(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);

	_bool Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool Mouse_Up(MOUSEKEYSTATE eMouse);

	MOUSEWHEELSTATE CheckMouseWheel(void);	//���콺 �� ������Ʈ�� ��ȯ

public: /* For.Timer_Manager */
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Update_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevel();			//���� ���� �޾ƿ���
public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	list< class CGameObject*> Get_GameObjects_Ref(_uint iLevelIndex, const wstring& strLayerTag);
	void Get_LayerTags_String(_uint iLevelIndex, vector<const char*>* vecRefLayerName);

	

	//240622 2113 PM �̹ο� �߰�
	//FOR CAMERA SWITCH
	HRESULT Add_Camera(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr); // ī�޶� �߰� �� �� �Լ� �ҷ��ߵ�
	void Set_MainCamera(_uint iCameraIndex); // ī�޶� ��ȯ �� ȣ��, �߰� ������ �ε���
	vector<class CCamera*> Get_Cameras();
	CCamera* Get_MainCamera();


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	_bool	IsPrototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif

	//�̹ο� �߰� 240621 1423PM
	void Set_ShadowEyeFocus(_vector vEye, _vector vFocus, _float fThreshold);
	_vector Get_ShadowEye();
	_vector Get_ShadowFocus();
	ID3D11Texture2D* Get_PrevDepthTex();

public: /* For.PipeLine */
	const _float4x4* Get_Transform_float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4x4* Get_Transform_float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4* Get_CamPosition_float4();
	_vector Get_CamPosition();
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);

public: /* For.Picking */	
	_bool Get_PickPos(_float4* pPickPos);
	void Update_Picking();

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Light_Clear();
	void LightOff(_uint iIndex);
	void LightOn(_uint iIndex);

	void LightOff_All();

	void Edit_Light(_uint iIndex, LIGHT_DESC* desc);
	list<class CLight*> Get_Lights();



public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring & strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring & strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, _bool isClear = true, ID3D11DepthStencilView* pDSView = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RenderTargetSRV(const wstring & strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_RenderTargetSRV_Compute(const wstring& strTargetTag, class CComputeShader_Texture* pComputeShader, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring & strTargetTag, ID3D11Texture2D* pDesc);
	_float	Sample_HZB(_float2 uv, UINT mipLevel);

public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
	_bool isIn_LocalFrustum(_fvector vPosition, _float fRange = 0.f);


public: /*for Calculator*/
	_vector Get_RayPos();					//���콺 ���̿� �浹�ϱ� ���� �Լ�����
	_vector Get_RayDir();					
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, class CTransform* pTransform);
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse);
	POINT Get_Mouse_Point();			//���콺 �޴°��� �׻� ���⿡�� 

public:		//for Event_Manager
	HRESULT EventUpdate();				//���� �ۿ��� �����Լ� ������ ȣ��
	void CreateObject(_uint Level, const wchar_t* Layer, const wstring& strPrototypeTag, void* pArg = nullptr); // �������� ��ü �����ϴ� ���
	void CreateObject_Self(_uint Level, const wchar_t* Layer, class CGameObject* pObj); //���� ������ ���ְ� ������ �� ��
	void Erase(CGameObject* _pObj); //�������� ��ü �����ϴ� ���

public: /* For PhysX */
	HRESULT AddActor(PxActor* pActor);
	PxPhysics* GetPhysics();
	PxScene* GetScene();
	PxControllerManager* GetControllerManager();
	 
public: // Sound Mgr
	void PlaySound_Z(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopAll();
	void StopSound(CHANNELID eID);

public:	//for UISorter ����ȣ
	HRESULT Add_UI(class CGameObject* ui, UISORT_PRIORITY type);

//�̹ο� �߰� 240710 2204PM
public: // For OctTree
	_bool IsVisibleObject(CGameObject* obj);
	_bool isVisible(_vector vPos, PxActor* actor);
	void AddCullingObject(CGameObject* obj, PxActor* pActor);


public:	// For Worker
	template<typename T, typename... Args>
	void AddWork(T&& Func, Args&&... args);
	




#ifdef _DEBUG
public:
	HRESULT Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CPhysX*					m_pPhysX = { nullptr };
	class CEventMgr*				m_pEvent_Manager = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };

	class CSoundMgr*				m_pSound_Manager = { nullptr };
	class CUISorter*				m_UISorter = { nullptr };


	class COctTree*				m_pOctTree = { nullptr };

	class CWorker*				m_pWorker = { nullptr };
	class CRenderWorker*		m_pRenderWorker = { nullptr };
public:	
	static void Release_Engine();
	virtual void Free() override;









};

END


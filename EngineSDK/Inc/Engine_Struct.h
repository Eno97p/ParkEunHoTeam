#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	enum class eKEY_STATE
	{
		NONE,
		TAP, //누를떄
		HOLD,//누르고 있을때
		AWAY, // 똇을때
		KEY_END,
	};
	struct tKeyInfo
	{
		eKEY_STATE	eState; //키의 상태값
		bool		bPrevPush;	//이전프레임에 눌렀는지 안눌렀는지
	};







	/* 엔진을 초기화하기위해 필요한 데이터 여러개를 묶었다. */
	typedef struct ENGINE_DESC
	{
		bool			isWindowed;
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	}ENGINE_DESC;

	typedef struct LIGHT_DESC
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_END };

		TYPE		eType = { TYPE_END };				

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		//For Spotlight
		float		innerAngle;
		float		outerAngle;


		/* 빛의 기본적인 색상. */
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	//0 0 1 1
	//1 1 0 1

	//0 0 0 1


	//{
	//	XMFLOAT4	vDiffuse;
	//}D3DMATERIAL9

	typedef struct MESH_MATERIAL
	{
		class CTexture*		MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	}KEYFRAME;

	typedef struct ENGINE_DLL VTXPOS
	{
		XMFLOAT3		vPosition;		

		static const unsigned int		iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];

	}VTXPOS;


	typedef struct ENGINE_DLL VTXPHYSX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vColor;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXPHYSX;





	typedef struct ENGINE_DLL VTXPOINT
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOINT;

	typedef struct ENGINE_DLL  VTXPOSTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXPOSTEX;

	 
	typedef struct ENGINE_DLL VTXCUBE
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXCUBE;

	typedef struct ENGINE_DLL VTXNORTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];

	}VTXNORTEX;

	typedef struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	}VTXMESH;

	typedef struct ENGINE_DLL VTXANIMMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int		iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];

	}VTXANIMMESH;

	typedef struct ENGINE_DLL VTXMATRIX
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT2			vLifeTime;
		_float			   vGravity;
	}VTXMATRIX;

	typedef struct ENGINE_DLL VTXINSTANCE_RECT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL VTXINSTANCE_POINT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL VTXINSTANCE_MESH
	{
		static const unsigned int		iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[8];
	}VTXINSTANCE_MESH;




	typedef struct ENGINE_DLL tEvent //이벤트 매니저 파라미터
	{
		eEVENT_TYPE		eEven;
		DWORD_PTR		lParam;
		DWORD_PTR		wParam;
		DWORD_PTR		pParam;
		DWORD_PTR		eParam;
		DWORD_PTR		fParam;
	}tEvent;

	
}
#endif // Engine_Struct_h__

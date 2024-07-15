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

	typedef struct ENGINE_DLL SwordTrailVertex
	{
		XMFLOAT3 Zero;
		XMFLOAT3 One;
		XMFLOAT3 Two;
		XMFLOAT3 Three;

		XMFLOAT2 texCoord0;
		XMFLOAT2 texCoord1;
		XMFLOAT2 texCoord2;
		XMFLOAT2 texCoord3;

		_float2 lifetime;
		static const unsigned int		iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[9];
	};



	typedef struct ENGINE_DLL tEvent //이벤트 매니저 파라미터
	{
		eEVENT_TYPE		eEven;
		DWORD_PTR		lParam;
		DWORD_PTR		wParam;
		DWORD_PTR		pParam;
		DWORD_PTR		eParam;
		DWORD_PTR		fParam;
	}tEvent;



	namespace Vehicle
	{

	#define MAX_NUM_WHEELS 4


		//차량을 만드는데 필요한 기본적인 정보들
		struct ENGINE_DLL VehicleBeginParams
		{
		public:
			//디폴트로 초기화 필요하다면 초기화
			PX_FORCE_INLINE VehicleBeginParams()
				: mass(1000.0f), dimensions(1.0f, 1.0f, 1.0f), numWheels(4) , maxSteeringAngle(PxPi / 4)
			{
				for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					wheelRadius[i] = 0.5f;
			}
			PX_FORCE_INLINE VehicleBeginParams(PxReal _mass, PxVec3 _dimensions, PxU32 _numWheels, PxReal _wheelRadius, PxReal _maxSteeringAngle)
				: mass(_mass), dimensions(_dimensions), numWheels(_numWheels), maxSteeringAngle(_maxSteeringAngle)
			{
				for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					wheelRadius[i] = _wheelRadius;
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleBeginParams)); }
		

			PxReal mass;			//차량의 질량
			PxVec3 dimensions;		//차량의 크기
			PxU32 numWheels;		//차량의 바퀴 갯수
			PxReal wheelRadius[MAX_NUM_WHEELS];		//바퀴의 반지름
			PxReal maxSteeringAngle;  // 최대 조향 각도 추가
		};

		struct ENGINE_DLL VehicleBeginState
		{
			public:
				//디폴트로 초기화 필요하다면 초기화
				PX_FORCE_INLINE VehicleBeginState()
					: transform(PxTransform(PxIdentity)), linearVelocity(PxVec3(0)), angularVelocity(PxVec3(0))	
				{
				}
				PX_FORCE_INLINE VehicleBeginState(PxTransform _transform, PxVec3 _linearVelocity, PxVec3 _angularVelocity)
					: transform(_transform), linearVelocity(_linearVelocity), angularVelocity(_angularVelocity)
				{
				}

				PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleBeginState));	}
				PX_FORCE_INLINE void setData(const VehicleBeginState& other)
				{
					transform = other.transform;
					linearVelocity = other.linearVelocity;
					angularVelocity = other.angularVelocity;
				}


				PxTransform transform;		//차량의 위치
				PxVec3 linearVelocity;		//차량의 선속도
				PxVec3 angularVelocity;		//차량의 각속도
		};

		struct ENGINE_DLL VehicleMiddleParams
		{
		public:
			PX_FORCE_INLINE VehicleMiddleParams()
				: engineTorque(1000.0f), brakeTorque(1500.0f), steeringAngle(0.0f), steeringSpeed(PxPi / 2)
			{
			}
			PX_FORCE_INLINE VehicleMiddleParams(PxReal _engineTorque, PxReal _brakeTorque, PxReal _steeringAngle, PxReal _steeringSpeed)
				: engineTorque(_engineTorque), brakeTorque(_brakeTorque), steeringAngle(_steeringAngle), steeringSpeed(_steeringSpeed)
			{
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleMiddleParams)); }



			PxReal engineTorque;		//엔진 토크
			PxReal brakeTorque;			//브레이크 토크
			PxReal steeringAngle;		//핸들 각도
			PxReal steeringSpeed;		//핸들 속도
		};

		struct ENGINE_DLL VehicleMiddleState
		{
			public:
				PX_FORCE_INLINE VehicleMiddleState()
				{
					PxMemZero(wheelSpeeds, sizeof(PxReal) * MAX_NUM_WHEELS);
					PxMemZero(suspensionCompression, sizeof(PxReal) * MAX_NUM_WHEELS);
					PxMemZero(&bodyRotation, sizeof(PxQuat));
				}
				PX_FORCE_INLINE VehicleMiddleState(PxReal _wheelSpeeds[MAX_NUM_WHEELS], PxReal _suspensionCompression[MAX_NUM_WHEELS], const PxQuat& BodyRotation  )
				{
					for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					{
						wheelSpeeds[i] = _wheelSpeeds[i];
						suspensionCompression[i] = _suspensionCompression[i];
					}
					bodyRotation = BodyRotation;
				}
				PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleMiddleState)); }
				PX_FORCE_INLINE void setData(const VehicleMiddleState& other)
				{
					for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					{
						wheelSpeeds[i] = other.wheelSpeeds[i];
						suspensionCompression[i] = other.suspensionCompression[i];
					}
					bodyRotation = other.bodyRotation;
				}

				PxReal wheelSpeeds[MAX_NUM_WHEELS];				//바퀴 속도
				PxReal suspensionCompression[MAX_NUM_WHEELS];	//서스펜션 압축
				PxQuat bodyRotation;							//바디 회전
		};

		struct ENGINE_DLL VehicleEndParams
		{
		public:

			PX_FORCE_INLINE VehicleEndParams()
				: maxSpeed(100.0f)
			{
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleEndParams)); }


			PxReal maxSpeed;		//최대 속도
		};

		struct ENGINE_DLL VehicleEndState
		{
		public:
			PX_FORCE_INLINE VehicleEndState()
				: finalPose(PxTransform(PxIdentity)), finalVelocity(PxVec3(0)), finalAngularVelocity(PxVec3(0))
			{
			}
			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleEndState)); }
			PX_FORCE_INLINE void setData(const VehicleEndState& other)
			{
				finalPose = other.finalPose;
				finalVelocity = other.finalVelocity;
				finalAngularVelocity = other.finalAngularVelocity;
			}



			PxTransform finalPose;	//최종 위치
			PxVec3 finalVelocity;	//최종 속도
			PxVec3 finalAngularVelocity;  // 최종 각속도

		};


	}

	struct ENGINE_DLL PxParticleBufferDesc
	{
		PxVec4* positions;               // 파티클의 위치와 반질량(1/mass)
		PxVec4* velocities;              // 파티클의 속도
		PxU32* phases;                  // 파티클의 phase 정보
		PxParticleVolume* volumes;       // 파티클의 볼륨 정보 (선택적)
		PxU32   numActiveParticles;      // 활성화된 파티클 수
		PxU32   maxParticles;            // 최대 파티클 수
		PxU32   numVolumes;              // 볼륨 수 (선택적)
		PxU32   maxVolumes;              // 최대 볼륨 수 (선택적)

		PxParticleBufferDesc()           // 초기화 리스트를 통한 기본 생성자
			: positions(NULL),
			velocities(NULL),
			phases(NULL),
			volumes(NULL),
			numActiveParticles(0),
			maxParticles(0),
			numVolumes(0),
			maxVolumes(0)
		{ }
	};




	
}
#endif // Engine_Struct_h__

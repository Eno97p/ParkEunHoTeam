#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	enum class eKEY_STATE
	{
		NONE,
		TAP, //������
		HOLD,//������ ������
		AWAY, // �H����
		KEY_END,
	};
	struct tKeyInfo
	{
		eKEY_STATE	eState; //Ű�� ���°�
		bool		bPrevPush;	//���������ӿ� �������� �ȴ�������
	};







	/* ������ �ʱ�ȭ�ϱ����� �ʿ��� ������ �������� ������. */
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


		/* ���� �⺻���� ����. */
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



	typedef struct ENGINE_DLL tEvent //�̺�Ʈ �Ŵ��� �Ķ����
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


		//������ ����µ� �ʿ��� �⺻���� ������
		struct ENGINE_DLL VehicleBeginParams
		{
		public:
			//����Ʈ�� �ʱ�ȭ �ʿ��ϴٸ� �ʱ�ȭ
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
		

			PxReal mass;			//������ ����
			PxVec3 dimensions;		//������ ũ��
			PxU32 numWheels;		//������ ���� ����
			PxReal wheelRadius[MAX_NUM_WHEELS];		//������ ������
			PxReal maxSteeringAngle;  // �ִ� ���� ���� �߰�
		};

		struct ENGINE_DLL VehicleBeginState
		{
			public:
				//����Ʈ�� �ʱ�ȭ �ʿ��ϴٸ� �ʱ�ȭ
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


				PxTransform transform;		//������ ��ġ
				PxVec3 linearVelocity;		//������ ���ӵ�
				PxVec3 angularVelocity;		//������ ���ӵ�
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



			PxReal engineTorque;		//���� ��ũ
			PxReal brakeTorque;			//�극��ũ ��ũ
			PxReal steeringAngle;		//�ڵ� ����
			PxReal steeringSpeed;		//�ڵ� �ӵ�
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

				PxReal wheelSpeeds[MAX_NUM_WHEELS];				//���� �ӵ�
				PxReal suspensionCompression[MAX_NUM_WHEELS];	//������� ����
				PxQuat bodyRotation;							//�ٵ� ȸ��
		};

		struct ENGINE_DLL VehicleEndParams
		{
		public:

			PX_FORCE_INLINE VehicleEndParams()
				: maxSpeed(100.0f)
			{
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleEndParams)); }


			PxReal maxSpeed;		//�ִ� �ӵ�
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



			PxTransform finalPose;	//���� ��ġ
			PxVec3 finalVelocity;	//���� �ӵ�
			PxVec3 finalAngularVelocity;  // ���� ���ӵ�

		};


	}

	struct ENGINE_DLL PxParticleBufferDesc
	{
		PxVec4* positions;               // ��ƼŬ�� ��ġ�� ������(1/mass)
		PxVec4* velocities;              // ��ƼŬ�� �ӵ�
		PxU32* phases;                  // ��ƼŬ�� phase ����
		PxParticleVolume* volumes;       // ��ƼŬ�� ���� ���� (������)
		PxU32   numActiveParticles;      // Ȱ��ȭ�� ��ƼŬ ��
		PxU32   maxParticles;            // �ִ� ��ƼŬ ��
		PxU32   numVolumes;              // ���� �� (������)
		PxU32   maxVolumes;              // �ִ� ���� �� (������)

		PxParticleBufferDesc()           // �ʱ�ȭ ����Ʈ�� ���� �⺻ ������
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

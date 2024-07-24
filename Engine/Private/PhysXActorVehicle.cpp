#include "PhysXActorVehicle.h"


#pragma region PhysXIntegrationParams
void PhysXIntegrationParams::create(const PxVehicleAxleDescription& axleDescription,
	const PxQueryFilterData& queryFilterData,
	PxQueryFilterCallback* queryFilterCallback,
	PxVehiclePhysXMaterialFriction* materialFrictions,
	const PxU32 nbMaterialFrictions,
	const PxReal defaultFriction,
	const PxTransform& actorCMassLocalPose,
	const PxVec3& actorBoxShapeHalfExtents,
	const PxTransform& actorBoxShapeLocalPose)
{
	physxRoadGeometryQueryParams.roadGeometryQueryType = PxVehiclePhysXRoadGeometryQueryType::eRAYCAST;
	physxRoadGeometryQueryParams.defaultFilterData = queryFilterData;
	physxRoadGeometryQueryParams.filterCallback = queryFilterCallback;
	physxRoadGeometryQueryParams.filterDataEntries = NULL;

	for (PxU32 i = 0; i < axleDescription.nbWheels; i++)
	{
		const PxU32  wheelId = axleDescription.wheelIdsInAxleOrder[i];
		physxMaterialFrictionParams[wheelId].defaultFriction = defaultFriction;
		physxMaterialFrictionParams[wheelId].materialFrictions = materialFrictions;
		physxMaterialFrictionParams[wheelId].nbMaterialFrictions = nbMaterialFrictions;

		physxSuspensionLimitConstraintParams[wheelId].restitution = 0.0f;
		physxSuspensionLimitConstraintParams[wheelId].directionForSuspensionLimitConstraint = PxVehiclePhysXSuspensionLimitConstraintParams::eROAD_GEOMETRY_NORMAL;

		physxWheelShapeLocalPoses[wheelId] = PxTransform(PxIdentity);
	}

	physxActorCMassLocalPose = actorCMassLocalPose;
	physxActorBoxShapeHalfExtents = actorBoxShapeHalfExtents;
	physxActorBoxShapeLocalPose = actorBoxShapeLocalPose;

}

PhysXIntegrationParams PhysXIntegrationParams::transformAndScale(const PxVehicleFrame& srcFrame,
	const PxVehicleFrame& trgFrame,
	const PxVehicleScale& srcScale,
	const PxVehicleScale& trgScale) const
{

	PhysXIntegrationParams r = *this;
	r.physxRoadGeometryQueryParams = physxRoadGeometryQueryParams.transformAndScale(srcFrame, trgFrame, srcScale, trgScale);
	for (PxU32 i = 0; i < PxVehicleLimits::eMAX_NB_WHEELS; i++)
	{
		r.physxSuspensionLimitConstraintParams[i] = physxSuspensionLimitConstraintParams[i].transformAndScale(srcFrame, trgFrame, srcScale, trgScale);
	}
	r.physxActorCMassLocalPose = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorCMassLocalPose);
	r.physxActorBoxShapeHalfExtents = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorBoxShapeHalfExtents);
	r.physxActorBoxShapeLocalPose = PxVehicleTransformFrameToFrame(srcFrame, trgFrame, srcScale, trgScale, physxActorBoxShapeLocalPose);
	return r;

}
#pragma endregion




#pragma region PhysXIntegrationState
void PhysXIntegrationState::create(const BaseVehicleParams& baseParams, const PhysXIntegrationParams& physxParams, PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial)
{
	setToDefault();

	const PxVehiclePhysXRigidActorParams physxActorParams(baseParams.rigidBodyParams, NULL);
	const PxBoxGeometry boxGeom(physxParams.physxActorBoxShapeHalfExtents);
	const PxVehiclePhysXRigidActorShapeParams physxActorShapeParams(boxGeom, physxParams.physxActorBoxShapeLocalPose, defaultMaterial, PxShapeFlags(0), PxFilterData(), PxFilterData());
	const PxVehiclePhysXWheelParams physxWheelParams(baseParams.axleDescription, baseParams.wheelParams);
	const PxVehiclePhysXWheelShapeParams physxWheelShapeParams(defaultMaterial, PxShapeFlags(0), PxFilterData(), PxFilterData());


	PxVehiclePhysXActorCreate(baseParams.frame,
		physxActorParams,
		physxParams.physxActorCMassLocalPose,
		physxActorShapeParams,
		physxWheelParams,
		physxWheelShapeParams,
		physics, params,
		physxActor);


	//physxConstraints needs to be populated with constraints.
	PxVehicleConstraintsCreate(baseParams.axleDescription, physics, *physxActor.rigidBody, physxConstraints);
}


void PhysXIntegrationState::destroy()
{

	PxVehicleConstraintsDestroy(physxConstraints);
	PxVehiclePhysXActorDestroy(physxActor);
}

#pragma endregion



#pragma region PhysXActorVehicle
bool PhysXActorVehicle::initialize(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial)
{
	mCommandState.setToDefault();

	if (!BaseVehicle::initialize())
		return false;

	if (!mPhysXParams.isValid(mBaseParams.axleDescription))
		return false;

	mPhysXState.create(mBaseParams, mPhysXParams, physics, params, defaultMaterial);

	return true;
}

void PhysXActorVehicle::setUpActor(PxScene& scene, const PxTransform& pose, const char* vehicleName)
{
	mPhysXState.physxActor.rigidBody->setGlobalPose(pose);
	scene.addActor(*mPhysXState.physxActor.rigidBody);

	if(vehicleName != nullptr)
		mPhysXState.physxActor.rigidBody->setName(vehicleName);
}
void PhysXActorVehicle::destroy()
{
	mPhysXState.destroy();

	BaseVehicle::destroy();
}

void PhysXActorVehicle::initComponentSequence(bool addPhysXBeginEndComponents)
{
	if (addPhysXBeginEndComponents)
		mComponentSequence.add(static_cast<PxVehiclePhysXActorBeginComponent*>(this));

	mComponentSequence.add(static_cast<PxVehiclePhysXRoadGeometrySceneQueryComponent*>(this));
}

void PhysXActorVehicle::getDataForPhysXActorBeginComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleCommandState*& commands, const PxVehicleEngineDriveTransmissionCommandState*& transmissionCommands, const PxVehicleGearboxParams*& gearParams, const PxVehicleGearboxState*& gearState, const PxVehicleEngineParams*& engineParams, PxVehiclePhysXActor*& physxActor, PxVehiclePhysXSteerState*& physxSteerState, PxVehiclePhysXConstraints*& physxConstraints, PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates, PxVehicleEngineState*& engineState)
{
		axleDescription = &mBaseParams.axleDescription;
		commands = &mCommandState;
		physxActor = &mPhysXState.physxActor;
		physxSteerState = &mPhysXState.physxSteerState;
		physxConstraints = &mPhysXState.physxConstraints;
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);

		transmissionCommands = NULL;
		gearParams = NULL;
		gearState = NULL;
		engineParams = NULL;
		engineState = NULL;

}

void PhysXActorVehicle::getDataForPhysXActorEndComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams, PxVehicleArrayData<const PxTransform>& wheelShapeLocalPoses, PxVehicleArrayData<const PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates, PxVehicleArrayData<const PxVehicleWheelLocalPose>& wheelLocalPoses, const PxVehicleGearboxState*& gearState, const PxReal*& throttle, PxVehiclePhysXActor*& physxActor)
{
	
		axleDescription = &mBaseParams.axleDescription;
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelParams.setData(mBaseParams.wheelParams);
		wheelShapeLocalPoses.setData(mPhysXParams.physxWheelShapeLocalPoses);
		wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);
		wheelLocalPoses.setData(mBaseState.wheelLocalPoses);
		physxActor = &mPhysXState.physxActor;

		gearState = NULL;
		throttle = &mCommandState.throttle;
	

}

void PhysXActorVehicle::getDataForPhysXConstraintComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams, PxVehicleArrayData<const PxVehiclePhysXSuspensionLimitConstraintParams>& suspensionLimitParams, PxVehicleArrayData<const PxVehicleSuspensionState>& suspensionStates, PxVehicleArrayData<const PxVehicleSuspensionComplianceState>& suspensionComplianceStates, PxVehicleArrayData<const PxVehicleRoadGeometryState>& wheelRoadGeomStates, PxVehicleArrayData<const PxVehicleTireDirectionState>& tireDirectionStates, PxVehicleArrayData<const PxVehicleTireStickyState>& tireStickyStates, PxVehiclePhysXConstraints*& constraints)
{
	
		axleDescription = &mBaseParams.axleDescription;
		rigidBodyState = &mBaseState.rigidBodyState;
		suspensionParams.setData(mBaseParams.suspensionParams);
		suspensionLimitParams.setData(mPhysXParams.physxSuspensionLimitConstraintParams);
		suspensionStates.setData(mBaseState.suspensionStates);
		suspensionComplianceStates.setData(mBaseState.suspensionComplianceStates);
		wheelRoadGeomStates.setData(mBaseState.roadGeomStates);
		tireDirectionStates.setData(mBaseState.tireDirectionStates);
		tireStickyStates.setData(mBaseState.tireStickyStates);
		constraints = &mPhysXState.physxConstraints;
	
}

void PhysXActorVehicle::getDataForPhysXRoadGeometrySceneQueryComponent(const PxVehicleAxleDescription*& axleDescription, const PxVehiclePhysXRoadGeometryQueryParams*& roadGeomParams, PxVehicleArrayData<const PxReal>& steerResponseStates, const PxVehicleRigidBodyState*& rigidBodyState, PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams, PxVehicleArrayData<const PxVehicleSuspensionParams>& suspensionParams, PxVehicleArrayData<const PxVehiclePhysXMaterialFrictionParams>& materialFrictionParams, PxVehicleArrayData<PxVehicleRoadGeometryState>& roadGeometryStates, PxVehicleArrayData<PxVehiclePhysXRoadGeometryQueryState>& physxRoadGeometryStates)
{
	
		axleDescription = &mBaseParams.axleDescription;
		roadGeomParams = &mPhysXParams.physxRoadGeometryQueryParams;
		steerResponseStates.setData(mBaseState.steerCommandResponseStates);
		rigidBodyState = &mBaseState.rigidBodyState;
		wheelParams.setData(mBaseParams.wheelParams);
		suspensionParams.setData(mBaseParams.suspensionParams);
		materialFrictionParams.setData(mPhysXParams.physxMaterialFrictionParams);
		roadGeometryStates.setData(mBaseState.roadGeomStates);
		physxRoadGeometryStates.setEmpty();
	
}

PhysXActorVehicle* PhysXActorVehicle::Create(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial)
{
	PhysXActorVehicle* pInstance = new PhysXActorVehicle();

	if (!pInstance->initialize(physics, params, defaultMaterial))
	{
		MSG_BOX("Failed To Created : CVehicleDefault");
		Safe_Vehicle_Release(pInstance);
		return nullptr;

	}
		return pInstance;


}

HRESULT PhysXActorVehicle::ReadDescroption(const BaseVehicleDesc& BaseDesc)
{
	if(FAILED(ReadAxleDesc(BaseDesc.axleDesc)))
		return E_FAIL;
	if (FAILED(ReadFrameDesc(BaseDesc.frameDesc)))
		return E_FAIL;
	if (FAILED(ReadScaleDesc(BaseDesc.scaleDesc)))
		return E_FAIL;
	if (FAILED(ReadRigidDesc(BaseDesc.rigidBodyDesc)))
		return E_FAIL;
	if (FAILED(ReadSuspensionStateCalculation(BaseDesc.suspensionStateCalculationDesc)))
		return E_FAIL;
	if (FAILED(ReadBrakeResponse(BaseDesc.brakeCommandResponseDesc[0])))
		return E_FAIL;
	if (FAILED(ReadHandBrakeResponse(BaseDesc.brakeCommandResponseDesc[1])))
		return E_FAIL;


	//FrameDesc
	//BaseDesc.frameDesc.







	return S_OK;
}

HRESULT PhysXActorVehicle::ReadAxleDesc(const AxleDescription& AxleDesc)
{
	//AxleDesc
	PxU32 AxelCounts = AxleDesc.AxleCount;

	if (AxelCounts > PxVehicleLimits::eMAX_NB_AXLES)
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}

	for (PxU32 i = 0; i < AxelCounts; i++)	//Ãà°¹¼ö
	{
		const PxU32 nbWheelIds = AxleDesc.WheelCounts[i];
		if (nbWheelIds > PxVehicleLimits::eMAX_NB_WHEELS)
		{
			MSG_BOX("Failed To Read Description: Too many wheels on axle");
			return E_FAIL;
		}

		PxU32 axleWheelIds[PxVehicleLimits::eMAX_NB_WHEELS] = { 0 };
		for (PxU32 j = 0; j < nbWheelIds; j++)
		{
			axleWheelIds[j] = AxleDesc.WheelIds[i][j];
		}
		mBaseParams.axleDescription.addAxle(nbWheelIds, axleWheelIds);

	}

	return S_OK;
}

HRESULT PhysXActorVehicle::ReadFrameDesc(const VehicleFrame& FrameDesc)
{
	const PxU32 lngAxis = FrameDesc.lngAxis;
	const PxU32 latAxis = FrameDesc.latAxis;
	const PxU32 vrtAxis = FrameDesc.vrtAxis;

	if ((lngAxis == latAxis) || (lngAxis == vrtAxis) || (latAxis == vrtAxis))
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}
	mBaseParams.frame.lngAxis = static_cast<PxVehicleAxes::Enum>(lngAxis);
	mBaseParams.frame.latAxis = static_cast<PxVehicleAxes::Enum>(latAxis);
	mBaseParams.frame.vrtAxis = static_cast<PxVehicleAxes::Enum>(vrtAxis);



	return S_OK;
}

HRESULT PhysXActorVehicle::ReadScaleDesc(const VehicleScale& ScaleDesc)
{
	if(ScaleDesc.scale < 0.0f)
	{
		MSG_BOX("Failed To Read Description : CVehicleDefault");
		return E_FAIL;
	}
	mBaseParams.scale.scale = ScaleDesc.scale;
	return S_OK;
}

HRESULT PhysXActorVehicle::ReadRigidDesc(const VehicleRigidBody& RigidDesc)
{
	mBaseParams.rigidBodyParams.mass = RigidDesc.mass;
	if(!RigidDesc.moi.isFinite())
		return E_FAIL;
	mBaseParams.rigidBodyParams.moi = RigidDesc.moi;


	return S_OK;
}

HRESULT PhysXActorVehicle::ReadSuspensionStateCalculation(const VehicleSuspensionStateCalculation& SuspensionStateCalculation)
{
	mBaseParams.suspensionStateCalculationParams.suspensionJounceCalculationType = static_cast<PxVehicleSuspensionJounceCalculationType::Enum>(SuspensionStateCalculation.JounceCalculationType);
	mBaseParams.suspensionStateCalculationParams.limitSuspensionExpansionVelocity = SuspensionStateCalculation.LimitSuspensionExpansionVelocity;

	return S_OK;
}

HRESULT PhysXActorVehicle::ReadBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse)
{
	if(FAILED(ReadBrakeCommandResponse(mBaseParams.axleDescription,BrakeResponse)))

		
	return S_OK;
}

HRESULT PhysXActorVehicle::ReadHandBrakeResponse(const VehicleBrakeCommandResponse& BrakeResponse)
{
	return S_OK;
}

HRESULT PhysXActorVehicle::ReadBrakeCommandResponse(const PxVehicleAxleDescription& AxelDesc, const VehicleBrakeCommandResponse& BrakeCommandResponse)
{
	mBaseParams.brakeResponseParams[0].maxResponse = BrakeCommandResponse.maxResponse;

	const PxU32 nbWheels = AxelDesc.nbWheels;

	const PxU32 Size =sizeof( BrakeCommandResponse.wheelResponseMultipliers) / sizeof(BrakeCommandResponse.wheelResponseMultipliers[0]);

	if(nbWheels< Size)
		return E_FAIL;

	if(Size > PxVehicleLimits::eMAX_NB_WHEELS)
		return E_FAIL;

	for (PxU32 i = 0; i < Size; i++)
	{
		mBaseParams.brakeResponseParams[0].wheelResponseMultipliers[i] = BrakeCommandResponse.wheelResponseMultipliers[i];
	}

	

	

	//if(AxelDesc.nbAxles > )

	//mBaseParams.axleDescription

	return S_OK;
}








#pragma endregion




















void setPhysXIntegrationParams(const PxVehicleAxleDescription& axleDescription, PxVehiclePhysXMaterialFriction* physXMaterialFrictions, PxU32 nbPhysXMaterialFrictions, PxReal physXDefaultMaterialFriction, PhysXIntegrationParams& physXParams)
{
	const PxQueryFilterData queryFilterData(PxFilterData(0, 0, 0, 0), PxQueryFlag::eSTATIC);
	PxQueryFilterCallback* queryFilterCallback = NULL;
	const PxTransform physxActorCMassLocalPose(PxVec3(0.0f, 0.55f, 1.594f), PxQuat(PxIdentity));
	const PxVec3 physxActorBoxShapeHalfExtents(0.84097f, 0.65458f, 2.46971f);
	const PxTransform physxActorBoxShapeLocalPose(PxVec3(0.0f, 0.830066f, 1.37003f), PxQuat(PxIdentity));


	physXParams.create(
		axleDescription,
		queryFilterData, queryFilterCallback,
		physXMaterialFrictions, nbPhysXMaterialFrictions, physXDefaultMaterialFriction,
		physxActorCMassLocalPose,
		physxActorBoxShapeHalfExtents, physxActorBoxShapeLocalPose);
}

#include <WP_CoreComponents/WP_Physics.h>
#include <WP_EngineSystem/WP_PhysicsSystem.h>
#include <WP_CoreComponents/WP_Transform3D.h>
#include <WP_CoreComponents/WP_BoxCollider.h>
#include <utility>

namespace {
	//macro for quick boolean to EActivation
#define WP_ACTIVATION_IS_ACTIVE(x)	(x)?JPH::EActivation::Activate:JPH::EActivation::DontActivate

//================================================================================
//	Macros to quickly write getter setters, apply for trival types
//================================================================================
#ifndef DEFINE_GETTER_SETTER_REF
#define DEFINE_GETTER_SETTER_REF(gFunc ,sFunc, type, member)  \
	const type& gFunc() const			\
	{ return member;}					\
										\
	void sFunc (type const& _newVal)	\
	{ member = _newVal;}				\

#endif

#ifndef DEFINE_GETTER_SETTER
#define DEFINE_GETTER_SETTER(gFunc ,sFunc, type, member)  \
	type gFunc() const					\
	{ return member;}					\
										\
	void sFunc (type _newVal)			\
	{ member = _newVal;}				\

#endif


#ifndef GETTER_SETTER_FLOAT_PERCENT_PHYS_BI
#define GETTER_SETTER_FLOAT_PERCENT_PHYS_BI(gFunc,sFunc,varname,sPhysFunc)		\
float WP_Physics3D::gFunc() const												\
{return varname;}																\
																				\
void WP_Physics3D::sFunc(float _ref)											\
{																				\
	varname = (_ref <= 1.0f)? _ref : (_ref - (int)_ref);						\
	if (m_bID.GetIndexAndSequenceNumber() == JPH::BodyID::cInvalidBodyID)		\
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().sPhysFunc(m_bID,varname);	\
}																				\

#endif

#ifndef GETTER_SETTER_FLOAT_PHYS_BI_DEBUG
#define GETTER_SETTER_FLOAT_PHYS_BI_DEBUG(gFunc,sFunc,varname,sPhysFunc,gPhysFunc)	\
float WP_Physics3D::gFunc() const													\
{	if (!(m_bID.IsInvalid()))														\
		return WP_PhysicsSystem::GetInstance()->GetPhysicsBI().gPhysFunc(m_bID);	\
	else { return varname; }														\
}																					\
																					\
void WP_Physics3D::sFunc(float _ref)												\
{																					\
	varname = _ref;																	\
	if (m_bID.GetIndexAndSequenceNumber() == JPH::BodyID::cInvalidBodyID)			\
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().sPhysFunc(m_bID,varname);		\
}																					\

#endif
#ifndef GETTER_SETTER_FLOAT_PHYS_BI
#define GETTER_SETTER_FLOAT_PHYS_BI(gFunc,sFunc,varname,sPhysFunc)				\
float WP_Physics3D::gFunc() const												\
{return varname;}																\
																				\
void WP_Physics3D::sFunc(float _ref)											\
{																				\
	varname = _ref;																\
	if (m_bID.GetIndexAndSequenceNumber() == JPH::BodyID::cInvalidBodyID)		\
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().sPhysFunc(m_bID,varname);	\
}																				\

#endif

#ifndef GETTER_SETTER_FLOAT_PERCENT_PHYS_BI_DEBUG
#define GETTER_SETTER_FLOAT_PERCENT_PHYS_BI_DEBUG(gFunc,sFunc,varname,sPhysFunc,gPhysFunc)		\
float WP_Physics3D::gFunc() const																\
{																								\
	if (!(m_bID.IsInvalid()))																	\
		return WP_PhysicsSystem::GetInstance()->GetPhysicsBI().gPhysFunc(m_bID);				\
	else { return varname; }																	\
}																								\
void WP_Physics3D::sFunc(float _ref)															\
{																								\
	varname = (abs(_ref) <= 1.0f)? _ref : (_ref - (int)_ref);									\
	if (!(m_bID.IsInvalid()))																	\
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().sPhysFunc(m_bID,varname);					\
}																								\

#endif
}

/*                                                                reflection
____________________________________________________________________________ */
#ifndef QUICK_REGISTER_RTTR_PROPERTY
#define QUICK_REGISTER_RTTR_PROPERTY(_name, _class, _getter,_setter) .property(_name, &_class::_getter,&_class::_setter)
#endif

RTTR_REGISTRATION
{
	rttr::registration::class_<WP_Physics3D>("WP_Physics3D")
		.property("m_isNPC", &WP_Physics3D::GetIsNPC, &WP_Physics3D::SetIsNPC)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		QUICK_REGISTER_RTTR_PROPERTY("m_maxSlopeAngle",WP_Physics3D,GetMaxSlopeInDegrees,SetMaxSlopeInDegrees)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		.property("m_motionType", &WP_Physics3D::GetMotionType, &WP_Physics3D::SetMotionType)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		.property("m_shapeType", &WP_Physics3D::m_shapeType)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		.property("m_shapeScale", &WP_Physics3D::GetScale, &WP_Physics3D::SetScale)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true),
			rttr::metadata(MetaDataTypes::PHYSICS_SHAPE_SCALE,true)
		)
		//.property("m_layer", &WP_Physics3D::GetObjectLayer, &WP_Physics3D::SetObjectLayer)
		.property("m_active", &WP_Physics3D::GetIsActive, &WP_Physics3D::SetIsActive)
		.property("m_pureStatic", &WP_Physics3D::GetIsPureStatic, &WP_Physics3D::SetIsPureStatic)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		QUICK_REGISTER_RTTR_PROPERTY("m_isTrigger",WP_Physics3D,GetIsTrigger,SetIsTrigger)
		(
			rttr::metadata(MetaDataTypes::DISABLE_ON_RUN_IMGUI,true)
		)
		.property("m_lockedAxes", &WP_Physics3D::GetLockedAxis, &WP_Physics3D::SetLockedAxis)
		(
			rttr::metadata(MetaDataTypes::CHAR_IS_BITMAP, true)
		)
		.property("m_posOffset", &WP_Physics3D::GetPosOffset, &WP_Physics3D::SetPosOffset)
		.property("m_rotOffset", &WP_Physics3D::GetRotOffsetAngles, &WP_Physics3D::SetRotOffsetAngles)
		.property("m_gravity", &WP_Physics3D::GetGravityScale, &WP_Physics3D::SetGravityScale)
		QUICK_REGISTER_RTTR_PROPERTY("m_friction",WP_Physics3D,GetFriction,SetFriction)
		QUICK_REGISTER_RTTR_PROPERTY("m_restitution",WP_Physics3D,GetCOR,SetCOR)
		QUICK_REGISTER_RTTR_PROPERTY("m_mass",WP_Physics3D,GetMass,SetMass)
	;

//================================================================================
//	Things owned under JPH body
// settings:
//	- active
//	- kinematic/ static/ dynamic 
//	- ObjectLayer
//	- Motion Quality (linearcst(precise stuff), discrete(big stuff))
//  - JPH Shape
//	
// Physics values:
//	- velocity (linear/angular)
//	- Position
//	- Rotation
//	- Friction
//	- Restitution (bounce factor)
//	- Gravity factor
//	- ManifoldReduction??
//	- rigidbody/softbody. default rigidbody. softbody is not to be supported.
// 
// All values implicitly owned by JPH::Body must be accessed using BodyInterface
// accessor methods. BodyInterface must be retrieved from PhysicsSystem to get the
// correct BodyInterface.
//================================================================================
}

/* _________________________________________________________________________ */

ComponentTypeBitSet WP_Physics3D::GetBitSet()
{
	return static_cast<ComponentTypeBitSet>(WP_ComponentType::Physics3D);
}

//================================================================================
//					WP_PhysicsShape operators
//================================================================================
WP_PhysicsShape& operator++(WP_PhysicsShape& _shape)
{
	typedef std::underlying_type<WP_PhysicsShape>::type utype;

	if (_shape == WP_PhysicsShape::NUM_PHYSICS_SHAPES)
	{
		_shape = WP_PhysicsShape::EMPTY;
	}
	else
	{
		_shape = static_cast<WP_PhysicsShape>(static_cast<utype>(_shape) + 1);
	}
	return _shape;
}
WP_PhysicsShape operator++(WP_PhysicsShape& _shape, int)
{
	typedef std::underlying_type<WP_PhysicsShape>::type utype;
	auto retval = _shape;
	++_shape;
	return retval;
}
WP_PhysicsShape& operator--(WP_PhysicsShape& _shape)
{
	typedef std::underlying_type<WP_PhysicsShape>::type utype;

	if (_shape == WP_PhysicsShape::NUM_PHYSICS_SHAPES)
	{
		_shape = WP_PhysicsShape::EMPTY;
	}
	else
		_shape = static_cast<WP_PhysicsShape>(static_cast<utype>(_shape) - 1);
	return _shape;
}
WP_PhysicsShape operator--(WP_PhysicsShape& _shape, int)
{
	typedef std::underlying_type<WP_PhysicsShape>::type utype;
	auto retval = _shape;
	--_shape;
	return retval;
}

//================================================================================
//						WP_Physics3D Component functions
//================================================================================
WP_Physics3D::WP_Physics3D()
{}

void WP_Physics3D::Init()
{}

void WP_Physics3D::OnEnabled()
{
	SetBodyActive();
}

void WP_Physics3D::OnDisabled()
{
	SetBodyUnactive();
}

WP_Physics3D::~WP_Physics3D()
{
}

WP_Physics3D::WP_Physics3D(WP_Physics3D&& _ref) noexcept
{
	*this = std::move(_ref);
}

WP_Physics3D::WP_Physics3D(WP_Physics3D const& _ref)
{
	*this = _ref;
}

WP_Physics3D& WP_Physics3D::operator=(WP_Physics3D&& _ref) noexcept
{
	if (!m_bID.IsInvalid()) { RemoveBody(); }
	m_charPtr.swap(_ref.m_charPtr);

	m_isActive = std::move(_ref.m_isActive);
	m_isPureStatic = std::move(_ref.m_isPureStatic);
	m_useTransformScale = std::move(_ref.m_useTransformScale);
	m_isNPC = std::move(_ref.m_isNPC);
	m_motionType = std::move(_ref.m_motionType);
	m_shapeType = std::move(_ref.m_shapeType);
	m_lockedAxis = std::move(_ref.m_lockedAxis);

	m_objectLayer = std::move(_ref.m_objectLayer);

	m_bID = std::move(_ref.m_bID);

	//advance settings
	m_gravityScale = std::move(_ref.m_gravityScale);
	m_friction = std::move(_ref.m_friction);
	m_restitution = std::move(_ref.m_restitution);
	m_mass = std::move(_ref.m_mass);
	m_maxSlopeAngleRadians = std::move(_ref.m_maxSlopeAngleRadians);
	m_maxSeperationDistance = std::move(_ref.m_maxSeperationDistance);

	m_posOffset = std::move(_ref.m_posOffset);
	m_rotOffset = std::move(_ref.m_rotOffset);
	// _ref is now an invalid object that will cause undefined behaviour if reused
	return *this;
}

WP_Physics3D& WP_Physics3D::operator=(WP_Physics3D const& _ref) 
{
	if (!m_bID.IsInvalid()) { RemoveBody(); }

	m_isActive = _ref.m_isActive;
	m_isPureStatic = _ref.m_isPureStatic;
	m_useTransformScale = _ref.m_useTransformScale;
	m_isNPC = _ref.m_isNPC;
	m_motionType = _ref.m_motionType;
	m_shapeType = _ref.m_shapeType;
	m_lockedAxis = _ref.m_lockedAxis;

	m_objectLayer = _ref.m_objectLayer;

	m_bID = JPH::BodyID(JPH::BodyID::cInvalidBodyID);

	//advance settings
	m_gravityScale = _ref.m_gravityScale;
	m_friction = _ref.m_friction;
	m_restitution = _ref.m_restitution;
	m_mass = _ref.m_mass;
	m_maxSlopeAngleRadians = _ref.m_maxSlopeAngleRadians;
	m_maxSeperationDistance = _ref.m_maxSeperationDistance;

	m_posOffset = _ref.m_posOffset;
	m_rotOffset = _ref.m_rotOffset;

	//due to the irregular addbody, removebody is not guranteed to be called. As such, addbody shall not be called.
	//AddBody();	//add to physics. don't make it a habit to create physics at runtime. no optimization is applied here.
	return *this;
}

void WP_Physics3D::SetScaleUniform(float _uniformScale)
{
	m_shapeScale = glm::vec3{_uniformScale,_uniformScale ,_uniformScale };
}

void WP_Physics3D::SetScale(glm::vec3 const& _scale)
{
	switch (m_shapeType) 
	{
	case WP_PhysicsShape::EMPTY:
		return;
	case WP_PhysicsShape::SPHERE:				//sphere, use x axis only
		SetScaleSphere(_scale.x);
		return;
	case WP_PhysicsShape::CUBE:					//cube
		SetScaleCube(_scale);
		return;
	case WP_PhysicsShape::CAPSULE:				//capsule, use x and y axis only
		SetScaleCapsule(_scale.x, _scale.y);
		return;
	case WP_PhysicsShape::CYLINDER:
		SetScaleCylinder(_scale.x, _scale.y);	//cylinder, use x and y axis only
		return;
	default:
		assert(0 && "Invalid Shape type found,  WP_Physics3D::SetScale(glm::vec3 const& _scale)");
		WP_ERROR("Invalid Shape type found,  WP_Physics3D::SetScale(glm::vec3 const& _scale)");
		return;
	}
}

void WP_Physics3D::SetScaleCube(glm::vec3 const& _xyzScale)
{
	m_shapeScale = _xyzScale;
}

void WP_Physics3D::SetScaleSphere(float _newRadius)
{
	m_shapeScale = glm::vec3{ _newRadius, _newRadius ,_newRadius };
}

void WP_Physics3D::SetScaleCapsule(float _radius, float _height)
{
	m_shapeScale = glm::vec3{ _radius, _height ,_radius };
}

void WP_Physics3D::SetScaleCylinder(float _radius, float _height)
{
	m_shapeScale = glm::vec3{ _radius, _height ,_radius };
}

glm::vec3 const& WP_Physics3D::GetScale() const
{
	return m_shapeScale;
}

JPH::EMotionType WP_Physics3D::GetMotionType() const
{
	//return WP_PhysicsSystem::GetInstance()->GetPhysicsBI().GetMotionType(m_bID);
	return m_motionType;
}
void WP_Physics3D::SetMotionType(JPH::EMotionType _motionType)
{
	//
	//if (m_isPureStatic) { return; } //pure static objects never change types
	m_motionType = _motionType;
	switch (_motionType)
	{	//for now, switch between 2 layers for demo
	case JPH::EMotionType::Static:
		//bodyInterface.SetObjectLayer(m_bID,Layers::NON_MOVING);
		m_objectLayer = Layers::NON_MOVING;
		m_isActive = false;
		m_isNPC = false;
		break;
	case JPH::EMotionType::Kinematic:
		m_isNPC = false;
	case JPH::EMotionType::Dynamic:			//expected fall through
		m_objectLayer = Layers::MOVING;
		SetIsTrigger(false);
		break;
	default:
		break;
	}
#ifdef PHYSICS_ALLOW_RUNTIME_ADJUSTMENTS
	if (m_bID.GetIndexAndSequenceNumber() == JPH::BodyID::cInvalidBodyID)
	{
		return;
	}

	auto& bodyInterface = WP_PhysicsSystem::GetInstance()->GetPhysicsBI();
	bodyInterface.SetMotionType(m_bID, _motionType, WP_ACTIVATION_IS_ACTIVE(bodyInterface.IsActive(m_bID)));
	bodyInterface.SetObjectLayer(m_bID, m_objectLayer);
#endif
	}


JPH::ObjectLayer WP_Physics3D::GetObjectLayer() const
{
	//return WP_PhysicsSystem::GetInstance()->GetPhysicsBI().GetObjectLayer(m_bID);
	return m_objectLayer;
}
void WP_Physics3D::SetObjectLayer(JPH::ObjectLayer _objLayer)
{
	m_objectLayer = _objLayer;
#ifdef PHYSICS_ALLOW_RUNTIME_ADJUSTMENTS
	if (m_bID.GetIndexAndSequenceNumber() != JPH::BodyID::cInvalidBodyID)
	{
		auto& bodyInterface = WP_PhysicsSystem::GetInstance()->GetPhysicsBI();
		bodyInterface.SetObjectLayer(m_bID, _objLayer);
	}
#endif // PHYSICS_ALLOW_RUNTIME_ADJUSTMENTS


}

bool WP_Physics3D::GetIsActive() const
{
	if(m_bID.IsInvalid())
		return m_isActive;
	return WP_PhysicsSystem::GetInstance()->GetPhysicsBI().IsActive(m_bID);
}
void WP_Physics3D::SetIsActive(bool _active)
{
	if (WP_PhysicsSystem::GetInstance()->GetIsPhysicsLocked()
		|| m_motionType == JPH::EMotionType::Static)
	{
		return;
	}	//static always inactive

	m_isActive = _active;										//track active state in component

	if (m_bID.IsInvalid()) { return; }	//if body not yet created, no change
	auto& bodyInterface = WP_PhysicsSystem::GetInstance()->GetPhysicsBI();
	if (_active)
		bodyInterface.ActivateBody(m_bID);
	else
		bodyInterface.DeactivateBody(m_bID);
}

bool WP_Physics3D::GetIsValid() const { return !m_bID.IsInvalid(); }

//================================================
//		GetterSetter: m_isPureStatic
//================================================
bool WP_Physics3D::GetIsPureStatic() const
{
	return m_isPureStatic;
}
void WP_Physics3D::SetIsPureStatic(bool _newBool)
{
	m_isPureStatic = _newBool;
	if(_newBool)
		m_isNPC = false;
}

bool WP_Physics3D::GetIsTrigger() const
{
	return m_isTrigger;
}
void WP_Physics3D::SetIsTrigger(bool _newBool)
{
	m_isTrigger = _newBool;
	
	if (!_newBool)
	{ return; }

	if (m_motionType == JPH::EMotionType::Dynamic) { m_motionType = JPH::EMotionType::Kinematic; }
	m_isNPC = false;
}

//DEFINE_GETTER_SETTER(WP_Physics3D::GetIsNPC, WP_Physics3D::SetIsNPC, bool, m_isNPC)
bool WP_Physics3D::GetIsNPC() const
{
	return m_isNPC;
}
void WP_Physics3D::SetIsNPC(bool _newBool)
{
	m_isNPC = _newBool;
	SetIsPureStatic(false);
	SetIsTrigger(false);
	SetMotionType(JPH::EMotionType::Dynamic);
}

DEFINE_GETTER_SETTER(WP_Physics3D::GetMaxSlope, WP_Physics3D::SetMaxSlope, float, m_maxSlopeAngleRadians)

#if _DEBUG
GETTER_SETTER_FLOAT_PHYS_BI_DEBUG(GetGravityScale, SetGravityScale, m_gravityScale, SetGravityFactor,GetGravityFactor)
#else
GETTER_SETTER_FLOAT_PHYS_BI(GetGravityScale, SetGravityScale, m_gravityScale, SetGravityFactor)
#endif
float WP_Physics3D::GetMaxSlopeInDegrees() const 
{
	return JPH::RadiansToDegrees(m_maxSlopeAngleRadians);
}

void WP_Physics3D::SetMaxSlopeInDegrees(float _angle)
{
	m_maxSlopeAngleRadians = JPH::DegreesToRadians(_angle);
}

//================================================
//		GetterSetter: m_posOffset
//================================================
glm::vec3 WP_Physics3D::GetPosOffset() const
{
	return WP_Physics::ToGLMVec3(m_posOffset);
}
void WP_Physics3D::SetPosOffset(glm::vec3 _newPos)
{
	m_posOffset = WP_Physics::ToJoltVec3(_newPos);
}

//================================================
//		GetterSetter: m_rotOffset
//================================================
glm::quat WP_Physics3D::GetRotOffset() const
{
	return WP_Physics::ToGLMQuat(m_rotOffset);
}
void WP_Physics3D::SetRotOffset(glm::quat _newRot)
{
	m_rotOffset = WP_Physics::ToJoltQuat(glm::normalize(_newRot));
}

glm::vec3 WP_Physics3D::GetRotOffsetAngles() const
{
	return GetDegreesFromRadianVector(WP_Physics::ToGLMVec3(m_rotOffset.GetEulerAngles()));
}

void WP_Physics3D::SetRotOffsetAngles(glm::vec3 _newRot) 
{
	GetRadianFromDegreesVector(_newRot);
	m_rotOffset = JPH::Quat::sEulerAngles(WP_Physics::ToJoltVec3(_newRot));
}


//================================================
//		GetterSetter: Offset transform
//================================================
glm::mat4x4 WP_Physics3D::GetOffsetTransformMat() const
{
	JPH::Mat44 retval = JPH::Mat44::sIdentity();
	retval = retval - JPH::Mat44::sIdentity();
	auto transComp = WP_ComponentList<WP_Transform3D>::GetComponentList()->GetComponent(GetGameObjectID());
	//retval.SetRotation(retval.sRotation(m_rotOffset * WP_Physics::ToJoltQuat(transComp->m_angle)));
	retval.SetTranslation(m_posOffset + WP_Physics::ToJoltVec3(transComp->m_position));
	return WP_Physics::ToGLMMat4x4(retval.sScale(WP_Physics::ToJoltVec3(transComp->m_scale)));
}

//================================================
//		GetterSetter: m_lockedAxis
//================================================
char WP_Physics3D::GetLockedAxis() const
{
	return static_cast<char>(m_lockedAxis);
}
void WP_Physics3D::SetLockedAxis(char _axes)
{
	m_lockedAxis = static_cast<JPH::EAllowedDOFs>(_axes);
}

//================================================
//			Character Functions
//================================================

void WP_Physics3D::CharacterSetLinearVelocity(glm::vec3 const& _vel)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	m_charPtr->SetLinearVelocity(WP_Physics::ToJoltVec3(_vel));

}
void WP_Physics3D::CharacterAddVelocity(glm::vec3 const& _vel)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	m_charPtr->AddLinearVelocity(WP_Physics::ToJoltVec3(_vel));
}
glm::vec3 WP_Physics3D::CharacterGetLinearVelocity() const
{
	if (m_bID.IsInvalid() || !m_charPtr) { return glm::vec3(0, 0, 0); }
	return WP_Physics::ToGLMVec3(m_charPtr->GetLinearVelocity());
}
void WP_Physics3D::CharacterAddImpulse(glm::vec3 const& _imp)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	m_charPtr->AddImpulse(WP_Physics::ToJoltVec3(_imp));
}

//rotation in degrees for each axis for the 3D Gimbal
void WP_Physics3D::CharacterSetRotation(glm::vec3 const& _rotInDegrees)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	m_charPtr->SetRotation(JPH::Quat::sEulerAngles(WP_Physics::ToJoltVec3(GetRadianFromDegreesVector(_rotInDegrees))));
}
void WP_Physics3D::CharacterRotate(glm::vec3 const& _addRot)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	auto rot = m_charPtr->GetRotation().GetEulerAngles() + WP_Physics::ToJoltVec3(GetRadianFromDegreesVector(_addRot));
	m_charPtr->SetRotation(JPH::Quat::sEulerAngles(rot));
}
//rotation for y axis only
void WP_Physics3D::CharacterRotate(float _angle)
{
	if (m_bID.IsInvalid() || !m_charPtr) { return; }
	auto rot = m_charPtr->GetRotation().GetEulerAngles() + WP_Physics::ToJoltVec3(glm::vec3(0, JPH::DegreesToRadians(_angle), 0));
	m_charPtr->SetRotation(JPH::Quat::sEulerAngles(rot));
}
glm::vec3 WP_Physics3D::CharacterGetRotattion() const
{
	if (m_bID.IsInvalid() || m_isNPC || !m_charPtr)
	{
		assert(0 && "no Physics body or is not a Physics Character! WP_Physics3D::CharacterGetRotattion()");
		return glm::vec3(0, 0, 0);
	}
	return GetDegreesFromRadianVector(WP_Physics::ToGLMVec3(m_charPtr->GetRotation().GetEulerAngles()));
}


//================================================
//				Add/Remove Functions
//================================================
void WP_Physics3D::AddBody()
{
	auto* transComp = WP_ComponentList<WP_Transform3D>::GetComponentList()->GetComponent(GetGameObjectID());

	WP_BoxCollider* boxComp = WP_ComponentList<WP_BoxCollider>::GetComponentList()->GetComponent(GetGameObjectID());

	std::unique_ptr<JPH::ShapeSettings> shapeSettingsPtr{ nullptr };	//pointer to switch between shapes

	switch (m_shapeType)	//get appropriate shape settings with default values
	{
	case WP_PhysicsShape::EMPTY:
	case WP_PhysicsShape::NUM_PHYSICS_SHAPES:	//expected fall through
		shapeSettingsPtr = std::make_unique<JPH::EmptyShapeSettings>();
		m_shapeType = WP_PhysicsShape::EMPTY;							//reset to 
		break;
	case WP_PhysicsShape::CUBE:
		
#ifndef IF_BOX_COLLIDER_DEPENDENT	//no dependency on box collider
		//calculate global scale
		//assume no position or angle offset
		//just to make it work currently
		if(boxComp)
		{
			glm::mat4 scaleMat = glm::mat4();
			scaleMat = glm::scale(scaleMat, boxComp->m_scale);
			glm::vec3 otherScale = transComp->m_globalScale;
			otherScale.x = otherScale.x / 2 * boxComp->m_scale.x;
			otherScale.y = otherScale.y / 2 * boxComp->m_scale.y;
			otherScale.z = otherScale.z / 2 * boxComp->m_scale.z;
			shapeSettingsPtr = std::make_unique<JPH::BoxShapeSettings>(WP_Physics::ToJoltVec3(otherScale));
		}
		else 
		{
			shapeSettingsPtr = std::make_unique<JPH::BoxShapeSettings>(WP_Physics::ToJoltVec3(m_shapeScale));
		}
#endif
		
		break;
	case WP_PhysicsShape::SPHERE:
	{	//create sphere with all avg scale
		shapeSettingsPtr = std::make_unique<JPH::SphereShapeSettings>(m_shapeScale.x);
	}
#if 0
	{	//reference to get to settings from a generic shape unique pointer
		JPH::SphereShapeSettings& sRef = dynamic_cast<JPH::SphereShapeSettings&>(*(shapeSettingsPtr.get()));	//this is pretty cursed...
	}
#endif
	break;
	case WP_PhysicsShape::CAPSULE:
	{
		shapeSettingsPtr = std::make_unique<JPH::CapsuleShapeSettings>(m_shapeScale.y, m_shapeScale.x);
	}
	break;
	case WP_PhysicsShape::CYLINDER:
	{
		shapeSettingsPtr = std::make_unique<JPH::CapsuleShapeSettings>(m_shapeScale.y, m_shapeScale.x);
	}
	break;
	default:
		assert(0 && "invalid shape type for physics component [%d]");
		break;
	};
#ifdef _DEBUG
	auto shapeChecker{ shapeSettingsPtr->Create() };
	assert(shapeChecker.IsValid());				//check for invalid shapes

	if (m_isNPC) { AddCharacter(shapeChecker); return; }	//if character, split off to character creation function

	JPH::BodyCreationSettings bodySettings = JPH::BodyCreationSettings
	(shapeChecker.Get()							//predefined & checked shape
		, JPH::Vec3Arg()						//zero vector position, values will be updated before first physics loop
		, JPH::QuatArg::sIdentity()				//Id Quartation for rotation, values will be updated before first physics loop
		, m_motionType							//preset motion type
		, m_objectLayer);						//preset objectLayer
#else
	if (m_isNPC) { AddCharacter(shapeSettingsPtr->Create()); return; }	//if character, split off to character creation function

	JPH::BodyCreationSettings bodySettings = JPH::BodyCreationSettings
	(shapeSettingsPtr->Create().Get()			//predefined shape
		, JPH::Vec3Arg()						//zero vector position, values will be updated before first physics loop
		, JPH::QuatArg::sIdentity()				//Id Quartation for rotation, values will be updated before first physics loop
		, m_motionType							//preset motion type
		, m_objectLayer);						//preset objectLayer
#endif // _DEBUG

	if (m_isPureStatic)
	{
		m_motionType = bodySettings.mMotionType = JPH::EMotionType::Static;	//must be static motion type
	}
	else
	{
		bodySettings.mAllowDynamicOrKinematic = true;			//allow switching within motion types
		bodySettings.mAllowedDOFs = m_lockedAxis;
	}
		
	bodySettings.mIsSensor = m_isTrigger;
	JPH::MassProperties massStats;
	massStats.ScaleToMass(m_mass);
	bodySettings.mMassPropertiesOverride = massStats;

	bodySettings.mGravityFactor = m_gravityScale;
	bodySettings.mFriction = m_friction;
	bodySettings.mRestitution = m_restitution;
	
	m_bID = WP_PhysicsSystem::GetInstance()->GetPhysicsBI().CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
	m_isInPhysicsSystem = true;
	//std::cout <<"created with ID[" << (m_bID.GetIndex()) << "]\n";
	//WP_WARN("created with ID[%d]\n",m_bID.GetIndex());
	assert(!m_bID.IsInvalid());
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().SetUserData(m_bID,GetGameObjectID());
}

void WP_Physics3D::AddCharacter(JPH::ShapeSettings::ShapeResult const& _shape)
{	//set ptr, redirect from add body
	if (m_charPtr) { return; }
	JPH::CharacterSettings settings {};

	settings.mShape = _shape.Get();
	settings.mFriction = m_friction;
	settings.mGravityFactor = m_gravityScale;
	settings.mLayer = m_objectLayer;
	settings.mMass = m_mass;
	settings.mMaxSlopeAngle = m_maxSlopeAngleRadians;

	m_charPtr = std::make_unique<JPH::Character>(&settings,
		JPH::Vec3Arg::sZero(),
		JPH::Quat::sZero(),
		0,
		&(WP_PhysicsSystem::GetInstance()->GetPhysicsSystem()));

	m_charPtr->AddToPhysicsSystem();
	m_isInPhysicsSystem = true;
	m_bID = m_charPtr->GetBodyID();
	assert(!m_bID.IsInvalid());
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().SetUserData(m_bID, GetGameObjectID());
}

void WP_Physics3D::RemoveBody() 
{	
	if (m_bID.IsInvalid()) { return; }	//catch no create body
	if (m_isNPC) { RemoveCharacter(); return; }					//special remove npc

	if (m_isInPhysicsSystem) 
	{ 
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().RemoveBody(m_bID);
		m_isInPhysicsSystem = false;
	}
	//remove body from system, then destroy the body.
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().DestroyBody(m_bID);
	m_bID = (JPH::BodyID)JPH::BodyID::cInvalidBodyID;
}


void WP_Physics3D::RemoveCharacter()
{	//unset ptr, redirect from remove body
	if (!m_charPtr) { return; }
	if (m_isInPhysicsSystem)
		m_charPtr->RemoveFromPhysicsSystem();
	
	m_isInPhysicsSystem = false;
	m_charPtr.reset();

	//WP_PhysicsSystem::GetInstance()->GetPhysicsBI().DestroyBody(m_bID);

	assert(!m_bID.IsInvalid());
	m_bID = (JPH::BodyID)JPH::BodyID::cInvalidBodyID;
}

void WP_Physics3D::SuspendBody()
{
	SetBodyActive(false);
}

void WP_Physics3D::SetBodyActive(bool _active) 
{
	if (_active) 
	{
		SetBodyActive();
		return;
	}
	SetBodyUnactive();
}

void WP_Physics3D::SetBodyActive() 
{
	assert(!m_isInPhysicsSystem || !m_bID.IsInvalid());			//debug mode assert
	if (m_isInPhysicsSystem || m_bID.IsInvalid()) { return; }	//catch on release 
	if (m_isNPC) {
		assert(m_charPtr);
		if (!m_charPtr) { return; }
		m_charPtr->AddToPhysicsSystem();
		m_isInPhysicsSystem = true; return;
	}
	else 
	{
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().AddBody(m_bID,WP_ACTIVATION_IS_ACTIVE(m_isActive));
		m_isInPhysicsSystem = true;
	}
}

void WP_Physics3D::SetBodyUnactive() 
{
	assert(m_isInPhysicsSystem && !m_bID.IsInvalid());			//debug mode assert
	if (!m_isInPhysicsSystem || m_bID.IsInvalid()) { return; }	//catch on release 
	if (m_isNPC) {
		assert(m_charPtr);
		if (!m_charPtr) { return; }
		m_charPtr->RemoveFromPhysicsSystem();
		m_isInPhysicsSystem = false; return;
	}
	else 
	{
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().RemoveBody(m_bID);
		m_isInPhysicsSystem = false;
	}
}

JPH::ObjectLayer GetObjectLayer(JPH::EMotionType _motionType) 
{
	switch (_motionType)
	{	//for now, switch between 2 layers for demo
	case JPH::EMotionType::Static:
		return Layers::NON_MOVING;
	case JPH::EMotionType::Kinematic:
	case JPH::EMotionType::Dynamic:
		return Layers::MOVING;
	default:
		assert(0 && "unsupported motion type found");
		return Layers::MOVING;
	}
}

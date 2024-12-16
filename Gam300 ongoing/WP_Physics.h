#pragma once
#include <WP_CORELib.h>

#include <WP_Reflection/WP_Registration.h>
#include <WP_EngineSystem/WP_CSharp/WP_CSharpSystem.h>
#include <WP_ECS/WP_Component.h>

#include <type_traits>

//Other Jolt includes
#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/EmptyShape.h>

#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/Character.h>					//character class for npc

enum class WP_PhysicsShape : char
{
	EMPTY,
	//PLANE,		//planes are special because i dont know how they work yet. WIP
	SPHERE,
	CUBE,
	CAPSULE,
	CYLINDER,
	NUM_PHYSICS_SHAPES
	//,CONVEX_HULL //custom shapes, currently unsupported
};
DLL_API WP_PhysicsShape& operator++(WP_PhysicsShape& _shape);
DLL_API WP_PhysicsShape operator++(WP_PhysicsShape& _shape, int);
DLL_API WP_PhysicsShape& operator--(WP_PhysicsShape& _shape);
DLL_API WP_PhysicsShape operator--(WP_PhysicsShape& _shape, int);

namespace
{
#ifndef DEFINE_GETTER_SETTER
#define DEFINE_GETTER_SETTER(gFunc,sFunc,type,varname)			\
type gFunc() const												\
{return varname;}												\
																\
void sFunc(type _ref)											\
{varname = _ref;}												\

#endif

#ifndef DEFINE_GETTER_SETTER_WITH_HANDLING
#define DEFINE_GETTER_SETTER_WITH_HANDLING(gFunc,sFunc,type,varname,func1,func2)\
type gFunc() const												\
{return func1(varname);}										\
																\
void sFunc(type _ref)											\
{varname = func2(_ref);}										\

#endif

#ifndef DECLARE_GETTER_SETTER
#define DECLARE_GETTER_SETTER(gFunc,sFunc,type,varname)			\
type gFunc() const;												\
void sFunc(type _ref);											\

#endif

#ifndef GETTER_SETTER_FLOAT_PERCENT
#define GETTER_SETTER_FLOAT_PERCENT(gFunc,sFunc,varname)		\
float gFunc() const												\
{return varname;}												\
																\
void sFunc(float _ref)											\
{																\
	varname = (_ref <= 1.0f)? _ref : (_ref - (int)_ref);		\
}																\

#endif

#ifndef DEFINE_GETTER_SETTER_REF
#define DEFINE_GETTER_SETTER_REF(gFunc,sFunc,type,varname)		\
type const& gFunc() const										\
{return varname;}												\
																\
void sFunc(type const& _ref)									\
{varname = _ref;}												\

#endif

template <typename T>
inline bool AccessAndTestComponent(T* _var, WP_GameObjectID _id) noexcept
{
	_var = WP_ComponentList<T>::GetComponentList()->GetComponent(_id);
	return !_var;
}

}

//store default shapes for body initalization
namespace WP_DefaultShapeSettings
{
	//inline static JPH::BoxShapeSettings			s_defaultCubeSetting{ JPH::Vec3Arg(0.5f,0.5f,0.5f) };	//Half width
	inline static JPH::SphereShapeSettings		s_defaultSphereSetting{ 0.5f };							//Radius
	inline static JPH::CapsuleShapeSettings		s_defaultCapsuleSetting{ 0.5f,0.5f };					//Half height of Capsule + radius
	inline static JPH::CylinderShapeSettings	s_defaultCylinderSetting{ 0.5f,0.5f };					//Half height of cylinder + radius, shape not recommended.
	inline static JPH::EmptyShapeSettings		s_defaultEmptySetting{};

	//inline static JPH::Plane					m_defaultPlaneSetting{ JPH::Vec3Arg(0.5f,0.5f,0.5f), 0.5f };	//????

	// WARNING: Pointer used for this functions
	JPH::ShapeSettings* GetDefaultShapeSetting(WP_PhysicsShape _shape);

	//for following functions, do not pass value to param _inConvexRadius unless you read the Jolt documentation on Bodies -> Shape -> ConvexRadius.
	//modifying this param will affect how collisions are resolved.
	JPH::BoxShapeSettings CreateBoxShapeSetting				(glm::vec3 = glm::vec3(0.5f,0.5f,0.5f), float _inConvexRadius = JPH::cDefaultConvexRadius);
	JPH::SphereShapeSettings CreateSphereShapeSetting		(float = 0.5f, float _inConvexRadius = JPH::cDefaultConvexRadius);
	JPH::CapsuleShapeSettings CreateCapsuleShapeSetting		(float = 0.5f,float = 0.5f, float _inConvexRadius = JPH::cDefaultConvexRadius);
	JPH::CylinderShapeSettings CreateCylinderShapeSetting	(float = 0.5f,float = 0.5f, float _inConvexRadius = JPH::cDefaultConvexRadius);
}

struct DLL_API WP_Physics3D : WP_Component
{
	//NOTE: all component types must be capable of default construction.
	static ComponentTypeBitSet GetBitSet();


	//RAII Physics bodies in construction & destruction
	WP_Physics3D();
	WP_Physics3D(WP_Physics3D&& _ref) noexcept;
	WP_Physics3D(WP_Physics3D const& _ref);
	~WP_Physics3D();

	WP_Physics3D& operator=(WP_Physics3D&& _ref) noexcept;
	WP_Physics3D& operator=(WP_Physics3D const& _ref);



	void Init() override;

	void OnEnabled() override;
	void OnDisabled() override;

	void AddBody();
	void SuspendBody();
	void RemoveBody();

	void AddCharacter(JPH::ShapeSettings::ShapeResult const& _shape);
	void RemoveCharacter();

	//Not for milestone 1
	void SetScale(float);					//deprecated
	void SetScaleCube(glm::vec3 const&);	//deprecated
	void SetScaleSphere(float);				//deprecated

	//is static, dynamic, kinematic
	JPH::EMotionType GetMotionType() const;
	void SetMotionType(JPH::EMotionType);

	JPH::ObjectLayer GetObjectLayer() const;
	void SetObjectLayer(JPH::ObjectLayer);

	bool GetIsActive() const;
	void SetIsActive(bool);

	void SetBodyActive(bool);	//implement game object function
	void SetBodyActive();		//implement game object function
	void SetBodyUnactive();		//implement game object function

	bool GetIsPureStatic() const;
	void SetIsPureStatic(bool);

	bool GetIsNPC() const;
	void SetIsNPC(bool);

	DECLARE_GETTER_SETTER(GetIsTrigger, SetIsTrigger, bool, m_isTrigger);

	bool GetIsValid() const;

	DECLARE_GETTER_SETTER(GetGravityScale, SetGravityScale,float, m_gravityScale);
	GETTER_SETTER_FLOAT_PERCENT(GetFriction, SetFriction, m_friction);
	GETTER_SETTER_FLOAT_PERCENT(GetCOR, SetCOR, m_restitution);

	DEFINE_GETTER_SETTER(GetMass, SetMass, float, m_mass);
	DECLARE_GETTER_SETTER(GetMaxSlope, SetMaxSlope, float, m_maxSlopeAngleRadians);
	DEFINE_GETTER_SETTER(GetMaxFloorSeperation, SetMaxFloorSeperation, float, m_maxSeperationDistance);

	float GetMaxSlopeInDegrees() const;
	void SetMaxSlopeInDegrees(float _angle);

	glm::vec3 GetPosOffset() const;
	void SetPosOffset(glm::vec3);

	glm::quat GetRotOffset() const;
	void SetRotOffset(glm::quat);

	DECLARE_GETTER_SETTER(GetRotOffsetAngles, SetRotOffsetAngles, glm::vec3, m_rotOffset);

	glm::mat4x4 GetOffsetTransformMat() const;	//try not to use, not effecient due to storage data

	char GetLockedAxis() const;
	void SetLockedAxis(char);

	//Book TODO: character specific velcoity stuff.
	void CharacterSetLinearVelocity(glm::vec3 const&);
	void CharacterAddVelocity(glm::vec3 const&);
	glm::vec3 CharacterGetLinearVelocity() const;

	void CharacterAddImpulse(glm::vec3 const&);

	void CharacterSetRotation(glm::vec3 const&);
	void CharacterRotate(glm::vec3 const&);
	void CharacterRotate(float);
	glm::vec3 CharacterGetRotattion() const;

	bool							m_isActive				{false};							//whether the body starts as active in the physics simulation, statics are always not active
	bool							m_isPureStatic			{false};							//if pure static, m_motionType is ignored. body will never move. ignored if body is npc.
	bool							m_isTrigger				{false};							//set the body to a trigger. dynamic is not allowed
	bool							m_useTransformScale		{true};								//if true, use the scale in transform3D to set shape size
	bool							m_isNPC					{false};							//If this component is on an npc
	bool							m_isInPhysicsSystem		{false};							//If this component has been added to physics system
	JPH::EMotionType				m_motionType			{JPH::EMotionType::Static};			//motion type of object
	WP_PhysicsShape					m_shapeType				{WP_PhysicsShape::CUBE};			//enum to represent shape
	JPH::EAllowedDOFs				m_lockedAxis			{JPH::EAllowedDOFs::All};			//store which axis of the body is locked. ignored if physics body is character

	JPH::ObjectLayer				m_objectLayer			{0};								//object layer of the body

	JPH::BodyID						m_bID					{JPH::BodyID::cInvalidBodyID};		//body id on jolt side

	//advance settings
	float							m_gravityScale			{ 1.0f };							//gravity affect on physics body
	float							m_friction				{ 0.2f };							//friction of physics body surface
	float							m_restitution			{ 0.0f };							//coefficient of restitution (aka bounciness). 1.0f for no energy lost on bounce.
	float							m_mass					{ 1.0f };							//affects collision resolution velocity and energy. if is character, set to 80.0f by default
	float							m_maxSlopeAngleRadians	{JPH::DegreesToRadians(50.0f)};		//maximum slope the body can climb, not used if phsyics body is not a character
	float							m_maxSeperationDistance	{1.0f};								//maximum distance the character can be floating off the slope.

	JPH::Vec3						m_posOffset				{JPH::Vec3::sZero()};				//stored offset for position
	JPH::Quat						m_rotOffset				{JPH::Quat::sIdentity()};			//stored offset for rotation
	//JPH::Mat44						m_offsetTransform		{JPH::Mat44::sIdentity()};		//store all transform offset as mat44

	std::unique_ptr<JPH::Character>	m_charPtr				{nullptr};							//store pointer to character.


	//store all shape settings for now (deprecated)
	//JPH::BoxShapeSettings			m_cubeSetting{ JPH::Vec3Arg(0.5f,0.5f,0.5f) };		//Half width
	//JPH::SphereShapeSettings		m_sphereSetting{ 0.5f };							//Radius
	//JPH::CapsuleShapeSettings		m_capsuleSetting{ 0.5f,0.5f };						//Half height of Capsule + radius
	//JPH::CylinderShapeSettings		m_cylinderSetting{ 0.5f,0.5f };						//Half height of cylinder + radius, shape not recommended.
	//JPH::EmptyShapeSettings			m_emptySetting{};

	//to remove??
	//JPH::BodyCreationSettings		m_bodySettings			{};									//Body creation settings used to store creation settings, unknown if required after creation.

	//?????????????????????????????
	//
	//	TODO: callback collision for each component
	//
	//??????????????????????????????

	//using ContactCallback = std::function<void(const JPH::Body&,const JPH::Body&,const JPH::ContactManifold&, JPH::ContactSettings&)>;
	//using BodyActivationCallback = std::function<void(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)>;

	//store callbacks for
	//static std::map<JPH::BodyID, ContactCallback> s_contactCallbackMap;
	//static std::map<JPH::BodyID, BodyActivationCallback> s_bodyActivationCallbackMap;

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
	//	- Position (always assume position to be centered on center of mass)
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

	//Add callback


	RTTR_ENABLE(WP_Component)
};

template<typename T>
T GetDegreesFromRadianVector(T _vec)
{
	_vec.x = JPH::RadiansToDegrees(_vec.x);
	_vec.y = JPH::RadiansToDegrees(_vec.y);
	_vec.z = JPH::RadiansToDegrees(_vec.z);
	return _vec;
}

template<typename T>
T GetRadianFromDegreesVector(T _vec)
{
	_vec.x = JPH::DegreesToRadians(_vec.x);
	_vec.y = JPH::DegreesToRadians(_vec.y);
	_vec.z = JPH::DegreesToRadians(_vec.z);
	return _vec;
}

//substitute functions, remove for final product
JPH::ObjectLayer GetObjectLayer(JPH::EMotionType _motionType);	//Get the right layer for a motion type
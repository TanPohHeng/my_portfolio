#pragma once
#include <WP_EngineSystem/WP_EngineSystem.h>
#include <WP_CoreComponents/WP_Physics.h>
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>				//multi-thread job system
#include <Jolt/Core/JobSystemSingleThreaded.h>			//single job system
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/StateRecorderImpl.h>
#include <map>

//would be ideal to move each class to different files
//DebugRenderer for debug integration??
#ifndef DEBUG_PHYS_CONTACT
//#define DEBUG_PHYS_CONTACT
#endif // !Debug_Physics_Contact

#ifndef JPH_MULTI_THREAD
#define JPH_MULTI_THREAD 1
#endif

#ifndef MAX_PHYSICS_UPDATES_PER_FRAME
#define MAX_PHYSICS_UPDATES_PER_FRAME 2
#endif


//class pre-declarations
class WP_PhysicsSystem;
class WP_ObjectLayerPairFilter;
class WP_BPLayerInterfaceImpl;
class WP_ObjectVsBroadPhaseLayerFilterImpl;
class WP_ContactListener;					//call contact object body id's callback function, call on contact
class WP_BodyActivationListener;			//call while collision active
class PhysicsSetFunction;


#ifndef TEMPLATE_PHYSICS_DELAY_CALLBACK
#define TEMPLATE_PHYSICS_DELAY_CALLBACK
// For INTERNAL Use only! do not attempt to use these functions.
// delayed callback shall be implmented through physics system only!!

//================================================================================
//					Quick Aliasing for potential Function types
//================================================================================
template <typename T>
using WP_PhysicsSystemFunction1 = void (WP_PhysicsSystem::*)(WP_GameObjectID, T);
template<typename T, typename U>
using WP_PhysicsSystemFunction2 = void (WP_PhysicsSystem::*)(WP_GameObjectID, T, U);
template <typename T, typename U, typename V>
using WP_PhysicsSystemFunction3 = void (WP_PhysicsSystem::*)(WP_GameObjectID, T, U, V);
template <typename T, typename U, typename V, typename W>
using WP_PhysicsSystemFunction4 = void (WP_PhysicsSystem::*)(WP_GameObjectID, T, U, V, W);
template <typename T, typename U, typename V, typename W, typename X>
using WP_PhysicsSystemFunction5 = void (WP_PhysicsSystem::*)(WP_GameObjectID, T, U, V, W, X);

//================================================================================
//		Template method pattern classes for each physics function types
//================================================================================
class PhysicsSetFunction
{
public:
	PhysicsSetFunction(WP_GameObjectID _id) :m_componentID{ _id } {/*Empty By Design*/ };
	virtual ~PhysicsSetFunction() {};
	virtual void operator()() = 0;
	WP_GameObjectID				m_componentID;
};

template	<typename T, WP_PhysicsSystemFunction1<T> _Func = nullptr>
class SetPhysicsDelayed1 final : public PhysicsSetFunction
{
	using Ttype = std::remove_reference_t<std::remove_const_t<T>>;
	Ttype m_param1;
public:
	template<typename T>
	SetPhysicsDelayed1(WP_GameObjectID _id, T _p1)
		: PhysicsSetFunction{ _id }, m_param1{ _p1 } {/*Empty By Design*/
	};


	void operator()() override
	{
		if constexpr (_Func != nullptr)
			(WP_PhysicsSystem::GetInstance()->*_Func)(PhysicsSetFunction::m_componentID, m_param1);
#ifdef DEBUG_PHYS_CONTACT
		WP_INFO("A physics delayed function of 1 param was called");
#endif
	}
};

template	<typename T, typename U, WP_PhysicsSystemFunction2<T, U> _Func = nullptr>
class SetPhysicsDelayed2 final : public PhysicsSetFunction
{
	using Ttype = std::remove_reference_t<std::remove_const_t<T>>;
	using Utype = std::remove_reference_t<std::remove_const_t<U>>;
	Ttype m_param1;
	Utype m_param2;
public:
	template<typename T, typename U>
	SetPhysicsDelayed2(WP_GameObjectID _id, T _p1, U _p2)
		: PhysicsSetFunction{ _id }, m_param1{ _p1 }, m_param2{ _p2 } {/*Empty By Design*/
	};

	void operator()() override {
		if constexpr (_Func != nullptr)
			(WP_PhysicsSystem::GetInstance()->*_Func)(PhysicsSetFunction::m_componentID, m_param1, m_param2);
	}
};

template	<typename T, typename U, typename V, WP_PhysicsSystemFunction3<T, U, V> _Func = nullptr>
class SetPhysicsDelayed3 final : public PhysicsSetFunction
{
	using Ttype = std::remove_reference_t<std::remove_const_t<T>>;
	using Utype = std::remove_reference_t<std::remove_const_t<U>>;
	using Vtype = std::remove_reference_t<std::remove_const_t<V>>;
	Ttype m_param1;
	Utype m_param2;
	Vtype m_param3;
public:
	template<typename T, typename U>
	SetPhysicsDelayed3(WP_GameObjectID _id, T _p1, U _p2, V _p3)
		: PhysicsSetFunction{ _id }, m_param1{ _p1 }, m_param2{ _p2 }, m_param3{ _p3 } {/*Empty By Design*/
	};

	void operator()() override {
		if constexpr (_Func != nullptr)
			(WP_PhysicsSystem::GetInstance()->*_Func)(PhysicsSetFunction::m_componentID, m_param1, m_param2, m_param3);
	}
};

template	<typename T, typename U, typename V, typename W, WP_PhysicsSystemFunction4<T, U, V, W> _Func = nullptr>
class SetPhysicsDelayed4 final : public PhysicsSetFunction
{
	using Ttype = std::remove_reference_t<std::remove_const_t<T>>;
	using Utype = std::remove_reference_t<std::remove_const_t<U>>;
	using Vtype = std::remove_reference_t<std::remove_const_t<V>>;
	using Wtype = std::remove_reference_t<std::remove_const_t<W>>;
	Ttype m_param1;
	Utype m_param2;
	Vtype m_param3;
	Wtype m_param4;
public:
	template<typename T, typename U>
	SetPhysicsDelayed4(WP_GameObjectID _id, T _p1, U _p2, V _p3, W _p4)
		: PhysicsSetFunction{ _id }, m_param1{ _p1 }, m_param2{ _p2 }, m_param3{ _p3 }, m_param4{ _p4 } {/*Empty By Design*/
	};

	void operator()() override {
		if constexpr (_Func != nullptr)
			(WP_PhysicsSystem::GetInstance()->*_Func)(PhysicsSetFunction::m_componentID, m_param1, m_param2, m_param3, m_param4);
	}
};

template	<typename T, typename U, typename V, typename W, typename X,
	WP_PhysicsSystemFunction5<T, U, V, W, X> _Func = nullptr>
class SetPhysicsDelayed5 final : public PhysicsSetFunction
{
	using Ttype = std::remove_reference_t<std::remove_const_t<T>>;
	using Utype = std::remove_reference_t<std::remove_const_t<U>>;
	using Vtype = std::remove_reference_t<std::remove_const_t<V>>;
	using Wtype = std::remove_reference_t<std::remove_const_t<W>>;
	using Xtype = std::remove_reference_t<std::remove_const_t<X>>;
	Ttype m_param1;
	Utype m_param2;
	Vtype m_param3;
	Wtype m_param4;
	Xtype m_param5;
public:
	template<typename T, typename U>
	SetPhysicsDelayed5(WP_GameObjectID _id, T _p1, U _p2, V _p3, W _p4, X _p5)
		: PhysicsSetFunction{ _id }, m_param1{ _p1 }, m_param2{ _p2 }, m_param3{ _p3 }, m_param4{ _p4 }, m_param5{ _p5 }
	{/*Empty By Design*/};

	void operator()() override {
		if constexpr (_Func != nullptr)
			(WP_PhysicsSystem::GetInstance()->*_Func)(PhysicsSetFunction::m_componentID, m_param1, m_param2, m_param3, m_param4, m_param5);
	}
};



// ===============================================================================
//						Compiler-able Example declaration
// ===============================================================================
//
//SetPhysicsDelayed1<glm::vec3 const&, &WP_PhysicsSystem::SetBodyPosition> tempData
//{ WP_INVALID_GAMEOBJECTID, glm::vec3(0, 0, 0) };



#ifndef DELAYED_PHYSICS_P1
#define DELAYED_PHYSICS_P1(_gID,_phyFunc,_Type, _var)															\
if(m_isPhysicsLocked)																							\
{m_DelayedPhysicsList.emplace_back(																				\
	std::make_unique<SetPhysicsDelayed1< _Type,&WP_PhysicsSystem:: _phyFunc>>( _gID , _var));					\
	return;																										\
}																												\

#endif

#ifndef DELAYED_PHYSICS_P2
#define DELAYED_PHYSICS_P2(_gID, _phyFunc, _Type1, _Type2, _param1, _param2)									\
if (m_isPhysicsLocked)																							\
{																												\
	m_DelayedPhysicsList.emplace_back(																			\
		std::make_unique<SetPhysicsDelayed2< _Type1, _Type2,&WP_PhysicsSystem:: _phyFunc>>						\
		( _gID , _param1, _param2)																				\
	);																											\
	return;																										\
}																												\

#endif

#ifndef DELAYED_PHYSICS_P3
#define DELAYED_PHYSICS_P3(_gID,_phyFunc, _Type1, _Type2, _Type3, _param1, _param2, _param3)					\
if (m_isPhysicsLocked)																							\
{m_DelayedPhysicsList.emplace_back(																				\
	std::make_unique<SetPhysicsDelayed3< _Type1, _Type2, _Type3 ,&WP_PhysicsSystem:: _phyFunc>>					\
	( _gID , _param1, _param2, _param3));																		\
	return;																										\
}																												\

#endif

#ifndef DELAYED_PHYSICS_P4
#define DELAYED_PHYSICS_P4(_gID,_phyFunc, _Type1, _Type2, _Type3, _Type4, _param1, _param2, _param3, _param4)	\
if (m_isPhysicsLocked)																							\
{m_DelayedPhysicsList.emplace_back(																				\
	std::make_unique<SetPhysicsDelayed4< _Type1, _Type2, _Type3, _Type4, &WP_PhysicsSystem:: _phyFunc>>			\
	( _gID , _param1, _param2, _param3, _param4));																\
	return;																										\
}																												\

#endif

#ifndef DELAYED_PHYSICS_P5
#define DELAYED_PHYSICS_P5(_gID,_phyFunc, _Type1, _Type2, _Type3, _Type4, _Type5, _param1, _param2, _param3, _param4, _param5)	\
if (m_isPhysicsLocked)																											\
{m_DelayedPhysicsList.emplace_back(																								\
	std::make_unique<SetPhysicsDelayed4< _Type1, _Type2, _Type3, _Type4, _Type5, &WP_PhysicsSystem:: _phyFunc>>					\
	( _gID , _param1, _param2, _param3, _param4, _parma5));																		\
	return;																														\
}																																\

#endif

#endif

//quick conversion operators for glm and jolt
namespace WP_Physics
{
	class WP_ContactManifold
	{
	public:
		WP_ContactManifold(JPH::ContactManifold const& _manifold);
		WP_ContactManifold& operator=(WP_ContactManifold const &) = delete;

		inline glm::vec3 GetWorldSpaceContactPointOn1(uint16_t _inIndex) const;
		inline glm::vec3 GetWorldSpaceContactPointOn2(uint16_t _inIndex) const;
		inline glm::vec3 GetRelativeContactPointOn1(uint16_t _inIndex) const;
		inline glm::vec3 GetRelativeContactPointOn2(uint16_t _inIndex) const;

		//It is not recommended to call this function. each call will create
		//a new contact manifold of 1kb size.
		// deprecated, requires memory allocation if JPH var is not exposed due to scoping.
		//WP_ContactManifold SwapShapes() const;

		//Penetration depth (move shape 2 by this distance to resolve the collision).
		// If this value is negative, this is a speculative contact point and may not
		// actually result in a velocity change as during solving the bodies may not actually collide.
		float const					m_penetrationDepth;
		JPH::ContactManifold const& m_manifold;
		glm::vec3 const				m_normal;
		glm::vec3 const				m_baseOffset;

	};
	class WP_ContactSettings
	{
	public:
		WP_ContactSettings(JPH::ContactSettings& _ref);
		~WP_ContactSettings();

		//If the contact should be treated as a sensor vs body contact (no collision response)
		bool	m_isSensor;
		float	m_combinedFriction;
		float	m_combinedRestitution;
		//Scale factor for the inverse mass of body 1 (0 = infinite mass, 1 = use original mass, 2 = body has half the mass).
		//For the same contact pair, you should strive to keep the value the same over time.
		float	m_inversedMassScale1;
		//Scale factor for the inverse inertia of body 1 (usually same as mInvMassScale1)
		float	m_inversedInertiaScale1;
		//Scale factor for the inverse mass of body 2 (0 = infinite mass, 1 = use original mass, 2 = body has half the mass).
		//For the same contact pair, you should strive to keep the value the same over time.
		float	m_inversedMassScale2;
		//Scale factor for the inverse inertia of body 2 (usually same as mInvMassScale2)
		float	m_inversedInertiaScale2;

		glm::vec3 m_relativeLinearSurfaceVelocity;
		glm::vec3 m_relativeAngularSurfaceVelocity;
	private:
		JPH::ContactSettings & m_contactSettings;
	};

	JPH::Vec4 operator+(JPH::Vec4 const&, glm::vec3 const&);	//manually add values
	JPH::Vec4 operator-(JPH::Vec4 const&, glm::vec3 const&);
	JPH::Vec4 operator+(JPH::Vec4 const&, glm::vec4 const&);	//hide direct casting, manually add values if alignment proves to be an issue
	JPH::Vec4 operator-(JPH::Vec4 const&, glm::vec4 const&);

	JPH::Vec4 ToJoltVec4(glm::vec3 const&);
	JPH::Vec4 ToJoltVec4(glm::vec4 const&);
	JPH::Vec3 ToJoltVec3(glm::vec3 const&);
	JPH::Vec3 ToJoltVec3(glm::vec4 const&);

	JPH::Quat ToJoltQuat(glm::quat const&);
	JPH::Quat ToJoltQuat(glm::vec4 const&);

	glm::vec3  ToGLMVec3(JPH::Vec4Arg&);
	glm::vec3  ToGLMVec3(JPH::Vec3Arg&);

	glm::vec4 ToGLMVec4(JPH::Vec4Arg&);
	glm::vec4 ToGLMVec4(JPH::QuatArg&);

	glm::quat ToGLMQuat(JPH::QuatArg&);
	glm::quat ToGLMQuat(JPH::Vec4Arg&);

	JPH::Mat44 ToJoltMat44(glm::mat4x4 const&);	//get row major jolt matrix
	glm::mat4x4 ToGLMMat4x4(JPH::Mat44 const&);		//get column major glm matrix

	glm::mat4x4 TransposeGLM4x4(glm::mat4x4 const&);

	JPH::Quat NormalizeJPHQuat(JPH::Quat const& _quat);	//prevent divide by zero error for all JPH quarternions when normalizing.
	JPH::Quat InverseJPHQuat(JPH::Quat const& _quat);	//prevent divide by zero error for all JPH quarternions when inversing.
}

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
// If functionallity to add object layers is wanted, please inform the guy who did physics :)
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;		//static objects
	static constexpr JPH::ObjectLayer MOVING = 1;			//non-static objects that have physics
	static constexpr JPH::ObjectLayer SENSOR = 2;			//static isTrigger objects
	static constexpr JPH::ObjectLayer DEBRIES = 3;			//non-static physics objects that dont collide with moving objects
	static constexpr JPH::ObjectLayer BULLET = 4;			//pure kinematics without collision.
	static constexpr JPH::ObjectLayer WEAPON = 5;			//?? Raycast layer?

	static constexpr JPH::ObjectLayer NUM_LAYERS = 6;

	static constexpr JPH::ObjectLayer s_nonMovingLayerMask = (1 << Layers::MOVING | 1 << Layers::DEBRIES | 1 << Layers::WEAPON);
	static constexpr JPH::ObjectLayer s_movingLayerMask = (1 << Layers::NON_MOVING | 1 << Layers::MOVING | 1 << Layers::SENSOR);
	static constexpr JPH::ObjectLayer s_sensorLayerMask = (1 << Layers::MOVING);
	static constexpr JPH::ObjectLayer s_debriesLayerMask = (1 << Layers::NON_MOVING);
	static constexpr JPH::ObjectLayer s_bulletLayerMask = (1 << Layers::WEAPON);
	static constexpr JPH::ObjectLayer s_weaponLayerMask = (1 << Layers::NON_MOVING | 1 << Layers::BULLET);
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);	//static
	static constexpr JPH::BroadPhaseLayer MOVING(1);		//dynamic
	static constexpr JPH::BroadPhaseLayer DEBRIES(2);		//dynamic collsion outside of moving objects

	static constexpr JPH::uint NUM_LAYERS(3);
};

//Which ObjectLayers should check collision with the other ObjectLayers.
class WP_ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		using namespace Layers;
		assert((inObject1 < NUM_LAYERS) && (inObject2 < NUM_LAYERS));	//both are valid layers

		switch (inObject1)
		{	//find if layers collide
		case NON_MOVING:
			return (1 << inObject2) & s_nonMovingLayerMask;
		case MOVING:
			return (1 << inObject2) & s_movingLayerMask;
		case DEBRIES:
			return (1 << inObject2) & s_debriesLayerMask;
		case SENSOR:
			return (1 << inObject2) & s_sensorLayerMask;
		case BULLET:
			return (1 << inObject2) & s_bulletLayerMask;
		case WEAPON:
			return (1 << inObject2) & s_weaponLayerMask;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

//quick return broad phase layers
class WP_BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
	WP_BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		m_ObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		m_ObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		m_ObjectToBroadPhase[Layers::DEBRIES] = BroadPhaseLayers::DEBRIES;
	}

	virtual JPH::uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer			GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return m_ObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::DEBRIES:		return "SENSOR";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer					m_ObjectToBroadPhase[Layers::NUM_LAYERS];
};

//which ObjectLayers should work on which BP_Layer?
class WP_ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		assert(inLayer1<Layers::NUM_LAYERS);
		assert(inLayer2.GetValue()<BroadPhaseLayers::NUM_LAYERS);
		using namespace Layers;
		switch (inLayer1)
		{
		case NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;	//switch with another selector for layer for more complicated layers
		case MOVING:
			return inLayer2 != BroadPhaseLayers::DEBRIES; // Moving collides with everything but raycast
		case DEBRIES:
			return inLayer2 != BroadPhaseLayers::MOVING; // raycasts collides with everything
		case SENSOR:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case BULLET:
			return false;
		case WEAPON:
			return inLayer2 != BroadPhaseLayers::DEBRIES;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

//use event system to broadcast
class WP_BodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
	}

	virtual void		OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
	}
private:
};			//call while collision active

//================================================================================
//							Main Physics System
//================================================================================
//store items required for physics system
class WP_PhysicsSystem : WP_EngineSystem
{
public:
	using physicsIdType = JPH::BodyID;
public:

	CREATE_ENGINE_INSTANCE_H(WP_PhysicsSystem);	//after this macro, items are public

	void OnUpdate() override;
	inline JPH::BodyInterface& GetBodyInterface() { return m_physics_system.GetBodyInterface(); }
	inline JPH::PhysicsSystem& GetPhysicsSystem() { return m_physics_system; }

	//================================================================================
	//							Contact Listener
	//================================================================================

	//on collision enter type callback
	class WP_ContactListener : public JPH::ContactListener
	{

	public:
		// See: ContactListener
		virtual JPH::ValidateResult	OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
			JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;

		virtual void			OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2,
			const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void			OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
			const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void			OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;


		//callback after all contact is identified this frame
		void					CallbackAllContacts();


		//clear all contacts before next physics frame.
		void					ClearContacts();

		class WP_ContactPayloadDelayed final : public EventPayload
		{
		public:
			WP_ContactPayloadDelayed(const WP_GameObjectID _gID1, const WP_GameObjectID _gID2)
				: m_gameObject1{ _gID1 }, m_gameObject2{ _gID2 }
			{/*Empty by Design*/}

			const WP_GameObjectID m_gameObject1;
			const WP_GameObjectID m_gameObject2;
		};

		class WP_ContactPayload final : public EventPayload
		{
		public:
			WP_ContactPayload(const WP_GameObjectID _gID1, const WP_GameObjectID _gID2,
				const JPH::ContactManifold& _cM, JPH::ContactSettings& _cS)
				: m_gameObject1{ _gID1 }, m_gameObject2{ _gID2 },
				m_contactManifold{ _cM }, m_contactSettings{ _cS }
			{/*Empty by Design*/}

			const WP_GameObjectID m_gameObject1;
			const WP_GameObjectID m_gameObject2;
			WP_Physics::WP_ContactManifold m_contactManifold;
			WP_Physics::WP_ContactSettings m_contactSettings;
		};

		class WP_ContactClearPayload final : public EventPayload
		{
		public:
			WP_ContactClearPayload(const WP_GameObjectID _gID1, const WP_GameObjectID _gID2)
				: m_gameObject1{ _gID1 }, m_gameObject2{ _gID2 }
			{/*Empty by Design*/
			}
			const WP_GameObjectID m_gameObject1;
			const WP_GameObjectID m_gameObject2;
		};

		std::vector<WP_ContactPayloadDelayed>							m_ContactAddedList;
		std::vector<WP_ContactPayloadDelayed>							m_ContactPersistList;
		std::vector<WP_ContactPayloadDelayed>							m_ContactRemovedList;
};


	//================================================================================
	//							End of Contact Listener
	//================================================================================

#if 0		//Who should haave access?
	physicsIdType AddBody(JPH::BodyCreationSettings);			//add body
	void SuspendBody(physicsIdType id);	//remove body
	void ResumeBody(physicsIdType id);	//re-add body
	void RemoveBody(physicsIdType id);	//suspend and remove.
	void UnloadAllBodies();				//remove all bodies
#endif
	void TestShapes();
	void RemoveTestShapes();

	inline JPH::BodyInterface& GetPhysicsBI() { return m_physics_system.GetBodyInterface(); }
	inline JPH::BodyInterface const& GetPhysicsBI() const { return m_physics_system.GetBodyInterface(); }
	inline JPH::NarrowPhaseQuery const& GetPhysicsNPQ() const { return m_physics_system.GetNarrowPhaseQuery(); }
private:
	WP_PhysicsSystem();
	~WP_PhysicsSystem();
	void OnApplicationEnd() override;
	void OnStartScene() override;
	void OnEndScene() override;

	//Hide all JPH stuff from users
	bool										m_isPhysicsLocked = false;
	bool										m_isPhysicsReloaded = false;
	int											m_maxPhysicsSteps;
	JPH::PhysicsSystem							m_physics_system;

	std::unique_ptr<JPH::TempAllocator>			temp_allocator = nullptr;
	std::unique_ptr<JPH::JobSystem>				job_system = nullptr;

	enum class WP_PHYSICS_SYSTEM_FUNCTIONS {};

	using WP_DelayedPhysicsList = std::vector<std::unique_ptr<PhysicsSetFunction>>;
	WP_DelayedPhysicsList						m_DelayedPhysicsList;

#if 1
	WP_BodyActivationListener					m_BodyActivationListener;						//call while collision active
	WP_ObjectLayerPairFilter					m_ObjectLayerPairFilter;
	WP_ObjectVsBroadPhaseLayerFilterImpl		m_ObjectVsBroadPhaseLayerFilterImpl;
	WP_BPLayerInterfaceImpl						m_BPLayerInterfaceImpl;
	WP_ContactListener							m_ContactListener;								//call contact object body id's callback function, call on contact
#else
	std::unique_ptr<WP_ContactListener>			m_ContactListener;								//call contact object body id's callback function, call on contact
	std::unique_ptr<WP_BodyActivationListener>	m_BodyActivationListener;						//call while collision active
#endif

	std::map<uint32_t, WP_GameObjectID>			m_bodyToID;
	//JPH::StateRecorderImpl						m_defaultState;

	using eventPair = std::pair<EventType, WP_EventCallback::idType>;
	//std::array<eventPair, 2>					m_eventSubscribers;

	//Test Vars
	JPH::BodyID sphere_id ;
	JPH::Body* floor = nullptr;

	//================================================================================
	//							Other System Functions
	//================================================================================
public:
#if 0	//Only if state saving is planned
	void SavePhysicsState();
	void LoadPhysicsState();
	void LoadPhysicsState(JPH::StateRecorder&);
#endif
	//ImGUI Inspector Properties render function

	void DisplayInspectorPropertyBody(JPH::BodyID _id, rttr::property& _property, rttr::instance& _instance);	//inspector body, WIP
	void OnEngineRun();
	void OnEngineStop();

	//event enabling functions
	//void OnEngineRun(EventPayload*const);
	//void OnEngineStop(EventPayload*const);

	WP_GameObjectID GetIDfromBodyID(uint32_t _bID);
	bool GetIsPhysicsLocked() const;

	//================================================================================
	//					JPH::Body Property retrieval functions
	//================================================================================
	WP_PhysicsShape		GetBodyPhysicsShape				(WP_GameObjectID _id) const;
	float				GetBodyFriction					(WP_GameObjectID _id) const;
	float				GetBodyRestitution				(WP_GameObjectID _id) const;
	float				GetBodyGravityFactor			(WP_GameObjectID _id) const;

	bool				GetBodyActive					(WP_GameObjectID _id) const;
	JPH::EMotionQuality GetBodyMotionQuality			(WP_GameObjectID _id) const;
	JPH::EMotionType	GetBodyMotionType				(WP_GameObjectID _id) const;
	JPH::ObjectLayer	GetBodyObjectLayer				(WP_GameObjectID _id) const;

	glm::vec3			GetBodyPosition					(WP_GameObjectID _id) const;
	glm::vec3			GetBodyCenterMass				(WP_GameObjectID _id) const;
	glm::vec4			GetBodyRotation					(WP_GameObjectID _id) const;
	void				GetBodyPositionAndRotation		(WP_GameObjectID _id, glm::vec3& _outPos, glm::vec4& _outRot) const;
	glm::mat4x4			GetBodyWorldTransform			(WP_GameObjectID _id) const;
	glm::mat4x4			GetBodyInverseInertia			(WP_GameObjectID _id) const;

	glm::vec3			GetBodyLinearVelocity			(WP_GameObjectID _id) const;
	glm::vec3			GetBodyPointVelocity			(WP_GameObjectID _id, glm::vec3 _point) const;
	glm::vec3			GetBodyAngularVelocity			(WP_GameObjectID _id) const;
	void				GetBodyLinearAndAngularVelocity	(WP_GameObjectID _id, glm::vec3& _outLinearVel, glm::vec3& _outAngularVel) const;


	//================================================================================
	//					JPH::Body Property mutator functions
	//================================================================================
private:
	void				SetBodyPhysicsShape				(WP_GameObjectID _id, WP_PhysicsShape _newShape, JPH::Shape& _newSettings , bool _updateMassProperties = true, bool _isActive = true);
	void				SetBodyPhysicsShape				(WP_GameObjectID _id, WP_PhysicsShape _newShape, bool _updateMassProperties = true, bool _isActive = true);

public:

	void				SetBodyFriction					(WP_GameObjectID _id, float _newFriction = 1.0f);
	void				SetBodyRestitution				(WP_GameObjectID _id, float _newCOR = 1.0f);
	void				SetBodyGravityFactor			(WP_GameObjectID _id, float _newGravityFactor = 1.0f);

	template <bool _isActive>
	void				SetBodyActive					(WP_GameObjectID _id);
	void				SetBodyMotionQuality			(WP_GameObjectID _id, JPH::EMotionQuality _newMotionQuality);
	void				SetBodyMotionType				(WP_GameObjectID _id, JPH::EMotionType _newMotionType);
	void				SetBodyObjectLayer				(WP_GameObjectID _id, JPH::ObjectLayer _newMotionLayer);

	void				SetBodyPosition					(WP_GameObjectID _id, glm::vec3 const& _newPos);
	void				SetBodyRotation					(WP_GameObjectID _id, glm::quat const& _newRot);
	void				SetBodyRotation					(WP_GameObjectID _id, glm::vec3 const& _newRot);

	void				SetBodyLinearVelocity			(WP_GameObjectID _id, glm::vec3 const& _newLinearVelocity);
	void				SetBodyAngularVelocity			(WP_GameObjectID _id, glm::vec3 const& _newAngularVelocity);

	//================================================================================
	//					JPH::Body Property Add Force functions
	//================================================================================

	void				AddForceToBody					(WP_GameObjectID _id, glm::vec3 const& _force, bool _isActive = true);
	void				AddForceToPoint					(WP_GameObjectID _id, glm::vec3 const& _force, glm::vec3 const& _point, bool _isActive = true);
	void				AddTorqueToBody					(WP_GameObjectID _id, glm::vec3 const& _torque, bool _isActive = true);
	void				AddForceAndTorqueToBody			(WP_GameObjectID _id, glm::vec3 const& _force, glm::vec3 const& _torque, bool _isActive = true);

	void				AddImpulseToBody				(WP_GameObjectID _id, glm::vec3 const& _impulse);
	void				AddImpulseToPoint				(WP_GameObjectID _id, glm::vec3 const& _impulse, glm::vec3 const& _point);
	void				AddAngularImpulseToBody			(WP_GameObjectID _id, glm::vec3 const& _angularImpulse);

	// JPH has a function for buoyancy impulse, but wont be implemented unless needed.
	//bool 	ApplyBuoyancyImpulse (const BodyID &inBodyID, RVec3Arg inSurfacePosition, Vec3Arg inSurfaceNormal, float inBuoyancy, float inLinearDrag, float inAngularDrag, Vec3Arg inFluidVelocity, Vec3Arg inGravity, float inDeltaTime)
	//================================================================================
	//						Character settings for ECS Component
	//================================================================================
	void				CharacterSetLinearVelocity		(WP_GameObjectID _id, glm::vec3 const& _vel);
	void				CharacterAddVelocity			(WP_GameObjectID _id, glm::vec3 const& _addVel);
	glm::vec3			CharacterGetLinearVelocity		(WP_GameObjectID _id) const;

	void				CharacterAddImpulse				(WP_GameObjectID _id, glm::vec3 const& _addImp);

	void				CharacterSetRotation			(WP_GameObjectID _id, glm::vec3 const& _rotInDegrees);
	void				CharacterRotate					(WP_GameObjectID _id, glm::vec3 const& _addRotInDegrees);
	void				CharacterRotate					(WP_GameObjectID _id, float _addAngleYaxis);
	glm::vec3			CharacterGetRotattion			(WP_GameObjectID _id) const;

	bool				CharacterGetIsGrounded			(WP_GameObjectID _id) const;

	//================================================================================
	//						Raycast functions for ECS Component
	//================================================================================

	class			CastBPMask final : public JPH::BroadPhaseLayerFilter
	{
	public:
		enum CastBPLayer : JPH::BroadPhaseLayer::Type
		{
			ALL			= 0b0111,
			NONE		= 0b1000,
			NON_MOVING	= 0b0001,
			MOVING		= 0b0010,
			SENSOR		= 0b0100
		};

		CastBPMask() = default;
		CastBPMask(uint8_t _ref) :m_IgnoredIDsMask{(CastBPLayer)_ref} {/*Empty By Design*/}
		CastBPMask(CastBPLayer _ref) :m_IgnoredIDsMask{_ref} {/*Empty By Design*/}

		virtual bool					ShouldCollide(JPH::BroadPhaseLayer _inLayer) const
		{
			return true;
			//return !(static_cast<uint8_t>(_inLayer) & CastBPLayer::NONE) && static_cast<uint8_t>(_inLayer) & m_mask;	//not none and is in mask
		}
	private:
		CastBPLayer m_IgnoredIDsMask = CastBPLayer::ALL;
	};
	using			CastBPLayer = CastBPMask::CastBPLayer;

	class			CastLayerMask final : public JPH::ObjectLayerFilter
	{
	public:
		enum CastLayer : JPH::ObjectLayer
		{
			ALL			= 0b0011'1111,
			NONE		= 0b0000'0000,
			NON_MOVING	= 0b0000'0001,
			MOVING		= 0b0000'0010,
			SENSOR		= 0b0000'0100,
			DEBRIES		= 0b0000'1000,
			BULLET		= 0b0001'0000,
			WEAPON		= 0b0010'0000
		};
		static constexpr CastLayer c_InvalidLayerMask = (CastLayer)(0b1100'0000);

		CastLayerMask() = default;
		CastLayerMask(uint16_t _mask) : m_IgnoredIDsMask{ (CastLayer)(_mask & ~c_InvalidLayerMask) } {/*Empty by Design*/}
		CastLayerMask(CastLayer _mask) : m_IgnoredIDsMask{ (CastLayer)(_mask & ~c_InvalidLayerMask) } {/*Empty by Design*/}

		virtual bool					ShouldCollide(JPH::ObjectLayer _inObject) const override
		{
			return true;
			using namespace Layers;
			//assert((_inObject < NUM_LAYERS));	//check valid layer
			//return !(_inObject & c_InvalidLayerMask) && m_mask & (1 << _inObject);	//not invalid and layer exist in mask.
		}

	private:
		CastLayer m_IgnoredIDsMask = CastLayer::ALL;
	};
	using			CastLayer = CastLayerMask::CastLayer;

	using			CastIDMask = std::set<WP_GameObjectID>;
	using			CastIDMasktRef = std::set<WP_GameObjectID> &;
	using			CastIDMaskConstRef = std::set<WP_GameObjectID> const&;
	class			CastIDFilter final : public JPH::BodyFilter
	{
	public:
		CastIDFilter() = default;
		CastIDFilter(CastIDMaskConstRef _ref) : m_IgnoredIDsMask{_ref} {/*Empty by Design*/ }

		virtual bool			ShouldCollide(const JPH::BodyID& _inBodyID) const
		{
#ifndef _DEBUG	//set.contains will fail build on release mode.
			return (m_IgnoredIDsMask.find(WP_PhysicsSystem::GetInstance()->m_bodyToID[_inBodyID.GetIndex()])
				== m_IgnoredIDsMask.end());
#else
			return !m_IgnoredIDsMask.contains(WP_PhysicsSystem::GetInstance()->m_bodyToID[_inBodyID.GetIndex()]);
#endif
		}
		virtual bool			ShouldCollideLocked(const JPH::Body& _inBody) const
		{
#ifndef _DEBUG	//set.contains will fail build on release mode.
			return (m_IgnoredIDsMask.find(WP_PhysicsSystem::GetInstance()->m_bodyToID[_inBody.GetID().GetIndex()])
				== m_IgnoredIDsMask.end());
#else
			return !m_IgnoredIDsMask.contains(WP_PhysicsSystem::GetInstance()->m_bodyToID[_inBody.GetID().GetIndex()]);
#endif
		}

	private:
		CastIDMask m_IgnoredIDsMask;	//store ids that collisions will ignore
	};

	using			WP_HitFraction = float;
	using			WP_RayResult = std::pair<WP_GameObjectID, WP_HitFraction>;
	using			WP_RayResultCollector = std::vector<WP_RayResult>;

	bool				CastRay							(glm::vec3 const& _origin,
														glm::vec3 const& _dirVector,
														WP_RayResult& _hit,					//out variable
														CastBPLayer _BroadPhaseLayerMask = CastBPLayer::ALL,
														CastLayer _ObjectLayerMask = CastLayer::ALL,
														CastIDMask const& _GameObjectIDMask = CastIDMask()) const;

	bool				CastRay							(glm::vec3 const& _origin,
														glm::vec3 const& _dirVector,
														WP_RayResultCollector& _hits,		//out all hits variable
														CastBPLayer _BroadPhaseLayerMask = CastBPLayer::ALL,
														CastLayer _ObjectLayerMask = CastLayer::ALL,
														CastIDMask const& _GameObjectIDMask = CastIDMask()) const;

	bool				CastRayAgainstObject			(WP_GameObjectID _objectToCheck,
														glm::vec3 const& _origin,
														glm::vec3 const& _dirVector) const;



	//================================================================================
	//						Create Body functions for ECS Component
	//================================================================================
	// create a shape setting for
#ifdef ILL_DO_THIS_PROPERLY_LATER_PLS	//deprecated
	physicsIdType AddBody(const JPH::ShapeSettings* inShape, glm::vec3 inPosition, glm::vec4 inRotation,
						  JPH::EMotionType inMotionType, JPH::ObjectLayer inObjectLayer);			//add body

	physicsIdType AddBody(WP_Physics3D& _comp);			//add body
#endif
	physicsIdType		AddBody							(WP_GameObjectID _id);	//add body
	void				SuspendBody						(WP_GameObjectID _id);	//suspend body
	void				ResumeBody						(WP_GameObjectID _id);	//re-add body
	void				RemoveBody						(WP_GameObjectID _id);	//suspend and remove.

	void AddPhysicsDelayedFunc()
	{
		//DEBUG CODE!!!
		JPH::ObjectLayer x = 1;
		WP_GameObjectID y = 2;
		//DELAYED_PHYSICS_P1(y,SetBodyObjectLayer, JPH::ObjectLayer,  x);
		//glm::vec3 up{0,9999999,0};
		//DELAYED_PHYSICS_P3(y, AddForceToPoint, glm::vec3 const&, glm::vec3 const&, bool, up, up, true);

		//m_DelayedPhysicsList.emplace_back(std::make_unique<SetPhysicsDelayed1< JPH::ObjectLayer, &WP_PhysicsSystem::SetBodyObjectLayer>>(y, x));
	}

};

//Raycast enum mask operator
using CastLayerEnum = WP_PhysicsSystem::CastLayerMask::CastLayer;
constexpr CastLayerEnum		operator &					(CastLayerEnum, CastLayerEnum);
constexpr CastLayerEnum		operator|					(CastLayerEnum, CastLayerEnum);
constexpr CastLayerEnum		operator^					(CastLayerEnum, CastLayerEnum);
constexpr CastLayerEnum		operator~					(CastLayerEnum);
constexpr CastLayerEnum&	operator&=					(CastLayerEnum&, CastLayerEnum);
constexpr CastLayerEnum&	operator|=					(CastLayerEnum&, CastLayerEnum);
constexpr CastLayerEnum&	operator^=					(CastLayerEnum&, CastLayerEnum);

using CastBPEnum = WP_PhysicsSystem::CastBPMask::CastBPLayer;

constexpr CastBPEnum		operator &					(CastBPEnum, CastBPEnum);
constexpr CastBPEnum		operator|					(CastBPEnum, CastBPEnum);
constexpr CastBPEnum		operator^					(CastBPEnum, CastBPEnum);
constexpr CastBPEnum		operator~					(CastBPEnum);
constexpr CastBPEnum&		operator&=					(CastBPEnum&, CastBPEnum);
constexpr CastBPEnum&		operator|=					(CastBPEnum&, CastBPEnum);
constexpr CastBPEnum&		operator^=					(CastBPEnum&, CastBPEnum);


template <bool _isActive>
void				WP_PhysicsSystem::SetBodyActive(WP_GameObjectID _id)
{
	WP_Physics3D* pComp = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_id);
	assert(pComp);
	if constexpr (_isActive)
	{GetPhysicsBI().ActivateBody(pComp->m_bID);}
	else
	{GetPhysicsBI().DeactivateBody(pComp->m_bID);}
}

template <typename T>
void				DelayPhysicsCall(WP_GameObjectID _id)
{
	WP_Physics3D* pComp = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_id);
	assert(pComp);
}

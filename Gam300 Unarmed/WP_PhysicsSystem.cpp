
#include <WP_EngineSystem/WP_PhysicsSystem.h>
#include <WP_CoreComponents/WP_Transform3D.h>
#include <WP_EngineSystem/WP_TimerSystem.h>
#include <WP_ECS/WP_ComponentSystem.h>

//#include <HelloWorldJolt.h>

CREATE_ENGINE_INSTANCE_CPP(WP_PhysicsSystem);	//after this macro, items are public

namespace {
	//macros for obtaining physics components, hide within file scope
#define OBTAIN_CONST_PHYSIC_COMPONENT(id)															\
const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(id));	\
assert(pComp);																						\

#define OBTAIN_PHYSIC_COMPONENT(id)																	\
WP_Physics3D* pComp = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(id);			\
assert(pComp);																						\

//macro for quick boolean to EActivation
#define WP_ACTIVATION_IS_ACTIVE(x)	(x)?JPH::EActivation::Activate:JPH::EActivation::DontActivate

//if all lovely JPH_ASSERTS should breakpoint the engine :')
#ifndef WP_PHYSICS_DEBUG_IGNORE_JPH_ASSERT
#define WP_PHYSICS_DEBUG_IGNORE_JPH_ASSERT	1
#endif

#ifndef USE_TEST_SHAPES
#define USE_TEST_SHAPES 1
#endif

#ifndef JPH_MULTI_THREAD
#define JPH_MULTI_THREAD 0
#endif

#ifndef MAX_PHYSICS_STEPS	//use this for modulo operations.
#define MAX_PHYSICS_STEPS (MAX_PHYSICS_UPDATES_PER_FRAME * m_maxPhysicsSteps + 1)
#endif
}

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
{m_DelayedPhysicsList.emplace_back(																				\
	std::make_unique<SetPhysicsDelayed2< _Type1, _Type2,&WP_PhysicsSystem:: _phyFunc>>							\
	( _gID , _param1, _param2));																				\
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
	( _gID , _param1, _param2, _param3, _parma4));																\
	return;																										\
}																												\

#endif

#ifndef DELAYED_PHYSICS_P5
#define DELAYED_PHYSICS_P5(_gID,_phyFunc, _Type1, _Type2, _Type3, _Type4, _Type5, _param1, _param2, _param3, _param4, _param5)	\
if (m_isPhysicsLocked)																											\
{m_DelayedPhysicsList.emplace_back(																								\
	std::make_unique<SetPhysicsDelayed4< _Type1, _Type2, _Type3, _Type4, _Type5, &WP_PhysicsSystem:: _phyFunc>>					\
	( _gID , _param1, _param2, _param3, _parma4, _parma5));																		\
	return;																														\
}																																\

#endif

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

//// All Jolt symbols are in the JPH namespace
//using namespace JPH;
//
//// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
//using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

//To be moved
const JPH::uint cMaxBodies = 1024;
const JPH::uint cNumBodyMutexes = 0;
const JPH::uint cMaxBodyPairs = 1024;
const JPH::uint cMaxContactConstraints = 1024;

// Callback for traces, connect this to your own trace function if you have one
void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;
#if !WP_PHYSICS_DEBUG_IGNORE_JPH_ASSERT
	// Breakpoint
	return true;
#else
	return false;
#endif
};

#endif // JPH_ENABLE_ASSERTS
#if 0
/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// An example contact listener
class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
	{
		cout << "Contact validate callback" << endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void			OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		cout << "A contact was added" << endl;
	}

	virtual void			OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
	{
		cout << "A contact was persisted" << endl;
	}

	virtual void			OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
	{
		cout << "A contact was removed" << endl;
	}
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body got activated" << endl;
	}

	virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
	{
		cout << "A body went to sleep" << endl;
	}
};
#endif

//quick conversion between jolt and glm
namespace WP_Physics
{
	JPH::Vec4 operator+(JPH::Vec4 const& _jphVec, glm::vec3 const& _glmVec)
	{
		return _jphVec + ToJoltVec4(_glmVec);
	}

	JPH::Vec4 operator-(JPH::Vec4 const& _jphVec, glm::vec3 const& _glmVec)
	{
		return _jphVec - ToJoltVec4(_glmVec);
	}

	JPH::Vec4 operator+(JPH::Vec4 const& _jphVec, glm::vec4 const& _glmVec)
	{
		return _jphVec + ToJoltVec4(_glmVec);
	}	//hide direct casting, manually add values if alignment proves to be an issue

	JPH::Vec4 operator-(JPH::Vec4 const& _jphVec, glm::vec4 const& _glmVec)
	{
		return _jphVec - ToJoltVec4(_glmVec);
	}

	JPH::Vec4 ToJoltVec4(glm::vec3 const& _glmVec)
	{
		return JPH::Vec4(_glmVec.x, _glmVec.y, _glmVec.z, 0);
	}

	JPH::Vec4 ToJoltVec4(glm::vec4 const& _glmVec)
	{
		return JPH::Vec4(_glmVec.x, _glmVec.y, _glmVec.z, _glmVec.w);
	}

	JPH::Vec3 ToJoltVec3(glm::vec3 const& _glmVec)
	{
		return JPH::Vec3(_glmVec.x, _glmVec.y, _glmVec.z);
	}

	JPH::Vec3 ToJoltVec3(glm::vec4 const& _glmVec)
	{
		return JPH::Vec3(_glmVec.x, _glmVec.y, _glmVec.z);
	}

	JPH::Quat ToJoltQuat(glm::quat const& _glmQuat)	//template in the future?
	{
		return JPH::Quat(_glmQuat.x, _glmQuat.y, _glmQuat.z, _glmQuat.w);
	}

	JPH::Quat ToJoltQuat(glm::vec4 const& _glmVec)
	{
		return JPH::Quat(_glmVec.x, _glmVec.y, _glmVec.z, _glmVec.w);
	}

	glm::vec3  ToGLMVec3(JPH::Vec4Arg& _joltVec)
	{
		return glm::vec3(_joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ());
	}

	glm::vec3  ToGLMVec3(JPH::Vec3Arg& _joltVec)
	{
		return glm::vec3(_joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ());
	}

	glm::vec4 ToGLMVec4(JPH::Vec4Arg& _joltVec)
	{
		return glm::vec4(_joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ(), _joltVec.GetW());
	}

	glm::vec4 ToGLMVec4(JPH::QuatArg& _joltVec)
	{
		return glm::vec4(_joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ(), _joltVec.GetW());
	}

	glm::quat ToGLMQuat(JPH::QuatArg& _joltVec)
	{
		return glm::quat(_joltVec.GetW(), _joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ());
	}

	glm::quat ToGLMQuat(JPH::Vec4Arg& _joltVec)
	{
		return glm::quat(_joltVec.GetW(), _joltVec.GetX(), _joltVec.GetY(), _joltVec.GetZ());
	}

	JPH::Mat44 ToJoltMat44(glm::mat4x4 const& _glmMat)
	{
		auto& tGlmMat = _glmMat;
		return { ToJoltVec4(tGlmMat[0]),
						   ToJoltVec4(tGlmMat[1]) ,
						   ToJoltVec4(tGlmMat[2]) ,
						   ToJoltVec4(tGlmMat[3]) };
	}

	glm::mat4x4 ToGLMMat4x4(JPH::Mat44 const& _jphMat)
	{
		glm::mat4x4 retVal;
#ifdef JUST_DIAGONALS
		auto vecMainDiag = _jphMat.GetDiagonal4();
		retVal[0][0] = vecMainDiag.GetX();
		retVal[1][1] = vecMainDiag.GetY();
		retVal[2][2] = vecMainDiag.GetZ();
		retVal[3][3] = vecMainDiag.GetW();
#else
		auto& t = _jphMat;
		for (JPH::uint i{}; i < 4; ++i)
		{
			auto col = t.GetColumn4(i);
			retVal[i][0] = col.GetX();
			retVal[i][1] = col.GetY();
			retVal[i][2] = col.GetZ();
			retVal[i][3] = col.GetW();
		}
#endif
		return retVal;
	}

	glm::mat4x4 TransposeGLM4x4(glm::mat4x4 const& _glmMat)
	{
		glm::mat4x4 retVal;
		for (glm::mat4x4::length_type i{}; i < 4; ++i)
		{
			for (glm::mat4x4::length_type j{}; i < 4; ++i)
			{
				retVal[i][j] = _glmMat[j][i];
			}
		}
		return retVal;
	}

	JPH::Quat NormalizeJPHQuat(JPH::Quat const& _quat)
	{
		if (_quat == JPH::Quat::sIdentity()) { return _quat; }
		if (_quat == JPH::Quat::sZero()) { return _quat; }
		return _quat.Normalized();
	}

	JPH::Quat InverseJPHQuat(JPH::Quat const& _quat)
	{
		if (_quat == JPH::Quat::sIdentity()) { return _quat; }
		if (_quat == JPH::Quat::sZero()) { return _quat; }
		return ToJoltQuat(glm::inverse(ToGLMQuat(_quat)));	//temporaru, until jph inverse function issue is identified. TODO
		//return _quat.Inversed();	//temporaru, until jph inverse function issue is identified. TODO
	}

	WP_ContactManifold::WP_ContactManifold(JPH::ContactManifold const& _manifold)
		: m_penetrationDepth{ _manifold.mPenetrationDepth },
		m_manifold{ _manifold },
		m_normal{ToGLMVec3(_manifold.mWorldSpaceNormal)},
		m_baseOffset { ToGLMVec3(_manifold.mBaseOffset)}
	{/*Empty by Design*/ }

	inline glm::vec3 WP_ContactManifold::GetWorldSpaceContactPointOn1(uint16_t _inIndex) const
	{
		return ToGLMVec3(m_manifold.GetWorldSpaceContactPointOn1(_inIndex));
	}

	inline glm::vec3 WP_ContactManifold::GetWorldSpaceContactPointOn2(uint16_t _inIndex) const
	{
		return ToGLMVec3(m_manifold.GetWorldSpaceContactPointOn2(_inIndex));
	}

	inline glm::vec3 WP_ContactManifold::GetRelativeContactPointOn1(uint16_t _inIndex) const
	{
		return ToGLMVec3(m_manifold.mRelativeContactPointsOn1[_inIndex]);
	}

	inline glm::vec3 WP_ContactManifold::GetRelativeContactPointOn2(uint16_t _inIndex) const
	{
		return ToGLMVec3(m_manifold.mRelativeContactPointsOn2[_inIndex]);
	}

	WP_ContactSettings::WP_ContactSettings(JPH::ContactSettings& _ref)
		: m_isSensor						{ _ref .mIsSensor },
		m_combinedFriction					{ _ref.mCombinedFriction },
		m_combinedRestitution				{ _ref.mCombinedRestitution },
		m_inversedMassScale1				{ _ref.mInvMassScale1 },
		m_inversedInertiaScale1				{ _ref.mInvInertiaScale1 },
		m_inversedMassScale2				{ _ref.mInvMassScale2 },
		m_inversedInertiaScale2				{ _ref.mInvInertiaScale2 },
		m_relativeLinearSurfaceVelocity		{ ToGLMVec3(_ref.mRelativeLinearSurfaceVelocity)},
		m_relativeAngularSurfaceVelocity	{ ToGLMVec3(_ref.mRelativeAngularSurfaceVelocity) },
		m_contactSettings					{ _ref }
	{/*Empty By Design*/}

	WP_ContactSettings::~WP_ContactSettings()
	{
		//update contact settings when this class goes out of scope/is destroyed
		m_contactSettings.mIsSensor = m_isSensor;
		m_contactSettings.mCombinedFriction = m_combinedFriction;
		m_contactSettings.mCombinedRestitution = m_combinedRestitution;
		m_contactSettings.mInvMassScale1 = m_inversedMassScale1;
		m_contactSettings.mInvInertiaScale1 = m_inversedInertiaScale1;
		m_contactSettings.mInvMassScale2 = m_inversedMassScale2;
		m_contactSettings.mInvInertiaScale2 = m_inversedInertiaScale2;
		m_contactSettings.mRelativeLinearSurfaceVelocity = ToJoltVec3(m_relativeLinearSurfaceVelocity);
		m_contactSettings.mRelativeAngularSurfaceVelocity = ToJoltVec3(m_relativeAngularSurfaceVelocity);
	}
}

namespace {
#ifdef _DEBUG
	namespace Debug
	{
		//void SubscribeContactAdd() { WP_EventSystem::GetInstance()->Subscribe
		//	<WP_PhysicsSystem::WP_ContactListener::WP_ContactPayload>
		//	(EventType::kPhysicsContactTrigger, DContAdd); }
		//void SubscribeContactPersist();
		//void SubscribeContactRemove();
		//void DContAdd(EventPayload* const);
		//void DContPersist();
		//void DContRemove();

	}
#endif
}

WP_PhysicsSystem::WP_PhysicsSystem()
	:WP_EngineSystem{ WP_EngineSystem::s_kSystemAllExceptOnPauseStillUpdate, "WP_PhysicsSystem" }
{
	m_ContactListener.m_ContactAddedList.reserve(1024);
	m_ContactListener.m_ContactPersistList.reserve(1024);
	m_ContactListener.m_ContactRemovedList.reserve(1024);


	// Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	JPH::RegisterDefaultAllocator();

	// Install trace and assert callbacks
	JPH::Trace = TraceImpl;
	using namespace JPH;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// It is not directly used in this example but still required.
		JPH::Factory::sInstance = new JPH::Factory();

	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	JPH::RegisterTypes();

	temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

#if JPH_MULTI_THREAD
	job_system = std::make_unique<JPH::JobSystemThreadPool>(
		JPH::cMaxPhysicsJobs, 
		JPH::cMaxPhysicsBarriers,
		(int)(thread::hardware_concurrency()) - 1);
#else
	job_system = std::make_unique<JPH::JobSystemSingleThreaded>(JPH::cMaxPhysicsJobs);
#endif
	//Absolutely the last thing to do before first update
	//m_physics_system.OptimizeBroadPhase();


	m_physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
		m_BPLayerInterfaceImpl, m_ObjectVsBroadPhaseLayerFilterImpl, m_ObjectLayerPairFilter);

	m_physics_system.SetBodyActivationListener(&m_BodyActivationListener);

	m_physics_system.SetContactListener(&m_ContactListener);

	m_physics_system.SetGravity(JPH::Vec3(0, -9.81f, 0));

	m_maxPhysicsSteps = (int)(thread::hardware_concurrency()) - 1;

	//m_physics_system.SaveState(m_defaultState);

	//m_eventSubscribers[0] = std::make_pair(EventType::kEditorPressPlay,
	//	WP_EventSystem::GetInstance()->Subscribe(
	//		EventType::kEditorPressPlay,
	//		&WP_PhysicsSystem::OnEngineRun,
	//		this));
	//
	//m_eventSubscribers[1] = std::make_pair(EventType::kEditorPressStop,
	//	WP_EventSystem::GetInstance()->Subscribe(
	//		EventType::kEditorPressStop,
	//		&WP_PhysicsSystem::OnEngineStop,
	//		this));
}

WP_PhysicsSystem::~WP_PhysicsSystem()
{
	//for (auto const& [ev_type, ev_id] : m_eventSubscribers)
	//{
	//	WP_EventSystem::GetInstance()->Unsubscribe(ev_type, ev_id);
	//}

	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void WP_PhysicsSystem::TestShapes()
{
	JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));
	floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

	// Create the shape
	JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

	using namespace JPH::literals;

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);

	// Create the actual rigid body
	floor = GetPhysicsBI().CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

	// Add it to the world
	GetPhysicsBI().AddBody(floor->GetID(), JPH::EActivation::DontActivate);
	m_bodyToID.emplace(floor->GetID().GetIndex(),WP_INVALID_GAMEOBJECTID);

	// Now create a dynamic body to bounce on the floor
	// Note that this uses the shorthand version of creating and adding a body to the world
#define HAVE_SPHERE		0
#ifndef HAVE_SPHERE  // (HAVE_SPHERE == 1)
	BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	sphere_id = GetPhysicsBI().CreateAndAddBody(sphere_settings, EActivation::Activate);

	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
	GetPhysicsBI().SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));
	GetPhysicsBI().SetRestitution(sphere_id, 1.0f);	//totally inelastic collision.
#endif
	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	//m_physics_system.OptimizeBroadPhase();
}

void WP_PhysicsSystem::RemoveTestShapes()
{
#ifndef HAVE_SPHERE  // (HAVE_SPHERE == 1)
	// Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
	GetPhysicsBI().RemoveBody(sphere_id);

	// Destroy the sphere. After this the sphere ID is no longer valid.
	GetPhysicsBI().DestroyBody(sphere_id);
#endif

	// Remove and destroy the floor
	if (!floor) { return; }
	GetPhysicsBI().RemoveBody(floor->GetID());
	GetPhysicsBI().DestroyBody(floor->GetID());
	floor = nullptr;
}

void WP_PhysicsSystem::OnEngineRun()
{
	//m_physics_system.RestoreState(m_defaultState);
#if USE_TEST_SHAPES
	TestShapes();
#endif
	//for ALL components regardless of usage state
	auto& phyCompVec = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponentVector();
	for (auto t : phyCompVec)
	{	//Trans -> Physics
		t->AddBody();
		m_bodyToID[t->m_bID.GetIndex()] = t->GetGameObjectID();
	}
	m_physics_system.OptimizeBroadPhase();
}

void WP_PhysicsSystem::OnEngineStop()
{
	//for ALL components regardless of usage state
	auto& phyCompVec = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponentVector();
	for (auto t : phyCompVec)
	{	//Trans -> Physics
		t->RemoveBody();
	}
#if USE_TEST_SHAPES
	RemoveTestShapes();
#endif
	//Clean Collision Cache after removing all physics bodies.
	m_isPhysicsReloaded = true;
	m_physics_system.Update(0.167f, 1, &*temp_allocator, &*job_system);
	m_isPhysicsReloaded = false;
	m_bodyToID.clear();
}

//void WP_PhysicsSystem::OnEngineRun([[maybe_unused]] EventPayload* const _payload)
//{
//	OnEngineRun();
//}
//
//void WP_PhysicsSystem::OnEngineStop([[maybe_unused]] EventPayload* const _payload)
//{
//	OnEngineStop();
//}

void WP_PhysicsSystem::OnApplicationEnd()
{
	OnEngineStop();
}

void WP_PhysicsSystem::OnStartScene()
{	//no insertion point in engine system!!! :(
	OnEngineRun();
}

void WP_PhysicsSystem::OnEndScene()
{	//no insertion point in engine system!!! :(
	OnEngineStop();
}

WP_GameObjectID WP_PhysicsSystem::GetIDfromBodyID(uint32_t _bID)
{
	if (m_bodyToID.contains(_bID))
	{
		return m_bodyToID[_bID];
	}
	if (m_isPhysicsReloaded) {
		return WP_INVALID_GAMEOBJECTID;
	}
	assert(0 && "Invalid Body Id check from PhysicsSystem::GetIDfromBodyID(uint32_t)");
	WP_WARN("Invalid Body Id check from PhysicsSystem::GetIDfromBodyID(uint32_t)");
	return WP_INVALID_GAMEOBJECTID;
}

bool WP_PhysicsSystem::GetIsPhysicsLocked() const { return m_isPhysicsLocked; }

void WP_PhysicsSystem::OnUpdate()
{
	static unsigned int s_rollbackFrames = 0;
	//================================================================
	//					Actual Physics Update
	//======================VVVVVVVVVVVVVVV===========================
	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	const float cDeltaTime = WP_TimerSystem::GetInstance()->GetDT();

	// Now we're ready to simulate the body, keep simulating until it goes to sleep
	//static JPH::uint step = 0;
	//if (GetPhysicsBI().IsActive(sphere_id))	//remove before Milestone 1! M1_TODO
	{
		// Next step
		//++step;

		// Output current position and velocity of the sphere
		//{
			/*RVec3 position = GetPhysicsBI().GetCenterOfMassPosition(sphere_id);
			Vec3 velocity = GetPhysicsBI().GetLinearVelocity(sphere_id);*/
			//cout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << endl;
		//}
		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = static_cast<int>(std::ceilf(cDeltaTime / (1.0f / 60.0f)));
		int collisionStepsThisUpdate{cCollisionSteps};

		//if collision steps exceed the max number of rollback frames, 
		if (cCollisionSteps > (MAX_PHYSICS_UPDATES_PER_FRAME * m_maxPhysicsSteps))
		{	//rollback all frames above max collision steps into next frame
			s_rollbackFrames += (cCollisionSteps - MAX_PHYSICS_UPDATES_PER_FRAME * m_maxPhysicsSteps);
			collisionStepsThisUpdate = MAX_PHYSICS_UPDATES_PER_FRAME * m_maxPhysicsSteps;
		}
		//if not max number of collision steps and there are some rollback frames, 
		if (s_rollbackFrames > 0 && cCollisionSteps < (MAX_PHYSICS_UPDATES_PER_FRAME * m_maxPhysicsSteps))
		{	//do some rollback frames 
			int oldRollbackFrames = s_rollbackFrames;	//save old number of rollback frames
			//make rollback frames the remainder after taking enough frames to make step the max amount of updates this frame
			s_rollbackFrames = (cCollisionSteps + s_rollbackFrames) % MAX_PHYSICS_STEPS;
			//make number of collision steps this frame to be the number of collision steps
			collisionStepsThisUpdate = cCollisionSteps + oldRollbackFrames - s_rollbackFrames;
		}

		//auto& phyCompVec = WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponentVector();
		auto&& phyCompVec = WP_ComponentSystem::WP_ComponentSystemIterator<WP_Physics3D>();
		for (WP_Physics3D& t : phyCompVec)
		{	//Trans -> Physics
			//TO_TEST: Point of failure, rotation and position offsets
			using namespace WP_Physics;
			auto transComp = WP_ComponentList<WP_Transform3D>::GetComponentList()->GetComponent(t.GetGameObjectID());

			//Update with Non direct access to transform member variables. ideally through a function.
			if (t.m_isNPC && t.m_charPtr)
			{
				t.m_charPtr->SetPositionAndRotation(
					ToJoltVec3(transComp->m_position + WP_Physics::ToGLMVec3(t.m_posOffset)),
					ToJoltQuat(glm::normalize(transComp->m_angle)) * t.m_rotOffset,
					WP_ACTIVATION_IS_ACTIVE(GetPhysicsBI().IsActive(t.m_bID)));
			}
			else
			{
				GetPhysicsBI().SetPositionAndRotationWhenChanged(
					t.m_bID ,
					ToJoltVec3(transComp->m_position + WP_Physics::ToGLMVec3(t.m_posOffset)),
					ToJoltQuat(glm::normalize(transComp->m_angle)) * t.m_rotOffset,
					WP_ACTIVATION_IS_ACTIVE(GetPhysicsBI().IsActive(t.m_bID)));
			}

			//GetPhysicsBI().GetShape(t.m_bID)->ScaleShape(WP_Physics::ToJoltVec3(transComp->m_scale));
#if 0
			switch (t.m_shapeType)
			{
			case WP_PhysicsShape::CUBE:
				//auto position = WP_Physics::ToJoltVec3(transComp->m_scale);
				//GetPhysicsBI().GetTransformedShape(t.m_bID).SetShapeScale(WP_Physics::ToJoltVec3(transComp->m_scale));

				auto position = GetPhysicsBI().GetTransformedShape(t.m_bID).GetShapeScale();
				auto scale = WP_Physics::ToJoltVec3(transComp->m_scale / t.m_LastScale);

				if (GetPhysicsBI().GetShape(t.m_bID)->IsValidScale(WP_Physics::ToJoltVec3(transComp->m_scale)))
				{
					std::cout << "Valid scale" << std::endl;
				}
				{
					GetPhysicsBI().GetShape(t.m_bID)->ScaleShape(WP_Physics::ToJoltVec3(transComp->m_scale));

					cout << "ID [" << (t.m_bID.GetIndex()) << "]Step " << step << ": Scale = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << ")" << endl;
					//std::cout.flush();
					//WP_Physics::ToJoltVec3(transComp->m_scale);
					auto a = WP_Physics::ToJoltVec3(transComp->m_scale) / 2;
					t.m_cubeSetting.mHalfExtent = a;
					//auto b = t.m_cubeSetting.Create();
					//GetPhysicsBI().SetShape(t.m_bID, t.m_cubeSetting.Create().Get(), true, WP_ACTIVATION_IS_ACTIVE(GetPhysicsBI().IsActive(t.m_bID)));
					//GetPhysicsBI().GetTransformedShape(t.m_bID).SetShapeScale(WP_Physics::ToJoltVec3(transComp->m_scale));


				}

				break;
				//Other cases TODO
			default:
				break;
			}
#endif
			//t.m_LastScale = transComp->m_scale;
			//auto a = GetPhysicsBI().GetShape(t.m_bID);
			//RVec3 position = GetPhysicsBI().GetPosition(t.m_bID);
			//Vec3 velocity = GetPhysicsBI().GetLinearVelocity(t.m_bID);
			//cout << "ID [" << (t.m_bID.GetIndex()) << "]Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << endl;
		}
		m_isPhysicsLocked = true;	//locked physics, all calls to setting functions are delayed
		
		while (cCollisionSteps && collisionStepsThisUpdate != 0)	//MAX_PHYSICS_UPDATES_PER_FRAME number of physics update loops
		{
			//find number of steps this update. must be less than hardware concurrency -1.
			int steps = (collisionStepsThisUpdate % (m_maxPhysicsSteps + 1));
			steps = (steps) ? steps: m_maxPhysicsSteps;
			//find DT fraction to complete these steps
			float thisUpdateDT = WP_TimerSystem::GetInstance()->GetFixedDT() * steps;
			//remove these steps from the number of steps we need this frame.
			collisionStepsThisUpdate -= steps;
		// Step the world
		if (steps)	//if no collisions this frame, skip update
			m_physics_system.Update(thisUpdateDT, steps, &*temp_allocator, &*job_system);
		}

		
		
		m_isPhysicsLocked = false;	//unlocked physics
		//run contact callback
		m_ContactListener.CallbackAllContacts();
		//remove contact event
		m_ContactListener.ClearContacts();

		for (auto& t : phyCompVec)
		{//Physics -> Trans
		// VVV this optimization is to be implemented TODO:
		 //if (GetPhysicsBI().GetMotionType(t.m_bID) == JPH::EMotionType::Static) { continue; }	//if static object, skip updating the transforms cus no movement
			//otherwise, update transform
			auto transComp = WP_ComponentList<WP_Transform3D>::GetComponentList()->GetComponent(t.GetGameObjectID());
//TO_TEST: Point of failure, rotation and position offsets
			using namespace WP_Physics;
			//Update with Non direct access to transform member variables. ideally through a function.
			transComp->m_position = ToGLMVec3(GetPhysicsBI().GetPosition(t.m_bID)) - ToGLMVec3((t.m_posOffset));	//to be confirmed.
			transComp->m_angle = ToGLMQuat(GetPhysicsBI().GetRotation(t.m_bID)) * inverse(ToGLMQuat(t.m_rotOffset));		//to be confirmed.

			if (t.m_isNPC && t.m_charPtr)
			{
				t.m_charPtr->PostSimulation(t.m_maxSeperationDistance);
			}
		}
	}
}


//================================================================================
//					JPH::Body Property retrieval functions
//================================================================================
//Shape and factors settings

WP_PhysicsShape		WP_PhysicsSystem::GetBodyPhysicsShape(WP_GameObjectID _id) const
{

	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		switch (GetPhysicsBI().GetShape(pComp->m_bID)->GetSubType()) {
		case JPH::EShapeSubType::Box:
			return WP_PhysicsShape::CUBE;
		case JPH::EShapeSubType::Sphere:
			return WP_PhysicsShape::SPHERE;
		case JPH::EShapeSubType::Capsule:
			return WP_PhysicsShape::CAPSULE;
		case JPH::EShapeSubType::Cylinder:
			return WP_PhysicsShape::CYLINDER;
		default:
			return WP_PhysicsShape::EMPTY;
		}
}

float				WP_PhysicsSystem::GetBodyFriction(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetFriction(pComp->m_bID);
}

float				WP_PhysicsSystem::GetBodyRestitution(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetRestitution(pComp->m_bID);
}

float				WP_PhysicsSystem::GetBodyGravityFactor(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetGravityFactor(pComp->m_bID);
}

bool				WP_PhysicsSystem::GetBodyActive(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().IsActive(pComp->m_bID);
}

//Other settings

JPH::EMotionQuality WP_PhysicsSystem::GetBodyMotionQuality(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetMotionQuality(pComp->m_bID);
}
JPH::EMotionType	WP_PhysicsSystem::GetBodyMotionType(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetMotionType(pComp->m_bID);
}
JPH::ObjectLayer	WP_PhysicsSystem::GetBodyObjectLayer(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return GetPhysicsBI().GetObjectLayer(pComp->m_bID);
}

//Transform settings

glm::vec3			WP_PhysicsSystem::GetBodyPosition(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		//return WP_Physics::ToGLMVec3(GetPhysicsBI().GetPosition(pComp->m_bID) + GetPhysicsBI().GetCenterOfMassPosition(pComp->m_bID));
		return WP_Physics::ToGLMVec3(GetPhysicsBI().GetPosition(pComp->m_bID));
}

glm::vec3			WP_PhysicsSystem::GetBodyCenterMass(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMVec3(GetPhysicsBI().GetCenterOfMassPosition(pComp->m_bID));
}

//TODO: QUAT?
glm::vec4			WP_PhysicsSystem::GetBodyRotation(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMVec4(GetPhysicsBI().GetRotation(pComp->m_bID));
}

void				WP_PhysicsSystem::GetBodyPositionAndRotation(WP_GameObjectID _id, glm::vec3& _outPos, glm::vec4& _outRot) const
{
	_outPos = GetBodyPosition(_id);
	_outRot = GetBodyRotation(_id);
}

glm::mat4x4			WP_PhysicsSystem::GetBodyWorldTransform(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMMat4x4(GetPhysicsBI().GetWorldTransform(pComp->m_bID));
}

glm::mat4x4			WP_PhysicsSystem::GetBodyInverseInertia(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMMat4x4(GetPhysicsBI().GetInverseInertia(pComp->m_bID));
}

//Velocity settings

glm::vec3			WP_PhysicsSystem::GetBodyLinearVelocity(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMVec3(GetPhysicsBI().GetLinearVelocity(pComp->m_bID));
}

glm::vec3			WP_PhysicsSystem::GetBodyPointVelocity(WP_GameObjectID _id, glm::vec3 _point) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMVec3(GetPhysicsBI().GetPointVelocity(pComp->m_bID, WP_Physics::ToJoltVec3(_point)));
}

glm::vec3			WP_PhysicsSystem::GetBodyAngularVelocity(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		return WP_Physics::ToGLMVec3(GetPhysicsBI().GetAngularVelocity(pComp->m_bID));
}

void				WP_PhysicsSystem::GetBodyLinearAndAngularVelocity(WP_GameObjectID _id, glm::vec3& _outLinearVel, glm::vec3& _outAngularVel) const
{
	_outLinearVel = GetBodyLinearVelocity(_id);
	_outAngularVel = GetBodyAngularVelocity(_id);
}


//================================================================================
//					JPH::Body Property mutator functions
//================================================================================

void				WP_PhysicsSystem::SetBodyPhysicsShape(WP_GameObjectID _id, WP_PhysicsShape _newShapeType,
	JPH::Shape& _newShape, bool _updateMassProperties, bool _isActive)
{
	if (m_isPhysicsLocked) { return; }
	OBTAIN_PHYSIC_COMPONENT(_id)
		pComp->m_shapeType = _newShapeType;
	GetPhysicsBI().SetShape(pComp->m_bID, &_newShape, _updateMassProperties, WP_ACTIVATION_IS_ACTIVE(_isActive));
}

void				WP_PhysicsSystem::SetBodyFriction(WP_GameObjectID _id, float _newFriction)
{
	DELAYED_PHYSICS_P1(_id, SetBodyFriction, float, _newFriction);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetFriction(pComp->m_bID, _newFriction);
}
void				WP_PhysicsSystem::SetBodyRestitution(WP_GameObjectID _id, float _newCOR)
{
	DELAYED_PHYSICS_P1(_id,SetBodyRestitution,float,_newCOR);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetRestitution(pComp->m_bID, _newCOR);
}
void				WP_PhysicsSystem::SetBodyGravityFactor(WP_GameObjectID _id, float _newGravityFactor)
{
	DELAYED_PHYSICS_P1(_id, SetBodyGravityFactor,float , _newGravityFactor);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetGravityFactor(pComp->m_bID, _newGravityFactor);
}

void				WP_PhysicsSystem::SetBodyMotionQuality(WP_GameObjectID _id, JPH::EMotionQuality _newMotionQuality)
{
	DELAYED_PHYSICS_P1(_id, SetBodyMotionQuality, JPH::EMotionQuality, _newMotionQuality);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetMotionQuality(pComp->m_bID, _newMotionQuality);
}
void				WP_PhysicsSystem::SetBodyMotionType(WP_GameObjectID _id, JPH::EMotionType _newMotionType)
{
	DELAYED_PHYSICS_P1(_id, SetBodyMotionType, JPH::EMotionType, _newMotionType);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetMotionType(pComp->m_bID, _newMotionType, WP_ACTIVATION_IS_ACTIVE(GetBodyActive(_id)));
}
void				WP_PhysicsSystem::SetBodyObjectLayer(WP_GameObjectID _id, JPH::ObjectLayer _newMotionLayer)
{
	DELAYED_PHYSICS_P1(_id, SetBodyObjectLayer, JPH::ObjectLayer, _newMotionLayer);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetObjectLayer(pComp->m_bID, _newMotionLayer);
}

void				WP_PhysicsSystem::SetBodyPosition(WP_GameObjectID _id, glm::vec3 const& _newPos)
{
	DELAYED_PHYSICS_P1(_id, SetBodyPosition, glm::vec3 const&, _newPos);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetPosition(pComp->m_bID, WP_Physics::ToJoltVec3(_newPos) - GetPhysicsBI().GetCenterOfMassPosition(pComp->m_bID)
			, WP_ACTIVATION_IS_ACTIVE(GetBodyActive(_id)));
}
void				WP_PhysicsSystem::SetBodyRotation(WP_GameObjectID _id, glm::quat const& _newRot)
{
	DELAYED_PHYSICS_P1(_id, SetBodyRotation, glm::quat const&, _newRot);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetRotation(pComp->m_bID, WP_Physics::ToJoltQuat(_newRot), WP_ACTIVATION_IS_ACTIVE(GetBodyActive(_id)));
}
void				WP_PhysicsSystem::SetBodyRotation(WP_GameObjectID _id, glm::vec3 const& _newRot)
{
	DELAYED_PHYSICS_P1(_id, SetBodyRotation, glm::vec3 const&, _newRot);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetRotation(pComp->m_bID,
			JPH::Quat::sEulerAngles(WP_Physics::ToJoltVec3(GetRadianFromDegreesVector(_newRot))),
			WP_ACTIVATION_IS_ACTIVE(GetBodyActive(_id)));
}

void				WP_PhysicsSystem::SetBodyLinearVelocity(WP_GameObjectID _id, glm::vec3 const& _newLinearVelocity)
{
	DELAYED_PHYSICS_P1(_id, SetBodyLinearVelocity, glm::vec3 const&, _newLinearVelocity);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetLinearVelocity(pComp->m_bID, WP_Physics::ToJoltVec3(_newLinearVelocity));
}
void				WP_PhysicsSystem::SetBodyAngularVelocity(WP_GameObjectID _id, glm::vec3 const& _newAngularVelocity)
{
	DELAYED_PHYSICS_P1(_id, SetBodyAngularVelocity, glm::vec3 const&, _newAngularVelocity);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().SetLinearVelocity(pComp->m_bID, WP_Physics::ToJoltVec3(_newAngularVelocity));
}

//================================================================================
//					JPH::Body Property Add Force functions
//================================================================================

void				WP_PhysicsSystem::AddForceToBody(WP_GameObjectID _id, glm::vec3 const& _force, bool _isActive)
{
	DELAYED_PHYSICS_P2(_id, AddForceToBody, glm::vec3 const&, bool, _force, _isActive);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddForce(pComp->m_bID, WP_Physics::ToJoltVec3(_force), WP_ACTIVATION_IS_ACTIVE(_isActive));
}
void				WP_PhysicsSystem::AddForceToPoint(WP_GameObjectID _id, glm::vec3 const& _force, glm::vec3 const& _point, bool _isActive)
{
	DELAYED_PHYSICS_P3(_id, AddForceToPoint, glm::vec3 const&, glm::vec3 const&, bool, _force, _point, _isActive);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddForce(pComp->m_bID, WP_Physics::ToJoltVec3(_force), WP_Physics::ToJoltVec3(_point), WP_ACTIVATION_IS_ACTIVE(_isActive));
}
void				WP_PhysicsSystem::AddTorqueToBody(WP_GameObjectID _id, glm::vec3 const& _torque, bool _isActive)
{
	DELAYED_PHYSICS_P2(_id, AddTorqueToBody, glm::vec3 const&, bool, _torque, _isActive);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddTorque(pComp->m_bID, WP_Physics::ToJoltVec3(_torque), WP_ACTIVATION_IS_ACTIVE(_isActive));
}
void				WP_PhysicsSystem::AddForceAndTorqueToBody(WP_GameObjectID _id, glm::vec3 const& _force, glm::vec3 const& _torque, bool _isActive)
{
	DELAYED_PHYSICS_P3(_id, AddForceAndTorqueToBody, glm::vec3 const&, glm::vec3 const&, bool, _force, _torque, _isActive);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddForceAndTorque(pComp->m_bID, WP_Physics::ToJoltVec3(_force), WP_Physics::ToJoltVec3(_torque), WP_ACTIVATION_IS_ACTIVE(_isActive));
}

void				WP_PhysicsSystem::AddImpulseToBody(WP_GameObjectID _id, glm::vec3 const& _impulse)
{
	DELAYED_PHYSICS_P1(_id, AddImpulseToBody, glm::vec3 const&, _impulse);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddImpulse(pComp->m_bID, WP_Physics::ToJoltVec3(_impulse));
}
void				WP_PhysicsSystem::AddImpulseToPoint(WP_GameObjectID _id, glm::vec3 const& _impulse, glm::vec3 const& _point)
{
	DELAYED_PHYSICS_P2(_id, AddImpulseToPoint, glm::vec3 const&, glm::vec3 const&, _impulse, _point);
	OBTAIN_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddImpulse(pComp->m_bID, WP_Physics::ToJoltVec3(_impulse), WP_Physics::ToJoltVec3(_point));
}
void				WP_PhysicsSystem::AddAngularImpulseToBody(WP_GameObjectID _id, glm::vec3 const& _angularImpulse)
{
	DELAYED_PHYSICS_P1(_id, AddAngularImpulseToBody, glm::vec3 const&, _angularImpulse);
	OBTAIN_CONST_PHYSIC_COMPONENT(_id)
		GetPhysicsBI().AddAngularImpulse(pComp->m_bID, WP_Physics::ToJoltVec3(_angularImpulse));
}

// JPH has a function for buoyancy impulse, but wont be implemented unless needed.
//bool 	ApplyBuoyancyImpulse (const BodyID &inBodyID, RVec3Arg inSurfacePosition, Vec3Arg inSurfaceNormal, float inBuoyancy, float inLinearDrag, float inAngularDrag, Vec3Arg inFluidVelocity, Vec3Arg inGravity, float inDeltaTime)

//================================================================================
//						Create Body functions for ECS Component
//================================================================================
// create a shape setting for
#ifdef ILL_DO_THIS_PROPERLY_LATER_PLS
physicsIdType WP_PhysicsSystem::AddBody(const JPH::ShapeSettings* inShape, glm::vec3 inPosition, glm::vec4 inRotation,
	JPH::EMotionType inMotionType, JPH::ObjectLayer inObjectLayer);			//add body

physicsIdType WP_PhysicsSystem::AddBody(WP_Physics3D& _comp);			//add body

#endif

WP_PhysicsSystem::physicsIdType WP_PhysicsSystem::AddBody(WP_GameObjectID _id)		//Depriecated?
{
	OBTAIN_PHYSIC_COMPONENT(_id)
		JPH::BodyCreationSettings temp{};
	return pComp->m_bID = WP_PhysicsSystem::GetInstance()->GetPhysicsBI().CreateBody(temp)->GetID();
}			//add body
void				WP_PhysicsSystem::SuspendBody(WP_GameObjectID _id)				//Depriecated?
{
	OBTAIN_PHYSIC_COMPONENT(_id)
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().RemoveBody(pComp->m_bID);
}	//suspend body
void				WP_PhysicsSystem::ResumeBody(WP_GameObjectID _id)				//Depriecated?
{
	if (m_isPhysicsLocked) { return; }
	OBTAIN_PHYSIC_COMPONENT(_id)
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().AddBody(pComp->m_bID, JPH::EActivation::Activate);
}	//re-add body
void				WP_PhysicsSystem::RemoveBody(WP_GameObjectID _id)				//Depriecated?
{
	if (m_isPhysicsLocked) { return; }
	OBTAIN_PHYSIC_COMPONENT(_id)
		WP_PhysicsSystem::GetInstance()->GetPhysicsBI().RemoveBody(pComp->m_bID);
	WP_PhysicsSystem::GetInstance()->GetPhysicsBI().DestroyBody(pComp->m_bID);
}	//suspend and remove.


void WP_PhysicsSystem::CharacterSetLinearVelocity(WP_GameObjectID _id, glm::vec3 const& _vel)
{
	DELAYED_PHYSICS_P1(_id, CharacterSetLinearVelocity, glm::vec3 const&, _vel);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	pComp->m_charPtr->SetLinearVelocity(WP_Physics::ToJoltVec3(_vel));

}
void WP_PhysicsSystem::CharacterAddVelocity(WP_GameObjectID _id, glm::vec3 const& _vel)
{
	DELAYED_PHYSICS_P1(_id, CharacterAddVelocity, glm::vec3 const&, _vel);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	pComp->m_charPtr->AddLinearVelocity(WP_Physics::ToJoltVec3(_vel));
}
glm::vec3 WP_PhysicsSystem::CharacterGetLinearVelocity(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return glm::vec3(0, 0, 0); }
	return WP_Physics::ToGLMVec3(pComp->m_charPtr->GetLinearVelocity());
}
void WP_PhysicsSystem::CharacterAddImpulse(WP_GameObjectID _id, glm::vec3 const& _imp)
{
	DELAYED_PHYSICS_P1(_id, CharacterAddImpulse, glm::vec3 const&, _imp);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	pComp->m_charPtr->AddImpulse(WP_Physics::ToJoltVec3(_imp));
}

//rotation in degrees for each axis for the 3D Gimbal
void WP_PhysicsSystem::CharacterSetRotation(WP_GameObjectID _id, glm::vec3 const& _rotInDegrees)
{
	DELAYED_PHYSICS_P1(_id, CharacterSetRotation, glm::vec3 const&, _rotInDegrees);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	pComp->m_charPtr->SetRotation(JPH::Quat::sEulerAngles(WP_Physics::ToJoltVec3(GetRadianFromDegreesVector(_rotInDegrees))));
}
void WP_PhysicsSystem::CharacterRotate(WP_GameObjectID _id, glm::vec3 const& _addRot)
{
	DELAYED_PHYSICS_P1(_id, CharacterRotate, glm::vec3 const&, _addRot);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	auto rot = pComp->m_charPtr->GetRotation().GetEulerAngles() + WP_Physics::ToJoltVec3(GetRadianFromDegreesVector(_addRot));
	pComp->m_charPtr->SetRotation(JPH::Quat::sEulerAngles(rot));
}
//rotation for y axis only
void WP_PhysicsSystem::CharacterRotate(WP_GameObjectID _id, float _angle)
{
	DELAYED_PHYSICS_P1(_id, CharacterRotate, float, _angle);
	OBTAIN_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_charPtr) { return; }
	auto rot = pComp->m_charPtr->GetRotation().GetEulerAngles() + WP_Physics::ToJoltVec3(glm::vec3(0, JPH::DegreesToRadians(_angle), 0));
	pComp->m_charPtr->SetRotation(JPH::Quat::sEulerAngles(rot));
}
glm::vec3 WP_PhysicsSystem::CharacterGetRotattion(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_isNPC|| !pComp->m_charPtr)
	{
		assert(0 && "no Physics body or is not a Physics Character! WP_Physics3D::CharacterGetRotattion()");
		return glm::vec3(0, 0, 0);
	}
	return GetDegreesFromRadianVector(WP_Physics::ToGLMVec3(pComp->m_charPtr->GetRotation().GetEulerAngles()));
}

bool WP_PhysicsSystem::CharacterGetIsGrounded(WP_GameObjectID _id) const
{
	OBTAIN_CONST_PHYSIC_COMPONENT(_id);
	if (pComp->m_bID.IsInvalid() || !pComp->m_isNPC || !pComp->m_charPtr)
	{
		assert(0 && "no Physics body or is not a Physics Character! WP_Physics3D::CharacterGetIsGrounded()");
		return false;
	}
	switch (pComp->m_charPtr->GetGroundState())
	{
	case JPH::Character::EGroundState::InAir :
	case JPH::Character::EGroundState::NotSupported :
		return false;
	case JPH::Character::EGroundState::OnGround :
	case JPH::Character::EGroundState::OnSteepGround :
		return true;
	default:
		assert(0 && "unknown ground state! WP_Physics3D::CharacterGetIsGrounded()");
		break;
	}
	return true;
}


//================================================================================
//						Ray Cast Functions for Physics
//================================================================================


//Use this function if trying to raycast and collect result for the closest object.
//Use CastRayAgainstObject() if attempting to raycast against specific objects.
bool WP_PhysicsSystem::CastRay(
	glm::vec3 const&							_origin,
	glm::vec3 const&							_dirVector,
	WP_PhysicsSystem::WP_RayResult&				_hit,					//out variable
	CastBPEnum									_BroadPhaseLayerMask,
	CastLayerEnum								_ObjectLayerMask,
	WP_PhysicsSystem::CastIDMask const&			_GameObjectIDMask ) const
{
	using namespace WP_Physics;

	JPH::RRayCast ray { ToJoltVec3(_origin),ToJoltVec3(_dirVector) };
	JPH::RayCastResult results;

	GetPhysicsNPQ().CastRay(
		JPH::RRayCast{ ToJoltVec3(_origin),ToJoltVec3(_dirVector) },
		results,
		WP_PhysicsSystem::CastBPMask(_BroadPhaseLayerMask),
		WP_PhysicsSystem::CastLayerMask(_ObjectLayerMask),
		WP_PhysicsSystem::CastIDFilter(_GameObjectIDMask));

	assert( m_bodyToID.contains(results.mBodyID.GetIndex()) );
	if (results.mBodyID.IsInvalid())
	{
		_hit.first = WP_INVALID_GAMEOBJECTID;
		return false;
	}
	_hit.first = m_bodyToID.find(results.mBodyID.GetIndex())->second;
	_hit.second = results.mFraction;

	return true;
}
/*
	WP_RayResult a;

	// CastRay( origin of ray(start),
	//			direction of ray(vector description of ray if ray starts at origin) ,
	//			result collector);

	if (CastRay(glm::vec3(0, 1, 0), glm::vec3(0, -2, 0), a))
	{
		std::cout << "hit id:[" << a.first << "] \n";
	}
	else
	{
		std::cout << "NO hit \n";
	}
*/

//default collector. when writing custom collector, shift to new function
//Use this function if trying to raycast and collect result for multiple objects.
//Use CastRayAgainstObject() if attempting to raycast against specific objects.
bool WP_PhysicsSystem::CastRay(
	glm::vec3 const&							_origin,
	glm::vec3 const&							_dirVector,
	WP_PhysicsSystem::WP_RayResultCollector&	_hits,		//out all hits variable
	CastBPEnum									_BroadPhaseLayerMask,
	CastLayerEnum								_ObjectLayerMask,
	WP_PhysicsSystem::CastIDMask const&			_GameObjectIDMask) const
{
	using namespace WP_Physics;

	_hits.clear();

	JPH::RRayCast ray{ ToJoltVec3(_origin),ToJoltVec3(_dirVector) };
	JPH::RayCastSettings raySettings{};

	JPH::AllHitCollisionCollector<JPH::CastRayCollector> results;

	GetPhysicsNPQ().CastRay(
		JPH::RRayCast{ ToJoltVec3(_origin),ToJoltVec3(_dirVector) },
		raySettings,
		results,
		WP_PhysicsSystem::CastBPMask(_BroadPhaseLayerMask),
		WP_PhysicsSystem::CastLayerMask(_ObjectLayerMask),
		WP_PhysicsSystem::CastIDFilter(_GameObjectIDMask));

	if (!results.mHits.size())	{	return false;	}

	for (auto& result : results.mHits) //move results to out variable
	{
		_hits.push_back(std::make_pair(m_bodyToID.find(result.mBodyID.GetIndex())->second,result.mFraction));

#if 0	//debug code
		//brute force
		for (auto& t : WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponentVector())
		{
			if (t->m_bID == result.mBodyID)
			{
				if (_hits.back().first != t->GetGameObjectID())
				{

					WP_INFO("OH WOE IS TO BE ME ");
					std::cout << "storedID [" << m_bodyToID.find(result.mBodyID.GetIndex())->second << "]";
					std::cout << "storedID [" << _hits.back().first << "]";
				}
				else
				{
					std::cout << "happy raycast ids stored internally are same [" << m_bodyToID.find(result.mBodyID.GetIndex())->second << "]";
				}

			}
		}
#endif
	}
	return true;
}
//some sample ray cast code for this function ^^
/*
	WP_RayResultCollector a;

	// CastRay( origin of ray(start),
	//			direction of ray(vector description of ray if ray starts at origin) ,
	//			results collector);

	if (CastRay(glm::vec3(0, 1, 0), glm::vec3(0, -2, 0), a))
	{
		std::cout << "hits collected:["<< a.size()<<"] \n";
	}
	else
	{
		std::cout << "NO hit \n";
	}
*/



//Use this function if trying to raycast against a specific gameobject.
//please refrain from using gameobjects ids that do not have corresponding physics component.
//Use CastRay if hit check against any object is required.
bool WP_PhysicsSystem::CastRayAgainstObject(
	WP_GameObjectID _objectToCheck,
	glm::vec3 const& _origin,
	glm::vec3 const& _dirVector) const
{
	using namespace WP_Physics;
	OBTAIN_CONST_PHYSIC_COMPONENT(_objectToCheck);
	if (!pComp) return false;

	JPH::RayCastResult results;
	return GetPhysicsBI().GetTransformedShape(pComp->m_bID).CastRay(
		JPH::RRayCast{ ToJoltVec3(_origin),ToJoltVec3(_dirVector) },
		results
	);
}
/*

			// CastRay( GameObjectID,
			//			origin of ray(start),
			//			direction/length of ray(vector description of ray if ray was moved to origin) );

			if (CastRayAgainstObject(2,glm::vec3(0, 1, 0), glm::vec3(0, -2, 0)))
			{
				std::cout << "hit id:[" << 2 << "] \n";
			}
			else
			{
				std::cout << "NO hit \n";
			}
*/


//Raycast enum mask operator Macros
#ifndef DEFINE_ENUM_LOGIC_OPERATOR
#define DEFINE_ENUM_LOGIC_OPERATOR(_Type,_op)																\
constexpr _Type		operator _op					(_Type _lhs , _Type _rhs)								\
{return (_Type)((std::underlying_type<_Type>::type)(_lhs) _op (std::underlying_type<_Type>::type)(_rhs));}	\

#endif

#ifndef DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR
#define DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(_Type,_op,_opNoEql)														\
constexpr _Type &	operator _op					(_Type & _lhs , _Type _rhs)											\
{return _lhs = (_Type)((std::underlying_type<_Type>::type)(_lhs) _opNoEql (std::underlying_type<_Type>::type)(_rhs));}	\

#endif

#ifndef DEFINE_ENUM_UNARY_LOGIC_OPERATOR
#define DEFINE_ENUM_UNARY_LOGIC_OPERATOR(_Type,_op)									\
constexpr _Type		operator _op					(_Type _ref)					\
{return (_Type)((_op(std::underlying_type<_Type>::type)(_ref)));}					\

#endif


DEFINE_ENUM_LOGIC_OPERATOR(CastLayerEnum, &);
DEFINE_ENUM_LOGIC_OPERATOR(CastLayerEnum, |);
DEFINE_ENUM_LOGIC_OPERATOR(CastLayerEnum, ^);
DEFINE_ENUM_UNARY_LOGIC_OPERATOR(CastLayerEnum, ~);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastLayerEnum, &=, &);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastLayerEnum, |=, |);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastLayerEnum, ^=, ^);

DEFINE_ENUM_LOGIC_OPERATOR(CastBPEnum, &);
DEFINE_ENUM_LOGIC_OPERATOR(CastBPEnum, |);
DEFINE_ENUM_LOGIC_OPERATOR(CastBPEnum, ^);
DEFINE_ENUM_UNARY_LOGIC_OPERATOR(CastBPEnum, ~);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastBPEnum, &=, &);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastBPEnum, |=, |);
DEFINE_ENUM_LOGIC_ASSIGNMENT_OPERATOR(CastBPEnum, ^=, ^);


//================================================================================
//						Delayed functioncall adder for Physics
//================================================================================

//================================================================================
//							Contact Listener functions
//================================================================================

#define PHYSICS_CONTACT_SCENE_RELOAD_GUARD								\
if (WP_PhysicsSystem::GetInstance()->m_isPhysicsReloaded) { return; }	\

using WP_CL = WP_PhysicsSystem::WP_ContactListener;

JPH::ValidateResult	WP_CL::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2,
	JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
{
	// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void			WP_CL::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2,
	const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	PHYSICS_CONTACT_SCENE_RELOAD_GUARD
	auto physics = WP_PhysicsSystem::GetInstance();
	m_ContactAddedList.emplace_back(
		physics->GetIDfromBodyID(inBody1.GetID().GetIndex()),
		physics->GetIDfromBodyID(inBody2.GetID().GetIndex()));

	//notify event
	WP_ContactPayload payload{
		physics->GetIDfromBodyID(inBody1.GetID().GetIndex()),
		physics->GetIDfromBodyID(inBody2.GetID().GetIndex()),
		inManifold, ioSettings };

	WP_EventSystem::GetInstance()->Notify(EventType::kPhysicsContactTrigger, &payload);
}

void			WP_CL::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
	const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	PHYSICS_CONTACT_SCENE_RELOAD_GUARD
	auto physics = WP_PhysicsSystem::GetInstance();
	m_ContactPersistList.emplace_back(
		physics->GetIDfromBodyID(inBody1.GetID().GetIndex()),
		physics->GetIDfromBodyID(inBody2.GetID().GetIndex()));

	//notify event
	WP_ContactPayload payload{
		physics->GetIDfromBodyID(inBody1.GetID().GetIndex()),
		physics->GetIDfromBodyID(inBody2.GetID().GetIndex()),
		inManifold, ioSettings };

	WP_EventSystem::GetInstance()->Notify(EventType::kPhysicsContactPersist, &payload);
}

void			WP_CL::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
	PHYSICS_CONTACT_SCENE_RELOAD_GUARD
	auto physics = WP_PhysicsSystem::GetInstance();
	m_ContactRemovedList.emplace_back(
		physics->GetIDfromBodyID(inSubShapePair.GetBody1ID().GetIndex()),
		physics->GetIDfromBodyID(inSubShapePair.GetBody2ID().GetIndex()));

	//notify event
	WP_ContactClearPayload payload{
		physics->GetIDfromBodyID(inSubShapePair.GetBody1ID().GetIndex()),
		physics->GetIDfromBodyID(inSubShapePair.GetBody2ID().GetIndex()) };

	WP_EventSystem::GetInstance()->Notify(EventType::kPhysicsContactExit, &payload);
}

void			WP_CL::CallbackAllContacts()
{//TODO: fix dangling reference
	PHYSICS_CONTACT_SCENE_RELOAD_GUARD
	EventType currentType = EventType::kPhysicsContactTriggerDelayed;
	auto notify = [&currentType](auto& _payload)
		{
			WP_EventSystem::GetInstance()->Notify(currentType, &_payload);
#ifdef DEBUG_PHYS_CONTACT
			switch (currentType)
			{
			case EventType::kTest:
				break;
			case EventType::kEditorPressPlay:
				break;
			case EventType::kEditorPressStop:
				break;
			case EventType::kAddComponent:
				break;
			case EventType::kDeleteComponent:
				break;
			case EventType::kReloadComponent:
				break;
			case EventType::kPhysicsBodyActivate:
				break;
			case EventType::kPhysicsBodyDeactivate:
				break;
			case EventType::kPhysicsContactTrigger:
				WP_INFO("ContactTrigger");
				break;
			case EventType::kPhysicsContactPersist:
				WP_INFO("ContactPersist");
				break;
			case EventType::kPhysicsContactExit:
				WP_INFO("ContactExit");
				break;
			case EventType::kPhysicsContactTriggerDelayed:
				WP_INFO("ContactTriggerDelayed");
				break;
			case EventType::kPhysicsContactPersistDelayed:
				WP_INFO("ContactPersistDelayed");
				break;
			case EventType::kPhysicsContactExitDelayed:
				WP_INFO("ContactExitDelayed");
				break;
			default:
				break;
			}
#endif // _DEBUG
		};	//end of lambda

	std::for_each(m_ContactAddedList.begin(), m_ContactAddedList.end(), notify);		//notify contact start
	currentType = EventType::kPhysicsContactPersistDelayed;
	std::for_each(m_ContactPersistList.begin(), m_ContactPersistList.end(), notify);	//notify contact persist
	currentType = EventType::kPhysicsContactExitDelayed;
	std::for_each(m_ContactRemovedList.begin(), m_ContactRemovedList.end(), notify);	//notify contact end

	//run all delayed calls to physics set functions
	for (auto& delayedPhysicsObject : WP_PhysicsSystem::GetInstance()->m_DelayedPhysicsList)
	{
		(*delayedPhysicsObject)();	//operator call for each delayed physics object
	}
}

void				WP_CL::ClearContacts()
{
	m_ContactAddedList.clear(); m_ContactPersistList.clear(); m_ContactRemovedList.clear();
	WP_PhysicsSystem::GetInstance()->m_DelayedPhysicsList.clear();	//remove all delayed calls to physics set functions
}

//================================================================================
//						End of Contact Listener functions
//================================================================================
/*!
* @file			WP_CS_Physic.hxx
* @author		Emma Natalie Soh, Tan Poh Heng
* @par			Emma: Email: e.soh\@digipen.edu , t.pohheng@digipen.edu
* @par			Course: CSD3401 Software Engineering Project 5
* @date			23 October 2024
*
* @brief		@todo
*
* Adapted from: https://kahwei.dev/2022/09/11/c-scripting-engine-part-3-hosting-net/
*
* All Content copyrighted 2024 DigiPen Institute of Technology Singapore.
* All Rights Reserved
*/

/*                                                                     guard
____________________________________________________________________________ */

#ifndef WP_SCRIPT_API_COMPONENT_PHYSIC_H_
#define WP_SCRIPT_API_COMPONENT_PHYSIC_H_

/*                                                                  includes
____________________________________________________________________________ */

#include <WP_CoreComponents/WP_CS_Component.hxx>
#include <WP_CoreComponents/WP_Physics.h>

/*                                                                   defines
____________________________________________________________________________ */

/*                                                                 functions
____________________________________________________________________________ */

/* _________________________________________________________________________ */

#ifndef DECLARE_CS_PROPERTIES
#define DECLARE_CS_PROPERTIES(_type, _name) \
property _type _name						\
{											\
	_type get();							\
	void set( _type );						\
};											\

#endif

#ifndef DECLARE_CS_PRIVATE_SET_PROPERTIES
#define DECLARE_CS_PRIVATE_SET_PROPERTIES(_type, _name) \
property _type _name						\
{											\
	_type get();							\
};											\

#endif

namespace ScriptAPI
{
	using namespace System::Numerics;
	public ref class WP_CS_Physics : WP_CS_Component
	{
	public:

		virtual property System::Boolean IsActive
		{
			System::Boolean get();
			void set(System::Boolean _is_active);
		}

		virtual property System::UInt32 EntityID
		{
			System::UInt32 get();
		}

		WP_CS_Physics(unsigned int _id, WP_Physics3D* _physics)
			:m_EntityID(_id), m_physics(_physics) {/*Empty by design*/ }

		using CS_Vec3 = System::Numerics::Vector3;

		void SetLinearVelocity(System::Numerics::Vector3 velocity);
		void AddVelocity(System::Numerics::Vector3 velocity);
		System::Numerics::Vector3 GetVelocity();

		DECLARE_CS_PROPERTIES(CS_Vec3, m_linearVelocity);

		void CharacterAddImpulse(CS_Vec3 const&);

		DECLARE_CS_PROPERTIES(CS_Vec3, m_rotation);

		void Rotate(CS_Vec3 const&);
		void RotateY(float);

		//currently only works if physics is a character (m_isNPC)
		System::Boolean IsGrounded();

		/*!***********************************************************************
		  \brief
			Adds the physics body from the physics simulation
			WARNING: This functions are costly and can cause physics to become
			a lot slower as all graph islands are re-calculated.
			Use only when absolutely required.
			Take note that there is NO safety net set up for this function. 
			As such, double adding CAN cause memory leaks.
			Therefore, Avoid using this function AT ALL COST!
		*************************************************************************/
		void AddBodyToSimulation();
		/*!***********************************************************************
		  \brief
			Remove the physics body from the physics simulation. All transient data
			auch as forces apllied and velocity will be reseted if re-added
			WARNING: This functions are costly and can cause physics to become
			a lot slower as all graph islands are re-calculated.
			Use only when absolutely required.
		*************************************************************************/
		void RemoveBodyFromSimulation();

		DECLARE_CS_PROPERTIES(System::Boolean, m_isActive);			//Runtime ok

		DECLARE_CS_PROPERTIES(System::Boolean, m_isTrigger);
		DECLARE_CS_PROPERTIES(System::Boolean, m_isCharacter); 
		DECLARE_CS_PRIVATE_SET_PROPERTIES(System::Boolean, m_isInPhysicsSystem);

		DECLARE_CS_PROPERTIES(System::Char, m_lockAxis);

#if JPH_OBJECT_LAYER_BITS == 16
		DECLARE_CS_PROPERTIES(System::UInt16, m_objectLayer);
#elif JPH_OBJECT_LAYER_BITS == 32
		DECLARE_CS_PROPERTIES(System::UInt32, m_objectLayer);
#endif
		
		DECLARE_CS_PROPERTIES(System::Single, m_gravityScale);		//Runtime ok
		DECLARE_CS_PROPERTIES(System::Single, m_friction);			//Runtime ok
		DECLARE_CS_PROPERTIES(System::Single, m_restitution);		//Runtime ok
		DECLARE_CS_PROPERTIES(System::Single, m_mass);				//Runtime ok
		DECLARE_CS_PROPERTIES(System::Single, m_maxSlopeAngle);
		DECLARE_CS_PROPERTIES(System::Single, m_maxSeperationDistance);

		DECLARE_CS_PROPERTIES(System::Numerics::Vector3, m_positionOffset);		//Runtime ok
		DECLARE_CS_PROPERTIES(System::Numerics::Quaternion, m_rotationOffset);	//Runtime ok

	internal:
		WP_CS_Physics(unsigned int id);
	private:
		WP_Physics3D* m_physics;
		unsigned int m_EntityID;
	};

	//WIP Plan: Add new namespace Physics, store physics system functions there
	public ref class WP_CS_PhysicsRaycast
	{
	public:
		/*!***********************************************************************
		\brief
			Casts a ray starting at _origin and ending _origin + _direction.
		\param [in] _origin
			Starting coordinates of the ray to be casted.
		\param [in] _direction
			the orientation/direction of the ray to be casted.
		\return
			if the ray hits a object.
		*************************************************************************/
		static System::Boolean Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction);
		/*!***********************************************************************
		\brief
			Casts a ray starting at _origin and ending _origin + _direction.
			return by reference the id and hitfraction of the first object
			hit by the ray.
		\param [in] _origin
			Starting coordinates of the ray to be casted.
		\param [in] _direction
			the orientation/direction of the ray to be casted.
		\param [out] _outID
			the gameobject id of the hit object.
		\param [out] _outHitFraction
			the hitFraction of the hit.
			(_origin + _direction * hitFraction = ray hit location)
		\return
			if the ray hits a object.
		*************************************************************************/
		static System::Boolean Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction
			, unsigned% _outID, float% _outHitFraction);
		/*!***********************************************************************
		\brief
			Casts a ray starting at _origin and ending _origin + _direction.
			return by reference the id and hitfraction of all game objects'
			hit by the ray.
		\param [in] _origin
			Starting coordinates of the ray to be casted.
		\param [in] _direction
			the orientation/direction of the ray to be casted.
		\param [out] _outIDs
			ArrayList pointer containing the gameobject ids of the objects hit 
			by the ray.
		\param [out] _outResults
			ArrayList pointer containing the hitFraction of all physics enabled 
			gameobject hit by the ray.
			(_origin + _direction * hitFraction = ray hit location)
		\return
			if the ray hits any object.
		*************************************************************************/
		static System::Boolean Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction
			, System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction);
		/*!***********************************************************************
		\brief
			Casts a ray starting at _origin and ending _origin + _direction.
		\param [in] _objectID
			the specific object to be raycasted against. All ray hits with physics
			enabled objects that do not have this id are ignored.
		\param [in] _origin
			Starting coordinates of the ray to be casted.
		\param [in] _direction
			the orientation/direction of the ray to be casted.
		\return
			if the ray hits the specific object.
		*************************************************************************/
		static System::Boolean RaycastHitObject(System::UInt32 _objectID, System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction);
	internal:
		WP_CS_PhysicsRaycast() {};

	};

#if 0	
	public ref class WP_CS_Physics
	{
	public:
		WP_CS_Physics()
			: m_physics (*(WP_PhysicsSystem::GetInstance()))  {/*Empty by design*/ }

		//static bool Raycast(Vector3^ _origin, Vector3^ _direction);
		//static bool Raycast(Vector3^ _origin, Vector3^ _direction, unsigned& _outHitID);
		//static bool Raycast(Vector3^ _origin, Vector3^ _direction, System::Collections::ArrayList _out);
	internal:
		
	private:
		WP_PhysicsSystem& m_physics;
	};
#endif
}

#endif // !WP_SCRIPT_API_COMPONENT_PHYSIC_H_
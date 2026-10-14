/*!
* @file			WP_CS_Physic.hxx
* @author		Emma Natalie Soh
* @par			Emma: Email: e.soh\@digipen.edu
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
		void MoveKinematic(System::Numerics::Vector3 _dest,float _timeToReachDestination);
		void MoveKinematic(System::Numerics::Vector3 _start, System::Numerics::Vector3 _dest, float _timeToReachDestination);
		
		DECLARE_CS_PROPERTIES(CS_Vec3, m_linearVelocity);

		void CharacterAddImpulse(CS_Vec3 const&);
		void SetMotionType(unsigned _motionType);

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
	



		//================================================================================
		//						Sphere/Cube cast functions
		// 
		//		Cast a single shape and check for hits
		//================================================================================

//++++++++++++++++++++++ Broad phase based functions +++++++++++++++++++++++++++++++++
		/*!***********************************************************************
		  \brief
			Cast a singular sphere against the physics system. Bodies hit will have
			their IDs returned by reference.
		  \param [in] _radius
			radius of the sphere to be casted
		  \param [in] _center
			the center/position of the sphere to be casted
		  \param [out] _outIDs
			a return by reference ArrayList containing the gameobject IDs of the 
			hit bodies
		  \return
			if a hit was observed by the sphere cast
		*************************************************************************/
		static System::Boolean SphereCast(
			float _radius,
			System::Numerics::Vector3^ _center,
			System::Collections::ArrayList^ _outIDs);

		/*!***********************************************************************
		  \brief
			Cast a singular cube against the physics system. Bodies hit will have
			their IDs returned by reference.
		  \param [in] _center
			the center/position of the sphere to be casted
		  \param [in] _globalRotation
			the quaternion containing the rotation of the cube to be casted
		  \param [in] _globalScale
			the Full extend / scale of the cube.
		  \param [out] _outIDs
			a return by reference ArrayList containing the gameobject IDs of the
			hit bodies
		  \return
			if a hit was observed by the cube cast
		*************************************************************************/
		static System::Boolean CubeCast(
			System::Numerics::Vector3^ _center,
			System::Numerics::Quaternion^ _globalRotation,
			System::Numerics::Vector3^ _globalScale,
			System::Collections::ArrayList^ _outIDs);

		/*!***********************************************************************
		  \brief
			Cast a singular cube against the physics system. Bodies hit will have
			their IDs returned by reference.
		  \param [in] _center
			the center/position of the sphere to be casted
		  \param [in] _globalScale
			the Full extend / scale of the cube.
		  \param [out] _outIDs
			a return by reference ArrayList containing the gameobject IDs of the
			hit bodies
		  \return
			if a hit was observed by the cube cast
		*************************************************************************/
		static System::Boolean CubeCast(
			System::Numerics::Vector3^ _center,
			System::Numerics::Vector3^ _globalScale,
			System::Collections::ArrayList^ _outIDs);

		//================================================================================
		//						Shape cast/collide Simple functions
		//		Use these for Creating Lasers or smthing
		//	A visual description of shapeCast is:
		// Shapes along this line hit by shape spam are considered hits
		// 
		//		O-> spam shapes along this line. 
		//		|-------------------------------------|
		//	  origin								origin + destination
		//	O											O
		//	^											^ These objects will not be 
		//	|-------------------------------------------| considered hit as they are
		//												  not in the span of the initial
		//												  ray
		//		think of a normal raycast but with volume
		//		this is a narrow phase query (expensive! Kinda!)
		//================================================================================

//++++++++++++++++++++++ Shape cast functions +++++++++++++++++++++++++++++++++
		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. an array list of ID hit by the casted shape
			is returned by reference. Creates a kebab of shapes on a line defined
			by _origin and _direction. if the line has a magnitude of 0, no shapes
			will be casted
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path 
			from _origin to _origin + _direction. It is recommended to pass in a 
			non-zero vector for this parameter.
		  \param [out] _outIDs
			gameobject ID of all physics objects hit by this shapecast
		  \return 
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCast(
			System::Char _shapeType,
			System::Numerics::Vector3^ _scale, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction, 
			System::Collections::ArrayList^ _outIDs);

		//vvv     Specialization of Shape Cast Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		//shape cast specialization functions. shape cast function can be used to achieve these function
		//these functions are for faster access to the shape you prefer.
		static System::Boolean SphereCast(float _radius, System::Numerics::Vector3^ _origin, 
			System::Numerics::Vector3^ _direction, 
			System::Collections::ArrayList^ _outIDs);
		static System::Boolean CubeCast(System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);
		static System::Boolean CylinderCast(float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);
		static System::Boolean CapsuleCast(float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);
#endif

//++++++++++++++++++++++ Shape cast against specific obejcts functions +++++++++++++++++++++++++++++++++
		
		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. the shape cast will only hit the gameobject
			with the _objectID ID, if it is physics enabled.
			Creates a satay stick of shapes on a line defined by _origin and _direction.
			if the line has a magnitude of 0, no shapes will be casted
		  \param [in] _objectID
			gameobject ID of the specific object you want the shape cast to hit
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path
			from _origin to _origin + _direction. It is recommended to pass in a
			zero vector for this parameter.
		  \return 
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCastHitObject(
			System::UInt32 _objectID,
			System::Char _shapeType,
			System::Numerics::Vector3^ _scale,
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction);

		//vvv     Specialization of Shape Cast Hit Object Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		static System::Boolean SphereCastHitObject(System::UInt32 _objectID, 
			float _radius, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction);

		static System::Boolean CubeCastHitObject(System::UInt32 _objectID, 
			System::Numerics::Vector3^ _scale, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction);

		static System::Boolean CylinderCastHitObject(System::UInt32 _objectID, 
			float _radius, float _height, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction);

		static System::Boolean CapsuleCastHitObject(System::UInt32 _objectID, 
			float _radius, float _height, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction);
#endif
//++++++++++++++++++++++ Shape Collision Cast functions +++++++++++++++++++++++++++++++++

		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. an array list of ID hit by the casted shape
			is returned by reference. all hit objects will experience collsion 
			resolution, where the hit object will trigger collision events and 
			will move as though it has collided into a physics body with the 
			same properties as the casted shape. This shape will only exist this
			frame and before the end of this function.

			Creates a skewer of shapes on a line defined by _origin and _direction.
			if the line has a magnitude of 0, no shapes will be casted
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path
			from _origin to _origin + _direction. It is recommended to pass in a
			zero vector for this parameter.
		  \param [out] _outIDs
			gameobject ID of all physics objects hit by this shapecast
		  \return
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCollide(
			System::Char _shapeType,
			System::Numerics::Vector3^ _scale,
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);
		
		//vvv     Specialization of Shape Collide Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		static System::Boolean SphereCollide(float _radius, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);

		static System::Boolean CubeCollide(System::Numerics::Vector3^ _scale, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);

		static System::Boolean CylinderCollide(float _radius, float _height, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);

		static System::Boolean CapsuleCollide(float _radius, float _height, 
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs);
#endif
	//================================================================================
	//						Shape cast/collide Advance functions
	//================================================================================

//++++++++++++++++++++++ Shape cast advance functions +++++++++++++++++++++++++++++++++

		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. Array lists of each of the properties of
			each hit by this Shape cast is returned by reference.
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path
			from _origin to _origin + _direction. It is recommended to pass in a
			zero vector for this parameter.
		  \param [out] _outIDs
			gameobject ID of all physics objects hit by this shapecast.
			
			[out] param array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \param [out] _outHitFraction
			float array containing the magnitude of the Collision Depth. 
			this multiplied by _outCollisionNormal will result in the 
			penetration vector for this hit for this cast.
			
			[out] param  array elements of index i belong to the same hit as 
			corresponding to [out] param arrays 
		  \param [out] _outCollisionPoint1
			Vec3 array containing the first point where the cast intersects
			with the hit object.
			
			[out] param  array elements of index i belong to the same hit as 
			corresponding to [out] param arrays 
		  \param [out] _outCollisionPoint2
			Vec3 array containing the second point where the cast intersects
			with the hit object.
			
			[out] param  array elements of index i belong to the same hit as 
			corresponding to [out] param arrays 
		  \param [out] _outCollisionNormal
			Vec3 array containing the normalized Pentration vector of the shape
			cast. This multiplied by _outHitFraction will result in the 
			penetration vector for this hit for this cast.
			
			[out] param  array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \return
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCastDetailed(
			System::Char _shapeType,	//Sphere = 1, Cube = 2, Capsule = 3, Cylinder = 4
			System::Numerics::Vector3^ _scale,
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		//vvv     Specialization of Shape Cast advance Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		static System::Boolean SphereCastDetailed(
			float _radius, System::Numerics::Vector3^ _origin, 
			System::Numerics::Vector3^ _direction, 
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CubeCastDetailed(
			System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CylinderCastDetailed(
			float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CapsuleCastDetailed(
			float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);
#endif

//++++++++++++++++++++++ Shape cast against specific obejcts advance functions +++++++++++++++++++++++++++++++++

		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. the shape cast will only hit the gameobject
			with the _objectID ID, if it is physics enabled.
		  \param [in] _objectID
			gameobject ID of the specific object you want the shape cast to hit
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path
			from _origin to _origin + _direction. It is recommended to pass in a
			zero vector for this parameter.
		  \param [out] _outIDs
			gameobject ID of all physics objects hit by this shapecast.
		  \param [out] _outHitFraction
			float containing the magnitude of the Collision Depth.
			this multiplied by _outCollisionNormal will result in the
			penetration vector for this hit for this cast.
		  \param [out] _outCollisionPoint1
			Vec3 containing the first point where the cast intersects
			with the hit object.
		  \param [out] _outCollisionPoint2
			Vec3 containing the second point where the cast intersects
			with the hit object.
		  \param [out] _outCollisionNormal
			Vec3 containing the normalized Pentration vector of the shape
			cast. This multiplied by _outHitFraction will result in the
			penetration vector for this hit for this cast.
		  \return
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCastHitObjectDetailed(
			System::UInt32 _objectID,
			System::Char _shapeType,
			System::Numerics::Vector3^ _scale,
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			float% _outHitFraction,
			System::Numerics::Vector3^ _outCollisionPoint1, 
			System::Numerics::Vector3^ _outCollisionPoint2,
			System::Numerics::Vector3^ _outCollisionNormal);

		//vvv     Specialization of Shape Cast Hit Object advance Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		static System::Boolean SphereCastHitObjectDetailed(
			System::UInt32 _objectID, float _radius, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			float% _outHitFraction,
			System::Numerics::Vector3^ _outCollisionPoint1,
			System::Numerics::Vector3^ _outCollisionPoint2,
			System::Numerics::Vector3^ _outCollisionNormal);

		static System::Boolean CubeCastHitObjectDetailed(
			System::UInt32 _objectID, System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			float% _outHitFraction,
			System::Numerics::Vector3^ _outCollisionPoint1,
			System::Numerics::Vector3^ _outCollisionPoint2,
			System::Numerics::Vector3^ _outCollisionNormal);

		static System::Boolean CylinderCastHitObjectDetailed(
			System::UInt32 _objectID, float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			float% _outHitFraction,
			System::Numerics::Vector3^ _outCollisionPoint1,
			System::Numerics::Vector3^ _outCollisionPoint2,
			System::Numerics::Vector3^ _outCollisionNormal);

		static System::Boolean CapsuleCastHitObjectDetailed(
			System::UInt32 _objectID, float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			float% _outHitFraction,
			System::Numerics::Vector3^ _outCollisionPoint1,
			System::Numerics::Vector3^ _outCollisionPoint2,
			System::Numerics::Vector3^ _outCollisionNormal);
#endif

//++++++++++++++++++++++ Shape Collision Cast advance functions +++++++++++++++++++++++++++++++++
	//These functions apply collision resolution.

		/*!***********************************************************************
		  \brief
			Shapecast a shape of x type. Array lists of hits by the casted 
			collision enabled shape is returned by reference. 
			All hit objects will experience collsion resolution, where the hit 
			object will trigger collision events and will move as though it has 
			collided into a physics body with the same properties as the casted 
			shape. This shape will only exist this frame and before the end 
			of this function.
		  \param [in] _shapeType
			char representing a shape type under WP_PhysicsShape
		  \param [in] _scale
			scale of the casted shape
		  \param [in] _origin
			origin of the casted shape
		  \param [in] _direction
			endpoint of the casted shape. The shape will be casted along the path
			from _origin to _origin + _direction. It is recommended to pass in a
			zero vector for this parameter.
		  \param [out] _outIDs
			gameobject ID of all physics objects hit by this shapecast.

			[out] param array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \param [out] _outHitFraction
			float array containing the magnitude of the Collision Depth.
			this multiplied by _outCollisionNormal will result in the
			penetration vector for this hit for this cast.

			[out] param  array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \param [out] _outCollisionPoint1
			Vec3 array containing the first point where the cast intersects
			with the hit object.

			[out] param  array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \param [out] _outCollisionPoint2
			Vec3 array containing the second point where the cast intersects
			with the hit object.

			[out] param  array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \param [out] _outCollisionNormal
			Vec3 array containing the normalized Pentration vector of the shape
			cast. This multiplied by _outHitFraction will result in the
			penetration vector for this hit for this cast.

			[out] param  array elements of index i belong to the same hit as
			corresponding to [out] param arrays
		  \return
			if a hit was observed by the shape cast
		*************************************************************************/
		static System::Boolean ShapeCollideDetailed(
			System::Char _shapeType,
			System::Numerics::Vector3^ _scale,
			System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs,System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);


		//vvv     Specialization of Shape Collide advance Functions     vvv
#if 1	//use this preprocessor block to hide this fat stack of code if your eyes hurt from all these declarations
		static System::Boolean SphereCollideDetailed(float _radius, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CubeCollideDetailed(System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CylinderCollideDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);

		static System::Boolean CapsuleCollideDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
			System::Numerics::Vector3^ _direction,
			System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
			System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
			System::Collections::ArrayList^ _outCollisionNormal);
#endif

	internal:
		WP_CS_PhysicsRaycast() {};

	};

	//class to add and modify physics layer behaviours
	public ref class WP_CS_PhysicsLayers {
	private:
#if JPH_OBJECT_LAYER_BITS == 16
		using indexType = System::UInt16;
#else
		using indexType = System::UInt32;
#endif
		//static System::Collections::Generic::Dictionary<System::String^, indexType>^ m_nameToLayerMap;
	public:
		static indexType s_InvalidLayerIndex = JPH_OBJECT_LAYER_BITS;

		//static indexType GetLayerIndex(System::String^ _layerName);
		//if layer already exists, layer will be named "_layerName(1)" / _layerName + "(1)" instead
		//static indexType AddLayer(System::String^ _layerName, System::Collections::BitArray^ _layerCollisionReaction);
		/*!***********************************************************************
		  \brief
			Get the layer interaction information of the object layer at _layerIndex
		  \param [in] _layerIndex
			index of layer to have its interaction information passed to _outArr
		  \param [out] _outArr
			return by reference bit array containing layer collision information
			for layer with index _layerIndex
		*************************************************************************/
		static void GetLayer(indexType _layerIndex, System::Collections::BitArray^ _outArr);
		/*!***********************************************************************
		  \brief
			Get the layer interaction information of the object layer at _layerIndex
			on whether it can collide with the non-moving broadphase layer
		  \param [in] _layerIndex
			index of layer to be checked
		  \return
			if layer at _indexLayer can collide with the non-moving broadphase layer.
		*************************************************************************/
		static bool GetLayerCollideWithStatic(indexType _layerIndex);
		/*!***********************************************************************
		  \brief
			Set the layer interaction information of the object layer at _layerIndex.
			layer 0 will never be set to collide with non-moving broadphase layer.
		  \param [in] _layerIndex
			index of layer to have its interaction information updated
		  \param [in] _layerCollisionReaction
			bit array containg the information if this layer should collide with them
		  \param [in] _collideWithStatic
			if this layer should collide with the non-moving broadphase layer.
			if _layerIndex is 0, this is ignored.
		*************************************************************************/
		static void UpdateLayer(indexType _layerIndex, System::Collections::BitArray^ _layerCollisionReaction, bool _collideWithStatic);
		/*!***********************************************************************
		  \brief
			Print the physics collision layer information through standard output
			stream. currently does not work via WP_INFO.
		*************************************************************************/
		static void PrintLayerInteractions();

		static indexType GetNumberOfPhysicsLayers();
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
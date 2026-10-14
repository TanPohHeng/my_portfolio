/*!
* @file			WP_CS_BoxCollider3D.cxx
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

/*                                                                  includes
____________________________________________________________________________ */

#include "WP_CS_Utility.hxx"
#include "../inc/WP_CoreComponents/WP_CS_Physics.hxx"
#include <WP_EngineSystem/WP_CSharp/WP_PhysicsCSExposer.h>

/*                                                                   defines
____________________________________________________________________________ */

/*                                                                 functions
____________________________________________________________________________ */

/* _________________________________________________________________________ */
#ifndef DEFINE_CS_PROPERTIES_GET
#define DECLARE_CS_PROPERTIES_GET(_type, _converter, _name, _func)	\
_type _name ::get ()												\
{ return _converter ( m_physics->_func() ); }						\

#endif

#ifndef DECLARE_CS_PROPERTIES_SET
#define DECLARE_CS_PROPERTIES_SET(_type, _converter, _name, _func)	\
void _name ::set ( _type _param)									\
{ m_physics-> _func ( _converter ( _param ));}						\

#endif

#ifndef DECLARE_CS_PROPERTIES_GET_SET
#define DECLARE_CS_PROPERTIES_GET_SET(_type, _converter, _name, _gFunc , _sFunc)	\
DECLARE_CS_PROPERTIES_GET( _type , _converter , _name , _gFunc )					\
DECLARE_CS_PROPERTIES_SET( _type , _converter , _name , _sFunc )					\

#endif


namespace ScriptAPI
{

	/* _____________________________________________________________________ */


	System::Boolean WP_CS_Physics::IsActive::get()
	{
		return WP_ECS::IsComponentActive<WP_Physics3D>(EntityID);
	}

	void WP_CS_Physics::IsActive::set(System::Boolean _is_active)
	{
		WP_ECS::SetComponentActive<WP_Physics3D>(EntityID, _is_active);
	}

	/* _____________________________________________________________________ */

	System::UInt32 WP_CS_Physics::EntityID::get()
	{
		return m_EntityID;
	}

	/* _____________________________________________________________________ */

	//vvv			Forces functions.				vvv

	void WP_CS_Physics::SetLinearVelocity(System::Numerics::Vector3 velocity)
	{
		WP_PhysicsCS2CPP::SetVelocity(m_EntityID, WP_CS_Utility::ConvertVector3(velocity));
	}
	void WP_CS_Physics::AddVelocity(System::Numerics::Vector3 velocity)
	{
		WP_PhysicsCS2CPP::AddVelocity(m_EntityID, WP_CS_Utility::ConvertVector3(velocity));
	}

	System::Numerics::Vector3 WP_CS_Physics::GetVelocity()
	{
		return m_linearVelocity;
	}

	void WP_CS_Physics::MoveKinematic(System::Numerics::Vector3 _dest, float _timeToReachDestination)
	{
		WP_PhysicsCS2CPP::PhysicsMoveKinematic(m_EntityID, WP_CS_Utility::ConvertVector3(_dest),_timeToReachDestination);
	}

	void WP_CS_Physics::MoveKinematic(System::Numerics::Vector3 _start, System::Numerics::Vector3 _dest, float _timeToReachDestination)
	{
		WP_PhysicsCS2CPP::PhysicsMoveKinematic(m_EntityID, 
			WP_CS_Utility::ConvertVector3(_start),
			WP_CS_Utility::ConvertVector3(_dest),
			_timeToReachDestination);
	}

	System::Numerics::Vector3 WP_CS_Physics::m_linearVelocity::get()
	{
		return WP_CS_Utility::ConvertVector3(WP_PhysicsCS2CPP::GetVelocity(m_EntityID));
	}

	void WP_CS_Physics::m_linearVelocity::set(System::Numerics::Vector3 _newVel)
	{
		SetLinearVelocity(_newVel);
	};

	void WP_CS_Physics::CharacterAddImpulse(CS_Vec3 const& _impulse)
	{
		WP_PhysicsCS2CPP::AddImpulse(m_EntityID, WP_CS_Utility::ConvertVector3(_impulse));
	}

	void WP_CS_Physics::SetMotionType(unsigned _motionType)
	{
		WP_PhysicsCS2CPP::SetMotionType(m_EntityID, _motionType);
	}

	WP_CS_Physics::WP_CS_Physics(unsigned int id)
		:m_EntityID{ id }
	{
		m_physics = WP_ECS::GetComponent<WP_Physics3D>(m_EntityID);
	}

	System::Boolean WP_CS_Physics::IsGrounded()
	{
		return WP_PhysicsCS2CPP::GetIsCharacterGrounded(m_EntityID);
	}


	//vvv			Add/Remove Body. Read header tooltip			vvv

	void WP_CS_Physics::AddBodyToSimulation()
	{
		m_physics->AddBody();
	}
	void WP_CS_Physics::RemoveBodyFromSimulation()
	{
		m_physics->RemoveBody();
	}

	//vvv			Properties Definition			vvv

	DECLARE_CS_PROPERTIES_GET_SET(System::Boolean, , WP_CS_Physics::m_isActive, GetIsActive, SetIsActive);
	DECLARE_CS_PROPERTIES_GET_SET(System::Boolean, , WP_CS_Physics::m_isTrigger, GetIsTrigger, SetIsTrigger);
	DECLARE_CS_PROPERTIES_GET_SET(System::Boolean, , WP_CS_Physics::m_isCharacter, GetIsNPC, SetIsNPC);

	DECLARE_CS_PROPERTIES_GET(System::Boolean, , WP_CS_Physics::m_isInPhysicsSystem, GetIsValid);

	DECLARE_CS_PROPERTIES_GET_SET(System::Char, , WP_CS_Physics::m_lockAxis, GetLockedAxis, SetLockedAxis);

#if JPH_OBJECT_LAYER_BITS == 16
	DECLARE_CS_PROPERTIES_GET_SET(System::UInt16, , WP_CS_Physics::m_objectLayer, GetObjectLayer, SetObjectLayer);
#elif JPH_OBJECT_LAYER_BITS == 32
	DECLARE_CS_PROPERTIES_GET_SET(System::UInt32, , WP_CS_Physics::m_objectLayer, GetObjectLayer, SetObjectLayer);
#endif

	DECLARE_CS_PROPERTIES_GET_SET(System::Single, , WP_CS_Physics::m_gravityScale, GetGravityScale, SetGravityScale);
	DECLARE_CS_PROPERTIES_GET_SET(System::Single, , WP_CS_Physics::m_friction, GetFriction, SetFriction);
	DECLARE_CS_PROPERTIES_GET_SET(System::Single, , WP_CS_Physics::m_restitution, GetCOR, SetCOR);
	DECLARE_CS_PROPERTIES_GET_SET(System::Single, , WP_CS_Physics::m_mass, GetMass, SetMass);
	DECLARE_CS_PROPERTIES_GET_SET(System::Single, , WP_CS_Physics::m_maxSlopeAngle, GetMaxSlopeInDegrees, SetMaxSlopeInDegrees);
	DECLARE_CS_PROPERTIES_GET_SET(System::Single, ,
		WP_CS_Physics::m_maxSeperationDistance, GetMaxFloorSeperation, SetMaxFloorSeperation);

	DECLARE_CS_PROPERTIES_GET_SET(System::Numerics::Vector3, WP_CS_Utility::ConvertVector3 ,
		WP_CS_Physics::m_positionOffset, GetPosOffset, SetPosOffset);
	DECLARE_CS_PROPERTIES_GET_SET(System::Numerics::Quaternion, WP_CS_Utility::ConvertQuaternion ,
		WP_CS_Physics::m_rotationOffset, GetRotOffset, SetRotOffset);


	//vvv			RayCast Definition			vvv


	System::Boolean WP_CS_PhysicsRaycast::Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction)
	{
		return WP_PhysicsCS2CPP::Raycast(WP_CS_Utility::ConvertVector3(_origin), WP_CS_Utility::ConvertVector3(_direction));
	}

	System::Boolean WP_CS_PhysicsRaycast::Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction
		, unsigned% _outID, float% _outHitFraction)
	{
		std::pair<unsigned, float> rResult;
		bool retVal = WP_PhysicsCS2CPP::Raycast(
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			rResult);
		_outID = rResult.first;
		_outHitFraction = rResult.second;
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::Raycast(System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction
		, System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFractions)
	{
		std::vector<std::pair<unsigned, float>> rResult;
		bool retVal = WP_PhysicsCS2CPP::Raycast(
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			rResult);
		for (auto& t : rResult)
		{
			_outIDs->Add(t.first);
			_outHitFractions->Add(t.second);
		}
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::RaycastHitObject
	(System::UInt32 _objectID, System::Numerics::Vector3^ _origin, System::Numerics::Vector3^ _direction)
	{
		return WP_PhysicsCS2CPP::RaycastHitObject(_objectID,
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction)
		);
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCast(
		float _radius, 
		System::Numerics::Vector3^ _center, 
		System::Collections::ArrayList^ _outIDs)
	{
		std::vector<unsigned> rResult;
		bool retVal = WP_PhysicsCS2CPP::SphereCast(
			_radius,
			WP_CS_Utility::ConvertVector3(_center),
			rResult);
		for (auto& t : rResult)
		{	//t is GameObjectIDs
			_outIDs->Add(t);
		}
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::CubeCast(
		System::Numerics::Vector3^ _center, 
		System::Numerics::Quaternion^ _globalRotation, 
		System::Numerics::Vector3^ _globalScale, //full extends
		System::Collections::ArrayList^ _outIDs)
	{
		std::vector<unsigned> rResult;
		bool retVal = WP_PhysicsCS2CPP::CubeCast(
			WP_CS_Utility::ConvertVector3(_center),
			WP_CS_Utility::ConvertQuaternion(_globalRotation),
			WP_CS_Utility::ConvertVector3(_globalScale),
			rResult);
		for (auto& t : rResult)
		{	//t is GameObjectIDs
			_outIDs->Add(t);
		}
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::CubeCast(
		System::Numerics::Vector3^ _center, 
		System::Numerics::Vector3^ _globalScale, 
		System::Collections::ArrayList^ _outIDs)
	{
		return WP_CS_PhysicsRaycast::CubeCast(_center,System::Numerics::Quaternion(0,0,0,1),_globalScale,_outIDs);
	}

	System::Boolean WP_CS_PhysicsRaycast::ShapeCast(
		System::Char _shapeType, 
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction, 
		System::Collections::ArrayList^ _outIDs)
	{
		std::vector<unsigned> results;
		System::Boolean retVal = WP_PhysicsCS2CPP::Shapecast(
			_shapeType,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			results);

		for (auto& t : results)
		{	//pass over the out ids
			_outIDs->Add(t);
		}
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCast(float _radius,
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction, 
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCast(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction,
			_outIDs);
	}

	System::Boolean WP_CS_PhysicsRaycast::CubeCast(System::Numerics::Vector3^ _scale,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCast(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction,
			_outIDs);
	}

	System::Boolean WP_CS_PhysicsRaycast::CylinderCast(float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCast(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs);
	}

	System::Boolean WP_CS_PhysicsRaycast::CapsuleCast(float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCast(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs);
	}

	System::Boolean WP_CS_PhysicsRaycast::ShapeCastHitObject(
		System::UInt32 _objectID,
		System::Char _shapeType,
		System::Numerics::Vector3^ _scale,
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction)
	{
		return WP_PhysicsCS2CPP::ShapecastHitObject(
			_shapeType,
			_objectID,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction));
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCastHitObject(System::UInt32 _objectID, float _radius,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction)
	{
		return ShapeCastHitObject(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction);
	}

	System::Boolean WP_CS_PhysicsRaycast::CubeCastHitObject(System::UInt32 _objectID,
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction)
	{
		return ShapeCastHitObject(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction);
	}

	System::Boolean WP_CS_PhysicsRaycast::CylinderCastHitObject(System::UInt32 _objectID, float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction)
	{
		return ShapeCastHitObject(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction);
	}

	System::Boolean WP_CS_PhysicsRaycast::CapsuleCastHitObject(System::UInt32 _objectID, float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction)
	{
		return ShapeCastHitObject(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction);
	}

	System::Boolean WP_CS_PhysicsRaycast::ShapeCollide(
		System::Char _shapeType, 
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction, 
		System::Collections::ArrayList^ _outIDs)
	{
		std::vector<unsigned> results;
		System::Boolean retVal = WP_PhysicsCS2CPP::ShapeCollide(
			_shapeType,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			results);

		for (auto& t : results)
		{	//pass over the out ids
			_outIDs->Add(t);
		}
		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCollide(float _radius,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCollide(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction,
			_outIDs);
	}
	System::Boolean WP_CS_PhysicsRaycast::CubeCollide(System::Numerics::Vector3^ _scale,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCollide(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction,
			_outIDs);
	}
	System::Boolean WP_CS_PhysicsRaycast::CylinderCollide(float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCollide(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs);
	}
	System::Boolean WP_CS_PhysicsRaycast::CapsuleCollide(float _radius, float _height,
		System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs)
	{
		return ShapeCollide(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs);
	}


	System::Boolean WP_CS_PhysicsRaycast::ShapeCastDetailed(
		System::Char _shapeType, 
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction, 
		System::Collections::ArrayList^ _outIDs, 
		System::Collections::ArrayList^ _outCollisionDepth, 
		System::Collections::ArrayList^ _outCollisionPoint1, 
		System::Collections::ArrayList^ _outCollisionPoint2, 
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		//create structures to store results
		std::vector<unsigned> idResults;
		std::vector<float> collisionDepth;
		std::vector<glm::vec3> collisionPoint1Results;
		std::vector<glm::vec3> collisionPoint2Results;
		std::vector<glm::vec3> collisionNormalResults;

		System::Boolean retVal = WP_PhysicsCS2CPP::Shapecast(
			_shapeType,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			idResults,
			collisionPoint1Results,
			collisionPoint2Results,
			collisionNormalResults,
			collisionDepth);

		for (int i{0}; i < idResults.size() ;++i)
		{	//pass over the out results
			_outIDs->Add(idResults[i]);
			_outCollisionDepth->Add(collisionDepth[i]);
			_outCollisionPoint1->Add(WP_CS_Utility::ConvertVector3(collisionPoint1Results[i]));
			_outCollisionPoint2->Add(WP_CS_Utility::ConvertVector3(collisionPoint2Results[i]));
			_outCollisionNormal->Add(WP_CS_Utility::ConvertVector3(collisionNormalResults[i]));
		}

		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCastDetailed(float _radius, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCastDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CubeCastDetailed(System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCastDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CylinderCastDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCastDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CapsuleCastDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCastDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}

	System::Boolean WP_CS_PhysicsRaycast::ShapeCastHitObjectDetailed(
		System::UInt32 _objectID, 
		System::Char _shapeType, 
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction,
		float% _outHitFraction,
		System::Numerics::Vector3^ _outCollisionPoint1,
		System::Numerics::Vector3^ _outCollisionPoint2,
		System::Numerics::Vector3^ _outCollisionNormal)
	{
		//create structures to store results
		float collisionDepth;
		glm::vec3 collisionPoint1Results;
		glm::vec3 collisionPoint2Results;
		glm::vec3 collisionNormalResults;

		System::Boolean retVal = WP_PhysicsCS2CPP::ShapecastHitObject(
			_shapeType,
			_objectID,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			collisionPoint1Results,
			collisionPoint2Results,
			collisionNormalResults,
			collisionDepth);

		_outHitFraction = collisionDepth;
		_outCollisionPoint1 = WP_CS_Utility::ConvertVector3(collisionPoint1Results);
		_outCollisionPoint2 = WP_CS_Utility::ConvertVector3(collisionPoint2Results);
		_outCollisionNormal = WP_CS_Utility::ConvertVector3(collisionNormalResults);

		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCastHitObjectDetailed(System::UInt32 _objectID, float _radius, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		float% _outHitFraction,
		System::Numerics::Vector3^ _outCollisionPoint1,
		System::Numerics::Vector3^ _outCollisionPoint2,
		System::Numerics::Vector3^ _outCollisionNormal)
	{
		return ShapeCastHitObjectDetailed(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CubeCastHitObjectDetailed(System::UInt32 _objectID, System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		float% _outHitFraction,
		System::Numerics::Vector3^ _outCollisionPoint1,
		System::Numerics::Vector3^ _outCollisionPoint2,
		System::Numerics::Vector3^ _outCollisionNormal)
	{
		return ShapeCastHitObjectDetailed(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CylinderCastHitObjectDetailed(System::UInt32 _objectID, float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		float% _outHitFraction,
		System::Numerics::Vector3^ _outCollisionPoint1,
		System::Numerics::Vector3^ _outCollisionPoint2,
		System::Numerics::Vector3^ _outCollisionNormal)
	{
		return ShapeCastHitObjectDetailed(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CapsuleCastHitObjectDetailed(System::UInt32 _objectID, float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		float% _outHitFraction,
		System::Numerics::Vector3^ _outCollisionPoint1,
		System::Numerics::Vector3^ _outCollisionPoint2,
		System::Numerics::Vector3^ _outCollisionNormal)
	{
		return ShapeCastHitObjectDetailed(
			_objectID,
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}


	System::Boolean WP_CS_PhysicsRaycast::ShapeCollideDetailed(
		System::Char _shapeType, 
		System::Numerics::Vector3^ _scale, 
		System::Numerics::Vector3^ _origin, 
		System::Numerics::Vector3^ _direction, 
		System::Collections::ArrayList^ _outIDs, 
		System::Collections::ArrayList^ _outCollisionDepth, 
		System::Collections::ArrayList^ _outCollisionPoint1, 
		System::Collections::ArrayList^ _outCollisionPoint2, 
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		//create structures to store results
		std::vector<unsigned> idResults;
		std::vector<float> collisionDepth;
		std::vector<glm::vec3> collisionPoint1Results;
		std::vector<glm::vec3> collisionPoint2Results;
		std::vector<glm::vec3> collisionNormalResults;

		System::Boolean retVal = WP_PhysicsCS2CPP::ShapeCollide(
			_shapeType,
			WP_CS_Utility::ConvertVector3(_scale),
			glm::quat(0, 0, 0, 1),
			WP_CS_Utility::ConvertVector3(_origin),
			WP_CS_Utility::ConvertVector3(_direction),
			idResults,
			collisionPoint1Results,
			collisionPoint2Results,
			collisionNormalResults,
			collisionDepth);

		for (int i{ 0 }; i < idResults.size(); ++i)
		{	//pass over the out results
			_outIDs->Add(idResults[i]);
			_outCollisionDepth->Add(collisionDepth[i]);
			_outCollisionPoint1->Add(WP_CS_Utility::ConvertVector3(collisionPoint1Results[i]));
			_outCollisionPoint2->Add(WP_CS_Utility::ConvertVector3(collisionPoint2Results[i]));
			_outCollisionNormal->Add(WP_CS_Utility::ConvertVector3(collisionNormalResults[i]));
		}

		return retVal;
	}

	System::Boolean WP_CS_PhysicsRaycast::SphereCollideDetailed(float _radius, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCollideDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::SPHERE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _radius, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CubeCollideDetailed(System::Numerics::Vector3^ _scale, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCollideDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CUBE),
			_scale,
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CylinderCollideDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCollideDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CYLINDER),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}
	System::Boolean WP_CS_PhysicsRaycast::CapsuleCollideDetailed(float _radius, float _height, System::Numerics::Vector3^ _origin,
		System::Numerics::Vector3^ _direction,
		System::Collections::ArrayList^ _outIDs, System::Collections::ArrayList^ _outHitFraction,
		System::Collections::ArrayList^ _outCollisionPoint1, System::Collections::ArrayList^ _outCollisionPoint2,
		System::Collections::ArrayList^ _outCollisionNormal)
	{
		return ShapeCollideDetailed(
			(std::underlying_type_t<WP_PhysicsShape>)(WP_PhysicsShape::CAPSULE),
			WP_CS_Utility::ConvertVector3(glm::vec3(_radius, _height, _radius)),
			_origin,
			_direction,
			_outIDs,
			_outHitFraction,
			_outCollisionPoint1,
			_outCollisionPoint2,
			_outCollisionNormal);
	}

	//WP_CS_PhysicsLayers::indexType WP_CS_PhysicsLayers::GetLayerIndex(System::String^ _layerName)
	//{
	//	WP_CS_PhysicsLayers::indexType retVal;
	//	if (m_nameToLayerMap->TryGetValue(_layerName,retVal))
	//		return retVal;
	//	else
	//		return s_InvalidLayerIndex;
	//}

	//WP_CS_PhysicsLayers::indexType WP_CS_PhysicsLayers::AddLayer
	//(System::String^ _layerName, System::Collections::BitArray^ _layerCollisionReaction)
	//{
	//	WP_CS_PhysicsLayers::indexType index = 
	//		static_cast<WP_CS_PhysicsLayers::indexType>(m_nameToLayerMap->Count) + 6;	//+6 for the original 6 preset layers
	//	if (index > (8 * sizeof(WP_CS_PhysicsLayers::indexType))) 
	//	{
	//		return s_InvalidLayerIndex;	//max layers reached
	//	}
	//	if (m_nameToLayerMap->ContainsKey(_layerName))
	//		m_nameToLayerMap->Add(_layerName + "(1)", index);
	//	else
	//		m_nameToLayerMap->Add(_layerName, index);
	//	
	//	return index;
	//}

	void WP_CS_PhysicsLayers::GetLayer(WP_CS_PhysicsLayers::indexType _layerIndex, System::Collections::BitArray^ _outArr)
	{
		constexpr unsigned size = sizeof(WP_PhysicsCS2CPP::physicsCharLayerArr);
		char layerReaction[size]{};
		memset(&layerReaction, 0, size);	//clean array
		WP_PhysicsCS2CPP::GetPhysicsLayerInteraction(_layerIndex, layerReaction);

		for (unsigned i{}; i < size; ++i)
		{
			_outArr->Set(i, layerReaction[i] ? true : false );
		}
	}

	bool WP_CS_PhysicsLayers::GetLayerCollideWithStatic(indexType _layerIndex)
	{
		return WP_PhysicsCS2CPP::GetPhysicsLayerCollideWithMoving(_layerIndex);
	}

	void WP_CS_PhysicsLayers::UpdateLayer(
		WP_CS_PhysicsLayers::indexType _layerIndex, 
		System::Collections::BitArray^ _layerCollisionReaction, 
		bool _collideWithStatic)
	{
		constexpr unsigned size = sizeof(WP_PhysicsCS2CPP::physicsCharLayerArr);
		char layerReaction[size] {};
		memset(&layerReaction,0, size);	//clean array

		for (unsigned i{};i < static_cast<unsigned>(_layerCollisionReaction->Count);++i)
		{
			layerReaction[i] = _layerCollisionReaction->Get(i);
		}
		WP_PhysicsCS2CPP::SetPhysicsLayerInteraction(_layerIndex, layerReaction);
		if (_layerIndex != 0)
			WP_PhysicsCS2CPP::SetPhysicsLayerCollideWithMoving(_layerIndex, _collideWithStatic);
		else
			WP_PhysicsCS2CPP::SetPhysicsLayerCollideWithMoving(_layerIndex, false);
	}
	void WP_CS_PhysicsLayers::PrintLayerInteractions()
	{
		WP_PhysicsCS2CPP::PrintPhysicsLayerInteraction();
	}
	WP_CS_PhysicsLayers::indexType WP_CS_PhysicsLayers::GetNumberOfPhysicsLayers()
	{
		return static_cast<WP_CS_PhysicsLayers::indexType>(sizeof(WP_CS_PhysicsLayers::indexType)) * 8;
	}
}

/*!
* @file			WP_CS_BoxCollider3D.cxx
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

	WP_CS_Physics::WP_CS_Physics(unsigned int id)
		:m_EntityID{ id }
	{
		m_physics = WP_ECS::GetComponent<WP_Physics3D>(m_EntityID);
	}

	//vvv			 Rotation functions.				vvv
	System::Numerics::Vector3 WP_CS_Physics::m_rotation::get()
	{
		return WP_CS_Utility::ConvertVector3(WP_PhysicsCS2CPP::GetRotation(m_EntityID));
	}

	void WP_CS_Physics::m_rotation::set(System::Numerics::Vector3 _newRot)
	{
		WP_PhysicsCS2CPP::SetRotation(m_EntityID,WP_CS_Utility::ConvertVector3(_newRot));
	};


	void WP_CS_Physics::Rotate(CS_Vec3 const& _newRot)
	{
		WP_PhysicsCS2CPP::AddRotation(m_EntityID, WP_CS_Utility::ConvertVector3(_newRot));
	}

	void WP_CS_Physics::RotateY(float _angle)
	{
		WP_PhysicsCS2CPP::AddRotation(m_EntityID, glm::vec3(0,_angle,0));
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
}
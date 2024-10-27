/******************************************************************************/
/*!
\file		Rigidbody.h
\author 	Tan Jun Rong, t.junrong, 2200972
\par    	email: t.junrong@digipen.edu
\date   	November 02, 2023
\brief		Source file containing rigidbody component.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "ECS/BaseComponent.h"
#include "Maths/Math_Includes.h"
#include "PhysicsECS.h"

using namespace ANC::ECS;
#if 0
struct RigidBody : public BaseComponent {

	static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::RIGIDBODY;

	COMPONENT_TYPES const GetCompType() override {
		return compType;
	}

	Math::Vec2 velocity{};
	bool isStatic{};
	float mass = 1.0f;
	float gravityScale = 1.0f;

	//=============================================================================
	//\brief
	//		Default Constructor for rigidbody component.
	//\parma [in] initMass
	//		Mass of rigidbody.
	//\param [in] flag
	//		Static flag of rigidbody.
	//=============================================================================
	RigidBody(float initMass, bool const flag) :
		mass{ initMass }, 
		isStatic{ flag },
		gravityScale{1.0f},
		c_velocity{ 0, 0 }
	{
	};

	//=============================================================================
	//\brief
	//		Default Constructor for rigidbody component.
	//\param [in] flag
	//		Static flag of rigidbody.
	//=============================================================================
	RigidBody(bool const flag) :
		mass{ 1.0f },
		isStatic{ flag },
		gravityScale{ 1.0f },
		c_velocity{ 0, 0 }
	{
	};

	RigidBody() = default;
	~RigidBody() = default;

	//=============================================================================
	//\brief
	//		Reset to initialized value.
	//=============================================================================
	void Reset() override {
		velocity = c_velocity;
	}

	//=============================================================================
	//\brief
	//		Add force to rigidbody.
	//=============================================================================
	void AddForce(Math::Vec2 additionalVelocity) {
		velocity += additionalVelocity;
	}

private:
	// Cached properties, initial state of components.
	Math::Vec2 c_velocity{};

};
#endif
#endif

/******************************************************************************/
/*!
\file		Collider2D.h
\author 	Tan Jun Rong, t.junrong, 2200972
\par    	email: t.junrong@digipen.edu
\date   	November 02, 2023
\brief		Source file containing Collider2D component.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#ifndef COLLIDER2D_H
#define COLLIDER2D_H

#include "ECS/BaseComponent.h"
#include "Maths/Math_Includes.h"
#include "CollisionECS.h"
#include "Discrete_Col.h"

using namespace ANC::ECS;
#if 0
struct Collider2D : public BaseComponent {

	static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::COLLIDER_2D;

	COMPONENT_TYPES const GetCompType() override {
		return compType;
	}

	COLLIDER_TYPE type;
	Collider offset;		// Offset from transform
	using callBack = void (*)(Collider2D);
	callBack onCollision {nullptr};
	bool isTrigger;

	//=============================================================================
	//\brief
	//		Default Constructor for collider2D component. Initialize with Rect collider.
	//=============================================================================
	Collider2D() :
		type{ COLLIDER_TYPE::RECT }, offset{ AABB{} }, isTrigger{false} {}

	//=============================================================================
	//\brief
	//		Default Constructor for collider2D component.
	//\parma [in] t
	//		Type of the collider passed in c.
	//\param [in] c
	//		The collider of this object to be initialized with.
	//=============================================================================
	Collider2D(COLLIDER_TYPE const& t, Collider const& c) :
		type{ t }, offset{ c }, isTrigger{ false } {}


	std::set<Collider2D*> collisionData;
};
#endif
#endif
/******************************************************************************/
/*!
\file		RigidbodySystem.h
\author 	Tan Jun Rong, t.junrong, 2200972
\par    	email: t.junrong@digipen.edu
\date   	November 02, 2023
\brief		Source file containing rigidbody system.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef RIGIDBODY_SYSTEM_H
#define RIGIDBODY_SYSTEM_H

#include "ECS/BaseSystem.h"

#include "Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "PhysicsECS.h"

#include "ECS/EntityManager.h"

#include "Engine/ANC_Timer.h"
#include "Engine/SceneManager.h"
#include "Engine/ANC_Graphics.h"
#include "Utils/Logger.h"

#include "Engine/Global.h"

using namespace ANC::ECS;
#if 0
struct RigidbodySystem : public BaseSystem {
public:
	//=============================================================================
	//\brief
	//		Check if entity has the required component for this system to operate on it.
	//\parma [in] entityId
	//		Id of entity.
	//=============================================================================
	bool CanOperateOn(EntityID entityId) override {
		return (entityMgr.HasComponent<Transform_Component>(entityId) 
			&& entityMgr.HasComponent<RigidBody>(entityId));
	}

	//=============================================================================
	//\brief
	//		Update all entities that this system manages.
	//=============================================================================
	void Update() override {
		for (EntityID const entityId : GetEntities()) {
			float dt = ANC::Time.DeltaTime();
			// Get components of entity,
			Transform_Component* transform = entityMgr.GetComponent<Transform_Component>(entityId);
			RigidBody* rigidBody = entityMgr.GetComponent<RigidBody>(entityId);

			// Update rigidbody velocity.
			rigidBody->velocity = (rigidBody->isStatic) ? rigidBody->velocity : rigidBody->velocity + Math::Vec2{ 0.0f, -GRAVITY * rigidBody->gravityScale * dt };

			// Update position based on velocity.
			transform->position += (rigidBody->velocity * dt);

			if (DEBUG_MODE && SHOW_EDITOR) { DrawDebugVector(transform, rigidBody, dt); }
		}
	}

	//=============================================================================
	//\brief
	//		Draw velocity vector.
	// \param [out] t
	//		Transform component of entity.
	// \param [out] rb
	//		Rigidbody component of entity.
	// \param [out] dt
	//		Delta time.
	//=============================================================================
	void DrawDebugVector(Transform_Component* t, RigidBody* rb, float const dt)
	{	//Draw a line in direction of the velocity
		ANC::Graphics.DrawLine(t->position, t->position + (rb->velocity * dt), 1, { 0.0,1.0,0.0 });
	}
};
#endif
#endif

/******************************************************************************/
/*!
\file		Collider2DSystem.h
\author 	Tan Jun Rong, t.junrong, 2200972
			(Secondary) Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.junrong@digipen.edu
			email: t.pohheng@digipen.edu
\date   	November 02, 2023
\brief		Source file containing collider2D system.
			Secondary author added Broad Phase Integration.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "ECS/BaseSystem.h"

#include "Transform.h"
//#include "ECS/Components/Collider2D.h"
#include "PhysicsECS.h"
#include "Discrete_Col.h"

#include "ECS/EntityManager.h"
#include "Utils/Logger.h"

#include "Engine/SceneManager.h"
#include "Engine/ANC_Graphics.h"
#include "Discrete_Col.h"

#include "Engine/ANC_Editor.h"
#include "Engine/Global.h"

/*================================================
///////////////Broad Phase Includes///////////////
==================================================*/
#include "Broad_Phase_Opt.h"
#include <tuple>
#include <vector>
//#include <map>
#include <unordered_map>
#define ANC_BPO_OPTIMIZE_LESS_PAIRS
/*================================================
///////////End of Broad Phase Includes////////////
==================================================*/

using namespace ANC::ECS;

/*================================================
///////////Broad Phase Support Defines////////////
==================================================*/
namespace temp{
std::pair<Math::Vec2, Math::Vec2> GetBounds
(COLLIDER_TYPE type, Collider const& col, Transform_Component const& trans);
long Round(f64 ref);
long RoundDown(f64 ref);
long RoundUp(f64 ref);
}


/*================================================
///////End of Broad Phase Support Defines/////////
==================================================*/

struct Collider2DSystem : public BaseSystem {
	ANC_DCS dcs;
	inline static COL_PAIR_LIST frameCollisionPairs{};
	inline static DCC frameComponentReference{};
	inline static std::set<ANC_DCC*> collisionData{};

	/*================================================
	///////////////Broad Phase Members////////////////
	\brief
		items in this section are intended for the use 
		of in-object Broad Phase implementation. These
		shall only be used within this object's functions
		and no where else.
	==================================================*/
	long broadPhaseInterval{2};		//edit this directly
#if 0
	using DC_COMPS = std::vector<std::tuple<Collider2D*, Transform_Component*, RigidBody*>>;
	using Const_DC_COMPS = std::vector<std::tuple<Collider2D const&, Transform_Component const&, RigidBody const&>>;
	using BP_RESULTS = std::unordered_map<size_t, std::unordered_set<size_t>>;
private:
	BP_RESULTS BroadPhaseCheck(DC_COMPS& colList, long interval);
	/*================================================
	///////////End of Broad Phase Members/////////////
	==================================================*/
#endif
public:
	

	//=============================================================================
	//\brief
	//		Check if entity has the required component for this system to operate on it.
	//\parma [in] entityId
	//		Id of entity.
	//=============================================================================
	bool CanOperateOn(EntityID entityId) override {
		return (entityMgr.HasComponent<Transform_Component>(entityId)
			&& entityMgr.HasComponent<Collider2D>(entityId));
	}

	//=============================================================================
	//\brief
	//		Update all entities that this system manages.
	//=============================================================================
	void FixedUpdate() override {
#if 0
		for (EntityID const entity : GetEntities()) {
			// Get components of entity.
			Collider2D* collider = entityMgr.GetComponent<Collider2D>(entity);
			Transform* transform = entityMgr.GetComponent<Transform>(entity);
			RigidBody* rigidbody = entityMgr.GetComponent<RigidBody>(entity);

			if (SHOW_COLLIDERS || ANC::Editor.GetSelectedEntity() == entity) {
				DrawCollider(collider, transform);
			} 

			for (EntityID const other : GetEntities()) {
				if (entity == other) continue;

				ANC_DCC c(collider->GetType(), collider->offset);

				Transform_Component t;
				t.position = transform->position;
				t.rotInRad = transform->rotation;
				t.scale = transform->scale;

				Physics_Component p;
				if (rigidbody) {
					p.setMass(rigidbody->mass);
					p.setVel(rigidbody->velocity);
					p.setIsStatic(rigidbody->isStatic);
				}
	
				Collider2D* otherCollider = entityMgr.GetComponent<Collider2D>(other);
				Transform* otherTransform = entityMgr.GetComponent<Transform>(other);
				RigidBody* otherRigidbody = entityMgr.GetComponent<RigidBody>(other);

				ANC_DCC c2(otherCollider->type, otherCollider->offset);

				Transform_Component t2;
				t2.position = otherTransform->position;
				t2.rotInRad = otherTransform->rotation;
				t2.scale = otherTransform->scale;

				Physics_Component p2;
				if (otherRigidbody) {
					p2.setMass(otherRigidbody->mass);
					p2.setVel(otherRigidbody->velocity);
					p2.setIsStatic(otherRigidbody->isStatic);
				}
				else {
					p2.setIsStatic(true);
					p2.setVel({ 0.0f, 0.0f, 0.0f });
				}
				
				//Check for collision.
				if (dcs.DetermineCollisionAndResponse(c, t, p, c2, t2, p2)) {
					if (rigidbody) {
						transform->position = t.position;
						rigidbody->velocity = p.getVel();
					}

					if (otherRigidbody) {
						otherTransform->position = t2.position;
						otherRigidbody->velocity = p2.getVel();
					}
				}
			}
		}
#else

		// Clear all old collison data at the start of frame.
		for (EntityID const entity : GetEntities()) {
			Collider2D* collider = entityMgr.GetComponent<Collider2D>(entity);
			if (!collider) { continue; }
			if (collider->collisionData.size()) { collider->collisionData.clear(); }
		}
		collisionData.clear();

	//========================================================
	//	Start of Broad Phase implmentation.
	//========================================================

		frameComponentReference = ConvertFromPtr(ANC::SceneMgr.GetEntitiesComponents<Collider2D, Transform_Component, RigidBody>());
		frameCollisionPairs = BroadPhaseCheck(frameComponentReference, broadPhaseInterval);

		for (auto& t : frameCollisionPairs)
		{	//for all collision pairs
			auto& [lhs, lhsTrans, lhsPhy] = frameComponentReference[t.first];	//find lhs

			for (auto& u : t.second)
			{	//of all unique set values for collision pair
				auto& [rhs, rhsTrans, rhsPhy] = frameComponentReference[u];		//find rhs

				//start collision checks, depending on if either objects are triggers
				if (lhs.isTrigger || rhs.isTrigger) {	//if either object is a trigger

					if (dcs.DetermineCollision(lhs, lhsTrans, rhs, rhsTrans)) {
						//If collision was detected for both colliders.
						lhs.collisionData.insert(&rhs);
						rhs.collisionData.insert(&lhs);
						collisionData.emplace(&lhs);
						collisionData.emplace(&rhs);

#ifdef SAMPLE_TRIGGER
						ANC_Debug::Log_Out a;
						for (auto const& t : lhs.collisionData)
						{
							a << " collided with: [" << t->GetEntityId() << "] & [" << lhs.GetEntityId() << "]\n";

						}

						if (lhs->isTrigger && lhs->onCollision) lhs->onCollision(*rhs);
						if (rhs->isTrigger && rhs->onCollision) rhs->onCollision(*lhs);
#endif
					}
				}
				//check collisions and apply response for collision pairs only
				else if(dcs.DetermineCollisionAndResponse(lhs, lhsTrans, lhsPhy,
					rhs, rhsTrans, rhsPhy))
				{
					//update the new values into the actual component ;;
					//lhsTrans.position = lhsTrans.position;
					//rhsTrans.position = rhsTrans.position;

					//lhsPhy.velocity = lhsPhy.getVel();
					//rhsPhy.velocity = rhsPhy.getVel();

					lhs.collisionData.insert(&rhs);
					rhs.collisionData.insert(&lhs);
					collisionData.emplace(&lhs);
					collisionData.emplace(&rhs);

					//do call back
					if (lhs.onCollision) lhs.onCollision(rhs);
					if (rhs.onCollision) rhs.onCollision(lhs);
#ifdef _DEBUG	//only print if debug mode build
					if (lhs.onCollision)
					{
						ANC_Debug::Debug.Log("Collision\n");
					}
#endif
				}
			}
		}
#endif
	}

	void Update() override {
		#ifndef _PACKAGE
		if (SHOW_EDITOR) {
			if (DEBUG_MODE) {
				// Draw coliders.
				DrawColliders();
			}
			else if (SHOW_COLLIDERS && ANC::Editor.GetSelectedEntity() != INVALID_ID) {
				// Draw selelected entity collider.
				Collider2D* collider = entityMgr.GetComponent<Collider2D>(ANC::Editor.GetSelectedEntity());
				Transform_Component* transform = entityMgr.GetComponent<Transform_Component>(ANC::Editor.GetSelectedEntity());
				if (collider && transform) { DrawCollider(collider, transform); }
			}
		}
		#endif	//_package
	}
	
	void DrawColliders() {
		for (EntityID const entity : GetEntities()) {
			Collider2D* collider = entityMgr.GetComponent<Collider2D>(entity);
			Transform_Component* transform = entityMgr.GetComponent<Transform_Component>(entity);

			if (collider && transform) {
				DrawCollider(collider, transform);
			}
		}
	}

	//=============================================================================
	//\brief
	//		Draw collider.
	// \param [out] collider
	//		Collider component of entity.
	// \param [out] transform
	//		Transform component of entity.
	//=============================================================================
	void DrawCollider(Collider2D* collider, Transform_Component* transform) {
		switch (collider->GetType()) {
			case COLLIDER_TYPE::RECT:
			{
				// Get the 4 points of a rect.
				Math::Vec3 point0{ transform->position.x - collider->offset.rect.half.x, transform->position.y + collider->offset.rect.half.y, 0 };
				Math::Vec3 point1{ transform->position.x + collider->offset.rect.half.x, transform->position.y + collider->offset.rect.half.y, 0 };
				Math::Vec3 point2{ transform->position.x + collider->offset.rect.half.x, transform->position.y - collider->offset.rect.half.y, 0 };
				Math::Vec3 point3{ transform->position.x - collider->offset.rect.half.x, transform->position.y - collider->offset.rect.half.y, 0 };

				// Draw lines to connect the corners of the rectangle.
				ANC::Graphics.DrawLine(point0, point1, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
				ANC::Graphics.DrawLine(point1, point2, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
				ANC::Graphics.DrawLine(point2, point3, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
				ANC::Graphics.DrawLine(point3, point0, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);

				break;
			}
			case COLLIDER_TYPE::CIRCLE:
			{
				static size_t SEGMENTS = 20;
				float angleIncrement = 2 * Math::ANC_PI_F / SEGMENTS;

				Math::Vec3 point0{};
				Math::Vec3 point1{};

				for (int i = 0; i < SEGMENTS; ++i) {
					// Calculate the starting and ending points of the line segment.
					point0.x = transform->position.x + collider->offset.circle.m_center.x + collider->offset.circle.m_radius * cosf(i * angleIncrement);
					point0.y = transform->position.y + collider->offset.circle.m_center.y + collider->offset.circle.m_radius * sinf(i * angleIncrement);
					point1.x = transform->position.x + collider->offset.circle.m_center.x + collider->offset.circle.m_radius * cosf((i + 1) * angleIncrement);
					point1.y = transform->position.y + collider->offset.circle.m_center.y + collider->offset.circle.m_radius * sinf((i + 1) * angleIncrement);

					// Draw a line segment between two points.
					ANC::Graphics.DrawLine(point0, point1, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
				}
				break;
			}
			case COLLIDER_TYPE::LINE:
			{
				ANC::Graphics.DrawLine(transform->position + collider->offset.line.m_pt0, transform->position + collider->offset.line.m_pt1, 1, { 0,1.0f,0 }, SORTING_LAYER::DEBUG);
				ANC::Graphics.DrawLine(transform->position + collider->offset.line.m_pt0
					, transform->position + collider->offset.line.m_pt0 + collider->offset.line.m_normal
					, 1, { 1.f,1.0f,0 }
					, SORTING_LAYER::DEBUG);
				break;
			}
		}

	}
};

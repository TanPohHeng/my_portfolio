/******************************************************************************/
/*!
\file		PhysicsECS.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	October 13, 2023
\brief		Source file containing physics and Forces functions definition.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "Pch.h"
#include "PhysicsECS.h"
#include "Engine/ANC_Graphics.h"
#include <utility>
#include <algorithm>

#include "ECS/EntityManager.h"

#include "Engine/ANC_Timer.h"
#include "Engine/SceneManager.h"
#include "Engine/ANC_Graphics.h"
#include "Utils/Logger.h"

#include "Engine/Global.h"

namespace
{
	//epsilon for floating point 0 checking
	constexpr f64 EPSILON = 0.0001f;
	constexpr double EPSILOND = 0.0001;
}

namespace ANC
{
namespace ECS 
{
//========================================================
//Start of Physics_Component definitions
//========================================================

	//========================================================
	//	\brief
	//		Constructor for Physics_Component.
	//========================================================
	Physics_Component::Physics_Component(Math::Vec3 const& velocityVal, ::f64 massVal, ::f64 corPercent, bool const& bFlag):
		isStatic{ bFlag }, mass{ massVal }, velocity{ velocityVal }, cor{ corPercent }, fm{ Force_Manager() }, c_velocity{velocity}
	{/*Empty by design*/}

	//========================================================
	//	\brief
	//		Constructor for Physics_Component. only sets static flag
	//		all other values are default initialized
	//========================================================
	Physics_Component::Physics_Component(bool const& bFlag):
		isStatic{ bFlag }, mass{f64()}, velocity{Math::Vec2()}, cor{f64()}, fm{Force_Manager()}, c_velocity{ velocity }
	{/*Empty by design*/}

	//========================================================
	//	\brief
	//		Constructor for Physics_Component. 
	//		only sets static flag & mass
	//		all other values are default initialized
	//========================================================
	Physics_Component::Physics_Component(f64 dMass, bool const& bFlag):
		isStatic{ bFlag }, mass{ dMass }, velocity{ Math::Vec2() }, cor{ f64() }, fm{ Force_Manager() }, c_velocity{ velocity }
	{/*Empty by design*/}

	/*Doxygen for each getter and setters TODO*/
	//========================================================
	//	\brief
	//		getter for each data stored in 
	//		the Physics_Component class
	//  \return
	//		TODO
	//========================================================
	bool Physics_Component::getIsStatic() const noexcept { return isStatic; }
	const Math::Vec3& Physics_Component::getVel() const noexcept { return velocity; }
	Math::Vec3 Physics_Component::getVel() noexcept { return velocity; }
	::f64 Physics_Component::getMass() const noexcept { return mass; }
	Force_Manager& Physics_Component::getFM() noexcept { return fm; }
	const Force_Manager& Physics_Component::getFM() const noexcept { return fm; }
	::f64 Physics_Component::getCOR()const noexcept { return cor; }
	
	Physics_Component& Physics_Component::setIsStatic(bool bFlag) noexcept { isStatic = bFlag; return *this; }
	Physics_Component& Physics_Component::setVel(Math::Vec3 const& newVel) noexcept { velocity = newVel; return *this; }
	Physics_Component& Physics_Component::setMass(::f64 newMass) noexcept { mass = newMass; return *this; }
	Physics_Component& Physics_Component::setFM(Force_Manager const& newFM) noexcept { fm = newFM; return *this; }
	Physics_Component& Physics_Component::setCOR(::f64 newCor) noexcept { cor = newCor; return *this; }

	//warning potential point of error vvv
	Physics_Component& Physics_Component::setVel(Math::Vec3&& newVel) noexcept { velocity = std::move(newVel); return *this; }
	Physics_Component& Physics_Component::setFM(Force_Manager&& newFM) noexcept { fm = std::move(newFM); return *this; }

	//Legacy Functions
	void Physics_Component::Reset() {velocity = c_velocity;}

//========================================================
//End of Physics_Component definitions
//========================================================

//========================================================
//Start of Physics_System definitions
//========================================================
	//========================================================
	//\brief
	//	getter for gravity set in the Physics_System
	//	gravitational accelaration is applied on all 
	//	Physics_Components Updated by this System Object.
	//\return
	//	the current gravity value(gravitational acceleration
	//	over one full second)
	//========================================================
	f64 Physics_System::getGravity() const noexcept { return gravity; }

	//========================================================
	//\brief
	//	setter for gravity set in the Physics_System
	//	gravitational accelaration is applied on all 
	//	Physics_Components Updated by this System Object.
	//\param [in] newVal
	//	the new gravity value(gravitational acceleration
	//	over one full second)
	//========================================================
	void Physics_System::setGravity(f64 newVal) noexcept { gravity = newVal; }
	//update for list and single
	// 
	//========================================================
	//\brief
	//	Update Physics_Components transforms depending on its
	//  current velocity. assumes translist is a list of index
	//	aligned transform_components to the Physics_Component.
	//\param [in/out] transList
	//	the vector of transforms index alligned to pList
	//\param [in/out] pList
	//	the vector of physics components index alligned to transList
	//\param [in] dt
	//	delta time.
	//========================================================
	void Physics_System::UpdateComponents(std::vector<Transform_Component>& transList ,std::vector<Physics_Component>& pList, f64 dt)
	{
		for (size_t i{}; i < pList.size(); ++i) 
		{
			UpdateComponent(transList[i], pList[i], dt);
		}
	}

	//new* update function. dependent on transform component.
	void Physics_System::UpdateComponent(Transform_Component& trans, Physics_Component& comp, f64 dt)
	{	//set the velocity to the velocity plus the delta time by gravity and accleration depending on if the object is static
		comp.setVel((comp.getIsStatic()) ? comp.getVel()
			: comp.getVel() + (Math::Vec3{ 0,(f64)(-gravity),0 } 
				+ comp.getFM().GetAccleration(comp.getMass())) * dt);
		trans.position += (comp.getVel() * static_cast<f64>(dt));
		//Update force manager. update after to ensure all forces added are used for atleast 1 frame.
		comp.getFM().Update(dt);
	}

	void Physics_System::DrawDebugVector(std::vector<Transform_Component>& transList, std::vector<Physics_Component>& pList, f64 dt)
	{
		for (size_t i{}; i < pList.size(); ++i)
		{// draw a line describing direction of velocity
			DrawDebugVector(transList[i], pList[i], dt);
		}
	}

	void Physics_System::DrawDebugVector(Transform_Component& t,Physics_Component& p, f64 dt)
	{	//Draw a line in direction of the velocity
		ANC::Graphics.DrawLine(t.position, t.position + (p.getVel()*dt), 1, {0.0,1.0,0.0});
	}
	
	void Physics_System::AccelerateComponent(Physics_Component& comp, Math::Vec3 additionalVelocity)
	{
		comp.setVel( comp.getVel() + additionalVelocity);
	}
	
	void Physics_System::ModifyComponentVelocity(Physics_Component& comp, Math::Vec3 newVelocity)
	{
		comp.setVel( newVelocity );
	}

	void Physics_System::AddForceToComponent(Physics_Component& comp, Force const& f) 
	{
		comp.getFM().AddForce(f);
	}

	//========================================================
	//	Legacy support functions
	//========================================================

	void Physics_System::FixedUpdate() {
		for (EntityID const entityId : GetEntities()) {
			//float dt = ANC::Time.DeltaTime();
			// Get components of entity,
			Transform_Component* transform = entityMgr.GetComponent<Transform_Component>(entityId);
			Physics_Component* rigidBody = entityMgr.GetComponent<Physics_Component>(entityId);

			// Update rigidbody velocity.
			rigidBody->velocity = (rigidBody->isStatic) ? rigidBody->velocity : rigidBody->velocity + Math::Vec2{ 0.0f, -GRAVITY * rigidBody->gravityScale * ANC::Time.FixedDeltaTime() };

			// Update position based on velocity.
			transform->position += (rigidBody->velocity * ANC::Time.FixedDeltaTime());

			if (DEBUG_MODE && SHOW_EDITOR) { DrawDebugVector(transform, rigidBody, ANC::Time.FixedDeltaTime()); }
		}
	}

	void Physics_System::DrawDebugVector(Transform_Component* t, Physics_Component* rb, float const dt)
	{	//Draw a line in direction of the velocity
		ANC::Graphics.DrawLine(t->position, t->position + (rb->getVel() * dt), 1, { 0.0,1.0,0.0 });
	}

//========================================================
//End of Physics_System definitions
//========================================================

//========================================================
//Start of Force definitions
//========================================================

	Force::Force(Math::Vec2 const& vec, ::f64 duration, ::f64 decayRate) noexcept 
		: lifetime{ duration }, decay{ decayRate }, force{ vec }
	{/*Empty by design*/ }

	Force::Force(Math::Vec2&& vec, ::f64 duration, ::f64 decayRate) noexcept
		: lifetime{ duration }, decay{ decayRate }, force{std::move(vec)}
	{/*Empty by design*/ }

//========================================================
//End of Force definitions
//========================================================

//========================================================
//Start of Force Manager definitions
//========================================================
	Force_Manager& Force_Manager::AddForce(Force const& force) 
	{	//add the force to our vector. return Force Manager object.
		forces.push_back(force);
		return *this;
	}

	Force_Manager& Force_Manager::AddForce(Force&& force)
	{	//add the force to our vector. return Force Manager object.
		forces.push_back(std::move(force));
		return *this;
	}
	
	Math::Vec2 Force_Manager::GetSumForces() const 
	{
		Math::Vec2 sumF {0.0f,0.0f};
		for (const Force f : forces)
		{	//forces are not proportional to their lifetimes
			sumF += f;
		}
		return sumF;
	}

	Math::Vec2 Force_Manager::GetSumForcesByLifetimes() const {
		Math::Vec2 sumF{ 0.0f,0.0f };
		for (const Force f : forces)
		{	//ensure forces added are proportional to their lifetimes
			sumF += (f.lifetime > 1.0f) ? f.force : f.force * f.lifetime;
		}
		return sumF;
	}

	Math::Vec2 Force_Manager::GetAccleration(::f64 mass) const
	{	//find total sum of forces, then find accleration based on F=mA
		if (mass == 0.0f) { return Math::Vec2(); }
		Math::Vec2 sumF{GetSumForces()};
		sumF /= mass;		// A = F/m
		return sumF;
	}

	void Force_Manager::Update(::f64 dt) 
	{
		for (std::vector<Force>::size_type i{ 0 }; i < forces.size(); ++i)
		{
			//reduce lifetime
			forces[i].lifetime -= dt;

			if (forces[i].decay > EPSILON)
			{	//apply decay
				forces[i].force -= forces[i].force * forces[i].decay * dt;
			}	

			//if the force is negligible or expired, move it to the last force, and pop it from the vector
			if (Math::lengthSquared(forces[i])<(EPSILON*EPSILON) ||
				forces[i].lifetime < EPSILON)
			{
				if (i == forces.size() - 1)
				{//if already at the last force, 
					forces.pop_back();
					continue;
				}
				//swap this force with the last force, then pop_back
				std::swap(forces[i],forces[forces.size()-1]);	
				forces.pop_back();
				--i;	//ensure the swapped element gets updated.
			}
		}
	}

	void Force_Manager::Reset() noexcept
	{	//clean reset for vector
		forces.clear();
	}
//========================================================
//End of Force Manager definitions
//========================================================
}//End of ECS namespace
}//End of ANC namespace
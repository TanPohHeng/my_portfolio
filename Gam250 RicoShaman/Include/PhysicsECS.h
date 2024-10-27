/******************************************************************************/
/*!
\file		PhysicsECS.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	October 13, 2023
\brief		Header file containing physics and Forces functions declarations 
			and interface.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once
#include "Pch.h"
#include "Maths/Geometric.h"

#include <vector>
#include "Transform.h"

#include "ECS/BaseComponent.h"
#include "ECS/BaseSystem.h"

namespace ANC
{
	namespace ECS
	{
#if 0
		//========================================================
		//	\class Base_Component
		//	\brief
		//		Basic Component class for ECS system.
		// 		allows for the checking of if a component is active.
		// 		This allows for object pooling without constant
		//		initialization and destruction. As of now, a temporary
		//		declaration.
		//========================================================
		class Base_Component
		{
		private:
			bool active;
		public:
			bool getActive() { return active; }		//get if the component is active
			void setActive(const bool bFlag = true) { active = bFlag; }	//set if the component is active
			operator bool() { return active; }		//substitude this class for a boolean of if the component is active
		};


		//reference for transform stuff.
		Physics_Component& Physics_Component::setRot(double angle) noexcept { rotInRad = angle; return *this; }
		Physics_Component& Physics_Component::setRotDegrees(double angle) noexcept { rotInRad = angle * Math::ANC_PI / 180; return *this; }

#endif

		//=============================================================================
		//\struct Force
		//\brief
		//	A force to be applied onto a Physics_Component. Force must have a lifetime
		//	and may decay over time. set lifetime to 0 for forces only active for one
		//	frame.
		//=============================================================================
		struct Force
		{
			::f64 lifetime;	//force with 0.0 lifetime will be removed after 1 update.
			::f64 decay;	//force reduction over time. in percent. within 0.0f to 1.0f
			Math::Vec2 force;
			//implicit conversion to Vec2
			operator Math::Vec2() const { return force; }
			//constructor
			//=============================================================================
			//\brief
			//	Force constructors
			//\param [in] vector
			//	Size of the force
			//\param [in] duration
			//	the duration of the force's lifetime
			//\param [in] decay rate
			//	the percentage of the force that will decay in 1 second
			//=============================================================================
			Force(Math::Vec2 const&, ::f64 = 0.0f, ::f64 = 0.0f) noexcept;
			Force(Math::Vec2&&, ::f64 = 0.0f, ::f64 = 0.0f) noexcept;
		};

		//=============================================================================
		//\class Force_Manager
		//\brief
		//	Handles the forces within a Physics_Component. acts like an isolated system
		//	of forces. Get functions return all forces applied in 1 second.
		//=============================================================================
		class Force_Manager
		{
		private:
			std::vector<Force> forces;	//unactive forces are automatically removed on update.
		public:
			Force_Manager& AddForce(Force const&);
			Force_Manager& AddForce(Force &&);
			Math::Vec2 GetSumForces() const;				// all forces are added without respect to their active lifetimes.
			Math::Vec2 GetSumForcesByLifetimes() const;		// all forces are proportional to their lifetimes. capped proportions to 1 second and below. Not reccomended
			Math::Vec2 GetAccleration(::f64 = 0.0) const;	//requires mass of object forces are applied to for F=mA
			void Reset() noexcept;
			void Update(::f64 dt);		//updates lifetimes of all forces, and their force decay. Epsilon or less magnitutde and expired forces are removed.
		};

		//=============================================================================
		//\class Physics_Component
		//\brief
		//	Physics Component of the Physics ECS Systems.
		//	Designed to contain data required for physics calculation in a physics
		//  System.
		//=============================================================================
		class Physics_Component: public BaseComponent
		{
		private:
		public:		//this accessor tag is to support legacy code accessing these hidden values as accessible. ideally this tag should not be required.
			bool isStatic;			//should this object be affected by gravity
			::f64 mass;				//mass of the object
			::f64 cor;				//coefficient of restitution. % of velocity retained per bounce 
			::f64 gravityScale {1.0f};	//scale of the acceleration gained from gravity per frame
			Math::Vec3 velocity;	//potentially update to vec 2. Operations to move objects should touch this value
			
			Force_Manager fm;		//all physics components shall have a force manager. non-gravity acclerations shall be calculated via this class
			//non-implemented variables
			//unsigned int weight;
		public:
			//friend systems?
			//default constructor
			Physics_Component(Math::Vec3 const& = { 0,0,0 }, ::f64 = 1.0f,::f64 = 1.0, bool const& bFlag = false);
			Physics_Component(f64 mass, bool const& bFlag);
			Physics_Component(bool const& bFlag);

			//getters
			bool getIsStatic() const noexcept;			//no un-const since bool is already so small, no reason to use reference
			const Math::Vec3& getVel() const noexcept;
			Math::Vec3 getVel() noexcept;
			::f64 getMass() const noexcept;
			Force_Manager& getFM() noexcept;
			const Force_Manager& getFM() const noexcept;
			::f64 getCOR()const noexcept;

			//setters
			Physics_Component& setIsStatic( bool = false) noexcept;
			Physics_Component& setVel(Math::Vec3 const& newVel) noexcept;
			Physics_Component& setMass(::f64 newMass) noexcept;
			Physics_Component& setFM(Force_Manager const& newFM) noexcept;
			Physics_Component& setCOR(::f64 newCor) noexcept;
			
			//warning potential point of error vvv
			Physics_Component& setVel(Math::Vec3&& newVel) noexcept;
			Physics_Component& setFM(Force_Manager&& newFM) noexcept;

			//legacy support
			Math::Vec3 c_velocity;
			static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::RIGIDBODY;

			COMPONENT_TYPES const GetCompType() override {
				return compType;
			}

			void AddForce(Math::Vec2 additionalVelocity) {velocity += additionalVelocity;}
			void Reset() override;
		};


		//=============================================================================
		//\class Physics_System
		//\brief
		//	Physics System of the Physics ECS Systems.
		//	Designed to contain operations required for physics calculation in a closed physics
		//  System. no external forces are exerted on any physics components from this 
		//	class outside of gravity and the component's own velocity.
		//=============================================================================
		class Physics_System : public BaseSystem
		{
		private:
			f64 gravity{9.81f};	//strength of the gravity applied by this Physics_System
		public:
			f64 getGravity() const noexcept;
			void setGravity(f64 newVal) noexcept;

			//update a component's force manager functions
			void AddForceToComponent(Physics_Component&, Force const& f);

			//update for list and single
			void AccelerateComponent(Physics_Component&, Math::Vec3 additionalVelocity);	//function assumes additionalVelocity is Accleration * dt
			void ModifyComponentVelocity(Physics_Component&, Math::Vec3 newVelocity);
			void UpdateComponents(std::vector<Transform_Component>&, std::vector<Physics_Component>&, f64 dt);
			void UpdateComponent(Transform_Component &,Physics_Component &, f64 dt);
			void DrawDebugVector(std::vector<Transform_Component>&, std::vector<Physics_Component>& componentList, f64 dt = 1.0f);
			void DrawDebugVector(Transform_Component& ,Physics_Component&, f64 dt = 1.0);

			//legacy support functions
			bool CanOperateOn(EntityID entityId) override {
				return (entityMgr.HasComponent<Transform_Component>(entityId)
					&& entityMgr.HasComponent<Physics_Component>(entityId));
			}

			//void Update() override;
			void FixedUpdate() override;

			void DrawDebugVector(Transform_Component* t, Physics_Component* rb, float const dt);
		};
		//legacy aliases
		using RigidBody = Physics_Component;
		using RigidbodySystem = Physics_System;
	}//End of ECS namespace
}//End of ANC namespace
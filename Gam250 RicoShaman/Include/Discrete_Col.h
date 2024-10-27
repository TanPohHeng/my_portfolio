/******************************************************************************/
/*!
\file		Discrete_Col.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	October 31, 2023
\brief		Header file containing discrete collision checks interface.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "CollisionECS.h"
#include "Maths/Math_Includes.h"
#include <optional>

namespace ANC 
{namespace ECS 
{
	//=============================================================================
	//\class Discrete_Collider_Component
	//\brief
	//	Collider Component to hold offsets required to find collider transform in
	//	respect to a transform Component
	//=============================================================================
	class Discrete_Collider_Component : public BaseComponent
	{
	private:

	public:	//legacy supported variables, if this accessor tag can be removed in the future, please remove it to maintain proper encapsulation.
		COLLIDER_TYPE type;		//type of the collider
		Collider offset;		//values of the collider's offset from the transform's position 
	

		using callBack = void (*)(Discrete_Collider_Component);	//alias for callBack function on collision
		callBack onCollision{ nullptr };		//function pointer for callbacks on collision
		bool isTrigger;
	
	public:
		//user defined constructors, due to union Collider not having a constructor.
		//=============================================================================
		//\brief
		//	default Constructor for Discrete_Collider_Component. creates a circle of 0
		//	radius at (0,0) in world position
		//=============================================================================
		Discrete_Collider_Component()
			: type{ COLLIDER_TYPE::CIRCLE }, offset{ Circle(0.0f,{0.0f,0.0f}) }, isTrigger{false}
		{/*Empty by design*/}

		//=============================================================================
		//\brief
		//	Constructor for Discrete_Collider_Component. creates a circle of 0
		//	radius at (0,0) in world position
		//\parma [in] t
		//	type of the collider passed in c
		//\param [in] c
		//	the collider of this object to be initialized with
		//=============================================================================
		Discrete_Collider_Component(COLLIDER_TYPE const& t, Collider const& c)
			: type{ t }, offset{ c }, isTrigger{false}
		{/*Empty by design*/ }

		//=============================================================================
		//\brief
		//	Constructor for Discrete_Collider_Component. creates a circle of 0
		//	radius at (0,0) in world position
		//\parma [in] t
		//	type of the collider passed in c
		//\param [in] c
		//	the collider of this object to be initialized with
		//\param [in] bflag
		//	If this collider is a trigger
		//=============================================================================
		Discrete_Collider_Component(COLLIDER_TYPE const& t, Collider const& c, bool bflag)
			: type{ t }, offset{ c }, isTrigger{ bflag }
		{/*Empty by design*/}
		
		//=============================================================================
		//\brief
		//	Constructor for Discrete_Collider_Component. creates a circle of 0
		//	radius at (0,0) in world position
		//\parma [in] t
		//	type of the collider passed in c
		//\param [in] c
		//	the collider of this object to be initialized with
		//\param [in] bflag
		//	If this collider is a trigger
		//\param [in] onCollision
		//	a call back function taking a collider and returning void that will run when
		//	the collider collideds with any other collider. 
		//=============================================================================
		Discrete_Collider_Component(COLLIDER_TYPE const& t, Collider const& c, bool bflag, callBack colFunc)
			: type{ t }, offset{ c }, isTrigger{bflag}, onCollision {colFunc}
		{/*Empty by design*/}

		//Getters for discrete colliders.
		COLLIDER_TYPE GetType() const { return type; }
		Collider& GetCollider() { return offset; }
		const Collider& GetCollider() const { return offset; }
		callBack GetOnCollisionFunc() { return onCollision; }
		const callBack GetOnCollisionFunc() const { return onCollision; }

		//legacy supported item
		std::set<Discrete_Collider_Component*> collisionData;
		static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::COLLIDER_2D;

		COMPONENT_TYPES const GetCompType() override {
			return compType;
		}

		//component's operating system
		friend class Discrete_Collider_System;

#ifdef _DEBUG
		//testing and debug function
		friend void testCollsionUpdate();
#endif // _DEBUG
	};
	//Class Alias for readability
	using ANC_DCC = Discrete_Collider_Component;
	using Collider2D = ANC_DCC;			//legacy alias

	//=============================================================================
	//\class Discrete_Collider_System
	//\brief
	//	Collider System to operate on data stored in the Discrete_Collider_Component
	//=============================================================================
	class Discrete_Collider_System 
	{	
	private:
		bool massBased{ false };
		bool useCOR{ false };
	public:
		//class Response_Info to return required response information 
		class DC_Response_Info 
		{
		public:
			//variants of required Response Informaation
			Math::Vec2 adjustmentVec;	//vector with adjustment for collider 1 to move out of collider 2 
			
			//vector redirectiom values
			Math::Vec2 normalOfCol;		//the normal of the reflection of the vector
		};

		//all collision check type event shall go here
		//=============================================================================
		//\brief
		//	Determines the Type of collision and runs that collision check.
		//	No Collision response is runned through this function.
		//\param [in/out] lhs
		//	the first collider to check collision with
		//\param [in] lhsTrans
		//  the Physics Component related to the first Collider Component.
		//\param [in/out] rhs
		//	the second collider to check collision with
		//\param [in] rhsTrans 
		//	the Physics Component related to the second Collider Component.
		//\return
		//	the results of the collision check.
		//=============================================================================
		bool DetermineCollision(ANC_DCC const& lhs, Transform_Component const& lhsTrans,
			ANC_DCC const& rhs, Transform_Component const& rhsTrans);

		//collision functions
		//=============================================================================
		//\brief 
		//	All Discrete Collision Check functions will follow this function signature. 
		//	Checks for collision between 2 colliders given 2 transforms.
		//\param [in] collider 1
		//	the collider of the first object. must be scoped to it's type.
		//\param [in] trans1
		//	the transform storing the position of the first object.
		//\param [in] collider 2
		//	the collider of the second object. must be scoped to it's type.
		//\param [in] trans2
		//	the transform storing the position of the second object.
		//\return
		//	if the collision between both component sets occured.TODO: return std::optional with response info.
		//=============================================================================
		bool D_Check_RectRect(AABB const&, Transform_Component const&,AABB const&, Transform_Component const&) const;
		bool D_Check_RectCircle(AABB const&, Transform_Component const&,Circle const&, Transform_Component const&) const;
		bool D_Check_RectLine(AABB const&, Transform_Component const&,LineSegment const&, Transform_Component const&) const;
		bool D_Check_CircleCircle(Circle const&, Transform_Component const&,Circle const&, Transform_Component const&) const;
		bool D_Check_CircleLine(Circle const&, Transform_Component const&,LineSegment const&, Transform_Component const&) const;


		//=============================================================================
		//\brief 
		//	All Discrete Collision functions will follow this function signature. 
		//	Checks for collision between 2 colliders given 2 transforms.
		//  All response information is for collider 1
		//\param [in] collider 1
		//	the collider of the first object. must be scoped to it's type.
		//\param [in] trans1
		//	the transform storing the position of the first object.
		//\param [in] collider 2
		//	the collider of the second object. must be scoped to it's type.
		//\param [in] trans2
		//	the transform storing the position of the second object.
		//\return
		//	std::optional with collision response info.
		//=============================================================================
		std::optional<DC_Response_Info> D_Collision_RectRect(AABB const&, Transform_Component const&, AABB const&, Transform_Component const&) const;
		std::optional<DC_Response_Info> D_Collision_RectCircle(AABB const&, Transform_Component const&, Circle const&, Transform_Component const&) const;
		std::optional<DC_Response_Info> D_Collision_CircleRect(Circle const&, Transform_Component const&, AABB const&, Transform_Component const&) const;
		std::optional<DC_Response_Info> D_Collision_RectLine(AABB const&, Transform_Component const&, LineSegment const&, Transform_Component const&) const;
		std::optional<DC_Response_Info> D_Collision_CircleCircle(Circle const&, Transform_Component const&, Circle const&, Transform_Component const&) const;
		std::optional<DC_Response_Info> D_Collision_CircleLine(Circle const&, Transform_Component const&, LineSegment const&, Transform_Component const&) const;

		//Response Functions
		//Update both items as if they are both dynamic objects
		//requires access to physics component for force check
		//first collider set passed is moved by response.
		//returns the adjusted position and the reflected velocity in that order.
		
		//=============================================================================
		//\brief 
		//	applies the response to the appropriate component. Pop moves the collider the
		//	smallest distance to remove the collider from the other collider. Reflection
		//	reflects the current vector of the physics component based on the normal of 
		//  the collision. 
		//\param [in] responseInfo
		//	reference response information to be applied.
		//\param [in] Component
		//	the component required for the response.
		//\return
		//	new vector after the response is applied
		//=============================================================================
		Math::Vec3 D_ApplyPop(DC_Response_Info const&, Transform_Component const&);
		Math::Vec2 D_ApplyReflection(DC_Response_Info const&, Physics_Component const&);
		
		//This function performs both of the above response functions. returns position and reflected velocity respectively
		std::pair<Math::Vec3, Math::Vec2> D_ApplyResponse(DC_Response_Info const&, Transform_Component const&, Physics_Component const&);
		
		//=============================================================================
		//\brief 
		//	Inverses the response information to apply to collider 2.
		//\param [in] responseInfo
		//	reference response information to be inversed
		//\return
		//	inversed collision response info.
		//=============================================================================
		DC_Response_Info inverseResponse(DC_Response_Info const&);
		DC_Response_Info inverseResponse(DC_Response_Info &&);

		//=============================================================================
		//\brief 
		//	Finds the proportion of the velcoity preserved after collision
		//\param [in] PhysicComp1
		//	first Physics component of the collision 
		//\param [in] PhysicComp2
		//	second Physics component of the collision 
		//\return
		//	pair of percent of velocity preserved after collision for first and second
		//	collider respectively
		//=============================================================================
		std::pair<f64,f64> MassBasedReflectionMagnitude( Physics_Component const&, Physics_Component const&);

		//=============================================================================
		//\brief
		//	Determines the Type of collision and runs that collision check.
		//	Collision response is applied afterwards if any collision has occured.
		//\param [in/out] lhs
		//	the first collider to check collision with
		//\param [in/out] lhsTrans
		//  the Transform Component related to the first Collider Component.
		//\param [in/out] lhsPhy
		//  the Physics Component related to the first Collider Component.
		//\param [in/out] rhs
		//	the second collider to check collision with
		//\param [in/out] rhsTrans 
		//	the Transform Component related to the second Collider Component.
		//\param [in/out] rhsPhy
		//  the Physics Component related to the second Collider Component.
		//\return
		//	the results of the collision check.
		//=============================================================================
		bool DetermineCollisionAndResponse(ANC_DCC& lhs, Transform_Component& lhsTrans, Physics_Component& lhsPhy, 
			ANC_DCC& rhs, Transform_Component& rhsTrans, Physics_Component& rhsPhy);


	//TODO: Init,Update,Destory functions (pending revision depending on further integration)
		
		//Discrete collider system settings getter and setters
		bool isCollisionMassBased();
		Discrete_Collider_System& SetMassBasedCollision(bool);
		bool isUsingCOR();
		Discrete_Collider_System& SetUsingCOR(bool);
	};
	//Class Alias for readability
	using ANC_DCS = Discrete_Collider_System;

#ifdef _DEBUG
	void testCollsionUpdate();	//debug/test function 
#endif // _DEBUG
}//end of ECS namespace
}//end of ANC namespace

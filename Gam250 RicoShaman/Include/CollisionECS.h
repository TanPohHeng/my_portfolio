/******************************************************************************/
/*!
\file		CollisionECS.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	September 05, 2023
\brief		Header file containing collider class types interface and definitions

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once

#ifndef ECS_COLLISION_H_
#define ECS_COLLISION_H_
#include "Pch.h"
#include "Maths/Geometric.h"
#include "PhysicsECS.h"
#include "Transform.h"

#include <type_traits>
#include <utility>
//GOAL: Only Collision Checks here. Response are secondary in Milestone one
namespace COL = ANC::ECS;

namespace ANC
{
	namespace ECS
	{
		//=============================================================================
		//\struct AABB
		//\brief
		//	Contains the values commonly required for AABB collision calculations
		//=============================================================================
		struct AABB
		{
			Math::Vec2	min;	//bottom left
			Math::Vec2	max;	//top right
			Math::Vec2 half;	//half width and height vector

			//=============================================================================
			//\brief
			//	Constructor for AABB. Depricated. use width and height based initializers
			//\param [in] bl
			//	the bottom left position of the Axis Aligned Bounding Box
			//\param [in] tr
			//	the top right position of the Axis Aligned Bounding Box
			//=============================================================================
			AABB(Math::Vec2 const& bl = { -0.5,-0.5 },
				 Math::Vec2 const& tr = { 0.5, 0.5 })
				: min{ bl }, max{ tr }, half{ (bl - tr) / 2 } 
			{	//get half width and height. abs to prevent shenanigans with signs
				half.x = fabs(half.x);
				half.y = fabs(half.y);
			}
			//=============================================================================
			//\brief
			//	Constructor for AABB. Depricated. use width and height based initializers
			//\param [in] c
			//	the center offset of the AABB values declared past this parameter
			//\param [in] bl
			//	the bottom left position of the Axis Aligned Bounding Box
			//\param [in] tr
			//	the top right position of the Axis Aligned Bounding Box
			//=============================================================================
			AABB(Math::Vec2 const& c ,
				Math::Vec2 const& tr ,
				Math::Vec2 const& bl ) 
				: min{ bl + c }, max{ tr + c }, half{ (bl - tr) / 2 } 
			{	//get half width and height. abs to prevent shenanigans with signs
				half.x = fabs(half.x);
				half.y = fabs(half.y);
			}

			//=============================================================================
			//\brief
			//	Constructor for AABB.
			//\param [in] c
			//	the center offset of the AABB values declared past this parameter
			//\param [in] w
			//	width of the AABB collider
			//\param [in] tr
			//	height of the AABB collider
			//=============================================================================
			AABB(Math::Vec2 const& c,
				f64 w,
				f64 h)
				: min{}, max{}, half{ w / 2.0f,h / 2.0f }
			{	//get half width and height. abs to prevent shenanigans with signs
				half.x = fabs(half.x);
				half.y = fabs(half.y);
				min = c - half;
				max = c + half;
			}
			//=============================================================================
			//\brief
			//	Constructor for AABB.
			//\param [in] w
			//	width of the AABB collider
			//\param [in] tr
			//	height of the AABB collider
			//=============================================================================
			AABB(f64 w,
				f64 h)
				: min{-w/2.0f,-h/2.0f}, max{ w / 2.0f,h / 2.0f }, half{max}
			{	//get half width and height. abs to prevent shenanigans with signs
				half.x = fabs(half.x);
				half.y = fabs(half.y);
			}
		};
		//=============================================================================
		//\brief
		//	Setter function for AABB.
		//\param [out] aabb 
		//	aabb collider to be set
		//\param [in] w
		//	width of the AABB collider
		//\param [in] h
		//	height of the AABB collider
		//\param [in] c
		//	the center offset of the AABB values declared past this parameter
		//\return
		//	this AABB object
		//=============================================================================
		AABB& setAABB(AABB& aabb, f64 w, f64 h, Math::Vec2 const& c = { 0,0 });

		//=============================================================================
		//\brief
		//	Setter function for AABB.
		//\param [out] aabb 
		//	aabb collider to be set
		//\param [in] half
		//	the half vector of the bottom left to the top right corner of the AABB
		//\param [in] c
		//	the center offset of the AABB values declared past this parameter
		//\return
		//	this AABB object
		//=============================================================================
		AABB& setAABB(AABB& aabb, Math::Vec2 const& h, Math::Vec2 const& c = { 0,0 });

		template<typename T>
		AABB& setAABB(AABB& aabb, T&& h, Math::Vec2 const& c = { 0,0 });

		template<typename T>
		AABB& setAABB(AABB& aabb, T&& h, Math::Vec2 const& c)
		{// r-value version
			//if (!(std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(c)>>, T>))
			if constexpr (!(std::is_same_v<Math::Vec2, T>))
			{//if not of the same type as Math::Vec2
#ifdef _DEBUG
			ANC_Debug::Log_Out a;
				a <<"[" << typeid(std::remove_reference_t<decltype(c)>).name() << "] vs [" << typeid(T).name() << "]" 
					//<<((std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(c)>>, T>) ? "ya\n" : "NAH\n");
					<<((std::is_same_v<Math::Vec2, T>) ? "ya\n" : "NAH\n");
				a << "decltype h [" << typeid(decltype(h)).name() << "]\n";
				a << "is r-value? decltype h [" << std::is_rvalue_reference_v<decltype(h)> << "]\n";
#endif // _DEBUG
				return aabb;
			}
			else
			aabb.half = std::forward<decltype(h)>(h);
			aabb.min = c - aabb.half;
			aabb.max = c + aabb.half;
			return aabb;
		}


		//=============================================================================
		//\struct Circle
		//\brief
		//	Contains the values commonly required for Circle collision calculations
		//=============================================================================
		struct Circle
		{
			f64	m_radius;		//radius of the circle
			Math::Vec2  m_center;	//the center offset of the cirle
			//=============================================================================
			//\brief
			//	default Constructor for Circle colliders. makes a circle of radius 1 at (0,0)
			//	world position.
			//=============================================================================
			Circle() :m_radius{ 1.0f }, m_center{0.0f,0.0f} {/*Empty by Design*/ };
			//=============================================================================
			//\brief
			//	Constructor for Circle colliders. makes a circle of radius 1 at (0,0)
			//	world position.
			//\param [in] r
			//	the radius of the circle
			//\param [in] c
			//  the Vector 2D representing the center of the circle to offset
			//=============================================================================
			Circle( f64 r, Math::Vec2 const& c) :m_radius{ r }, m_center{c} {/*Empty by Design*/ };
		};
		struct LineSegment
		{
			Math::Vec2	m_pt0;
			Math::Vec2	m_pt1;
			Math::Vec2	m_normal;
			//=============================================================================
			//\brief
			//	default Constructor for Line colliders. makes a line of 0 width at (0,0)
			//=============================================================================
			LineSegment() = default;
			//=============================================================================
			//\brief
			//	Constructor for Line colliders. creates the normal based on start and end of
			//	line passed into the parameters
			//\parma [in] p0
			//	position of the start of the line
			//\param [in] p1
			//	position of the end of the line
			//=============================================================================
			LineSegment (const Math::Vec2& p0, const Math::Vec2& p1)
			{
				// assign the line segment's start and end points
				m_pt0 = p0;
				m_pt1 = p1;
				//find the vector of the line segment
				Math::Vec2 p0p1 = p1 - p0;
				//find normalized normal of p0p1
				m_normal = Math::normalize(Math::Vec2{ p0p1.y, -p0p1.x });
			}
		};


		void BuildLineSegment(COL::LineSegment& lineSegment,								//Line segment reference - input
			const Math::Vec2& p0,									//Point P0 - input
			const Math::Vec2& p1);									//Point P1 - input

		//=============================================================================
		//\brief
		//	Enum class representing the types of colliders
		//=============================================================================
		enum class COLLIDER_TYPE{
			RECT,		//AABB
			CIRCLE,		//Circle
			LINE		//Line Segment
		};
		//=============================================================================
		//\brief
		//	Collider Union to be used to streamline all Colliders into one class variable
		//=============================================================================
		union Collider
		{	//sub class level union
			AABB rect;				//potentially dynamic
			Circle circle;			//potentially dynamic
			LineSegment line;		//take note, line is always static
		public:
			//update when optimizing later
			Collider(AABB const& r) { rect = r; }
			Collider(Circle const& c) { circle = c; }
			Collider(LineSegment const& l) { line = l; }

			//update when understanding of && is better.
			Collider(AABB&& r) { rect = std::move(r); }
			Collider(Circle&& c) { circle = std::move(c); }
			Collider(LineSegment&& l) { line = std::move(l); }
		};
#if 0	//Continous collision is not a piority due to discrete collision implementation.
		//if Physics_Component is Static, this component will never check collision against other static items (unimplemented)
		//=============================================================================
		//\class Collider_Component
		//\brief
		//	Collider Component to hold offsets required to find collider values
		//=============================================================================
		class Collider_Component
		{
		private:
			Collider_Type type;	//type of collider
			Collider collider;	//collider, to be updated with transform
			Collider offset;	//offset from transform
		public:
			friend class Collider_System;
			Collider_Component(Collider_Type t, Collider const& c) : type{ t }, collider{ c }, offset{ collider }
			{}
			
			//Debug items
			Collider_Component() : type{ Collider_Type::RECT }, collider{ AABB{Math::Vec2(0,0), Math::Vec2(0,0)} }, offset{ collider } {}
			//friend void testDraw(Physics_Component&, Collider_Component&, Math::Vec3);
			//friend void testInit();
			//friend Collider Funnel(Physics_Component const& p, Collider_Component const& c);
		};


		//=============================================================================
		//\class Collider_System
		//\brief
		//	Collider System of the Collider ECS Systems.
		//	Designed to contain operations required for collision detection and response
		//	Updating of a Collider component is dependent on a paired Physic_Component
		//  for a Parent transform.
		//=============================================================================
		class Collider_System 
		{
		public:
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
			/*bool DetermineCollisionType(Collider_Component& lhs, Transform_Component& lhsTrans,
				Collider_Component& rhs, Transform_Component& rhsTrans, f64 dt);*/
			//all Collision shall be determined then executed by above functions in the Update
			//=============================================================================
			//\brief
			//	Determines the Type of collision and runs that collision check.
			//	Applys the Appropriate Responses for that collision check.(unimplemented, M2 goal)
			//\param [in/out] lhs
			//	the first collider to check collision with
			//\param [in] lhsTrans
			//  the Physics Component related to the first Collider Component.
			//\param [in/out] rhs
			//	the second collider to check collision with
			//\param [in] rhsTrans 
			//	the Physics Component related to the second Collider Component.
			//=============================================================================
	/*		void Update(Collider_Component& lhs, Transform_Component& lhsTrans,
						Collider_Component& rhs, Transform_Component& rhsTrans, f64 dt);*/
			/*friend void testDraw(Physics_Component&,Collider_Component&, Math::Vec3);
			friend void testInit();*/
		};
		//test items

		void testDraw(Physics_Component&, Collider_Component&, Math::Vec3);
		static Transform_Component eArr[3];
		static Collider_Component cArr[3];
		static Collider_System cSys;
		static Physics_System pSys;

		void testCollsion();
		void testInit();
#endif
	}//End of ECS namespace
}//End of ANC namespace

#endif
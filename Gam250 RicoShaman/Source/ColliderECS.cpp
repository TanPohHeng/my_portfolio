/******************************************************************************/
/*!
\file		CollisionECS.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	September 05, 2023
\brief		Source file containing collider class function Sdefinitions

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "Pch.h"
#include "CollisionECS.h"
#include "Maths/Math_Includes.h"

namespace ANC
{
	namespace ECS
	{
		AABB& setAABB(AABB& aabb, f64 w, f64 h, Math::Vec2 const& c)
		{	//update AABB using width and height
			aabb.half = { w / 2.0f, h / 2.0f };
			aabb.min = c - aabb.half;
			aabb.max = c + aabb.half;
			return aabb;
		}

		AABB& setAABB(AABB& aabb, Math::Vec2 const& h, Math::Vec2 const& c)
		{
			aabb.half = h;
			aabb.min = c - aabb.half;
			aabb.max = c + aabb.half;
			return aabb;
		}

/******************************************************************************/
/*!
*	\brief
*		use 2 vector 2D to create a line segment object
	\param [out] lineSegment
		returned by reference line segment created
	\param [in] p0
		the starting point of the line segment
	\param [in] p1
		the ending point of the line segment
 */
 /******************************************************************************/
void BuildLineSegment(COL::LineSegment& lineSegment,
	const Math::Vec2& p0,
	const Math::Vec2& p1)
{
	// assign the line segment's start and end points
	lineSegment.m_pt0 = p0;
	lineSegment.m_pt1 = p1;
	//find the vector of the line segment
	Math::Vec2 p0p1 = p1 - p0;
	//find normalized normal of p0p1
	lineSegment.m_normal = Math::normalize(Math::Vec2{ p0p1.y, -p0p1.x });
}

	}//end of ECS namespace
}//end of ANC namespace

#if 0	//items in this file have not been refactored to match architecture updates to transform, physics and collider classes
namespace ANC
{
	namespace ECS 
	{	//all collision check type event shall go here

		//Funnel function will recreate a new Collider component for every new transform to be added.
		//Extremely ineffcient but will allow use of Collision function without updating their signature.
		//TODO: Refactor all collision functions to not require this function to exist. By MileStone2!
		Collider Funnel(Physics_Component const& p, Collider_Component const& c)
		{
			if (c.type == Collider_Type::RECT)
			{
				AABB temp{ c.collider.rect.min,c.collider.rect.max , p.getTrans() };
				return temp;
			}
			if (c.type == Collider_Type::CIRCLE)
			{
				Circle temp;
				temp.m_center = c.collider.circle.m_center;
				temp.m_radius = c.collider.circle.m_radius;
				return temp;
			}
			if (c.type == Collider_Type::LINE)
			{
				LineSegment temp;
				BuildLineSegment(temp, c.collider.line.m_pt0 + p.getTrans(), c.collider.line.m_pt1 + p.getTrans());
				return temp;
			}
		}


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
		bool Collider_System::DetermineCollisionType
		(Collider_Component& lhs, Physics_Component& lhsTrans,
		Collider_Component& rhs, Physics_Component& rhsTrans, f64 dt)
		{	//this will be used as long as copy constructor for Vec 2 from a Vec3 is not defined
			Math::Vec2 lhsVel , rhsVel;
			lhsVel = lhsTrans.getVel();
			rhsVel = rhsTrans.getVel();

			//dummy variables for circle collisions
			Math::Vec2 interPt, cNormal;
			float interTime;
			bool edgeCheck{ true };
			switch (lhs.type)
			{
			case Collider_Type::RECT:
				switch (rhs.type)
				{
				case Collider_Type::RECT:
					return CollisionIntersection_RectRect(Funnel(lhsTrans,lhs).rect,lhsVel, Funnel(rhsTrans, rhs).rect,rhsVel,dt);
				case Collider_Type::CIRCLE:
					return CollisionIntersection_RectCircle(Funnel(lhsTrans, lhs).rect, lhsVel, Funnel(rhsTrans, rhs).circle, rhsVel, dt);
				case Collider_Type::LINE:
					return CollisionIntersection_RectLine(Funnel(lhsTrans, lhs).rect, lhsVel, Funnel(rhsTrans, rhs).line, dt);
				default:
					ANC_Debug::Debug.Log("Collision between Unknown collider types");
					return 0;
				}
				break;
			case Collider_Type::CIRCLE:
				switch (rhs.type)
				{
				case Collider_Type::RECT:
					return CollisionIntersection_RectCircle(Funnel(rhsTrans, rhs).rect, rhsVel, Funnel(lhsTrans, lhs).circle, lhsVel, dt);
				case Collider_Type::CIRCLE:
					return CollisionIntersection_CircleCircle(Funnel(lhsTrans, lhs).circle, lhsVel, Funnel(rhsTrans, rhs).circle, rhsVel, dt);
				case Collider_Type::LINE:
					return CollisionIntersection_CircleLineSegment(Funnel(lhsTrans, lhs).circle, lhsVel * dt, Funnel(rhsTrans, rhs).line,
						interPt, cNormal, interTime, edgeCheck);
				default:
					ANC_Debug::Debug.Log("Collision between Unknown collider types");
					return 0;
				}
				break;
			case Collider_Type::LINE:
				switch (rhs.type)
				{
				case Collider_Type::RECT:
					return CollisionIntersection_RectLine(Funnel(rhsTrans, rhs).rect, rhsVel, Funnel(lhsTrans, lhs).line, dt);
				case Collider_Type::CIRCLE:
					return CollisionIntersection_CircleLineSegment(Funnel(rhsTrans, rhs).circle, rhsVel * dt, Funnel(lhsTrans, lhs).line,
						interPt, cNormal, interTime, edgeCheck);
				case Collider_Type::LINE:
					return 0;	//static types never collide
				default:
					ANC_Debug::Debug.Log("Collision between Unknown collider types");
					return 0;
				}
				break;
			default:
				ANC_Debug::Debug.Log("Collision between Unknown collider types");
				return 0;
			}
		}
		
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
		void Collider_System::Update
		(Collider_Component& lhs, Physics_Component& lhsTrans,
		Collider_Component& rhs, Physics_Component& rhsTrans, f64 dt) 
		{
			//assume static physics object are settled by the physics system to have 0,0 velocity
			if (DetermineCollisionType(lhs, lhsTrans, rhs, rhsTrans, dt)) 
			{
				//play collision response here (Milestone 2 requirement)
				ANC_Debug::Debug.Log("Ping for Collsion");
			}
		}

		void testDraw(Physics_Component& p, Collider_Component& c, Math::Vec3 clr)
		{
			switch (c.type)
			{
			case Collider_Type::RECT:
				ANC::Graphics.DrawRect(p.getTrans(), {2,2} , 0, clr);
				break;
			case Collider_Type::CIRCLE:
				ANC::Graphics.DrawCircle(p.getTrans(), c.collider.circle.m_radius, clr);
				break;
			case Collider_Type::LINE:
				ANC::Graphics.DrawLine(c.collider.line.m_pt0 + p.getTrans(), c.collider.line.m_pt1 + p.getTrans(), 0.5, clr);
				break;
			default:
				break;
			}
		}

		void testInit() 
		{
			for (int i{}; i < 3; ++i)
			{	//set test Physics component value
				eArr[i].setActive(true);
				eArr[i].setIsStatic(false);
				eArr[i].setTrans({ 0,(float)-i * 5,0 });
				eArr[i].setRot(0);
				eArr[i].setVel({ (float)i,0,0 });
			}
			eArr[2].setIsStatic(true);
			eArr[1].setIsStatic(true);

			COL::Collider circle{ COL::Circle(2,{0,0}) };
			COL::Collider rect{ COL::AABB({-1,5},{1,5}) };

			Math::Vec2 p0{ -5, -5 };
			Math::Vec2 p1{ 5, -5 };
			LineSegment ls;
			BuildLineSegment(ls, p0, p1);
			COL::Collider line{ ls };

			cArr[0] = { COL::Collider_Type::CIRCLE, circle };
			//cArr[0].type = Collider_Type::CIRCLE;
			cArr[1] = {COL::Collider_Type::CIRCLE, circle };
			cArr[2] = {COL::Collider_Type::LINE, line };
		}

		//test update here
		void testCollsion()
		{
			float dt = ANC::Time.DeltaTime()/10;
			ANC_Debug::Debug.Log("Collision Test: Step IN");
			ANC_Debug::Debug.Log("Default: trans 1{%f, %f} :: trans2 {%f, %f} :: trans3{%f,%f} \n",
			eArr[0].getTrans().x, eArr[0].getTrans().y,
			eArr[1].getTrans().x, eArr[1].getTrans().y,
			eArr[2].getTrans().x, eArr[2].getTrans().y);

			for (int i{}; i < 3; ++i)
			{	//Update using physics system.
				pSys.Update(eArr[i], dt);
				bool col{ false };
				for (int j{}; j < 3 ; ++j) 
				{
					if (j == i) { continue; }
					if (cSys.DetermineCollisionType(cArr[i], eArr[i], cArr[j], eArr[j], dt)) 
					{
						ANC_Debug::Debug.Log("collsion of trans(%d) and trans(%d)",i,j);
						col = true;
					}
				}
				if(col)
					testDraw(eArr[i], cArr[i], {0.1,0.11,1});
				else 
					testDraw(eArr[i], cArr[i], {0,0,0});
			}
			ANC_Debug::Debug.Log("trans 1{%f, %f} :: trans2 {%f, %f} :: trans3{%f,%f} ",
				eArr[0].getTrans().x, eArr[0].getTrans().y,
				eArr[1].getTrans().x, eArr[1].getTrans().y,
				eArr[2].getTrans().x, eArr[2].getTrans().y);
			ANC_Debug::Debug.Log("Collision Test: Step Out \n\n");
		}
	}
}
#endif
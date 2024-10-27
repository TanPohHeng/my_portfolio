/******************************************************************************/
/*!
\file		Discrete_Col.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	November 21, 2023
\brief		Source file containing discrete collision checks definitions 

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "Pch.h"
#include "Discrete_Col.h"
#include "Engine/ANC.h"
#include <utility>

namespace ANC
{
namespace ECS
{
	//all collision check type event shall go here
//=============================================================================
//\brief
//	Determines the Type of collision and runs that collision check.
//	No Collision response is runned through this function.
//\param [in/out] lhs
//	the first collider to check collision with
//\param [in] lhsTrans
//  the Transform Component related to the first Collider Component.
//\param [in/out] rhs
//	the second collider to check collision with
//\param [in] rhsTrans 
//	the Transform Component related to the second Collider Component.
//\return
//	the results of the collision check.
//=============================================================================
	bool ANC_DCS::DetermineCollision(ANC_DCC const& lhs, Transform_Component const& lhsTrans,
		ANC_DCC const& rhs, Transform_Component const& rhsTrans) 
	{	//call the appropriate collision function based on the types of both colliders
		switch (lhs.type)
		{	
		case COLLIDER_TYPE::RECT:	//lhs = rect
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				return D_Check_RectRect(lhs.offset.rect, lhsTrans, rhs.offset.rect, rhsTrans);
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				return D_Check_RectCircle(lhs.offset.rect, lhsTrans, rhs.offset.circle, rhsTrans);
			case COLLIDER_TYPE::LINE:	//rhs = line
				return D_Check_RectLine(lhs.offset.rect, lhsTrans, rhs.offset.line, rhsTrans);
			default:
#if _DEBUG 
				ANC_Debug::Debug.Log("Collision between Unknown collider types");
#endif
				return 0;
			}
			break;
		case COLLIDER_TYPE::CIRCLE:	//lhs = circle
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				return D_Check_RectCircle(rhs.offset.rect, rhsTrans, lhs.offset.circle, lhsTrans);
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				return D_Check_CircleCircle(lhs.offset.circle, lhsTrans, rhs.offset.circle, rhsTrans);
			case COLLIDER_TYPE::LINE:	//rhs = line
				return D_Check_CircleLine(lhs.offset.circle, lhsTrans, rhs.offset.line, rhsTrans);
			default:
#if _DEBUG
				ANC_Debug::Debug.Log("Collision between Unknown collider types");
#endif
				return 0;
			}
			break;
		case COLLIDER_TYPE::LINE:	//lhs = line
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				return D_Check_RectLine(rhs.offset.rect, rhsTrans, lhs.offset.line, lhsTrans);
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				return D_Check_CircleLine(rhs.offset.circle, rhsTrans, lhs.offset.line, lhsTrans);
			case COLLIDER_TYPE::LINE:	//rhs = line
				return 0;	//static types never collide
			default:
#if _DEBUG
				ANC_Debug::Debug.Log("Collision between Unknown collider types");
#endif
				return 0;
			}
			break;
		default:					//Unknown Collider.
#if _DEBUG
			ANC_Debug::Debug.Log("Collision between Unknown collider types");
#endif
			return 0;
		}
	}

	//collision functions
	//AABB-to-AABB discrete collision check. Does not support rotation. 
	bool ANC_DCS::D_Check_RectRect
	(AABB const& aabb1, Transform_Component const& trans1,
	AABB const& aabb2, Transform_Component const& trans2) const
	{	//return false when no collision, else is true
		Math::Vec2 r1Center{ (trans1.position + aabb1.min + aabb1.half) },
				r2Center{ (trans2.position + aabb2.min + aabb2.half) };

		return ((aabb1.min.x + r1Center.x) > (aabb2.max.x + r2Center.x) ||	//check no collision on left of AABB1
			(aabb1.max.y + r1Center.y) < (aabb2.min.y + r2Center.y) ||	//check no collision on top of AABB1
			(aabb1.max.x + r1Center.x) < (aabb2.min.x + r2Center.x) ||	//check no collision on right of AABB1
			(aabb1.min.y + r1Center.y) > (aabb2.max.y + r2Center.y))		//check no collision to the bottom of AABB1
			? false : true ;
	}

	//AABB-to-Circle discrete collision check. Does not support rotation. 
	bool ANC_DCS::D_Check_RectCircle
	(AABB const& rect, Transform_Component const& trans1, 
	Circle const& circle, Transform_Component const& trans2) const
	{
		//dbc = absolute distance between colliders 
		Math::Vec2 dbc{(trans1.position + rect.min + rect.half)-trans2.position + circle.m_center};
		dbc.x = fabs(dbc.x);
		dbc.y = fabs(dbc.y);

		if (dbc.x > (rect.half.x + circle.m_radius) || dbc.y > (rect.half.y + circle.m_radius)) 
		{return false;}	//too far from center to collide
		if (dbc.x < rect.half.x || dbc.y < rect.half.y) 
		{return true;}	//inside of the rect. must have had collision
		//if the circle is within 1 radus of the edges of the AABB
		//dbcmm = squared distance between colliders magnitude, minus the half width/height of the AABB.
		f64 dbcmm = Math::lengthSquared(dbc - rect.half);

		// if this is below radius squared, both colliders should be colliding. 
		// this only works because our dbcmm is non-directional.
		return (dbcmm > (circle.m_radius * circle.m_radius)) ? false : true;
	}

	//AABB-to-Line discrete collision check. Line can be rotated via setting 
	//it's starting and ending points 
	bool ANC_DCS::D_Check_RectLine
	(AABB const& rect, Transform_Component const& trans1,
	LineSegment const& line, Transform_Component const& trans2) const
	{
		Math::Vec2 rectCenter{ (trans1.position + rect.min + rect.half) };
		//time based.
		Math::Vec2 lineDelta{ (trans2.position + line.m_pt1) - (trans2.position + line.m_pt0)};
		f64 signX{(lineDelta.x < 0)? -1.0f : 1.0f },
			signY{ (lineDelta.y < 0) ? -1.0f : 1.0f };
		//get near and far times (to reach p0) 
		//find largest neartime
		f64 nearTimeX = (trans1.position.x - (signX * rect.half.x) 
						- (trans2.position.x + line.m_pt0.x + rectCenter.x)) / lineDelta.x;	//percent line delta to reach closest x axis intersection.
		f64 nearTimeY = (trans1.position.y - (signY * rect.half.y) 
						- (trans2.position.y + line.m_pt0.y + rectCenter.y))/ lineDelta.y;		//percent line delta to reach closest y axis intersection.
		f64 nearTime = (nearTimeX > nearTimeY) ? nearTimeX : nearTimeY;		
		
		//find smallest fartime
		f64 farTimeX = (trans1.position.x + (signX * rect.half.x) 
						- (trans2.position.x + line.m_pt0.x + rectCenter.x)) / lineDelta.x;	//percent line delta to reach furtherst x axis intersection.
		f64 farTimeY = (trans1.position.y + (signY * rect.half.y) 
						- (trans2.position.y + line.m_pt0.y + rectCenter.y)) / lineDelta.y;	//percent line delta to reach furtherst y axis intersection.
		f64 farTime = (farTimeX < farTimeY) ? farTimeX : farTimeY;				

		//if the closest intersection time in one axis is larger than the furtherest intersection time of the opposite axis, no possible collisions.
		if (nearTime > farTime)
		{ return false;}

		//if the near time is more than line's axis delta, or less than 0 (moving in wrong direction of line delta), no collision possible
		return (nearTime >= 1 || farTime <= 0)? false : true;
	}

	//Circle-to-Circle discrete collision check. 
	bool ANC_DCS::D_Check_CircleCircle
	(Circle const& c1, Transform_Component const& trans1,
		Circle const& c2, Transform_Component const& trans2) const
	{	//Do classic squared distance from center check between circles. 
		// If it is less than the squared additions of both circle radius, it must be not colliding, else it must be colliding
		//sdcb = the squared distance between both collider centers.
		f64 sdcb = Math::lengthSquared(trans2.position + c1.m_center - trans1.position + c2.m_center);
		return (sdcb > (c1.m_radius+c2.m_radius)* (c1.m_radius + c2.m_radius)) ? false : true ;
	}

	//Circle-to-Line discrete collision check. Line can be rotated via setting 
	//it's starting and ending points 
	bool ANC_DCS::D_Check_CircleLine
	(Circle const& circle, Transform_Component const& trans1, 
		LineSegment const& line, Transform_Component const& trans2) const
	{
		// cp0 = circle to p0 on line
		Math::Vec2 cp0{ trans2.position + line.m_pt0 - trans1.position +  circle.m_center };
			
		//use np0 to get perpendicular distance from circle to infinte extension of line. 
		//if it is more than the radius, circle will not collide.
		f64 shortestDist = Math::dot(line.m_normal, cp0);
		if (fabs(shortestDist) > circle.m_radius)
		{	//if too far from infinite extension of line, cannot be colliding
			return false;
		}

		// cp1 = circle to p1 on line
		Math::Vec2 cp1{ trans2.position + line.m_pt1 - trans1.position + circle.m_center };
		
		//is the circle at the line edge?
		if (Math::lengthSquared(cp0) <= circle.m_radius * circle.m_radius 
			|| Math::lengthSquared(cp1) <= circle.m_radius * circle.m_radius) 
		{ return true; }

		//return true if the signs of cp0 and cp1 are opposite for each axis.
		return ( Math::dot(cp0,cp1) < 0 ) ? true : false ;
	}

#if 0
	//Debuging test function.
	void testCollsionUpdate()
	{//initialization
		static bool firstLoop{true};
		static Transform_Component tArr[8];
		static Physics_Component pArr[8];
		static ANC_DCC dcArr[8]
		{ {COLLIDER_TYPE::CIRCLE , Circle(1.0f,{0,0})},
			{COLLIDER_TYPE::RECT , AABB({0,0},5,2)},
			{COLLIDER_TYPE::RECT , AABB({0,0},5,2)},
			{COLLIDER_TYPE::CIRCLE , Circle(1.0f,{0,0})},
			//
			{COLLIDER_TYPE::LINE , LineSegment()},	//bottom
			{COLLIDER_TYPE::LINE , LineSegment()},	//right
			{COLLIDER_TYPE::LINE , LineSegment()},	//top
			{COLLIDER_TYPE::LINE , LineSegment()}	//left
			////, 
			//{COLLIDER_TYPE::RECT , AABB({0,0},20,1)},	//top
			//{COLLIDER_TYPE::RECT , AABB({0,0},1,20)},	//right
			//{COLLIDER_TYPE::RECT , AABB({0,0},20,1)},	//bottom
			//{COLLIDER_TYPE::RECT , AABB({0,0},1,20)}	//left
		};
		static ANC_DCS dcSys;
		static Physics_System pSys;
		static int frame;
		f64 dt;

		dt = (SceneMgr.IsPaused()) ? 0.0f : ANC::Time.DeltaTime() / 2.0f;

		dcSys.SetMassBasedCollision(false);
		dcSys.SetUsingCOR(false);
		pSys.setGravity(0.0f);
		if (firstLoop) 
		{
			frame = 0;
			BuildLineSegment(dcArr[4].offset.line,  {10,-8},{ -10,-10 });
			BuildLineSegment(dcArr[7].offset.line, { 10,-10 }, { 10,10 });
			BuildLineSegment(dcArr[6].offset.line, { -10,10 }, { 10,10 });
			BuildLineSegment(dcArr[5].offset.line, { -10,10 }, { -10,-10 });
			for (int i{};i<6;++i)
			{
				tArr[i].position = Math::Vec3{(f64)0,0,0};		//init pos here!
			}//pArr[1].setIsStatic(true);
			//pArr[2].setIsStatic(true);
			pArr[4].setIsStatic(true);
			pArr[5].setIsStatic(true);
			pArr[6].setIsStatic(true);
			pArr[7].setIsStatic(true);

			//tArr[4].position += {0, -5, 0};
			//tArr[5].position += {10, 0, 0};
			//tArr[6].position += {0, -10, 0};
			//tArr[7].position += {-10, 0, 0};
/**/
			tArr[0].position += {4,-4.5,0};
			pArr->setVel({ 0,-4.f,0 });
			pArr->setCOR(0.85f);
			(pArr + 1)->setCOR(0.85f); (pArr + 1)->setVel({ -12.0f,1.0f,0 });
			(pArr+2)->setVel({ -1.0f,1.0f,0 });(pArr+3)->setVel({ -12.6f,0,0 });
			tArr[1].position += {4, 3, 0};
			tArr[2].position += {4, 0.5f, 0};
			tArr[3].position += {4, -6.5, 0};
			
		}

		firstLoop = false;
		frame++;
	//Updating
		//ANC_Debug::Debug.Log("Current Frame[%d]", frame);
		for (int i{}; i < 8; ++i)
		{	//Update using physics system.
			pSys.UpdateComponent(tArr[i], pArr[i], dt);
		}
		for (int i{}; i < 8; ++i)
		{//do collision checks
			bool col{ false };
			for (int j{}; j < 8; ++j)
			{
				//if (i != 1) { continue; }	//only one collision pair
				if (j == i) { continue; }
				//ANC_Debug::Debug.Log("between items at index [%d] and [%d]",i,j);
				if (dcSys.DetermineCollisionAndResponse(dcArr[i], tArr[i],pArr[i], dcArr[j], tArr[j],pArr[j]))
				{
					ANC_Debug::Debug.Log("collsion of trans(%d) and trans(%d) at Frame[%d]", i, j,frame);
					col = true;
				}
			}
			Math::Vec3 clr{};
			//setting render colors
			if (col)
				clr = { 0.1f,0.11f,1.0f};
			else
				clr = { i*0.12f,0,0 };
			//Drawing within loop since there isn't a collision response.
			switch (dcArr[i].type)
			{
			case COLLIDER_TYPE::RECT:
				ANC::Graphics.DrawRect(tArr[i].position + dcArr[i].offset.rect.min + dcArr[i].offset.rect.half, dcArr[i].offset.rect.half * 2, 0.0f, clr);
				ANC::Graphics.DrawCircle(tArr[i].position + dcArr[i].offset.rect.min , 1.0f, {0,0.8f,0.8f});
				ANC::Graphics.DrawCircle(tArr[i].position + dcArr[i].offset.rect.max , 1.0f, {0,8.8f,0.0f});
				break;
			case COLLIDER_TYPE::CIRCLE:
				ANC::Graphics.DrawCircle(tArr[i].position + dcArr[i].offset.circle.m_center, dcArr[i].offset.circle.m_radius, clr);
				break;
			case COLLIDER_TYPE::LINE:
				ANC::Graphics.DrawLine(dcArr[i].offset.line.m_pt0 + tArr[i].position, dcArr[i].offset.line.m_pt1 + tArr[i].position, 0.5, clr);
				ANC::Graphics.DrawLine(tArr[i].position + dcArr[i].offset.line.m_pt1 , tArr[i].position + dcArr[i].offset.line.m_pt1 +
					dcArr[i].offset.line.m_normal * 5.0f, 1, { 0,0.6f,0.6f });
				break;
			default:
				break;
			}
		}

		for (int i{}; i < 6; ++i)
		{	//Draw debug vector using physics system.
			pSys.DrawDebugVector(tArr[i], pArr[i]);
		}
	}
#endif
//=============================================================================
// Start of Discrete Collision Responese
//=============================================================================
	//Response info versions of collision checks

	//AABB-to-AABB discrete collision check. returns a response info if collision occurs
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_RectRect
	(AABB const& aabb1, Transform_Component const& trans1,
		AABB const& aabb2, Transform_Component const& trans2) const
	{	//return nullopt when no collision
		if (!D_Check_RectRect(aabb1, trans1, aabb2, trans2)) { return std::nullopt; }

		//beyond this point, collision is guranteed
		DC_Response_Info ret;	//return response info

		Math::Vec2 r1Center{ (trans1.position ) },
			r2Center{ (trans2.position) };

		Math::Vec2 nearAxis;	//nearest distance to each axis	

		//take smallest distance for each axis
		nearAxis.x = (aabb2.max.x + r2Center.x) - (aabb1.min.x + r1Center.x);			//AABB1 min x -> AABB2 max x = A
		f64 maxNearDist = (aabb2.min.x + r2Center.x) - (aabb1.max.x + r1Center.x);		//AABB1 max x -> AABB2 min x = B
		nearAxis.x = (fabs(nearAxis.x) < fabs(maxNearDist)) ? nearAxis.x: maxNearDist;	//closest = shortest ? A : B ;

		nearAxis.y = (aabb2.max.y + r2Center.y) - (aabb1.min.y + r1Center.y);			//AABB1 min y -> AABB2 max y = A
		maxNearDist = (aabb2.min.y + r2Center.y) - (aabb1.max.y + r1Center.y);			//AABB1 max y -> AABB2 min y = B
		nearAxis.y = (fabs(nearAxis.y) < fabs(maxNearDist)) ? nearAxis.y : maxNearDist;	//closest = shortest ? A : B ;
		
		//smallest axis shall be used for pop out AABB1 from AABB2
		ret.adjustmentVec = (fabs(nearAxis.x) > fabs(nearAxis.y)) ? Math::Vec2{0.0f,nearAxis.y} : Math::Vec2{ nearAxis.x, 0.0f };

		//set up the normal of colision to be the normal of the closest edge to the center of aabb2
		//AABB1 is to be moved.
		ret.normalOfCol = (fabs(nearAxis.x) < fabs(nearAxis.y)) ?												//	cases for normal of col
			((r2Center.x - r1Center.x) < 0) ? Math::Vec2{1,0} : Math::Vec2{ -1,0 }								// (x axis closest) ? A : B
			: ((r2Center.y - r1Center.y) < 0) ? Math::Vec2{ 0,1 } : Math::Vec2{ 0,-1 };							// (y axis closest) ? A : B
		return ret;
	}

	//AABB-to-Circle discrete collision check. returns a response info if collision occurs
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_RectCircle
	(AABB const& rect, Transform_Component const& trans1,
		Circle const& circle, Transform_Component const& trans2) const
	{
		//dbc = absolute distance between colliders 
		Math::Vec2 rectCenter{ (trans1.position + rect.min + rect.half) };
		Math::Vec2 dbc{ (Math::Vec2)trans2.position + circle.m_center - rectCenter };
		dbc.x = fabs(dbc.x);
		dbc.y = fabs(dbc.y);

		if (dbc.x > (rect.half.x + circle.m_radius) || dbc.y > (rect.half.y + circle.m_radius))
		{
			return std::nullopt;
		}	//too far from center to collide
		
		DC_Response_Info ret;	//return response info
		if (dbc.x < rect.half.x || dbc.y < rect.half.y)
		{	//Apply the same methodology as Rect Rect using c.center.
			Math::Vec2 nearAxis;	//nearest distance to each axis	
			//take smallest distance for each axis
			nearAxis.x = (circle.m_center.x + trans2.position.x) - (-rect.half.x + rectCenter.x - circle.m_radius);			//AABB min x -> Circle center = A
			f64 maxNearDist = (circle.m_center.x + trans2.position.x) - (rect.half.x + rectCenter.x + circle.m_radius);		//AABB max x -> Circle center = B
			nearAxis.x = (fabs(nearAxis.x) < fabs(maxNearDist)) ? nearAxis.x : maxNearDist;						//closest = shortest ? A : B ;

			nearAxis.y = (circle.m_center.y + trans2.position.y) - (-rect.half.y + rectCenter.y - circle.m_radius);			//AABB min y -> Circle center = A
			maxNearDist = (circle.m_center.y + trans2.position.y) - (rect.half.y + rectCenter.y + circle.m_radius);			//AABB max y -> Circle center = B
			nearAxis.y = (fabs(nearAxis.y) < fabs(maxNearDist)) ? nearAxis.y : maxNearDist;						//closest = shortest ? A : B ;

			ret.adjustmentVec = (fabs(nearAxis.x) > fabs(nearAxis.y)) ? Math::Vec2{ 0.0f,nearAxis.y } : Math::Vec2{ nearAxis.x, 0.0f };
			//set up the normal of colision to be the normal of the closest edge to the center of the Circle
			//AABB is assumed to be moved.
			ret.normalOfCol = (fabs(nearAxis.x) < fabs(nearAxis.y)) ?
				((circle.m_center.x + trans2.position.x - rectCenter.x < 0)) ? Math::Vec2{ 1,0 } : Math::Vec2{ -1,0 }								// (x axis closest) ? A : B
			: ((circle.m_center.y + trans2.position.y - rectCenter.y < 0)) ? Math::Vec2{ 0,1 } : Math::Vec2{ 0,-1 };								// (y axis closest) ? A : B
			return ret;
		}	//inside of the rect. must have had collision
			//if the circle is within 1 radus of the edges of the AABB
		//dbcmm = squared distance between colliders magnitude, minus the half width/height of the AABB.
		f64 dbcmm = Math::lengthSquared(dbc - rect.half);
		// if this is below radius squared, both colliders should be colliding. 
		// this only works because our dbcmm is non-directional.
		if ((dbcmm > (circle.m_radius * circle.m_radius))) 
		{return std::nullopt;}

		//response here is normal = center to center
		//depth = sqrt(c.radius^2 - dbcmm);
		//this is for rect col to circle, cirle to square should use the shorter axis to detemine normal
		ret.normalOfCol = Math::normalize(rectCenter - (Math::Vec2)trans2.position + circle.m_center);		
		ret.adjustmentVec = ret.normalOfCol * sqrt((circle.m_radius * circle.m_radius) - dbcmm);
		return ret;
	}

	//Unused collision response function. Planned for an inversed response but not used
	//due to the response being un-natural.
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_CircleRect
	( Circle const& circle, Transform_Component const& trans1,
		AABB const& rect, Transform_Component const& trans2)  const
	{
		//dbc = absolute distance between colliders 
		Math::Vec2 rectCenter{ (trans2.position + rect.min + rect.half) };
		Math::Vec2 dbc{ rectCenter - (Math::Vec2)trans1.position + circle.m_center };
		dbc.x = fabs(dbc.x);
		dbc.y = fabs(dbc.y);

		if (dbc.x > (rect.half.x + circle.m_radius) || dbc.y > (rect.half.y + circle.m_radius))
		{
			return std::nullopt;
		}	//too far from center to collide

		//if the circle is within 1 radus of the edges of the AABB
		//dbcmm = squared distance between colliders magnitude, minus the half width/height of the AABB.
		f64 dbcmm = Math::lengthSquared(dbc - rect.half);
		// if this is below radius squared, both colliders should be colliding. 
		// this only works because our dbcmm is non-directional.
		if ((dbcmm > (circle.m_radius * circle.m_radius)))
		{
			return std::nullopt;
		}
		//beyond here, collision must have happened
		//Apply RectRect style response here here. circle may slip on edges.
		DC_Response_Info ret;	//return response info
		//Apply the same methodology as Rect Rect using c.center.
		Math::Vec2 nearAxis;	//nearest distance to each axis	
		//take smallest distance for each axis
		nearAxis.x = (circle.m_center.x + trans1.position.x) - (rect.min.x + rectCenter.x - circle.m_radius);			//AABB min x -> Circle center = A
		f64 maxNearDist = (circle.m_center.x + trans1.position.x) - (rect.max.x + rectCenter.x + circle.m_radius);		//AABB max x -> Circle center = B
		nearAxis.x = (fabs(nearAxis.x) < fabs(maxNearDist)) ? nearAxis.x : maxNearDist;						//closest = shortest ? A : B ;

		nearAxis.y = (circle.m_center.y + trans1.position.y) - (rect.min.y + rectCenter.y - circle.m_radius);			//AABB min y -> Circle center = A
		maxNearDist = (circle.m_center.y + trans1.position.y) - (rect.max.y + rectCenter.y + circle.m_radius);			//AABB max y -> Circle center = B
		nearAxis.y = (fabs(nearAxis.y) < fabs(maxNearDist)) ? nearAxis.y : maxNearDist;						//closest = shortest ? A : B ;

		ret.adjustmentVec = (fabs(nearAxis.x) > fabs(nearAxis.y)) ? Math::Vec2{ 0.0f,nearAxis.y } : Math::Vec2{ nearAxis.x, 0.0f };
		//set up the normal of colision to be the normal of the closest edge to the center of the Circle
		//AABB is assumed to be moved.
		ret.normalOfCol = (fabs(nearAxis.x) < fabs(nearAxis.y)) ?
			((nearAxis.x > 0)) ? Math::Vec2{ -1,0 } : Math::Vec2{ 1,0 }								// (x axis closest) ? A : B
		: ((nearAxis.y > 0)) ? Math::Vec2{ 0,1 } : Math::Vec2{ 0,-1 };							// (y axis closest) ? A : B
		return ret;
	}

	//AABB-to-Line discrete collision check. returns a response info if collision occurs
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_RectLine
	(AABB const& rect, Transform_Component const& trans1,
		LineSegment const& line, Transform_Component const& trans2) const
	{
		Math::Vec2 rectCenter{ (trans1.position + rect.min + rect.half) };
		//make this a continous dot and rect collision
		//time based.
		Math::Vec2 lineDelta{ (trans2.position + line.m_pt1) - (trans2.position + line.m_pt0) };
		f64 signX{ (lineDelta.x < 0) ? -1.0f : 1.0f },
			signY{ (lineDelta.y < 0) ? -1.0f : 1.0f };
		//get near and far times (to reach p0) 
		//find largest neartime
		f64 nearTimeX = (rectCenter.x - (signX * rect.half.x)
			- (line.m_pt0.x + trans2.position.x)) / lineDelta.x;	//percent line delta to reach closest x axis intersection.
		f64 nearTimeY = (rectCenter.y - (signY * rect.half.y)
			- (line.m_pt0.y + trans2.position.y)) / lineDelta.y;		//percent line delta to reach closest y axis intersection.
		f64 nearTime = (nearTimeX > nearTimeY) ? nearTimeX : nearTimeY;

		//find smallest fartime
		f64 farTimeX = (rectCenter.x + (signX * rect.half.x)
			- (line.m_pt0.x + trans2.position.x)) / lineDelta.x;	//percent line delta to reach furtherst x axis intersection.
		f64 farTimeY = (rectCenter.y + (signY * rect.half.y)
			- (line.m_pt0.y + trans2.position.y)) / lineDelta.y;	//percent line delta to reach furtherst y axis intersection.
		f64 farTime = (farTimeX < farTimeY) ? farTimeX : farTimeY;

		//if the closest intersection time in one axis is larger than the furtherest intersection time of the opposite axis, no possible collisions.
		if (nearTime > farTime)
		{
			return std::nullopt;
		}
		//ret normal = line normal in direction of square center
		//if the near time is more than line's axis delta, or less than 0 (moving in wrong direction of line delta), no collision possible
		if (nearTime >= 1 || farTime <= 0) { return std::nullopt; }
		
		//collision is guranteed after this point
		ANC_DCS::DC_Response_Info ret;

		f64 shortestDist = Math::dot(static_cast<Math::Vec2>(trans2.position + line.m_pt0) - static_cast<Math::Vec2>(rectCenter)
									,line.m_normal);
		f64 sign = (shortestDist > 0) ? 1.0f: -1.0f;
		ret.normalOfCol = sign * line.m_normal;	//*-1 to magnitude.

		//sdv = shortest distance vector, ci = closest intersection given the shortest distance vector
		Math::Vec2 corners[4];
		
		//find corners
		corners[0] = rectCenter + rect.max;	//tr top right
		corners[1] = rectCenter + Math::Vec2(rect.max.x, rect.min.y);	//br bottom right
		corners[2] = rectCenter + rect.min;	//bl bottom left
		corners[3] = rectCenter + Math::Vec2(rect.min.x, rect.max.y);	//tl top left

		//find if corners are intruding via dot product. Find cheaper method if possible 
		f64 cornerDP[4];
		//compare directions of corners from intersection and the shortest distance by normal line.
		//while magnitude is to scale of the normal of collision.
		//(vector is negative and smallest absolute value after comparing with other negative vectors.)
		cornerDP[0] = Math::dot(static_cast<Math::Vec2>(trans2.position + line.m_pt0) - corners[0], ret.normalOfCol);
		cornerDP[1] = Math::dot(static_cast<Math::Vec2>(trans2.position + line.m_pt0) - corners[1], ret.normalOfCol);
		cornerDP[2] = Math::dot(static_cast<Math::Vec2>(trans2.position + line.m_pt0) - corners[2], ret.normalOfCol);
		cornerDP[3] = Math::dot(static_cast<Math::Vec2>(trans2.position + line.m_pt0) - corners[3], ret.normalOfCol);

		size_t popCorner{0};
		for (size_t i = 1; i < 4; i++)
		{
			popCorner = (cornerDP[i] > 0) ? popCorner :				//is the DP negative? if not, must not be an intruded(collided) corner
				(fabs(cornerDP[i]) > fabs(cornerDP[popCorner])) ?	//is the absolute magnitude larger than the current absolute magnitude?
				popCorner : i;										//if so, reject, otherwise, make it the corner to pop resolution for.
		}
		ret.adjustmentVec = cornerDP[popCorner] * ret.normalOfCol;
		return ret;
	}

	//Circle-to-Circle discrete collision check. returns a response info if collision occurs
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_CircleCircle
	(Circle const& c1, Transform_Component const& trans1,
		Circle const& c2, Transform_Component const& trans2) const
	{	//Do classic squared distance from center check between circles. 
		// If it is less than the squared additions of both circle radius, it must be not colliding, else it must be colliding
		//sdcb = the squared distance between both collider centers.
		f64 sdcb = Math::lengthSquared(trans2.position - trans1.position);
		if (sdcb > (c1.m_radius + c2.m_radius) * (c1.m_radius + c2.m_radius))
		{return std::nullopt;}
		
		//below must have collided.
		DC_Response_Info ret;

		//make the return normal the vector from c1 to c2 centers, 
		//with the depth being the difference between the lenght of dbc and the sum of both circle radius
		//Assume Circle 1 is to move.
		Math::Vec2 dbc = (c1.m_center + trans1.position) - (c2.m_center + trans2.position);	//from c2 to c1
		ret.normalOfCol = Math::normalize(dbc);
		f64 depthOfCol = (c1.m_radius + c2.m_radius) - Math::length(dbc);
		ret.adjustmentVec = ret.normalOfCol * depthOfCol;
		return ret;
	}

	//Circle-to-Line discrete collision check. returns a response info if collision occurs
	std::optional<ANC_DCS::DC_Response_Info> ANC_DCS::D_Collision_CircleLine
	(Circle const& circle, Transform_Component const& trans1,
		LineSegment const& line, Transform_Component const& trans2) const
	{
		// cp0 = circle to p0 on line
		Math::Vec2 cp0{ trans2.position + line.m_pt0 - trans1.position + circle.m_center};

		//use np0 to get perpendicular distance from circle to infinte extension of line. 
		//if it is more than the radius, circle will not collide.
		f64 shortestDist = Math::dot(line.m_normal, cp0);
		f64 sign = (shortestDist < 0) ? -1.0f : 1.0f;	//carry the signess of the projection magnitude
		if (fabs(shortestDist) > circle.m_radius)
		{	//if too far from infinite extension of line, cannot be colliding
			return std::nullopt;
		}

		// cp1 = circle to p1 on line
		Math::Vec2 cp1{ trans2.position + line.m_pt1 - trans1.position + circle.m_center};
		ANC_DCS::DC_Response_Info ret;

		//is the circle at the line edge?
		if (Math::lengthSquared(cp0) <= circle.m_radius * circle.m_radius
			|| Math::lengthSquared(cp1) <= circle.m_radius * circle.m_radius)
		{	
			f64 dcp0sqr = Math::lengthSquared(cp0) - (circle.m_radius * circle.m_radius),	//dcp0sqr = distance of c to p0 squared, minus radius
				dcp1sqr = Math::lengthSquared(cp1) - (circle.m_radius * circle.m_radius);	//dcp0sqr = distance of c to p1 squared, minus radius
			
			f64 depthOfCol = (fabs(dcp0sqr) > fabs(dcp1sqr))? sqrt(fabs(dcp1sqr)) : sqrt(fabs(dcp0sqr));				//depth = shorter length - radius
			//find normal of collision and the adjustment vector by finding the smaller squared distance
			ret.normalOfCol = (fabs(dcp0sqr) > fabs(dcp1sqr)) ? Math::normalize(cp1):Math::normalize(cp0);
			//ret.normalOfCol *= sign * -1.0f;
			ret.adjustmentVec =  ret.normalOfCol * depthOfCol;
			return ret;
		}
		//if not inbetween both points
		if ((Math::dot(cp0, cp1) > 0)) 
		{return std::nullopt;}
		//collision against line edge
		ret.normalOfCol = sign * line.m_normal;
		f64 depthOfCol = fabs(shortestDist) - circle.m_radius + 0.5f;
		ret.adjustmentVec = ret.normalOfCol * depthOfCol;
		return ret;
	}

//=============================================================================
// Start of Discrete Collision Responese calculations
//=============================================================================
	//This function is a composite of the next two functions. pair of both function's return value is returned
	std::pair<Math::Vec3, Math::Vec2> ANC_DCS::D_ApplyResponse(DC_Response_Info const& res, Transform_Component const& trans, Physics_Component const& phy)
	{
		Math::Vec3 popPosition{ D_ApplyPop(res,trans)};						//apply de-intersection vector for transform position
		Math::Vec2 reflection{ D_ApplyReflection(res,phy) };				//apply reflection for velocity
		return std::make_pair(popPosition, reflection);	//return them as a pair
	}
	
	//return a position vector of the transform position after the pop vector has been added
	Math::Vec3 ANC_DCS::D_ApplyPop(DC_Response_Info const& res, Transform_Component const& trans)
	{	//return the transform position after adjustments
		return  trans.position + Math::Vec3(res.adjustmentVec);
	}

	//returns the velocity vector of the physics after it has been reflected across the tangent.
	Math::Vec2 ANC_DCS::D_ApplyReflection(DC_Response_Info const& res, Physics_Component const& phy)
	{	//find the tangent of the reflection
		Math::Vec2 tangent{ Math::Vec2{res.normalOfCol.y, -res.normalOfCol.x} };
		//reflected vector. currently initialized to the intrusion vector.
		Math::Vec2 reflected = phy.getVel();
		//pit = projected intrusion vector onto tangent.
		f64 pit = Math::dot(reflected,tangent);
		reflected += 2.0f * ((pit * tangent) - reflected);		//make the reflected vector plus 2 times of the vector from intrusion vector to tangentline
		return reflected;
	}

	// response inversion functions
	ANC_DCS::DC_Response_Info ANC_DCS::inverseResponse(ANC_DCS::DC_Response_Info const& ref) 
	{	//copy reference into ret
		DC_Response_Info ret{ ref };
		//invert all response information
		ret.adjustmentVec *= -1.0f;
		ret.normalOfCol *= -1.0f;
		return ret;
	}
	// rvalue style inversion function
	ANC_DCS::DC_Response_Info ANC_DCS::inverseResponse(ANC_DCS::DC_Response_Info && ref)
	{	//copy reference into ret
		DC_Response_Info ret{ std::move(ref) };
		//invert all response information
		ret.adjustmentVec *= -1.0f;
		ret.normalOfCol *= -1.0f;
		return ret;
	}
	// find the percentage of velocity preserved after a mass based collision response 
	std::pair<f64, f64> ANC_DCS::MassBasedReflectionMagnitude(Physics_Component const& lhs, Physics_Component const& rhs)
	{
		bool lhsStatic{ lhs.getIsStatic() },
			rhsStatic{ rhs.getIsStatic() };
		if (lhsStatic && rhsStatic) { return std::make_pair(0.0f,0.0f); }
		std::pair<f64, f64> magnitutdes;
		//total kinetic energy in this system of collision. This is here in case there is a need for a more realistic response in the future.
		/*f64 totalKE = (0.5 * lhs.getMass() * Math::lengthSquared(lhs.getVel())) 
					+ (0.5 * rhs.getMass() * Math::lengthSquared(rhs.getVel()));*/

		//get variables required to do mass based velocity magnitude calculations
		f64&& m1{ lhs.getMass() },
			&& m2{ rhs.getMass() };
		f64&& u1{ Math::length(lhs.getVel())},
			&& u2{ Math::length(rhs.getVel()) };

		//apply elastic transfer of forces
		magnitutdes.first = (u1 * (m1 - m2)/(m1 + m2)) + (u2 * (2 * m2)/(m1 + m2));
		magnitutdes.second = (u1 * (2 * m2) / (m1 + m2)) + (u2 * (m1 - m2) / (m1 + m2));

		//if the other object is static, apply the full magnitutde to the current object.
		magnitutdes.first += (rhsStatic) ? magnitutdes.second: 0.0f ;
		magnitutdes.second += (lhsStatic) ? magnitutdes.first : 0.0f;

		//apply co-efficient of restitution
		if (useCOR) 
		{// we approx cor via the average of the expected cor of both items. if elasticity is added in the future, this will be changed
			f64 avgCOR = (lhs.getCOR() + rhs.getCOR()) / 2;
			magnitutdes.first *= avgCOR;
			magnitutdes.second *= avgCOR;
		}
		return magnitutdes;
	}

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
	bool ANC_DCS::DetermineCollisionAndResponse(ANC_DCC& lhs, Transform_Component& lhsTrans, Physics_Component& lhsPhy,
		ANC_DCC& rhs, Transform_Component& rhsTrans, Physics_Component& rhsPhy) 
	{
		bool lhsStatic{ lhsPhy.getIsStatic() },
			rhsStatic{ rhsPhy.getIsStatic() };
		if (lhsStatic && rhsStatic) { return false; }		//both entities are static and hence will skip collision check.

		std::optional<ANC_DCS::DC_Response_Info> resInfo;	//store the response information
		//get response info for resolution
		switch (lhs.type)
		{
		case COLLIDER_TYPE::RECT:	//lhs = rect
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				resInfo = D_Collision_RectRect(lhs.offset.rect, lhsTrans, rhs.offset.rect, rhsTrans);
				break;
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				resInfo = D_Collision_RectCircle(lhs.offset.rect, lhsTrans, rhs.offset.circle, rhsTrans);
				break;
			case COLLIDER_TYPE::LINE:	//rhs = line
				resInfo = D_Collision_RectLine(lhs.offset.rect, lhsTrans, rhs.offset.line, rhsTrans);
				break;
			default:
#if _DEBUG
				ANC_Debug::Debug.Log("DetermineCollisionAndResponse(): Collision between Unknown collider types");
#endif
				return false;
			}
			break;
		case COLLIDER_TYPE::CIRCLE:	//lhs = circle
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				//resInfo = D_Collision_CircleRect(lhs.offset.circle, lhsTrans, rhs.offset.rect, rhsTrans);
				resInfo = D_Collision_RectCircle(rhs.offset.rect, rhsTrans, lhs.offset.circle, lhsTrans);
				resInfo = (resInfo) ? inverseResponse(resInfo.value()) : resInfo;
				break;
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				resInfo = D_Collision_CircleCircle(lhs.offset.circle, lhsTrans, rhs.offset.circle, rhsTrans);
				break;
			case COLLIDER_TYPE::LINE:	//rhs = line
				resInfo = D_Collision_CircleLine(lhs.offset.circle, lhsTrans, rhs.offset.line, rhsTrans);
				break;
			default:
#if _DEBUG
				ANC_Debug::Debug.Log("DetermineCollisionAndResponse(): Collision between Unknown collider types");
#endif
				return false;
			}
			break;
		case COLLIDER_TYPE::LINE:	//lhs = line
			switch (rhs.type)
			{
			case COLLIDER_TYPE::RECT:	//rhs = rect
				resInfo = D_Collision_RectLine(rhs.offset.rect, rhsTrans, lhs.offset.line, lhsTrans);
				break;
			case COLLIDER_TYPE::CIRCLE:	//rhs = circle
				resInfo = D_Collision_CircleLine(rhs.offset.circle, rhsTrans, lhs.offset.line, lhsTrans);
				break;
			case COLLIDER_TYPE::LINE:	//rhs = line
				;	//static types never collide
			default:
#if _DEBUG
				ANC_Debug::Debug.Log("DetermineCollisionAndResponse(): Collision between Unknown collider types");
#endif
				return false;
			}
			break;
		default:					//Unknown Collider.
#if _DEBUG
			ANC_Debug::Debug.Log("DetermineCollisionAndResponse(): Collision between Unknown collider types");
#endif
			return false;
		}

		if (!resInfo) { return resInfo.has_value(); }	//no collision occured

		std::pair<f64, f64> rMag{ (massBased) ? 
			MassBasedReflectionMagnitude(lhsPhy,rhsPhy) 
			: std::make_pair(0.0f,0.0f)};		//reflected magnitude

		//collision occurs beyond this point
		Math::Vec2 lhsVel = lhsPhy.getVel();

		if (!lhsStatic)				
		{	//get the resolution vectors, then update the position and velocity accordingly
			std::pair<Math::Vec3, Math::Vec2> results = ANC_DCS::D_ApplyResponse(resInfo.value(), lhsTrans, lhsPhy);
			lhsTrans.position = results.first;
			//prevent reflection if lhs velocity is in a similiar direction to the normal of collision
			if ((Math::dot(resInfo.value().normalOfCol, (Math::Vec2)lhsPhy.getVel()) < 0) || (rhs.GetType() == COLLIDER_TYPE::LINE)){
				lhsPhy.setVel((massBased) ? 
					Math::normalize(results.second) * rMag.first 
					: results.second);
			}
		}
		if (!rhsStatic)
		{	//invert the response information, get the resolution vectors, then update the velocity accordingly
			resInfo = inverseResponse(resInfo.value());
			std::pair<Math::Vec3, Math::Vec2> results = ANC_DCS::D_ApplyResponse(resInfo.value(), rhsTrans, rhsPhy);
			rhsTrans.position = results.first;
			
			//prevent reflection if rhs velocity is in a similiar direction to the normal of collision
			if ((Math::dot((Math::Vec2)(rhsPhy.getVel()), resInfo.value().normalOfCol) < 0) || (lhs.GetType() == COLLIDER_TYPE::LINE))
			{
				rhsPhy.setVel(results.second);	//reflect
			}
			else
				rhsPhy.setVel(Math::normalize((Math::Vec2)rhsPhy.getVel() + lhsVel) * Math::length(rhsPhy.getVel()));	//adjust
			
			//check if the system is mass based, apply mass based adjustment to movement vector. 
			//direction is the merged velocity direction of both colliders, then multiply it by the magnitute from calculations above
			rhsPhy.setVel((massBased) ?
				Math::normalize((Math::Vec2)rhsPhy.getVel()+lhsVel) * rMag.second
				: (Math::Vec2)rhsPhy.getVel());
		}
		return true;
	}

	//getter and setter for if the current collision system is massed based
	bool ANC_DCS::isCollisionMassBased() 
	{return massBased;}
	ANC_DCS& ANC_DCS::SetMassBasedCollision(bool bFlag) 
	{massBased = bFlag; return *this;}

	//getter and setter for applying Co-efficient of restitution of a material.
	bool ANC_DCS::isUsingCOR() 
	{ return useCOR; }
	//set if the system is using COR. Only applied when mass based collisions are enabled 
	ANC_DCS& ANC_DCS::SetUsingCOR(bool bflag) 
	{ useCOR = bflag; return *this; }

}//end of ECS namespace
}//end of ANC namespace
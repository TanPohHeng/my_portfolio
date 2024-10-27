/******************************************************************************/
/*!
\file		AABB.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	September 24, 2023
\brief		Source file containing function related to continous AABB collisions

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "Pch.h"
#ifdef ANC_CONTINOUS_COL	//only allow this if continous collision was mandated
#include "AABB.h"
#include <algorithm>
namespace
{
	//epsilon for floating point 0 checking
	constexpr float EPSILON = 0.0001f;
	constexpr double EPSILOND = 0.0001;
}

/**************************************************************************/
/*!
\brief	Check the collisions between 2 AABB objects. Checks for static collison 
		,then checks for dynamic collision using axis aligned intersections.
\param [in] aabb1
		AABB of the first object
\param [in] vel1
		velocity of the first object
\param [in] aabb2
		AABB of the second object
\param [in] vel2
		velocity of the second object
\return 
		If a collision occured, true. If not, false.
	*/
/**************************************************************************/
bool CollisionIntersection_RectRect(const COL::AABB & aabb1, const Math::Vec2& vel1,
									const COL::AABB & aabb2, const Math::Vec2& vel2,
									f64 dt)
{
	//Step 1:
	//attempt static (not) no collision check CW around AABB1
	if (!(aabb1.min.x > aabb2.max.x ||	//check no collision on left of AABB1
		aabb1.max.y < aabb2.min.y ||	//check no collision on top of AABB1
		aabb1.max.x < aabb2.min.x ||	//check no collision on right of AABB1
		aabb1.min.y > aabb2.max.y))		//check no collision to the bottom of AABB1
	{	//if collision found, return true
		return true;
	}
	// no collision, thus continue to 
	
	// Step 2:
	//find resulting vector from combining both object's velocities
	Math::Vec2 vb;					//relative velocity store
	vb.x = vel2.x - vel1.x;		//relative velocity in x axis
	vb.y = vel2.y - vel1.y;		//relative velocity in y axis
	if (!vb.x && !vb.y)
		return false;			//if no velocity at all, short cut to falses as neither object will ever collide

	f64 tFirst{ 0.0 },	//set tFirst to 0 for both axis as zero seconds into movement, whichever axis intersects later
		tLast{ dt };//set tLast to delta time for both axis as the last point in time we calculate movement for, whichever axis intersects earlier
	
	//NOTE: all calculations in step 3, 4 are done from object 2 to object 1!


	//Step 3: X-Axis Dynamic collision intersection calulations
	if (vb.x < 0)
	{	//if the relative vector (object 2 towards object 1) is to the left <-,
		if (aabb2.max.x < aabb1.min.x) //object 2 is left of object 1
		{	//case 1: object 2 is already pass object 1 in direction of relative vector (object 2 towards left of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (aabb2.min.x > aabb1.max.x)	//object 2 is right of object 1, if yet to enter colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards right of object 1)
			f64 xFirstIntersect{ (aabb2.max.x - aabb1.min.x) / vb.x };	//d till first impact over current relative velocity
			tFirst = (xFirstIntersect > tFirst) ? xFirstIntersect : tFirst;	//let first impact be whichever variable larger, between 0 second and first second of X intersection
		}
		if (aabb2.max.x > aabb1.min.x)	//object 2 is right of object 1, if yet to exit collision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards right of object 1)
			f64 xLastIntersect{ (aabb2.min.x - aabb1.max.x) / vb.x };	//d till end of intersection in X-Axis over current relative velocity
			tLast = (xLastIntersect < tLast) ? xLastIntersect : tLast;	//let last collision time be whichever variable smaller, between last second of our calculation and last second of X intersection
		}
	}
	if (vb.x > 0)
	{	//if the relative vector (object 2 towards object 1) is to the right ->,
		if (aabb2.min.x > aabb1.max.x)	//object 2 is right of object 1
		{	//case 3: object 2 is already pass object 1 in direction of relative vector (object 2 towards right of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (aabb2.max.x < aabb1.min.x)	//object 2 is left of object 1, if yet to enter colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards left of object 1)
			f64 xFirstIntersect{ (aabb2.max.x - aabb1.min.x) / vb.x };	//d till first impact over current relative velocity
			tFirst = (xFirstIntersect > tFirst) ? xFirstIntersect : tFirst;	//let first impact be whichever variable larger, between 0 second and first second of X intersection
		}
		if (aabb2.min.x < aabb1.max.x)	//object 2 is left of object 1, if yet to exit colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards left of object 1)
			f64	xLastIntersect{ (aabb2.min.x - aabb1.max.x) / vb.x };	//d till end of intersection in X-Axis over current relative velocity
			tLast = (xLastIntersect < tLast) ? xLastIntersect : tLast;	//let last collision time be whichever variable smaller, between last second of our calculation and last second of X intersection
		}
	}
	if (vb.x == 0)
	{	//if X-axis has not already intersected, return false
		if (!(aabb1.min.x > aabb2.max.x && aabb1.max.x < aabb2.min.x)) { return false; }
	}

	//Step 4: Y-Axis Dynamic collision intersection calulations

	if (vb.y < 0)
	{	//if the relative vector (object 2 towards object 1) is to the bottom vv ,
		if (aabb2.max.y < aabb1.min.y) //object 2 is below of object 1
		{	//case 1: object 2 is already pass object 1 in direction of relative vector (object 2 towards bottom of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (aabb2.min.y > aabb1.max.y)	//object 2 is above of object 1, if yet to enter colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards top of object 1)
			f64 yFirstIntersect{ (aabb2.max.y - aabb1.min.y) / vb.y };	//d till first impact over current relative velocity
			tFirst = (yFirstIntersect > tFirst) ? yFirstIntersect : tFirst;	//let first impact be whichever variable larger, between tFirst and first second of Y intersection
		}
		if (aabb2.max.y > aabb1.min.y)	//object 2 is above of object 1, if yet to exit colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards top of object 1)
			f64 yLastIntersect{ (aabb2.min.y - aabb1.max.y) / vb.y };	//d till end of intersection in Y-Axis over current relative velocity
			tLast = (yLastIntersect < tLast) ? yLastIntersect : tLast;	//let last collision time be whichever variable smaller, between tLast and last second of Y intersection
		}
	}
	if (vb.y > 0)
	{	//if the relative vector (object 2 towards object 1) is to the top ^^ ,
		if (aabb2.min.y > aabb1.max.y)	//object 2 is above of object 1
		{	//case 3: object 2 is already pass object 1 in direction of relative vector (object 2 towards top of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (aabb2.max.y < aabb1.min.y)	//object 2 is below of object 1, if yet to enter colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards bottom of object 1)
			f64 yFirstIntersect{ (aabb2.max.y - aabb1.min.y) / vb.y };	//d till first impact over current relative velocity
			tFirst = (yFirstIntersect > tFirst) ? yFirstIntersect : tFirst;	//let first impact be whichever variable larger, between tFirst and first second of Y intersection
		}
		if (aabb2.min.y < aabb1.max.y)	//object 2 is below of object 1, if yet to exit colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards bottom of object 1)
			f64 yLastIntersect{ (aabb2.min.y - aabb1.max.y) / vb.y };	//d till end of intersection in Y-Axis over current relative velocity
			tLast = (yLastIntersect < tLast) ? yLastIntersect : tLast;	//let last collision time be whichever variable smaller, between tLast and last second of Y intersection
		}
	}
	if (vb.y == 0)
	{	//if y axis has not already intersected, return false
		if (!(aabb1.min.y > aabb2.max.y && aabb1.max.y < aabb2.min.y)) { return false; }
	}

	//step 5: is our largest tFirst greater than our smallest tLast?
	// if tLast>tFirst, collision at tFirst to tLast, else, no collision
	return (tFirst>tLast)?false:true;
	/*
	Implement the collision intersection over here.

	The steps are:	
	Step 1: Check for static collision detection between rectangles (before moving). 
				If the check returns no overlap you continue with the following next steps (dynamics).
				Otherwise you return collision true

	Step 2: Initialize and calculate the new velocity of Vb
			tFirst = 0
			tLast = dt

	Step 3: Working with one dimension (x-axis).
			if(Vb < 0)
				case 1
				case 4
			if(Vb > 0)
				case 2
				case 3

			case 5

	Step 4: Repeat step 3 on the y-axis

	Step 5: Otherwise the rectangles intersect

	*/
}

/**************************************************************************/
/*!
\brief	Check the collisions between 2 AABB objects. Checks for static collison
		,then checks for dynamic collision using axis aligned intersections.
\param [in] aabb
		AABB of the first object
\param [in] vel1
		velocity of the first object
\param [in] circle
		Circle collider of the second object
\param [in] vel2
		velocity of the second object
\param [in] dt
		the delta time (time passed since last rendered frame).
\return
		If a collision occured, true. If not, false.
*/
/**************************************************************************/
bool CollisionIntersection_RectCircle(const COL::AABB& aabb, const Math::Vec2& vel1,
	const COL::Circle& circle, const Math::Vec2& vel2, f64 dt) 
{
	//find the relative velocity now as it is required for all subsequent true corner collision checks
	//Step 1:
	//perform static collision check on circle and AABB
	// vbc = vector between colliders
	Math::Vec2 centerAABB = aabb.min + ((aabb.max - aabb.min) * 0.5);	//find center of AABB
	Math::Vec2 vbc = centerAABB - circle.m_center;
	vbc.x = fabs(vbc.x);
	vbc.y = fabs(vbc.y);
	
	//find aabb's width and height
	f64 aabbWidth = aabb.max.x - aabb.min.x;
	f64 aabbHeight = aabb.max.y - aabb.min.y;

	if ((vbc.x > (aabbWidth/2 + circle.m_radius) || vbc.y > (aabbHeight / 2 + circle.m_radius)))
	{/*uncertain negative collision at starting position, move to dynamic collision checks. 
	 This check is required for the next else/if case logic to work.*/ }
	else if (vbc.x <= (aabbWidth / 2 ) || vbc.y <= (aabbHeight / 2 ))
	{return true;}
	else 
	{	//corner distance squared check
		//dbc = distance between collider centers, squared
		//dwAABB = diagonal half width of AABB(across min to to AABB center), squared
		//cDist = distance to corner collision between circle and rect, squared
		f64 dbc = fabs(Math::distanceSquared(circle.m_center,centerAABB));
		f64 dwAABB = fabs(Math::distanceSquared(aabb.min, centerAABB));
		f64 cDist = (dwAABB)+(circle.m_radius * circle.m_radius);
		if (dbc <= cDist) { return true; }
	}
	//all other cases go to dynamic collision

	//Step 2: get resulting vector for a static to dynamic collision check
	Math::Vec2 vb;					//relative velocity store
	vb.x = vel2.x - vel1.x;			//relative velocity in x axis
	vb.y = vel2.y - vel1.y;			//relative velocity in y axis
	if (!vb.x && !vb.y)
		return false;			//if no velocity at all, short cut to falses as neither object will ever collide
	f64 tFirst{ 0.0 },	//set tFirst to 0 for both axis as zero seconds into movement, whichever axis intersects later
		tLast{ dt };//set tLast to delta time for both axis as the last point in time we calculate movement for, whichever axis intersects earlier

	//if either x or y velocity is 0, return?

	//Step 4: More precise checks?


	//Step 3: X-Axis Dynamic collision intersection calulations
	float circleMaxX = circle.m_center.x + circle.m_radius,
		circleMinX = circle.m_center.x - circle.m_radius;
	float circleMaxY = circle.m_center.y + circle.m_radius,
		circleMinY = circle.m_center.y - circle.m_radius;
	//Step 3: X-Axis Dynamic collision intersection calulations
	if (vb.x < 0)
	{	//if the relative vector (object 2 towards object 1) is to the left <-,
		if (circleMaxX < aabb.min.x) //object 2 is left of object 1
		{	//case 1: object 2 is already pass object 1 in direction of relative vector (object 2 towards left of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (circleMinX > aabb.max.x)	//object 2 is right of object 1, if yet to enter colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards right of object 1)
			f64 xFirstIntersect{ (circleMaxX - aabb.min.x) / vb.x };	//d till first impact over current relative velocity
			tFirst = (xFirstIntersect > tFirst) ? xFirstIntersect : tFirst;	//let first impact be whichever variable larger, between 0 second and first second of X intersection
		}
		if (circleMaxX > aabb.min.x)	//object 2 is right of object 1, if yet to exit collision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards right of object 1)
			f64 xLastIntersect{ (circleMinX - aabb.max.x) / vb.x };	//d till end of intersection in X-Axis over current relative velocity
			tLast = (xLastIntersect < tLast) ? xLastIntersect : tLast;	//let last collision time be whichever variable smaller, between last second of our calculation and last second of X intersection
		}
	}
	if (vb.x > 0)
	{	//if the relative vector (object 2 towards object 1) is to the right ->,
		if (circleMinX > aabb.max.x)	//object 2 is right of object 1
		{	//case 3: object 2 is already pass object 1 in direction of relative vector (object 2 towards right of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (circleMinX < aabb.min.x)	//object 2 is left of object 1, if yet to enter colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards left of object 1)
			f64 xFirstIntersect{ (circleMaxX - aabb.min.x) / vb.x };	//d till first impact over current relative velocity
			tFirst = (xFirstIntersect > tFirst) ? xFirstIntersect : tFirst;	//let first impact be whichever variable larger, between 0 second and first second of X intersection
		}
		if (circleMinX < aabb.max.x)	//object 2 is left of object 1, if yet to exit colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards left of object 1)
			f64	xLastIntersect{ (circleMinX - aabb.max.x) / vb.x };	//d till end of intersection in X-Axis over current relative velocity
			tLast = (xLastIntersect < tLast) ? xLastIntersect : tLast;	//let last collision time be whichever variable smaller, between last second of our calculation and last second of X intersection
		}
	}
	if (vb.x == 0)
	{	//if X-axis has not already intersected, return false
		if (!(aabb.min.x > circleMaxX && aabb.max.x < circleMinX)) { return false; }
	}

	//Step 4: Y-Axis Dynamic collision intersection calulations

	if (vb.y < 0)
	{	//if the relative vector (object 2 towards object 1) is to the bottom vv ,
		if (circleMaxY < aabb.min.y) //object 2 is below of object 1
		{	//case 1: object 2 is already pass object 1 in direction of relative vector (object 2 towards bottom of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (circleMinY > aabb.max.y)	//object 2 is above of object 1, if yet to enter colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards top of object 1)
			f64 yFirstIntersect{ (circleMaxY - aabb.min.y) / vb.y };	//d till first impact over current relative velocity
			tFirst = (yFirstIntersect > tFirst) ? yFirstIntersect : tFirst;	//let first impact be whichever variable larger, between tFirst and first second of Y intersection
		}
		if (circleMaxY > aabb.min.y)	//object 2 is above of object 1, if yet to exit colision
		{	//case 4: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards top of object 1)
			f64 yLastIntersect{ (circleMinY - aabb.max.y) / vb.y };	//d till end of intersection in Y-Axis over current relative velocity
			tLast = (yLastIntersect < tLast) ? yLastIntersect : tLast;	//let last collision time be whichever variable smaller, between tLast and last second of Y intersection
		}
	}
	if (vb.y > 0)
	{	//if the relative vector (object 2 towards object 1) is to the top ^^ ,
		if (circleMinY > aabb.max.y)	//object 2 is above of object 1
		{	//case 3: object 2 is already pass object 1 in direction of relative vector (object 2 towards top of object 1)
			//thus objects will fail to intersect with current velocities, no collision this frame
			return false;
		}
		if (circleMaxY < aabb.min.y)	//object 2 is below of object 1, if yet to enter colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards bottom of object 1)
			f64 yFirstIntersect{ (circleMaxY - aabb.min.y) / vb.y };	//d till first impact over current relative velocity
			tFirst = (yFirstIntersect > tFirst) ? yFirstIntersect : tFirst;	//let first impact be whichever variable larger, between tFirst and first second of Y intersection
		}
		if (circleMinY < aabb.max.y)	//object 2 is below of object 1, if yet to exit colision
		{	//case 2: object 2 is has not pass object 1 in direction of relative vector at T 0 seconds (towards bottom of object 1)
			f64 yLastIntersect{ (circleMinY - aabb.max.y) / vb.y };	//d till end of intersection in Y-Axis over current relative velocity
			tLast = (yLastIntersect < tLast) ? yLastIntersect : tLast;	//let last collision time be whichever variable smaller, between tLast and last second of Y intersection
		}
	}
	if (vb.y == 0)
	{	//if y axis has not already intersected, return false
		if (!(aabb.min.y > circleMaxY && aabb.max.y < circleMaxY)) { return false; }
	}

	//step 5: is our largest tFirst greater than our smallest tLast?
	// if tLast < tFirst no collision, else corner case needs to be checked
	if (tFirst > tLast) { return false; }

	//Step 6:
	// Check for corner cases
	// Check if vb is larger than the combined distance of both colliders minus max radius and half width of both colliders.
	//vb = resulting velocity, 
	//dwAABB = diagonal half width of AABB(across min to to AABB center), squared
	//cDist = distance to corner collision between circle and rect, squared

	f64 dwAABB = fabs(Math::distanceSquared(aabb.min, centerAABB));
	f64 cDist = (dwAABB) + (circle.m_radius * circle.m_radius);

	//ncm = normalized circle movement vector normal
	Math::Vec2 circleEnd = circle.m_center + vb * (float)dt;
	Math::Vec2 ncm { vb.y, -vb.x };
	ncm = Math::normalize(ncm);
	//find shortest pertendicular distance n.p - n.p0. no abs since we will be testing in squared value later.
	f64 shortestDist = Math::dot(ncm, circle.m_center - centerAABB) 
					   - Math::dot(ncm, circleEnd - centerAABB);
	
	//if shortest distance squared never crossed maximum distance for collision squared, return false, else true.
	return ((shortestDist * shortestDist) - cDist > EPSILOND)? false: true;
/*
Implement the collision intersection over here.

The steps are:
Step 1: Check Absolute distance from circle to Square

Step 2: Check if Absolute distance is larger than circle and Square width combined

*/
}


/**************************************************************************/
/*!
\brief	Check the collisions between an AABB and a Line Segment.
		Checks for static collison. Uses seperated axis theorem
		,then checks for dynamic collision using axis aligned intersections.
		if checking for static and dynamic, set the appropriate velocity param
		to a vector 2D of {0,0}.
\param [in] aabb
		AABB of the first object
\param [in] vel1
		velocity of the first object
\param [in] line
		line segment as the second object
\param [in] dt
		the delta time (time passed since last rendered frame).
\return
		If a collision occured, true. If not, false.
*/
/**************************************************************************/
bool CollisionIntersection_RectLine(const COL::AABB& aabb, const Math::Vec2& vel1,
	const COL::LineSegment& line, f64 dt) 
{
#if 0
	//currently unactive as later checks overlap with this check.
	if ((aabb.min.x > line.m_pt0.x && aabb.min.x > line.m_pt1.x) ||	//check both lines on left of AABB1
		(aabb.max.y < line.m_pt0.y && aabb.max.y < line.m_pt1.y) ||	//check both lines on top of AABB1
		(aabb.max.x < line.m_pt0.x && aabb.max.x < line.m_pt1.x) ||	//check both lines on right of AABB1
		(aabb.min.y > line.m_pt0.y && aabb.min.y > line.m_pt1.y))	//check both lines to the bottom of AABB1
	{	//if both points of line segment are on the same side, no collision.
		return false;
	}
#endif
	

	//Step 1: Check between static line segment and static AABB collision. 
	//Solve this like point-to-AABB collision 
	if (((aabb.max.x > line.m_pt0.x && aabb.min.x < line.m_pt0.x)
		&& (aabb.max.y > line.m_pt0.y && aabb.min.y < line.m_pt0.y))	//p0 is in aabb check
		|| ((aabb.max.x > line.m_pt1.x && aabb.min.x < line.m_pt1.x)
			&& (aabb.max.y > line.m_pt1.y && aabb.min.y < line.m_pt1.y)))	//p1 is in aabb check
	{	//if either line segment points are in the AABB, collision must be true
		return true;
	}

	Math::Vec2 dLine{line.m_pt1 - line.m_pt0};	//delta of the line vector. start to end point vector of line segment

	//find the nearest time to reach an edge of the AABB from the start of the line segment p0 point
	f64 nearTimeX = (aabb.min.x - line.m_pt0.x) / dLine.x;
	f64 nearTimeY = aabb.min.y - line.m_pt0.y / dLine.y;
	f64 farTimeX = aabb.max.x - line.m_pt0.x / dLine.x;
	f64 farTimeY = aabb.max.y - line.m_pt0.y / dLine.y;
	
	//swap the near and far times if they are inverted
	if (fabs(nearTimeX) > fabs(farTimeX)) { std::swap(nearTimeX, farTimeX); }
	if (fabs(nearTimeY) > fabs(farTimeY)) { std::swap(nearTimeY, farTimeY); }
	
	f64 nearTime = (fabs(nearTimeX) < fabs(nearTimeY)) ? nearTimeY : nearTimeX;
	//f64 farTime = (fabs(farTimeX) > fabs(farTimeY)) ? farTimeY : farTimeX;

	//if near time is within the time taken to go from p0 to p1 of line segment, represented by 0 to 1
	//too big, point p0 is traveling towards AABB but does not collide before reaching p1
	//if too small, p0 will never collide with AABB
	//Only check nearTime as we don't care about p0 exiting collision.
	if (1 >= nearTime && 0 <= nearTime) { return true; }

	//Step 2: Use boundary condition of points to check?
	f64 tFirst{dt+1.0f};	//time of first intersection.
	
	//find np0 for all corners of our AABB
	Math::Vec2 aabbCorners[4];
	aabbCorners[0] = aabb.max;
	aabbCorners[1] = Math::Vec2{aabb.max.x, aabb.min.y};
	aabbCorners[2] = aabb.min;
	aabbCorners[3] = Math::Vec2{ aabb.min.x, aabb.max.y };
	
	for (int i{}; i < 4; ++i) 
	{//for each corner of the AABB
		//store our start and end of movement points projections to line segment normal for each corner
		f64 startProj { Math::dot(line.m_normal, line.m_pt0 - aabbCorners[i]) },
			endProj	  { Math::dot(line.m_normal, line.m_pt0 - (aabbCorners[i] + vel1)) };
		if (startProj * endProj < 0) 
		{	//signs of the projection changed, point moved across the line segment.
			//take the shortest time to intersect. all calulations assume velocity is applied over 1 second.
			tFirst = (startProj < tFirst) ? fabs(startProj)/(fabs(startProj)+fabs(endProj)) : tFirst;
		}
	}

	return (tFirst > dt)? false : true ;
}
#endif
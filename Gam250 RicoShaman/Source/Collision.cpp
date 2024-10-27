/******************************************************************************/
/*!
\file		Collision.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	September 24, 2023
\brief		Source file containing collision functions declared in Collision.h

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/

//#include "main.h"
#include "Pch.h"
#include "Math.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Collision.h"

//#include "Matrix3x3.h"
#include <Maths/Math_Includes.h>
#include "CollisionECS.h"

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
void BuildLineSegment(COL::LineSegment &lineSegment,
	const Math::Vec2& p0,
	const Math::Vec2& p1)
{
	// assign the line segment's start and end points
	lineSegment.m_pt0 = p0;
	lineSegment.m_pt1 = p1;
	//find the vector of the line segment
	Math::Vec2 p0p1 = p1 - p0;
	//find normalized normal of p0p1
	lineSegment.m_normal = Math::normalize(Math::Vec2{p0p1.y, -p0p1.x});
}

/******************************************************************************/
/*!
*	\brief
*		checks for collision along the body of the line segment against a disk.
*		assumes dt applied to Velocity outside of function
*	\param [in] circle
*		the circle to check collision of
*	\param [in] ptEnd
*		the end point of the circle's line of travel
*	\param [in] lineSeg
*		the line segment to check against the circle's line of travel
*	\param [out] interPt
*		returned by reference the point of intersection should a collision occur
*	\param [out] normalAtCollision
*		returned by reference the reflected normal at the point of collision
*	\param [out] interTime
*		returned by reference ti, the scale of the circle's magnitiude of travel
*		when the circle collided with the line segment
*	\param [in] checkLineEdges
*		whether this function should check collision against the edges of the line
*		segment if the movement checks against the body of the line segments were
*		inconclusive.
*	\return
*		1 if collision occured, 0 if no collision was found
 */
/******************************************************************************/
int CollisionIntersection_CircleLineSegment(const COL::Circle &circle,
	const Math::Vec2 &ptEnd,
	const COL::LineSegment &lineSeg,
	Math::Vec2 &interPt,
	Math::Vec2 &normalAtCollision,
	float &interTime,
	bool & checkLineEdges)
{
	//approaching from LHS
	if (Math::dot(lineSeg.m_normal, circle.m_center)
	- Math::dot(lineSeg.m_normal, lineSeg.m_pt0) <= -circle.m_radius)
	{
		COL::LineSegment lsPrime{};
		//updated line segment
		BuildLineSegment(lsPrime,
		lineSeg.m_pt0 - circle.m_radius*lineSeg.m_normal,
		 lineSeg.m_pt1 - circle.m_radius*lineSeg.m_normal);

		//create vectors required for this section's calculations 
		Math::Vec2 bsp0{ lsPrime.m_pt0 - circle.m_center },	//bsp0, circle center to start of line segment
			bsp1{ lsPrime.m_pt1 - circle.m_center },			//bsp1, circle center to end of line segment
			bsbeN{ ptEnd - circle.m_center };					//bsbeN, the normalized normal of the circle's path of travel
		bsbeN = { bsbeN.y,-bsbeN.x };	//find and assign bsbe's normal vector
		bsbeN = Math::normalize(bsbeN);

		//if angle of movement may NOT result in collision(along the main line segment), request an edge check and return
		if ((Math::dot(bsbeN,bsp0 ) * Math::dot(bsbeN, bsp1))
		>= 0) {
			if (checkLineEdges) {
				return	CheckMovingCircleToLineEdge(false, circle, ptEnd,
					lineSeg, interPt, normalAtCollision,
					interTime);
			}; return 0;
		}
		Math::Vec2 bsbe{ ptEnd - circle.m_center };			//bsbe, the circle's path of travel
		//ls prime should have the same normal as lineSeg.  sub radius here to move radius closer to the disk on the LHS of p0p1,
		// essentially making this a line segment of radius thickness against point collision
		//to find the scale of magnitude of the line of travel by the circle when the circle collides with the line segment body, 
		//make (p0.lsNormal - bs.lsNormal - radius) / bsbe.lsNormal. by dot product lsNormal, both vectors become the same scale.
		// for neumarator, find projection p0b1 along the normal of p0p1, subtracting radius to get the intersection magnitiude of 
		// bsbe at point of intersection. By making it a ratio to bsbe projected along p0p1's normal, we can find the magnitutde of 
		// the travel vector compared to its full travel magnitude at time of first intersection
		//musings: to be fair, if we did it without subing radius, we could do it outside with radius/n.bs if we store r/n.bs in a variable.
		//		   still, more calculations
		float ti{ (Math::dot(lsPrime.m_normal,lineSeg.m_pt0)
			- Math::dot(lsPrime.m_normal,circle.m_center) - circle.m_radius)
		/ Math::dot(lsPrime.m_normal,bsbe) };

		interTime = ti;		//pass intersection time into the return by reference variable.
		//if ti is not within this frame, return
		if (ti < 0 || ti > 1) { return 0; }
		//otherwise, find intersection point and reflected normal of the collision
		interPt = circle.m_center + (bsbe)*ti;
		normalAtCollision = -lsPrime.m_normal;
		return 1;
	}
	//approaching from RHS
	else if (Math::dot(lineSeg.m_normal, circle.m_center)
	- Math::dot(lineSeg.m_normal, lineSeg.m_pt0)
	>= circle.m_radius)
	{
		COL::LineSegment lsPrime{};
		//updated line segment
		BuildLineSegment(lsPrime, lineSeg.m_pt0 + lineSeg.m_normal * circle.m_radius, lineSeg.m_pt1 + lineSeg.m_normal * circle.m_radius);
		
		//create vectors required for this section's calculations 
		Math::Vec2 bsp0{ lsPrime.m_pt0 - circle.m_center },	//bsp0, circle center to start of line segment
			bsp1{ lsPrime.m_pt1 - circle.m_center },			//bsp1, circle center to end of line segment
			bsbeN{ ptEnd - circle.m_center };					//bsbeN, the normalized normal of the circle's path of travel
		bsbeN = { bsbeN.y,-bsbeN.x };	//find and assign bsbe's normal vector
		bsbeN = Math::normalize(bsbeN);
		
		//if angle of movement may NOT result in collision(along the main line segment), request a corner check and return
		if (Math::dot(bsbeN, bsp0) * Math::dot(bsbeN, bsp1)
			>= 0) {
			if (checkLineEdges)
			{
				return	CheckMovingCircleToLineEdge(false, circle, ptEnd,
					lineSeg, interPt, normalAtCollision,
					interTime);
			} return 0;
		}
		Math::Vec2 bsbe{ ptEnd - circle.m_center };
		//ls prime should have the same normal as lineSeg. add radius here to move radius closer to the disk on the RHS of p0p1, 
		// essentially making this a line segment of radius thickness against point collision
		//To find the scale of magnitude of the line of travel by the circle when the circle collides with the line segment body, 
		//make (p0.lsNormal - bs.lsNormal + radius) / bsbe.lsNormal. by dot product lsNormal, both vectors become the same scale.
		// for neumarator, find projection p0b1 along the normal of p0p1, subtracting radius to get the intersection magnitiude of 
		// bsbe at point of intersection. By making it a ratio to bsbe projected along p0p1's normal, we can find the magnitutde of 
		// the travel vector compared to its full travel magnitude at time of first intersection
		float ti{ (Math::dot(lsPrime.m_normal,lineSeg.m_pt0)
			- Math::dot(lsPrime.m_normal,circle.m_center) + circle.m_radius)
		/ Math::dot(lsPrime.m_normal,bsbe) };
		interTime = ti;
		//if ti is not within this frame, return
		if (ti < 0 || ti > 1) { return 0; }
		interPt = circle.m_center + (bsbe)*ti;
		normalAtCollision = lsPrime.m_normal;
		return 1;
	}
	else 
	{	//if bsp0 is within radius, circle starting position is aligned with line segment's direction.
		//Only edge check required
		if (checkLineEdges)
		{	//check against edges when required.
			return CheckMovingCircleToLineEdge(true,circle,ptEnd,
					lineSeg,interPt,normalAtCollision,
					interTime);
		}
	}
	return 0; // no intersection, clearly unreachable
}

/******************************************************************************/
/*!
* *	\brief
*		checks for collision along the edges of the line segment against a disk.
*	\param [in] withinBothLines
*		if the circle starting position is between the infinite extension of the line 
		segment when it has a thickness of 2 radius to either of its faces
*	\param [in] circle
*		the circle to check collision of
*	\param [in] ptEnd
*		the end point of the circle's line of travel
*	\param [in] lineSeg
*		the line segment to check against the circle's line of travel
*	\param [out] interPt
*		returned by reference the point of intersection should a collision occur
*	\param [out] normalAtCollision
*		returned by reference the reflected normal at the point of collision
*	\param [out] interTime
*		returned by reference ti, the scale of the circle's magnitiude of travel
*		when the circle collided with the line segment
*	\return 
*		1 if collision occured, 0 if no collision was found
*/
/******************************************************************************/
int CheckMovingCircleToLineEdge(bool withinBothLines,
	const COL::Circle &circle,
	const Math::Vec2 &ptEnd,
	const COL::LineSegment &lineSeg,
	Math::Vec2 &interPt,
	Math::Vec2 &normalAtCollision,
	float &interTime)
{	// find vectors bsbe(path of travel of the circle), Vn(the vector of bsbe normalized)
	Math::Vec2 bsbe{ptEnd - circle.m_center}, Vn;
	Vn = Math::normalize(bsbe);
	// your code goes here
	if (withinBothLines) 
	{ //When it�s true, is to say that Bs is starting from between both imaginary lines
		Math::Vec2 bsp0{lineSeg.m_pt0 - circle.m_center},	//bsp0, the vector from the circle's starting position to the start of the line segment
			p0p1{lineSeg.m_pt1 - lineSeg.m_pt0};				//p0p1, the vector of the linesegment's start to end point
		float m{ Math::dot(bsp0, p0p1) };		//m, supposed to be for a later check. currently used to check directionality of the path of movement
																//use this directionality to find which side might be first to collide along the path of travel
		if ( m > 0)	//p0 side
		{	//make m the directional check if the current path of travel is towards the line segment.
			// m is the scale of the path of travel when the closest projected distance to the line segment's edge was found
			//musing: would it have been better to simply do this first? considering that p0 and p1 would be at the same direction of the circle,
			//		 doing this check first would save needing to check which point is closer to colliding if the circle has a vector that makes
			//		 it unable to collide with the line segment. This should be possible as the circle must be aligned to the line segment to
			//		 reach this point in the code, thus bsp0 and bsp1 must have the same directionality. will fail if circle can phase through line
			//		 segments
			m = Math::dot(bsp0, Vn);
			if ( m <= 0) { return 0; }	//circle will never reach the line segment on its direction of movement

			//find normal of the path of movement(bsbeN), and normalized version of it(M)
			Math::Vec2 bsbeN{ bsbe.y,-bsbe.x }, M;
			M = Math::normalize(bsbeN);

			//find dist 0, the closest point of the line of travel infinitly extended to the line segment's closest point of intersection
			float dist0 { Math::dot(bsp0, M)};	//project circle's starting point to line segment's closest point onto the normalized normal of travel path
			if (abs(dist0) > circle.m_radius) { return 0; }		//if the closest perpendicular distance magnitude is larger than the radius, path will never collide with closest point on line segment
			//find half the intersected length of the path of travel where circle will intersect with the line segment
			float s{ sqrt(circle.m_radius * circle.m_radius - dist0 * dist0) };
			//find ti(magnitude of the travel path when intersection occurs) by finding m - s (the smallest scale of travel path for any collision to occur)
			//over the scale of the current travel path.
			float ti{ (m - s) / Math::length(bsbe) };
			interTime = ti;
			if (ti <= 1) //if colliding in this frame
			{	//find the closest intersection point and the reflected normal of the collision
				interPt = circle.m_center + bsbe * ti;
				normalAtCollision = Math::normalize(interPt - lineSeg.m_pt0);
				return 1;
			}
			return 0;	//not colliding this frame, return no collision
		}
		else	//p1 side
		{	
			//bsp1, the vector of the starting point of travel to the closest line edge
			Math::Vec2 bsp1{ lineSeg.m_pt1 - circle.m_center };
			//make m the directional check if the current path of travel is towards the line segment.
			// m is the scale of the path of travel when the closest projected distance to the line segment's edge was found
			//musing: would it have been better to simply do this first? considering that p0 and p1 would be at the same direction of the circle,
			//		 doing this check first would save needing to check which point is closer to colliding if the circle has a vector that makes
			//		 it unable to collide with the line segment. This should be possible as the circle must be aligned to the line segment to
			//		 reach this point in the code, thus bsp0 and bsp1 must have the same directionality. will fail if circle can phase through line
			//		 segments
			m = Math::dot(bsp1, Vn);
			if (m <= 0) {return 0;}	//circle will never reach the line segment on its direction of movement
			
			//find normal of the path of movement(bsbeN), and normalized version of it(M)
			Math::Vec2 bsbeN{ bsbe.y,-bsbe.x }, M;
			M = Math::normalize(bsbeN);

			float dist1{ Math::dot(bsp1, M) };// project circle's starting point to line segment's closest point onto the normalized normal of travel path
			if (abs(dist1) > circle.m_radius) { return 0; }		//if the closest perpendicular distance magnitude is larger than the radius, path will never collide with closest point on line segment
			//find half the intersected length of the path of travel where circle will intersect with the line segment
			float s{ sqrt(circle.m_radius * circle.m_radius - dist1 * dist1) };
			//find ti(magnitude of the travel path when intersection occurs) by finding m - s (the smallest scale of travel path for any collision to occur)
			//over the scale of the current travel path.
			float ti{ (m - s) / Math::length(bsbe) };
			interTime = ti;
			if (ti <= 1)//if colliding in this frame
			{	//find the closest intersection point and the reflected normal of the collision
				interPt = circle.m_center + bsbe * ti;
				normalAtCollision = Math::normalize(interPt - lineSeg.m_pt1);
				return 1;
			}
			return 0;//if not colliding in this frame
		}
		//return 0;//not colliding this frame, return no collision
	}
	else	//not within both primes
	{
		bool p0side{false};		//start assuming the edge to check against is p1
		Math::Vec2 bsp0{ lineSeg.m_pt0 - circle.m_center },	//bsp0, circle start to line segment start
			bsp1{ lineSeg.m_pt1 - circle.m_center },			//bsp1, circle start to line segment end
			p0p1{ lineSeg.m_pt1 - lineSeg.m_pt0 },				//p0p1, the line segment's vector
			bsbeN{ ptEnd - circle.m_center },					//bsbeN, the normal of the path of travel
			M;													//M, normalized bsbeN
		bsbeN = { bsbeN.y,-bsbeN.x };	//find and assign bsbe's normal vector
		M = Math::normalize(bsbeN);

		//find the smallest perpendicular distance to both edges, along with their magnitudes
		float dist0{ Math::dot(bsp0, M) },
			dist1{ Math::dot(bsp1, M) },
			d0abs{ abs(dist0) },
			d1abs{ abs(dist1) };
		if (d0abs > circle.m_radius && d1abs > circle.m_radius) 
		{	//if the infinately extended path will never be collide with the edges 
			return 0;
		}
		else if (d0abs <= circle.m_radius && d1abs <= circle.m_radius) 
		{	//if the infinately extended path will collide with the Both edges at some point,
			//forced to compare the magnitudes(distance) of both point from circle starting distance
			float m0{ Math::dot(bsp0, Vn) },
				m1{ Math::dot(bsp1, Vn) },
				m0abs{ abs(m0) },
				m1abs{ abs(m1) };
			if (m0abs < m1abs) 
			{p0side = true;}	//check if p0 is closer to the start
		}
		else if (d0abs<=circle.m_radius) { p0side = true; }	//if only p0 is closer, make code checck p0 side

		//depending on edge to check, check ti
		if (p0side) 
		{
			//make m the directional check if the current path of travel is towards the line segment.
			// m is the scale of the path of travel when the closest projected distance to the line segment's edge was found
			float m{ Math::dot(bsp0,Vn) };
			if (m < 0) { return 0; }	//circle does not move in the direction of the intersection point
			
			//find half the intersected length of the path of travel where circle will intersect with the line segment
			float s{ sqrt(circle.m_radius * circle.m_radius - dist0 * dist0) };
			//find ti(magnitude of the travel path when intersection occurs) by finding m - s (the smallest scale of travel path for any collision to occur)
			//over the scale of the current travel path.
			float ti{ (m - s) / Math::length(bsbe) };
			interTime = ti;
			if (ti <= 1)
			{	//if colliding in this frame, calculate intersection point and reflected normal of collision
				interPt = circle.m_center + bsbe * ti;
				normalAtCollision = Math::normalize(interPt - lineSeg.m_pt0);
				return 1;	//return collision
			}
			return 0;	//scale of magnitiude of path travel when intersect is more than distance traveled this frame, return no collision
		}
		else 
		{
			//make m the directional check if the current path of travel is towards the line segment.
			// m is the scale of the path of travel when the closest projected distance to the line segment's edge was found
			float m{ Math::dot(bsp1,Vn) };
			if (m < 0) { return 0; }	//circle does not move in the direction of the intersection point
			
			//find half the intersected length of the path of travel where circle will intersect with the line segment
			float s{ sqrt(circle.m_radius * circle.m_radius - dist1 * dist1) };
			//find ti(magnitude of the travel path when intersection occurs) by finding m - s (the smallest scale of travel path for any collision to occur)
			//over the scale of the current travel path.
			float ti{ (m - s) / Math::length(bsbe) };
			interTime = ti;
			if (ti <= 1)
			{	//if colliding in this frame, calculate intersection point and reflected normal of collision
				interPt = circle.m_center + bsbe * ti;
				normalAtCollision = Math::normalize(interPt - lineSeg.m_pt1);
				return 1;//return collision
			}
			return 0;	//scale of magnitiude of path travel when intersect is more than distance traveled this frame, return no collision
		}

	}
	//return 0;//no collision, unreachable
}

/******************************************************************************/
/*!
* *	\brief
*		checks for collision along the body of the line segment against a disk.
*	\param [in] ptInter
*		the point of intersection should a collision occur
*	\param [in] normal
*		the reflected normal at the point of collision
*	\param [in/out] ptEnd
*		the end point of the circle's line of travel.
*		return by reference the new end point after reflection has occured
*	\param [out] reflected
*		the returned by reference vector of movement after the reflection
*/
/******************************************************************************/
void CollisionResponse_CircleLineSegment(const Math::Vec2 &ptInter,
	const Math::Vec2 &normal,
	Math::Vec2 &ptEnd,
	Math::Vec2 &reflected)
{
	// bibe, path of travel after collision. aka the pentration vector
	Math::Vec2 bibe{ ptEnd - ptInter };
	float nbibe{ Math::dot(normal,bibe) };	//the magnitude of the perpendicular distance traveled by the penetration vector with respects to the reflected normal
	ptEnd += -(nbibe* normal*2);	//find the end point of the reflection of bibe, and return that point by reference
	reflected = Math::normalize((ptEnd-ptInter));		//find the new direction vector of the reflected path of motion.
}

bool CollisionIntersection_CircleCircle(const COL::Circle& circle1,			//Circle data - input
	const Math::Vec2& vel1,			//Circle 1's velocity
	const COL::Circle& circle2,			//Circle data - input
	const Math::Vec2& vel2,			//Circle 2's velocity,
	f64 dt							//delta time
) 
{
	//step 1: check static collision
	//if the squared distance of the circles at the start are smaller or equal to 
	//the squared distance of the circle's combined radius, circles have static collision 
	if (Math::distanceSquared(circle2.m_center, circle1.m_center)
		<= (circle1.m_radius + circle2.m_radius) * (circle1.m_radius + circle2.m_radius))
	{return true;}	

	//step 2: do circle point collision with a static circle that has a combined radius of both circles
	// vb = relative velocity of both circles
	Math::Vec2 vb;				//relative velocity store
	vb.x = vel2.x - vel1.x;		//relative velocity in x axis
	vb.y = vel2.y - vel1.y;		//relative velocity in y axis
	if (!vb.x && !vb.y)
		return false;			//if no velocity at all, short cut to falses as neither object will ever collide

	//circle2Line = make the second circle to a line represeting circle 2's center as it moves along the relative velocity. 
	// Scaled to delta time. will be unscaled if this function is required to find intersection point
	COL::LineSegment circle2Line;
	BuildLineSegment(circle2Line, circle2.m_center, circle2.m_center + vb * (float)dt);

	//make a circle at circle 1 with the combined radius of both circles
	COL::Circle circleComb;
	circleComb.m_center = circle1.m_center;
	circleComb.m_radius = circle1.m_radius + circle2.m_radius;

	//do n.p0 - n.p1 for closes distance across point travel.
	//shortestDist = perpendicular distance from circle to line 
	f64 shortestDist = Math::dot(circle2Line.m_normal, circle2Line.m_pt0 - circleComb.m_center)
		- Math::dot(circle2Line.m_normal, circle2Line.m_pt1 - circleComb.m_center);
	//if the shortest distance is less than the combined radius of both circles, collision must have occured.
	return (fabs(shortestDist) < circleComb.m_radius) ? true : false;
}


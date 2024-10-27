/******************************************************************************/
/*!
\file		AABB.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	September 24, 2023
\brief		Header file containing function declaration related to Continous 
			AABB collisions. functions are not yet refactored.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifdef ANC_CONTINOUS_COL	//only allow this if continous collision was mandated
#ifndef AABB_COLLISION_H_
#define AABB_COLLISION_H_

#include "Maths/Geometric.h"
#include "CollisionECS.h"

/**************************************************************************/
/*!
\struct AABB
\brief 
	a struct holing the points required for AABB colisions
	*/
/**************************************************************************/
#if 0	//Already defined in CollisionECS.h
struct AABB
{
	//AEVec2	c; // center
	//float  r[2]; // holds half width and half height
	
	Math::Vec2	min;
	Math::Vec2	max;
};
#endif
//aliases for readability
namespace COL = ANC::ECS;

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
\param [in] dt
		the delta time (time passed since last rendered frame).
\return
		If a collision occured, true. If not, false.
*/
/**************************************************************************/
bool CollisionIntersection_RectRect(const COL::AABB & aabb1, const Math::Vec2& vel1,
									const COL::AABB & aabb2, const Math::Vec2& vel2,
									f64 dt);

/**************************************************************************/
/*!
\brief	Check the collisions between 2 AABB objects. Checks for static collison
		,then checks for dynamic collision using axis aligned intersections.
		if checking for static and dynamic, set the appropriate velocity param
		to a vector 2D of {0,0}.
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
	const COL::Circle& circle, const Math::Vec2& vel2,f64 dt);

/**************************************************************************/
/*!
\brief	Check the collisions between an AABB and a Line Segment. 
		Checks for static collison
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
	const COL::LineSegment& line, f64 dt);
#endif // AABB_COLLISION_H_

#endif// ANC_CONTINOUS_COL
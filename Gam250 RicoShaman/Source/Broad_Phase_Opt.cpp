/******************************************************************************/
/*!
\file		Broad_Phase_Opt.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	November 18, 2023
\brief		Header file containing Broad Phase Optimization Extenstions definition
			for discrete collisions.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "PCH.h"
#include "Broad_Phase_Opt.h"
#include <cmath>
#include <array>
#include <utility>

//Unused Includes
//#include "ECS/BaseSystem.h"
//#include "Transform.h"
//#include "ECS/Components/Collider2D.h"
//#include "PhysicsECS.h"

//#include "ECS/EntityManager.h"
//#include "Engine/SceneManager.h"

namespace ANC
{
namespace ECS
{
	//=============================================================================
	//\brief
	//	Get the min and max bounds AABB style from a tranform and collider component
	//\param [in] type
	//	the type of the collider
	//\param [in] col
	//	the collider to find the bounds of
	//\param [in] trans
	//	the transform with the position of the collider
	//\return
	//	A pair denoting the min and max bounds of the collider
	//=============================================================================
	std::pair<Math::Vec2, Math::Vec2> GetBounds
	(COLLIDER_TYPE type, Collider const& col, Transform_Component const& trans)
	{
		Math::Vec2 offset;	//use to act like AABB half for circle
		bool lineMinX, lineMinY;
		switch (type)
		{	//find the bounds given the type of the collider
		case ANC::ECS::COLLIDER_TYPE::RECT:
			return { trans.GetPosition() + col.rect.min
					,trans.GetPosition() + col.rect.max};
		case ANC::ECS::COLLIDER_TYPE::CIRCLE:
			offset = { col.circle.m_radius,col.circle.m_radius };
			return { trans.GetPosition() + col.circle.m_center + (offset * -1.0f)
					, trans.GetPosition() + col.circle.m_center + offset };
		case ANC::ECS::COLLIDER_TYPE::LINE:		//pt0 is assumed to always be left of the line
			lineMinX = col.line.m_pt0.x > col.line.m_pt1.x;		//true if pt1 is minimum in X axis
			lineMinY = col.line.m_pt0.y > col.line.m_pt1.y;		//true if pt1 is minimum in Y axis
			return { {trans.GetPosition().x + (lineMinX) ? col.line.m_pt1.x : col.line.m_pt0.x
			,trans.GetPosition().y + (lineMinY) ? col.line.m_pt1.y : col.line.m_pt0.y}			//set min bounds
			,{trans.GetPosition().x + (lineMinX) ? col.line.m_pt0.x : col.line.m_pt1.x
			,trans.GetPosition().y + (lineMinY) ? col.line.m_pt0.y : col.line.m_pt1.y} };		//set max bounds
		default:	//unknown collider
			break;
		}
		return {};	//for unkown collider, return a default set of bounds
	}

	//=============================================================================
	//\brief
	//	round the bounds of a floating point type into the closest long integer number.
	//\param [in] ref
	//	the value to be rounded
	//\return
	//	the value rounded to it's closest long integer number.
	//=============================================================================
	long Round(f64 ref) 
	{
		ref += 0.5f - (ref < 0);		//towards closest integer 
		return static_cast<long>(ref);
	}

	long RoundDown(f64 ref)
	{
		ref += -(ref < 0);				//always towards closest integer to -inf
		return static_cast<long>(ref);
	}

	long RoundUp(f64 ref)
	{
		ref += (ref < 0);				//always towards closest integer to +inf
		return static_cast<long>(ref);
	}

	DCC ConvertFromPtr(DCC_PTR && colList) 
	{
		DCC retVal;
		for (auto& t : colList)
		{
			auto const& [lhs, lhsTrans, lhsPhy] = t;
			retVal.emplace_back(*lhs,*lhsTrans,*lhsPhy);
		}
		return retVal;
	}

	//BroadPhase check returning pair indexes that are potentially colliding
	COL_PAIR_LIST BroadPhaseCheck(DCC& colList, long interval)
	{	//apply AABB logic to Bounds for Broad Phase
		COL_PAIR_LIST retVec;		//the return list
		if (interval < 1) { return retVec; }
		size_t lhsIndex{ 0 };			//store the lhs index for iteration
		for (auto& t : colList)
		{	//for all tuple representing Entities that require collision check,
			auto const& [lhs, lhsTrans, lhsPhy] = t;
			std::pair<Math::Vec2, Math::Vec2> lhsBounds{ GetBounds(lhs.GetType(),lhs.GetCollider(),lhsTrans) };
			
			//find the bounds of the lhs collider
			std::array<long, 2> lhsBoundX{ RoundDown(lhsBounds.first.x) / interval,RoundUp(lhsBounds.second.x) / interval },
				lhsBoundY{ RoundDown(lhsBounds.first.y) / interval,RoundUp(lhsBounds.second.y) / interval };
#if 0
			std::cout << "Min: (" << lhsBoundX[0] << "," << lhsBoundY[0] << ") , ";
			std::cout << "Max: (" << lhsBoundX[1] << "," << lhsBoundY[1] << ") \n";
#endif
			size_t rhsIndex{ 0 };		//track rhs indexes
			for (auto& u : colList)
			{//for each rhs tuple representing Entities that have to be compared for collision checks,
				if (rhsIndex <= lhsIndex) { ++rhsIndex; continue; }		//avoid self collider and repeat checks
				//apply auto binding and find bounds in X axis
				auto const& [rhs, rhsTrans, rhsPhy] = u;
				std::pair<Math::Vec2, Math::Vec2> rhsBounds{ GetBounds(rhs.GetType(),rhs.GetCollider(),rhsTrans) };

#ifdef ANC_BPO_OPTIMIZE_LESS_PAIRS	//more collision pairs, less processes. Apply Bounds checks on a AABB type bounds.
				//use compile time if to switch between alogrithms for checks.
				//bounds check operation here
				//check on x axis bounds first!
				std::array<long, 2> rhsBoundAxis{ RoundDown(rhsBounds.first.x) / interval,RoundUp(rhsBounds.second.x) / interval };
				if (((rhsBoundAxis[1] + 1) < (lhsBoundX[0] - 1))			//max bounds in X-axis of rhs is smaller than the min bounds of lhs
					|| ((rhsBoundAxis[0] - 1) > (lhsBoundX[1] + 1)))		//min bounds in X-axis of rhs is larger than the max bounds of lhs
				{++rhsIndex; continue;}

				//check on y axis bounds if x is not definitive.
				rhsBoundAxis[0] = RoundDown(rhsBounds.first.y) / interval;
				rhsBoundAxis[1] = RoundUp(rhsBounds.second.y) / interval;

				if (((rhsBoundAxis[1] + 1) < (lhsBoundY[0] - 1))			//max bounds in Y-axis of rhs is smaller than the min bounds of lhs
					|| ((rhsBoundAxis[0] - 1) > (lhsBoundY[1] + 1)))		//min bounds in Y-axis of rhs is larger than the max bounds of lhs
				{++rhsIndex; continue;}

				//collision only past this point! add it to vector, increment the index and move on to next tuple.
				retVec[lhsIndex].insert(rhsIndex);
				++rhsIndex;
#else	//more collision pairs, less processes. applies sweep on 2 axis seperately
				//bounds check operation here
//check on x axis bounds first!
				std::array<long, 2> rhsBoundAxis{ RoundDown(rhsBounds.first.x) / interval,RoundUp(rhsBounds.second.x) / interval };
				if ((lhsBoundX[0] < rhsBoundAxis[0] && lhsBoundX[1] > rhsBoundAxis[0])			//min bounds in X-axis of rhs is in bounds of lhs
					|| (lhsBoundX[0] < rhsBoundAxis[1] && lhsBoundX[1] > rhsBoundAxis[1]))		//max bounds in X-axis of rhs is in bounds of lhs
				{retVec[lhsIndex].insert(rhsIndex); ++rhsIndex; continue;}

				//check on y axis bounds if x is not definitive.
				rhsBoundAxis[0] = RoundDown(rhsBounds.first.y) / interval;
				rhsBoundAxis[1] = RoundUp(rhsBounds.second.y) / interval;

				if ((lhsBoundY[0] < rhsBoundAxis[0] && lhsBoundY[1] > rhsBoundAxis[0])			//min bounds in Y-axis of rhs is in bounds of lhs
					|| (lhsBoundY[0] < rhsBoundAxis[1] && lhsBoundY[1] > rhsBoundAxis[1]))		//max bounds in Y-axis of rhs is in bounds of lhs
				{retVec[lhsIndex].insert(rhsIndex); ++rhsIndex; continue;}

				//no collision past this point! increment the index and move on to next tuple.
				++rhsIndex;
#endif
			}
			++lhsIndex;	//increment the index and move on to next tuple.
		}
		return retVec;
	}


	/*inline bool DoubleSweep() {}
	inline bool DualAxisSweep() {}*/

#if _DEBUG
	//a sample function to represent how the author expects the functions for broad phase collision would be used.
	void sampleExample() 
	{
		ANC_DCC a{}; Transform_Component b{}; Physics_Component c{true};
		
		DCC vec = { {a,b,c} };	//default all
		//establish all entity that require collisions TODO
		
		ANC_DCS cSys;
		auto retVec = BroadPhaseCheck(vec, 5);				//get all collision pairs from broad phase checks
		for (auto& t : retVec) 
		{	//for all collision pairs
			auto& [lhs, lhsTrans, lhsPhy] = vec[t.first];	//find lhs
			for (auto& u : t.second) 
			{	//of all unique set values for collision pair
				auto& [rhs, rhsTrans, rhsPhy] = vec[u];		//find rhs
				//check collisions and apply response for collision pairs only
				cSys.DetermineCollisionAndResponse(lhs,lhsTrans,lhsPhy,
													rhs,rhsTrans,rhsPhy);
				//add on collision call back here when implemented.
			}
			
		}
	}
#endif
}//End of ECS namespace
}//End of ANC namespace
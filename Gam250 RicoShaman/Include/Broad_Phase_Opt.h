/******************************************************************************/
/*!
\file		Broad_Phase_Opt.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	November 18, 2023
\brief		Header file containing Broad Phase Optimization Extenstions declaration
			for discrete collisions. 

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "Discrete_Col.h"
#include <tuple>
#include <vector>
//#include <map>
#include <unordered_map>
#define ANC_BPO_OPTIMIZE_LESS_PAIRS

namespace ANC
{
namespace ECS
{	//aliases for compact declarations of some nested containers.
	using DCC = std::vector<std::tuple<ANC_DCC &, Transform_Component &, Physics_Component &>>;
	using Const_DCC = std::vector<std::tuple<ANC_DCC const&, Transform_Component const&, Physics_Component const&>>;
	//ptr based version
	using DCC_PTR = std::vector<std::tuple<ANC_DCC *, Transform_Component *, Physics_Component *>>;
	using Const_DCC_PTR = std::vector<std::tuple<ANC_DCC const*, Transform_Component const*, Physics_Component const*>>;

	using COL_PAIR_LIST = std::unordered_map<size_t,std::unordered_set<size_t>>;
	//unused aliases
	using BOUNDS = std::pair<unsigned int, unsigned int>;
	using BOUNDS_PAIR = std::pair<BOUNDS,BOUNDS>;

	DCC ConvertFromPtr(DCC_PTR&&);

	//=============================================================================
	//\brief
	//	Determines if a pair of colliders have collided in a sweep style broad phase 
	//  implicit grid check.
	//\param [in/out] colList
	//	A vector containing a tuple that references the discrete collider, Transform
	//	component and Physics component of an Entity respectively
	//\param [in] Interval
	//	The interval at which the implicit grid will class the collider bounds in.
	//	i.e. if 10, value of the bounds of the collider with a min bound of (24,19)
	//  will be changed to (2,1). this simplified value will be used to check if 
	//	the min and max bounds of 2 colliders will collide. Larger bounds will result 
	//	in less accurate Broad Phase checks.
	//\return
	//	A map where the index of the tuple in the vector is the key to a set of indexes
	//	that represent the collision pairs for that entity.
	//=============================================================================
	COL_PAIR_LIST BroadPhaseCheck(DCC& colList, long interval);


}//End of ECS namespace
}//End of ANC namespace
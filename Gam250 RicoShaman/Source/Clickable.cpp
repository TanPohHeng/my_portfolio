/******************************************************************************/
/*!
\file		Clickable.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	January 13, 2024
\brief		Source file containing ClickableSystem definition.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "pch.h"
#include "ECS/Components/Clickable.h"
#include <tuple>
#include <vector>
#include "Engine/SceneManager.h"

namespace ANC
{namespace ECS
{

bool ClickableSystem::DetermineCoordWithinClickable(Math::Vec3 const& mouseCoord, Clickable* ref)
{
	if (!ref) { return false; }
	return DetermineCoordWithinClickable(mouseCoord, ref
			, entityMgr.GetComponent<Transform_Component>(ref->GetEntityId()));
}



bool ClickableSystem::DetermineCoordWithinClickable(Math::Vec3 const& mouseCoord, Clickable* ref, Transform_Component* trans, f64 zoomScale)
{
	if (!trans || !ref) { return false; }
	static ANC_DCS checker{};
	/// find bounds of clickable

	///	convert bounds into world coordinates(CreateAABB)
	/// Compare in world space
	/// 
	Transform_Component newTrans{};
	newTrans.SetPosition(mouseCoord);
	newTrans.AddToPosition(trans->GetOffset());

#ifdef _DEBUG
	ANC_Debug::Log_Out a;
	a << "==============================================\n";
	a << "mouse test pos:(" << mouseCoord.x << ", " << mouseCoord.y << ")\nvs\n";
	a << "new mouse pos:    (" << newTrans.position.x << ", " << newTrans.position.y << ")\n";
	a << "object true pos:    (" << trans->position.x << ", " << trans->position.y << ")\n";
	a << " possible collision? [" << checker.D_Check_RectCircle(ref->GetBounds(), *trans,
		Circle(0.1f, { 0,0 }), newTrans) << "]\n";
	a << "==============================================\n";
#endif // DEBUG

	/// rotatable BB and point collision, or AABB and circle of size 1 collision
	/// return result

	return checker.D_Check_RectCircle(ref->GetBounds(), *trans,
		Circle(0.1f / zoomScale, { 0,0 }), mouseCoord);
}

void ClickableSystem::HoverScaleUpdate(Clickable* ref, Transform_Component* trans, bool isEntry)
{
	if (!trans || !ref) { return; }
	HoverScaleUpdate(ref,trans,isEntry, ref->GetScaleFactor());
}

void ClickableSystem::HoverScaleUpdate(Clickable* ref, Transform_Component* trans, bool isEntry, f64 scale)
{
	if (!trans || !ref) { return; }
	if(isEntry)	//if entering a button
		trans->SetScale(trans->GetScale() * scale);	//apply scaling
	else
	{
		if (fabs(scale) > FLT_EPSILON)	//if scale value is larger than epsilon, do division. try avoid math error
		{trans->SetScale(trans->GetScale() / scale);}	//reverse scaling, expect floating point error.
	}
}
}//end of ECS namespace
}//end of ANC namespace

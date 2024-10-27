/******************************************************************************/
/*!
\file		ButtonLogic.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	January 13, 2024
\brief		Header file containing sample button logic interface.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "ECS/Components/Clickable.h"
#include "C2CEvents.h"
#include "Utils/Logger.h"
#include "ECS/BaseSystem.h"
#include <string>

namespace ANC
{namespace ECS
{	//function declaration.
	void DebugHover(std::pair<std::string const&, BaseComponent*> eventKey);
	void DebugClick(std::pair<std::string const&, BaseComponent*> eventKey);
	void DebugRelease(std::pair<std::string const&, BaseComponent*> eventKey);

	//=============================================================================
	// \class Sample button
	// \brief
	//	sample button logic implemtation to be used with Clickable.h. depricated, 
	//	alternate logic via components planned.
	//=============================================================================
	class SampleButton : public BaseSystem 
	{
		//Event keys for this set of button logics
		static inline std::string hoverEvent{"HoverDebug"},
								clickEvent{"ClickDebug"},
								releaseEvent{"ReleaseDebug"};
	public:
		//out of class function for function pointers. change if an inheritable version of the class is established
		friend void DebugHover(std::pair<std::string const&, BaseComponent*>);
		friend void DebugClick(std::pair<std::string const&, BaseComponent*>);
		friend void DebugRelease(std::pair<std::string const&, BaseComponent*>);
		
		SampleButton() 
		{	//subscribe friend functions
			KeywordEvents().Subscribe(DebugHover);
			KeywordEvents().Subscribe(DebugClick);
			KeywordEvents().Subscribe(DebugRelease);
		}

		void Update() override
		{/*Empty by design*/ }

		bool CanOperateOn(EntityID entityId) override
		{
			return (entityMgr.HasComponent<Clickable>(entityId)
				&& entityMgr.HasComponent<Transform_Component>(entityId));
		}
	};

	static inline bool mouseDowned{ false }, start{true}; //track if mouse down and before first click

	void DebugClick(std::pair<std::string const&, BaseComponent*> eventKey) 
	{	//if the event handle does not match the click event handle here, return
		if (eventKey.first.compare(SampleButton::clickEvent)) 
		{return;}

		static ANC_Debug::Log_Out a;
		mouseDowned = true;
		a << "Sample button Clicked!";
	}
	
	void DebugHover(std::pair<std::string const&, BaseComponent*> eventKey) 
	{	//if the event handle does not match the Hover event handle here, return
		if (eventKey.first.compare(SampleButton::hoverEvent))
		{return;}

		static ANC_Debug::Log_Out a;
		a << "Sample button Hover!\n" << SceneMgr.GetEntity(eventKey.second->GetEntityId())->GetName();
		if (mouseDowned) { a << "downed\n"; }
		else if(start == false) { a << "upped\n"; }
	}
	
	void DebugRelease(std::pair<std::string const&, BaseComponent*> eventKey) 
	{	//if the event handle does not match the release event handle here, return
		if (eventKey.first.compare(SampleButton::releaseEvent))
		{return;}

		mouseDowned = false;
		start = false;
		static ANC_Debug::Log_Out a;
		a << "Sample button Release!";
	}

}//end of ANC namespace
}//end of ECS namespace
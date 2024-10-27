/******************************************************************************/
/*!
\file		Clickable.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	January 19, 2024
\brief		Header file containing Component-to-Component keyword based event 
			wrapper interface.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "Utils/Event.h"
#include "ECS/BaseComponent.h"

#include <string>


namespace ANC
{namespace ECS
{	
	//=============================================================================
	// \class C2CEvents
	// \brief
	//	a wrapper class for event class in Event.h
	//=============================================================================
	template <typename T>
	class C2CEvents 
	{
		Event<T> eventHandler;
	public:
		Event<T>& operator()() noexcept
		{
			return eventHandler;
		}
	};

	//singleton keyword event, we shall not support pointer to member functions as Event<T> does not support member function pointer.
	static inline C2CEvents<std::pair<std::string const&, BaseComponent*>> KeywordEvents;

	static inline C2CEvents<std::pair<std::string const&, BaseComponent*>> VictoryDefeatEvents;


}//end of ANC namespace
}//end of ECS namespace
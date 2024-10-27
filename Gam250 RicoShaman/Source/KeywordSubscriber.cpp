#include "pch.h"
#include "KeywordSubscriber.h"
#include "Logics/KeywordSystem.h"
#include <utility>

namespace ANC
{namespace ECS 
{	
	//=================================
	//	KeywordSubscriber functions
	//=================================

	KeywordSubscriber::KeywordSubscriber(std::set<std::string> const& ref)
		: eventsSubscribed{ ref }, BaseComponent{}
	{
		for (auto const& t : eventsSubscribed)
		{//for all events to be subscribed to, 
			SubscribeToEvent(t);
		}
	}
	
	KeywordSubscriber::KeywordSubscriber(std::set<std::string>&& ref)
		: eventsSubscribed{ std::move(ref) }, BaseComponent {}
	{
		for (auto const& t : eventsSubscribed)
		{//for all events to be subscribed to, 
			SubscribeToEvent(t);
		}
	}

	KeywordSubscriber::KeywordSubscriber(std::string const& eventHandle)
		: BaseComponent{}
	{//subscribe to event 
		SubscribeToEvent(eventHandle);
	}
	
	KeywordSubscriber::KeywordSubscriber(KeywordSubscriber const& ref)
	: BaseComponent{}
	{	//use assignment to copy ref
		*this = ref;
	}
	
	KeywordSubscriber& KeywordSubscriber::operator=(KeywordSubscriber const& ref) 
	{
		//clear previous tracking
		for (auto const& t : eventsSubscribed)
		{// t = event handle, for each handle, remove subscription from static tracking
			//erase from static tracking
			UnSubscribeFromTracking(t);
		}
		eventsSubscribed.clear();

		for (auto const& t : ref.eventsSubscribed) 
		{//for all events to be subscribed to, 
			SubscribeToEvent(t);
		}
		BaseComponent* instance = this;
		*instance = *(reinterpret_cast<const BaseComponent*>(&ref));
		return *this;
	}

	KeywordSubscriber::~KeywordSubscriber()
	{
		if(ANC::Core.Run())
		UnSubscribeToAll();
	}

	void KeywordSubscriber::SubscribeToEvent(std::set<std::string> const& toCopy)
	{
		for (auto const& t : toCopy) 
		{//for all strings in the set to copy, sub to the event
			SubscribeToEvent(t);
		}
	}

	//TODO: sub and unsub keyword function
	void KeywordSubscriber::SubscribeToEvent(std::string const& eventHandle) 
	{	//insert this object into the map tracking all subscribed items
		KeywordSubscriber::subscribedList[eventHandle].emplace(GetEntityId(), this);
		//insert into local tracking
		eventsSubscribed.insert(eventHandle);
	}
	
	void KeywordSubscriber::UnSubscribeToEvent(std::string const& eventHandle)
	{	//erase from local tracking
		auto itr{ eventsSubscribed.find(eventHandle) };
		if (itr != eventsSubscribed.end())
			eventsSubscribed.erase(itr);

		//erase from static tracking
		UnSubscribeFromTracking(eventHandle);		
	}	//no effect if event not found

	void KeywordSubscriber::UnSubscribeToAll()
	{
		for (auto const& t : eventsSubscribed)
		{// t = event handle, for each handle, remove subscription from static tracking
			//erase from static tracking
			UnSubscribeFromTracking(t);
		}
		//not required as variable is destroyed. if line uncommented, it is a safety barrier.
		eventsSubscribed.clear();
	}

	void KeywordSubscriber::UnSubscribeFromTracking(std::string const& eventHandle) 
	{	//erase from static tracking
		if (KeywordSubscriber::subscribedList.empty()) { return; }
		if (!KeywordSubscriber::subscribedList.contains(eventHandle))
		{return;}
		auto subItr{ KeywordSubscriber::subscribedList[eventHandle].find(GetEntityId()) };
		if (subItr != KeywordSubscriber::subscribedList[eventHandle].end())
		{	//if subscription exist, erase it from static map
			KeywordSubscriber::subscribedList[eventHandle].erase(subItr);
		}
	}

	//=================================
	//	KeywordSystem functions
	//=================================
	//Add event to static map functions
	void KeywordSystem::AddEvent(ClickableEventLogic const& eventToAdd) 
	{
		allEvents[std::string(eventToAdd.GetEventName())] = eventToAdd;
	}
	
	void KeywordSystem::AddEvent(ClickableEventLogic&& eventToAdd)
	{
		allEvents[std::string(eventToAdd.GetEventName())] = std::move(eventToAdd);
	}

	void KeywordSystem::AddEvent(std::string const& eventHandle, callback ref) 
	{
		if (!ref) { return; }
		ClickableEventLogic temp;
		temp.SetEvent(ref).SetEventName(eventHandle);
		AddEvent(std::move(temp));
	}

	//Handle events through KeywordSystem
	void HandleEvent(std::pair<std::string const&, BaseComponent*> param)
	{
#if 0
		if (KeywordSystem::allEvents.find(param.first) == KeywordSystem::allEvents.end())
		{return;}	//quick return if eventHandle does not match any eventHandles previously stored
#endif		

#ifdef HANDLE_PAUSE_HERE
		if(!param.first.compare("pause"))
			SceneMgr.SetGamePaused(!SceneMgr.IsGamePaused());
#endif // HANDLE_PAUSE_HERE
		if (!KeywordSubscriber::GetSubscribed().contains(param.first)) { return; }
		for (auto const& t : KeywordSubscriber::GetSubscribed()[param.first]) 
		{//for each element in the map with all subscribed keywordsubscribers, <ID, KeywordSubscriber*>
			KeywordSystem::allEvents[param.first](param,t.second);
		}

		for (auto const& t : allClickableEvents) 
		{
			if (t.GetEventName().compare(param.first)) 
			{continue;}
			if (!KeywordSubscriber::GetSubscribed().contains(param.first))
			{break;}

			for (auto const& u : KeywordSubscriber::GetSubscribed()[param.first])
			{
				t(param,u.second);
			}
		}
	}

}//end of ECS namespace
}//end of ANC namespace
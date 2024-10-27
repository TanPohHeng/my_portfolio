#pragma once
#include "ECS/Components/Clickable.h"
#include "ECS/BaseComponent.h"
#include "ECS/BaseSystem.h"
#include "C2CEvents.h"

#include <map>
//#include <unordered_map>
#include <set>
#include <string>


namespace ANC
{namespace ECS
{
	//=============================================================================
	//\brief
	//	Forward declaration of friend function HandleEvent to be wrapped by the 
	//  KeywordSystem class
	//=============================================================================
	void HandleEvent(std::pair<std::string const&, BaseComponent*> param);

	//=============================================================================
	//\class KeywordSubscriber
	//\brief
	//	Marks the entity holding this component as an object to be modified by an 
	//	event call.
	//=============================================================================
	class KeywordSubscriber : public BaseComponent
	{
	private:
		//set for quick referenc of which events were subscribed. To be serialized
		std::set<std::string> eventsSubscribed;
		
		//map of subscribers according to their subscribed keywords, <event name, vector<id, ptr to component> >
		//RAII style management of objects in this map is required. RAII shall prevent any mid scene transition issues.
		//Originally planned to be an unordered_map, however due to an unknown reason, unordered_map will fail to find (access of nullptr) during 
		//destruction when trying to unsubscribe from the static list. this somehow does not occur when using map.
		inline static std::map<std::string, std::map<size_t, KeywordSubscriber*>> subscribedList;
		/*======================================================
		*	Rules of usage. 
		*	if added, insert <eventkey, <entityID, ptr to this>>
		*	if removed, iterate map and erase for all keys <eventkey,<find(entityID)>>>
		*	if second map is empty(size()), erase <eventKey>? is creation of a map every time worth the processes of just leaving this empty?
		*   DO NOT SERIALIZE THIS MAP! use constructors and functions to add items to it when deserializing instead.
		======================================================*/
		
		//function for internal use only!
		void UnSubscribeFromTracking(std::string const&);
	public:
		static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::KEYWORD_SUBSCRIBER;

		COMPONENT_TYPES const GetCompType() override {
			return compType;
		}

		//TODO: RAII management constructors and destructor
		KeywordSubscriber() = default;						//not subscribed ctor
		KeywordSubscriber(std::set<std::string> const&);	//
		KeywordSubscriber(std::set<std::string>&&);
		KeywordSubscriber(std::string const &);
		KeywordSubscriber(KeywordSubscriber const&);
		KeywordSubscriber(KeywordSubscriber&&) = default;
		~KeywordSubscriber() override;

		KeywordSubscriber& operator=(KeywordSubscriber const&);
		KeywordSubscriber& operator=(KeywordSubscriber &&) = default;

		std::set<std::string> const& GetSubscribedEvents() const noexcept { return eventsSubscribed; }
		//For Serializing ONLY!!
		std::set<std::string>& GetSubscribedEvents() noexcept { return eventsSubscribed; }

		//TODO: sub and unsub keyword function
		void SubscribeToEvent(std::set<std::string> const&);	//use this function to deserialize.
		void SubscribeToEvent(std::string const&);
		void UnSubscribeToEvent(std::string const&);	//no effect if event not found
		void UnSubscribeToAll();	//no effect if event not found
		
		//Originally planned to be an unordered_map, however due to an unknown reason, unordered_map will fail to find (access of nullptr) during 
		//destruction when trying to unsubscribe from the static list. this somehow does not occur when using map.
		static std::map<std::string, std::map<size_t, KeywordSubscriber*>>& GetSubscribed() noexcept { return subscribedList; }

		void Reset() override 
		{/*Empty by design, no game time updates expected*/ }

		void Cache() override 
		{/*Empty by design, no game time updates expected*/}

		///=========================================================
		///	CallBacks are stored at system level as internal 
		/// functions, or in another component/class owned by System 
		/// for reference to functions to use
		///=========================================================
		//const_callback GetCallbackFunc() const { return func; } //this shouldn't be an issue/requirement right????
		//callback GetCallbackFunc() const { return func; }
		//KeywordSubscriber& SetEvent(callback ref) { func = ref; return *this; }
	};

	//alias to pointer to function types
	using callback = void(*)(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	using const_callback = void(* const)(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);

	//=============================================================================
	//\class ClickableEventLogic
	//\brief
	//	Clickable Event logic function. This class shall wrap non-class function that
	//	performs an event logic when event is triggered by the Clickable system.
	//	objects of this class will simply wrap always available event logic functions,
	//	and can therefore be considered always valid unless func is nullptr. 
	//	This class shall be used only via KeywordSystem.
	//	This class should not require serialization, as constexpr version is used 
	//	in KeywordSystem
	//=============================================================================
	class ClickableEventLogic 
	{
	private:
		std::string_view eventName {""};
		callback func {nullptr};
	public:
		ClickableEventLogic() = default;
		constexpr ClickableEventLogic(std::string_view name, callback call)
			: eventName{ name }, func{call}
		{/*Empty by design*/ }

		//allow this class to be used as a functor
		void operator()(std::pair<std::string const&, BaseComponent*> param, KeywordSubscriber* ref) const { if(func) func(param,ref); }

		//const_callback GetCallbackFunc() const { return func; } //this shouldn't be an issue/requirement right????
		//Getters and setters
		std::string_view const& GetEventName() const noexcept { return eventName; }
		ClickableEventLogic& SetEventName(std::string ref) { eventName = ref; return *this; }
		callback GetCallbackFunc() const noexcept { return func; }
		ClickableEventLogic& SetEvent(callback ref) { func = ref; return *this; }
	};

#if NOT_USING_CONSTEXPR_EVENT_TRACKING && 0
	//constexpr ClickableEventLogic allEvents[] = {{"str",nullptr}};
	//constexpr std::array<ClickableEventLogic, 1> allEvents{ {{"Debug",DebugEvent}} };

	//=============================================================================
	//\class KeywordSystem
	//\brief
	//	class to apply ClickableEventLogic onto KeywordSubscriber when a Clickable
	//	broadcasts an event via KeywordEvent. only subscribed subscribers are called.
	//=============================================================================
	class KeywordSystem : public BaseSystem
	{
	private:
		//only accessible for this system. Use this single vector of callbacks
		//to set Keyword Subscriber's event callback via 
		//subscriber shall reference them by name.
		// store as <EventHandle, EventObject>
		inline static std::map<std::string,ClickableEventLogic> allEvents{};	
	
	public:
		KeywordSystem() 
		{	//subscribe our handler function to the Keyword events.
			KeywordEvents().Subscribe(HandleEvent);
		}
		//this is a passive logic class
		void Update() override {/*Empty by Design*/};
		//Add event to static map functions
		static void AddEvent(ClickableEventLogic const&);
		static void AddEvent(ClickableEventLogic &&);
		static void AddEvent(std::string const&, callback ref);

		//friend function as our event system does not support pointer to member function.
		friend void HandleEvent(std::pair<std::string const&, BaseComponent*> param);
	};
#endif

}//end of ECS namespace
}//end of ANC namespace
/******************************************************************************/
/*!
\file		Clickable.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	January 13, 2024
\brief		Header file containing Clickable component interface.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Engine/ANC.h"
#include "Utils\Event.h"
//#include <memory>
#include <vector>
#include <string>
#include <bitset>

#include "Discrete_Col.h"

#include "ECS/BaseComponent.h"
#include "ECS/BaseSystem.h"
#include "Engine/ANC_Editor.h"
#include "Engine/Editor/Editor_Scene.h"

#include "Engine/SceneManager.h"
#include "C2CEvents.h"

#include "Engine/ANC_Graphics.h"
#include "Engine/AudioManager.h"


namespace ANC 
{namespace ECS 
{
	//TODO: support ratio based scaling on clickables
	//=============================================================================
	//\class Clickable
	//\brief
	//	Clickable Component allows an object to interact with the mouse in the world
	//	space of the game. Uses C2CEvents to callback subscribed functions using 
	//	a pair of string and a base component pointer. string is used as an event handle
	//	to identify which event occured.
	//=============================================================================
	class Clickable : public BaseComponent
	{	
	public:
		//enum for public access for bitset accessor
		enum SOUND_ID
		{
			ON_CLICK_SOUND = 0,
			ON_RELEASE_SOUND,
			ON_HOVER_TRIGGER,
			ON_HOVER_EXIT
		};


		static constexpr size_t NUM_OF_SFX = 4;
	private:
		//Event handle storing
		std::string HoverEvent{},
					ClickEvent{},
					ReleaseEvent{};

		//edge events for hover
		std::string HoverTriggerEvent{},
					HoverExitEvent{};
		
		//find better way of spitting responsibility if possible
		//click and release sounds file names
		std::string ClickEventSound{},
					ReleaseEventSound{},
					HoverTriggerSound{},
					HoverExitSound{};

		//Collider for mouse check, does not affect discrete collisions
		AABB bounds;
		AABB c_bound;
		f64 scaleFactor{1.0f};

		bool isLogicActive{true};
		bool runWhilePaused{ false };

		bool wasHovered{false};	//whether the button was hovered during the last frame.
		bool wasClicked{false};	//whether the button was Clicked during the last frame.
		bool useScaleAnimations{false};

		//Sound boolean flags
		std::bitset<NUM_OF_SFX> useButtonSFXs{0};
	public:
		static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::CLICKABLE;

		COMPONENT_TYPES const GetCompType() override {
			return compType;
		}

		~Clickable() = default;

		Clickable(AABB collider = AABB()):
			c_bound{ collider }, bounds{collider}
		{/*Empty by Design*/ }

		//getter & setters, trying new formatting
		std::string const& GetHoverEventKeyword() const noexcept		{ return HoverEvent; }
		std::string const& GetClickEventKeyword() const noexcept		{ return ClickEvent; }
		std::string const& GetReleaseEventKeyword() const noexcept		{ return ReleaseEvent; }
		std::string const& GetHoverTriggerEventKeyword() const noexcept	{ return HoverTriggerEvent; }
		std::string const& GetHoverExitEventKeyword() const noexcept	{ return HoverExitEvent; }
		//sounds
		std::string const& GetClickEventSound() const noexcept			{ return ClickEventSound; }
		std::string const& GetReleaseEventSound() const noexcept		{ return ReleaseEventSound; }
		std::string const& GetHoverTriggerEventSound() const noexcept	{ return HoverTriggerSound; }
		std::string const& GetHoverExitEventSound() const noexcept		{ return HoverExitSound; }

		AABB& GetBounds() noexcept							{ return bounds; }
		AABB const& GetBounds() const noexcept				{ return bounds; }
		inline f64 GetScaleFactor() const noexcept			{ return scaleFactor; }
		inline bool GetIsLogicActive() const noexcept		{ return isLogicActive; }
		inline bool GetWhilePaused() const noexcept			{ return runWhilePaused; }
		inline bool GetWasHovered() const noexcept			{ return wasHovered;}
		inline bool GetWasClicked() const noexcept			{ return wasClicked;}
		inline bool GetUseScaleAnims() const noexcept		{ return useScaleAnimations;}
		inline std::bitset<4>& GetUseButtonSFXSet() noexcept{ return useButtonSFXs; }

		//template function to get bitset
		template <size_t index>
		inline bool GetUseButtonSFX() const noexcept		
		{
			if constexpr (index < NUM_OF_SFX) { return useButtonSFXs.test(index);}	//access within bitset
			else { return false; }	//access outside bitset
		}

		template <SOUND_ID index>
		inline std::string const& GetButtonSFX() const		
		{
			switch (index)
			{	//switch for index within the enum values, then return the proper string of the sound 
			case SOUND_ID::ON_CLICK_SOUND:
				return ClickEventSound;
			case SOUND_ID::ON_RELEASE_SOUND:
				return ReleaseEventSound;
			case SOUND_ID::ON_HOVER_TRIGGER:
				return HoverTriggerSound;
			case SOUND_ID::ON_HOVER_EXIT:
				return HoverExitSound;
			}
			throw;
		}

		inline std::string const& GetButtonSFX(SOUND_ID index) const		
		{
			switch (index)
			{	//switch for index within the enum values, then return the proper string of the sound 
			case SOUND_ID::ON_CLICK_SOUND:
				return ClickEventSound;
			case SOUND_ID::ON_RELEASE_SOUND:
				return ReleaseEventSound;
			case SOUND_ID::ON_HOVER_TRIGGER:
				return HoverTriggerSound;
			case SOUND_ID::ON_HOVER_EXIT:
				return HoverExitSound;
			}
			throw;
		}

		inline f64& GetScaleFactor() noexcept { return scaleFactor; }
		
		Clickable& SetHoverKeyword
		(std::string const& key) noexcept							{ HoverEvent = key; return *this; }
		Clickable& SetClickKeyword
		(std::string const& key) noexcept							{ ClickEvent = key; return *this; }
		Clickable& SetReleaseKeyword
		(std::string const& key) noexcept							{ ReleaseEvent = key; return *this; }
		Clickable& SetHoverTriggerKeyword
		(std::string const& key) noexcept							{ HoverTriggerEvent = key; return *this; }
		Clickable& SetHoverExitKeyword
		(std::string const& key) noexcept							{ HoverExitEvent = key; return *this; }
		//sounds
		Clickable& SetClickSound
		(std::string const& key) noexcept							{ ClickEventSound = key; return *this; }
		Clickable& SetReleaseSound
		(std::string const& key) noexcept							{ ReleaseEventSound = key; return *this; }
		Clickable& SetHoverTriggeSound
		(std::string const& key) noexcept							{ HoverTriggerSound = key; return *this; }
		Clickable& SetHoverExitSound
		(std::string const& key) noexcept							{ HoverExitSound = key; return *this; }
		template <SOUND_ID index>
		Clickable& SetButtonSFX(std::string const& key) noexcept
		{
			switch (index)
			{	//switch for index within the enum values, then return the proper string of the sound 
			case SOUND_ID::ON_CLICK_SOUND:
				ClickEventSound = key;
				break;
			case SOUND_ID::ON_RELEASE_SOUND:
				ReleaseEventSound = key;
				break;
			case SOUND_ID::ON_HOVER_TRIGGER:
				HoverTriggerSound = key;
				break;
			case SOUND_ID::ON_HOVER_EXIT:
				HoverExitSound = key;
				break;
			}
			return *this;
		}

		Clickable& SetButtonSFX(SOUND_ID index,std::string const& key) noexcept
		{
			switch (index)
			{	//switch for index within the enum values, then return the proper string of the sound 
			case SOUND_ID::ON_CLICK_SOUND:
				ClickEventSound = key;
				break;
			case SOUND_ID::ON_RELEASE_SOUND:
				ReleaseEventSound = key;
				break;
			case SOUND_ID::ON_HOVER_TRIGGER:
				HoverTriggerSound = key;
				break;
			case SOUND_ID::ON_HOVER_EXIT:
				HoverExitSound = key;
				break;
			}
			return *this;
		}

		//void SetBounds(const AABB& aabb) const { bounds = aabb; }

		Clickable& SetIsLogicActive(bool bflag) noexcept	{ isLogicActive = bflag; return *this; }
		Clickable& SetWhilePaused(bool bflag) noexcept		{ runWhilePaused = bflag; return *this; }
		Clickable& SetHovered(bool bflag) noexcept			{ wasHovered = bflag; return *this; }
		Clickable& SetClicked(bool bflag) noexcept			{ wasClicked = bflag; return *this; }
		Clickable& SetUseScaleAnims(bool bflag) noexcept	{ useScaleAnimations = bflag; return *this; }
		Clickable& SetHoverScale(f64 newSF) noexcept		{ scaleFactor = newSF; return *this; }
		
		//template function to set all bits in bitset
		template<bool bflag>
		Clickable& SetUseButtonSFX() noexcept				
		{	
			if constexpr (bflag)	{useButtonSFXs.set;}	//all true
			else					{useButtonSFXs.reset;}	//all false
			return *this; 
		}
		
		//template function to set specific bits in bitset
		template <size_t index>
		Clickable& SetUseIndexButtonSFX(bool bflag)
		{
			if constexpr (index < NUM_OF_SFX) 
			{
				if(bflag)
					useButtonSFXs.set(index);
				else
					useButtonSFXs.reset(index);
			}	//access within bitset
			return *this;
		}
    
		Clickable& SetUseIndexButtonSFX(size_t index,bool bflag)
		{
			if (index < NUM_OF_SFX)
			{
				if (bflag)
					useButtonSFXs.set(index);
				else
					useButtonSFXs.reset(index);
			}	//access within bitset
			return *this;
		}

		void Reset() override 
		{
			bounds = c_bound;
		};

		void Cache() override 
		{
			c_bound = bounds;
		};
	};

	//=============================================================================
	//\class ClickableSystem
	//\brief
	//	Clickable System. Performs polling type updates to check if the mouse is 
	//  hovering on any Clickable component collider, then uses events to invoke 
	//	subscribers with a event handle(string). if subscriber has matching key,
	//	subscriber should execute the logic required for the event (event logic is 
	//	the responisbility of the subscriber).
	//=============================================================================
	class ClickableSystem: public BaseSystem
	{
		//=============================================================================
		//\brief
		//	Constructor for Discrete_Collider_Component. creates a circle of 0
		//	radius at (0,0) in world position. If no transform is given, searches for the 
		//	entity's
		//\parma [in] mouseCoord
		//	current mouse coordinates in world space
		//\param [in] ref
		//	a pointer to the Clickable component to check with
		//\param [in] trans
		//	a pointer to the Transform component of the Clickable's entity
		//\return 
		//	if the mouse position intersects with the world position of the referenced
		//	Clickable's collider
		//=============================================================================
		bool DetermineCoordWithinClickable(Math::Vec3 const& mouseCoord, Clickable* ref);
		bool DetermineCoordWithinClickable(Math::Vec3 const& mouseCoord, Clickable* ref, Transform_Component* trans, f64 zoomScale = FLT_EPSILON);

		void HoverScaleUpdate(Clickable* ref, Transform_Component* trans, bool isEntry);
		void HoverScaleUpdate(Clickable* ref, Transform_Component* trans, bool isEntry, f64 scale);

		//=============================================================================
		//\brief
		//	Draws AABB collider information visually
		//=============================================================================
		void DrawDebugCollider(AABB const& collider, Transform_Component* transform) {

		// Get the 4 points of a rect.
		Math::Vec3 point0{ transform->position.x - collider.half.x, transform->position.y + collider.half.y, 0 };
		Math::Vec3 point1{ transform->position.x + collider.half.x, transform->position.y + collider.half.y, 0 };
		Math::Vec3 point2{ transform->position.x + collider.half.x, transform->position.y - collider.half.y, 0 };
		Math::Vec3 point3{ transform->position.x - collider.half.x, transform->position.y - collider.half.y, 0 };

		// Draw lines to connect the corners of the rectangle.
		ANC::Graphics.DrawLine(point0, point1, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
		ANC::Graphics.DrawLine(point1, point2, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
		ANC::Graphics.DrawLine(point2, point3, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);
		ANC::Graphics.DrawLine(point3, point0, 1, { 0, 1.0f, 0 }, SORTING_LAYER::DEBUG);

		}

		
	public:
		//workaround non-GSM based scene load function
		inline static bool changeSceneThisFrame{ false };	//global boolean to only load scene after all other updates
		inline static std::string sceneToLoadName{"LVL1.scn"};
		//=============================================================================
		//\brief
		//	Update all Clickable components. invokes events based on updated states of
		//	the components.
		//=============================================================================
		void Update() override
		{
			//========================================================
			//	strange linking error occurs if this function is defined 
			//	outside of the class. Cause of linking error is beyond 
			//	me, and thus update is defined here.
			//========================================================
			static bool wasMouseDown{ false };	//cache to store last mouse state
#ifdef DEBUG
			ANC_Debug::Log_Out a;
			a << "test update\n";
			a << "mouse test pos:(" << ANC::Input.GetMouseScreenPos().x -10 << ", " << ANC::Input.GetMouseScreenPos().y << ")\n";
#endif // DEBUG
			//get clickable objects
			auto clickableList = ANC::SceneMgr.GetEntitiesComponents<Clickable,Transform_Component>();
			auto camera = ANC::SceneMgr.GetMainCamera();
			if (!camera) { ANC_Debug::Debug.Log("Clickable System Update: Failed to obtain scene camera."); return; }
			//ANC::Graphics.DrawPoint(ANC::Input.GetMouseScreenPos(), 50.0F, { 1,0,1 }, SORTING_LAYER::UI);
			//if possible, change these into event based.
			for (auto t : clickableList)
			{	//for each clickable object, 
				auto& [button,trans] = t;
				//if the button logic is not active, skip this update.
				if (!button->GetIsLogicActive()) { continue; }
				if (SceneMgr.IsGamePaused() && !button->GetWhilePaused()) { continue; }
				f64 zoomScale{ (camera->GetZoom() < FLT_EPSILON) ? FLT_EPSILON : camera->GetZoom()};

				bool wasInBounds{ DetermineCoordWithinClickable
					(	//mouse/zoom to get screen scale pos with zoom percent.
						(ANC::SceneMgr.GetMouseGamePos()),
						button, trans, zoomScale
					)};
				if (!wasInBounds) 
				{	//if not in bounds, and if the last frame was still hovered
					if (button->GetWasHovered()) 
					{
						//play hover exit sound
						if (button->GetUseButtonSFX<Clickable::SOUND_ID::ON_HOVER_EXIT>()) 
						{ AudioMgr.PlaySFX(button->GetButtonSFX<Clickable::SOUND_ID::ON_HOVER_EXIT>()); }
						
						//invoke event
						KeywordEvents().Invoke(std::make_pair(button->GetHoverExitEventKeyword(), dynamic_cast<BaseComponent*>(button))); 
						if (button->GetUseScaleAnims())
						{//update hover scale if required by the button
							HoverScaleUpdate(button, trans, false);
							if (button->GetWasClicked()) 
							{
								HoverScaleUpdate(button, trans, false);
							}
						}
						//update button state.
						button->SetHovered(false);
						button->SetClicked(false);
						wasMouseDown = false;
					}
					continue; 
				}
				if (ANC::Input.GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)
					&& !wasMouseDown)
				{	//if there is a mouse button currently down, and mouse was not clicked last frame,
					//play click sound
					if (button->GetUseButtonSFX<Clickable::SOUND_ID::ON_CLICK_SOUND>()) 
					{ AudioMgr.PlaySFX(button->GetButtonSFX<Clickable::SOUND_ID::ON_CLICK_SOUND>()); }
					
					//invoke click event via KeywordEvent pipeline
					KeywordEvents().Invoke(std::make_pair(button->GetClickEventKeyword(), dynamic_cast<BaseComponent*>(button)));

					wasMouseDown = true;
					if (button->GetUseScaleAnims())
					{//update hover scale if required by the button
						HoverScaleUpdate(button, trans, true);
					}
					button->SetHovered(true);
					button->SetClicked(true);
				}
				else if (ANC::Input.GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)
					&& wasMouseDown)		//this condition is to be replaced with mouse position check with UI element
				{	//if there is a mouse button currently up, and mouse was down last frame,
					
					//play release sfx
					if (button->GetUseButtonSFX<Clickable::SOUND_ID::ON_RELEASE_SOUND>()) 
					{ AudioMgr.PlaySFX(button->GetButtonSFX<Clickable::SOUND_ID::ON_RELEASE_SOUND>()); }
					
					//invoke Release event via KeywordEvent pipeline
					KeywordEvents().Invoke(std::make_pair(button->GetReleaseEventKeyword(), dynamic_cast<BaseComponent*>(button)));
					wasMouseDown = false;
					if (button->GetUseScaleAnims())
					{//update hover scale if required by the button
						HoverScaleUpdate(button, trans, false);
					}
					button->SetHovered(true);
					button->SetClicked(false);
				}
				else
				{	//invoke Hover event via KeywordEvent pipeline. this is the last if statement to prevent it being invoked during the other events.
#ifdef DEBUG
					a << "Hover Event\n";
#endif
					if (!button->GetWasHovered()) 
					{
						//hover enter sound
						if (button->GetUseButtonSFX<Clickable::SOUND_ID::ON_HOVER_TRIGGER>()) 
						{ AudioMgr.PlaySFX(button->GetButtonSFX<Clickable::SOUND_ID::ON_HOVER_TRIGGER>()); }
						//invoke event
						KeywordEvents().Invoke(std::make_pair(button->GetHoverTriggerEventKeyword(), dynamic_cast<BaseComponent*>(button)));
						if (button->GetUseScaleAnims())
						{//update hover scale if required by the button
							HoverScaleUpdate(button, trans, true);
						}
					}
					KeywordEvents().Invoke(std::make_pair(button->GetHoverEventKeyword(), dynamic_cast<BaseComponent*>(button)));
					button->SetHovered(true);
				}

#if 0
#ifndef _PACKAGE
				if (SHOW_EDITOR) {
					if (DEBUG_MODE) {
						// Draw coliders.
						Transform_Component* trans = (button->GetTrackedObject()) 
							? button->GetTrackedObject()											//get parent object transform
							: entityMgr.GetComponent<Transform_Component>(button->GetEntityId());	//get own object transform
						if (!trans) { continue; }
						//draw the debug collider if transform exist
						DrawDebugCollider(button->GetBounds(),trans);
					}
					else if (SHOW_COLLIDERS && ANC::Editor.GetSelectedEntity() != INVALID_ID) {
						// Draw this entity collider if selected.
						if (button->GetEntityId() != ANC::Editor.GetSelectedEntity()) { continue; }
						Transform_Component* transform = entityMgr.GetComponent<Transform_Component>(ANC::Editor.GetSelectedEntity());
						if (transform) { DrawDebugCollider(button->GetBounds(), transform); }
					}
				}
#endif
#endif
				
			}
			if (changeSceneThisFrame) { changeSceneThisFrame = false; SceneMgr.LoadScene(sceneToLoadName); }
		}//end of update

		bool CanOperateOn(EntityID entityId) override 
		{
			return (entityMgr.HasComponent<Clickable>(entityId)
				&& entityMgr.HasComponent<Transform_Component>(entityId));
		}

	};
}//end of ECS namespace
}//end of ANC namespace

/******************************************************************************/
/*!
\file		KeywordSystem.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	February 27, 2024
\brief		Contain logic for the Keyword system

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "C2CEvents.h"
#include "ECS/BaseComponent.h"
#include "ECS/BaseSystem.h"
#include "ECS/Components/Clickable.h"
#include "KeywordSubscriber.h"
#include "Logics/ClickableCompLogic.h"

#include <map>
//#include <unordered_map>
#include <set>
#include <string>
#include <array>
#include <functional>


namespace ANC
{
	namespace ECS
	{
		//=============================================================================
		//\brief
		//	Forward declaration of friend function HandleEvent to be wrapped by the 
		//  KeywordSystem class
		//=============================================================================
		void HandleEvent(std::pair<std::string const&, BaseComponent*> param);

		//cannot constexpr std::function class. require own implementation to work
		//constexpr std::function<void(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*)> a{ ToScene<"LVL1.scn"> };
		/*
		auto f = get_f(ToScene<"LVL1.scn">);
		constexpr std::array<decltype(f), 1> CEvents
		{
			f
		};*/

		template <typename T>
		struct CharArrayReturner 
		{
			const std::string_view strLiteral;
			constexpr CharArrayReturner(auto&& c)
				:strLiteral{ c } {/*Empty By Design*/ }
			constexpr decltype(auto) GetArr() const
			{
				return strLiteral;
			}
		};

		constexpr std::array<const char [100], 2> sceneNames	//Use this to store constexpr cstrings in one place????
		{
			"INTROCUTSCENE.scn",
			"MenuTest.scn"
		};

		//=============================================================================
		// \detail
		//	For the use of Clickable Event Logics, the following requirements are needed:
		//	- Keyword Subscriber with the correct Event Handle must be selected.
		//  - Animation component should be used for sprite rendering 
		//  - default Panel elements must have animation state named "Panel" for active panel
		//  - Control page elements must have animation states named "CPS" or "CPS2"
		//	  denoting which Control panel page they are.
		//  - Credits page elements must have animation states named "Credits"
		//  - Paused-only elements must have animation state named "Paused" and start 
		//	  on the "Default" state.
		//  - Unpaused elements should have animation state named "Unpaused"
		//  - All UI elements must be subscribed to All UIEvents. list is TBA
		//  Not doing any of the above should result in UI elements having undefined
		//	behaviour.
		//	Note: Paused Items are assumed to not exist in a menu scene
		//=============================================================================
		constexpr std::array<const char [100], 9> PanelKeys	//Use this to store constexpr cstrings in one place????
		{
			"Panel",
			"CPS",
			"CPS2",
			"Credits",
			"CPS3",
			"Unpaused",
			"SS",
			"ConfirmScreen",
			"ConfirmDefeat"
		};
		
		constexpr std::array<const char [100], 1> ButtonAnimState	//Use this to store constexpr cstrings in one place????
		{
			"ClickEntry"
		};
		//=============================================================================
		//\brief
		//	Constexpr array of all enigne supported logic available for UI.
		//=============================================================================

#if 1	//in case of other's modification, allow for easier merges
		constexpr std::array<ClickableEventLogic, 33> allClickableEvents
		{ {{"Debug",DebugEvent}
		,{"MM_00",TogglePanels<PanelKeys[0]>}	//"ToMainMenu", Panel
		,{"MM_01",ToScene<sceneNames[0]>}		//"INTROCUTSCENE.scn"
		,{"MM_04",QuitGame}
		,{"MM_03",TogglePanels<PanelKeys[3]>}	//"ToCredits", Credits
		,{"MM_02",TogglePanels<PanelKeys[1]>}	//"ToControlPanelPg1", CPS
		,{"CP_02",TogglePanels<PanelKeys[2]>}	//"ToControlPanelPg2", CPS2
		,{"CP_03",TogglePanels<PanelKeys[4]>}	//"ToControlPanelPg3", CPS3
		,{"PM_00",PausePanelShow}
		,{"PM_01",PausePanelHide}
		,{"PM_02",RestartScene}
		,{"PM_03",TogglePanels<PanelKeys[1]>}	//"ToControlPanelPg1", CPS
		,{"PM_04",ToScene<sceneNames[1]>}		//"MenuTest.scn"
		,{"BB_01",OnClickButtonSkipScene}
		,{"BB_02",OnClickReleaseSkipScene}
		,{"CA_00",TransitSprite<ButtonAnimState[0],false>}
		,{"SS_00",ToSettingsMenu}	//"ToSettingsPanel", SS
		,{"SS_01", SetVolumeFlag<AUDIO_VOL_FUNC::MASTER, true>}		//increase master volume
		,{"SS_02", SetVolumeFlag<AUDIO_VOL_FUNC::MASTER, false>}	//decrease master volume
		,{"SS_03", SetVolumeFlag<AUDIO_VOL_FUNC::BGM, true>}		//increase bgm volume
		,{"SS_04", SetVolumeFlag<AUDIO_VOL_FUNC::BGM, false>}
		,{"SS_05", SetVolumeFlag<AUDIO_VOL_FUNC::SFX, true>}		//increase sfx volume
		,{"SS_06", SetVolumeFlag<AUDIO_VOL_FUNC::SFX, false>}
		,{"SS_07", SetVolumeFlag<AUDIO_VOL_FUNC::VO, true>}			//increase VO volume
		,{"SS_08", SetVolumeFlag<AUDIO_VOL_FUNC::VO, false>}
		,{"SS_09", SetAudioFlag<AUDIO_MUTE_FUNC::MASTER>}		//mute master track
		//,{"SS_10", SetAudioFlag<AUDIO_MUTE_FUNC::MASTER, false>}
		,{"SS_11", SetAudioFlag<AUDIO_MUTE_FUNC::BGM>}		//mute BGM track
		//,{"SS_12", SetAudioFlag<AUDIO_MUTE_FUNC::BGM, false>}
		,{"SS_13", SetAudioFlag<AUDIO_MUTE_FUNC::SFX>}		//mute SFX track
		//,{"SS_14", SetAudioFlag<AUDIO_MUTE_FUNC::SFX, false>}
		,{"SS_15", SetAudioFlag<AUDIO_MUTE_FUNC::VO>}			//mute VO track
		,{"GO_00", ToPrevScene}
		,{"VS_00", ToNextScene}
		//,{"SS_16", SetAudioFlag<AUDIO_MUTE_FUNC::VO, false>}
		, {"CS_00", TogglePanels<PanelKeys[7]>}
		, {"CS_01", TogglePanels<PanelKeys[8]>}
		} };
#else
		constexpr std::array<ClickableEventLogic, 18> allClickableEvents
		{ {{"Debug",DebugEvent}
		,{"MM_00",TogglePanels<PanelKeys[0]>}	//"ToMainMenu", Panel
		,{"MM_01",ToScene<sceneNames[0]>}		//"INTROCUTSCENE.scn"
		,{"MM_04",QuitGame}
		,{"MM_03",TogglePanels<PanelKeys[3]>}	//"ToCredits", Credits
		,{"MM_02",TogglePanels<PanelKeys[1]>}	//"ToControlPanelPg1", CPS
		,{"CP_02",TogglePanels<PanelKeys[2]>}	//"ToControlPanelPg2", CPS2
		,{"CP_03",TogglePanels<PanelKeys[4]>}	//"ToControlPanelPg3", CPS3
		,{"PM_00",PausePanelShow}
		,{"PM_01",PausePanelHide}
		,{"PM_02",RestartScene}
		,{"PM_03",TogglePanels<PanelKeys[1]>}	//"ToControlPanelPg1", CPS
		,{"PM_04",ToScene<sceneNames[1]>}		//"MenuTest.scn"
		,{"BB_01",OnClickButtonSkipScene}
		,{"BB_02",OnClickReleaseSkipScene}
		,{"SS_00",TogglePanels<PanelKeys[6]>}	//"ToSettingsPanel", SS
		,{"SS_01",MasterVolumeIncrease}
		,{"CA_00",TransitSprite<ButtonAnimState[0],false>}
		} };
#endif
		//=============================================================================
		//\class KeywordSystem
		//\brief
		//	class to apply ClickableEventLogic onto KeywordSubscriber when a Clickable
		//	broadcasts an event via KeywordEvent. only subscribed subscribers are called.
		//	Class does not require serialization.
		//=============================================================================
		class KeywordSystem : public BaseSystem
		{
		private:
			//only accessible for this system. Use this single vector of callbacks
			//to set Keyword Subscriber's event callback via 
			//subscriber shall reference them by name.
			// store as <EventHandle, EventObject>
			inline static std::map<std::string, ClickableEventLogic> allEvents{};

		public:
			KeywordSystem()
			{	//subscribe our handler function to the Keyword events.
				KeywordEvents().Subscribe(HandleEvent);
			}


			bool CanOperateOn(EntityID entityId) override
			{
				return (entityMgr.HasComponent<KeywordSubscriber>(entityId));
			}
			//this is a passive logic class
			void Update() override {/*Empty by Design*/ };
			//Add event to static map functions
			static void AddEvent(ClickableEventLogic const&);
			static void AddEvent(ClickableEventLogic&&);
			static void AddEvent(std::string const&, callback ref);

			//friend function as our event system does not support pointer to member function.
			friend void HandleEvent(std::pair<std::string const&, BaseComponent*> param);
		};

		void AddMenuLogics();

	}//end of ECS namespace
}//end of ANC namespace
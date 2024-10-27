/******************************************************************************/
/*!
\file		ClickableCompLogic.cpp
\author 	Tan Poh Heng, t.pohheng, 620006722
			Pang Tun Dagh 2000688
\par    	email: t.pohheng@digipen.edu
				   tundagh.pang@digipen.edu
\date   	February 21, 2024
\brief		Contain logic and control for clickable component logic

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#include "pch.h"
#include "Logics/ClickableCompLogic.h"
#include "Engine/SceneManager.h"
#include "ECS/Components/Animation.h"
#include "ECS/Components/CutsceneManager.h"
#include "ECS/Components/Dialogue.h"




//macro for quick function definiton using this signature
#define FUNCDEFINE(t) void(t)(std::pair<std::string const&, BaseComponent*> eventHandle, KeywordSubscriber* subscriber)

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

namespace 
{	//update as necessary
	constexpr std::string_view menuSceneName {"MenuTest.scn"};
}

namespace ANC 
{namespace ECS
{
	//=============================================================================
	//\brief
	//		debug event for testing purposes
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	void DebugEvent(std::pair<std::string const&, BaseComponent*> eventHandle, KeywordSubscriber* subscriber)
	{
		(subscriber); (eventHandle);
		ANC_Debug::Log_Out a;
		a << ":P activation\n";
		//SceneMgr.RemoveComponentFromEntity<KeywordSubscriber>(*SceneMgr.GetEntity(subscriber->GetEntityId()));
	}
	//=============================================================================
	//\brief
	//		Transitions from main menu scene to the intro cutsscene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//============================================================================= 
	void MainToGame(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*)
	{
		SceneMgr.SetLateUpdateLoadScene("INTROCUTSCENE.scn");
	}
	//=============================================================================
	//\brief
	//		Quits the game from the main menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	void QuitGame(std::pair<std::string const&, BaseComponent*> , KeywordSubscriber*) //parameters for function signature required for function pointer callback
	{
		//force panel to reset
		//auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		//if (!entity || !entity->HasComponent<Animation>()) { return; }
		//auto animator = entity->GetComponent<Animation>();
		//if (animator->GetStates().contains("Panel"))
		//{
		//	animator->SwitchAnimationStates("Panel"); entity->SetActive(true);
		//}		//subscriber with the state will be switched to active state
		//else
		//{
		//	animator->SwitchAnimationStates("Default"); entity->SetActive(false);
		//}	//subscriber without the state will be switched to default state
		//request to quit game
		if (SceneMgr.GetEntityByName("ConfirmScreen"))
		{
			SceneMgr.GetEntityByName("ConfirmScreen")->SetActive(false);
			SceneMgr.GetEntityByName("ConfirmScreen")->GetComponent<Animation>()->SwitchAnimationStates("Default");
		}
		if (SceneMgr.GetEntityByName("ExitConfirmButton"))
		{
			SceneMgr.GetEntityByName("ExitConfirmButton")->SetActive(false);
			SceneMgr.GetEntityByName("ExitConfirmButton")->GetComponent<Animation>()->SwitchAnimationStates("Default");
		}
		if (SceneMgr.GetEntityByName("ExitCancelButton"))
		{
			SceneMgr.GetEntityByName("ExitCancelButton")->SetActive(false);
			SceneMgr.GetEntityByName("ExitCancelButton")->GetComponent<Animation>()->SwitchAnimationStates("Default");
		}
		if (SceneMgr.GetEntityByName("ExitText"))
		{
			SceneMgr.GetEntityByName("ExitText")->SetActive(false);
			SceneMgr.GetEntityByName("ExitText")->GetComponent<Animation>()->SwitchAnimationStates("Default");
		}
		if (SceneMgr.GetEntityByName("ExitButton"))
		{
			SceneMgr.GetEntityByName("ExitButton")->SetActive(true);
		}
		Core.StopRun();
	}
	//=============================================================================
	//\brief
	//		Show the main menu panels and hides all others
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(MenuPanelShow) 
	{
		(eventHandle);
		auto& entity = *SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity.HasComponent<Animation>()) { return; }
		auto animator = entity.GetComponent<Animation>();
		if (animator->GetStates().contains("Panel"))
		{
			animator->SwitchAnimationStates("Panel"); entity.SetActive(true);
		}		//subscriber with the state will be switched to active state
		else 
		{
			animator->SwitchAnimationStates("Default"); entity.SetActive(false);
		}	//subscriber without the state will be switched to default state
	}
	//=============================================================================
	//\brief
	//		Shows the first page of the controls screen and hides all other screens
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(ControlsPanelShow) 
	{//assume the state "CPS" is avaialable in the subscribed functions, else switch to default anim state.
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		ANC_Debug::Log_Out a;

		if (!entity || !entity->HasComponent<Animation>()) { a<<"failed to find animator or entity of id("<<subscriber->GetEntityId()<<")\n"; return; }
		
		auto animator = entity->GetComponent<Animation>();
		if (!animator) { a << "failed to find animator of id(" << subscriber->GetEntityId() << ")\n"; return; }
		if (animator->GetStates().contains("CPS")) 
		{
			animator->SwitchAnimationStates("CPS"); entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else 
		{animator->SwitchAnimationStates("Default"); entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
	}
	//=============================================================================
	//\brief
	//		Shows the second page of the controls screen
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(ControlsPanelShowSecond) 
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<Animation>()) { return; }
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains("CPS2")) 
		{animator->SwitchAnimationStates("CPS2"); entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else 
		{animator->SwitchAnimationStates("Default"); entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
	}
	//=============================================================================
	//\brief
	//		Shows the credits screen
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(CreditsPanelShow)
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<Animation>()) { return; }
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains("Credits"))
		{
			animator->SwitchAnimationStates("Credits");
			entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{
			animator->SwitchAnimationStates("Default");
			entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
	}
	//=============================================================================
	//\brief
	//		Shows the pause menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(PausePanelShow) 
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<Animation>()) { return; }
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains("Paused"))
		{
			animator->SwitchAnimationStates("Paused");
			entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else if (animator->GetStates().contains("Panel"))
		{
			animator->SwitchAnimationStates("Panel");
			entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{
			animator->SwitchAnimationStates("Default");
			entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
		SceneMgr.SetGamePaused(true);
	}
	//=============================================================================
	//\brief
	//		Hides the pause menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================	
	FUNCDEFINE(PausePanelHide) 
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<Animation>()) { return; }
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains("Unpaused"))
		{
			animator->SwitchAnimationStates("Unpaused");
			entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{
			animator->SwitchAnimationStates("Default");
			entity->SetActive(false);
		}	//subscriber without the state will be switched to default state]
		SceneMgr.SetGamePaused(false);
	}
	//=============================================================================
	//\brief
	//		restarts the current scene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(RestartScene)
	{	
		// Grab the entity instances, so as to allow us to manipulate them even when inactive
		auto bg = SceneMgr.GetEntityByName("DefeatBackground");
		auto defeatText = SceneMgr.GetEntityByName("DefeatText");
		auto restartButton = SceneMgr.GetEntityByName("DefeatRestartButton");
		auto mainMenuButton = SceneMgr.GetEntityByName("DefeatMainMenuButton");


		//if any item doesn't exist, return
		if (!(!bg || !defeatText || !restartButton || !mainMenuButton))
		{ 
			// If all items exist, run this section
			ANC::SceneMgr.SetGamePaused(false);

			// deactivate all buttons and set to default animation to hide the UI
			bg->GetComponent<Animation>()->SwitchAnimationStates("Default");
			defeatText->GetComponent<Animation>()->SwitchAnimationStates("Default");
			restartButton->GetComponent<Animation>()->SwitchAnimationStates("Default");
			mainMenuButton->GetComponent<Animation>()->SwitchAnimationStates("Default");

			bg->SetActive(false);
			defeatText->SetActive(false);
			restartButton->SetActive(false);
			mainMenuButton->SetActive(false);
		}

		//restart the scene at the end of update loops
		SceneMgr.SetLateUpdateLoadScene(SceneMgr.GetCurrentScene());
	}
	//=============================================================================
	//\brief
	//		Returns from current scene to the main menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(BackToMenu)
	{	//go to the menu scene at the end of update loops
		SceneMgr.SetLateUpdateLoadScene(std::string(menuSceneName));
	}
	//=============================================================================
	//\brief
	//		Skips the current scene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(SkipScene)
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<CutsceneManager>()) { return;}

		auto animator = entity->GetComponent<Animation>();
		auto cutscenemanager = entity->GetComponent<CutsceneManager>();

		if (animator->GetStates().contains("Skip"))
		{
			animator->SwitchAnimationStates("Skip");
			entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{
			animator->SwitchAnimationStates("Default");
			entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
		SceneMgr.SetLateUpdateLoadScene(cutscenemanager->NextScene);
	}
	//=============================================================================
	//\brief
	//		Behaviours for clicking on the skip scene button
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(OnClickButtonSkipScene)
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<CutsceneManager>() || !entity->HasComponent<DialogueManager>()) { return; }

		auto animator = entity->GetComponent<Animation>();
		if (entity->HasComponent<CutsceneManager>())
		{
			auto cutscenemanager = entity->GetComponent<CutsceneManager>();

			if (animator->GetStates().contains("Skip"))
			{
				animator->SwitchAnimationStates("Skip");
				entity->SetActive(true);
			}		//subscriber with the state will be switched to active state
			else
			{
				animator->SwitchAnimationStates("Default");
				entity->SetActive(false);
			}	//subscriber without the state will be switched to default state
			SceneMgr.SetLateUpdateLoadScene(cutscenemanager->NextScene);
		}
		if (entity->HasComponent<DialogueManager>())
		{
			auto dialougemanager = entity->GetComponent<DialogueManager>();

			if (animator->GetStates().contains("Skip"))
			{
				animator->SwitchAnimationStates("Skip");
				entity->SetActive(true);
			}		//subscriber with the state will be switched to active state
			else
			{
				animator->SwitchAnimationStates("Default");
				entity->SetActive(false);
			}	//subscriber without the state will be switched to default state
			dialougemanager->index = 0;
			SceneMgr.SetLateUpdateLoadScene(dialougemanager->NextScene);
		}
	}
	//=============================================================================
	//\brief
	//		Behaviours for releasing the click on the skip scene button
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(OnClickReleaseSkipScene)
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<CutsceneManager>()) { return; }
		else
		{
			auto CSM = entity->GetComponent<CutsceneManager>();
			CSM->counter = 0;
			SceneMgr.SetLateUpdateLoadScene(CSM->NextScene);
		}

	}
	//=============================================================================
	//\brief
	//		Shows the sound settings screen and hides all else
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(ToSettingsMenu)
	{
		(eventHandle);
		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		ANC_Debug::Log_Out a;

		if (!entity || !entity->HasComponent<Animation>()) { a << "failed to find animator or entity of id(" << subscriber->GetEntityId() << ")\n"; return; }

		auto animator = entity->GetComponent<Animation>();
		if (!animator) { a << "failed to find animator of id(" << subscriber->GetEntityId() << ")\n"; return; }
		if (animator->GetStates().contains("SS"))
		{
			animator->SwitchAnimationStates("SS"); entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{	 
			animator->SwitchAnimationStates("Default"); entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
		if (entity->GetName() == "MainMute" && ANC::AudioMgr.getMuteMaster()) {
			if (animator->GetStates().contains("SSCheck")) {
				animator->SwitchAnimationStates("SSCheck");
			}
		}
		if (entity->GetName() == "MusicMute" && ANC::AudioMgr.getMuteBGM()) {
			if (animator->GetStates().contains("SSCheck")) {
				animator->SwitchAnimationStates("SSCheck");
			}
		}
		if (entity->GetName() == "SFXMute" && ANC::AudioMgr.getMuteSFX()) {
			if (animator->GetStates().contains("SSCheck")) {
				animator->SwitchAnimationStates("SSCheck");
			}
		}
		if (entity->GetName() == "VOMute" && ANC::AudioMgr.getMuteVO()) {
			if (animator->GetStates().contains("SSCheck")) {
				animator->SwitchAnimationStates("SSCheck");
			}
		}
		
		if (entity->GetName() == "MasterBar") {
			VolumeSwitch("MasterBar", animator);
		}
		if (entity->GetName() == "MusicBar") {
			VolumeSwitch("MusicBar", animator);
		}
		if (entity->GetName() == "SFXBar") {
			VolumeSwitch("SFXBar", animator);
		}
		if (entity->GetName() == "VOBar") {
			VolumeSwitch("VOBar", animator);
		}
	}
	//=============================================================================
	//\brief
	//		ENUM Class for volume control
	//=============================================================================
	enum class VOLUME_LEVEL {
		ZERO = 0,
		ONE = 1,
		TWO,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN
	};
	//=============================================================================
	//\brief
	//		Helper function for controlling volume bar length
	//\param std::string
	//		Contains the name of that specific volume bar entity
	//\param Animation*
	//		Points to that volume bar entity's animation component
	//=============================================================================
	void VolumeSwitch(std::string string, Animation* animator) {
		enum VOLUME_LEVEL level = VOLUME_LEVEL::ZERO;
		if (string == "MasterBar") { level = static_cast<VOLUME_LEVEL>((ANC::AudioMgr.getMasterVolume() * 11) > 10 ? 10 : (ANC::AudioMgr.getMasterVolume() * 11)); }
		if (string == "MusicBar") { level = static_cast<VOLUME_LEVEL>((ANC::AudioMgr.getBGMVolume() * 11) > 10 ? 10 : (ANC::AudioMgr.getBGMVolume() * 11)); }
		if (string == "SFXBar") { level = static_cast<VOLUME_LEVEL>((ANC::AudioMgr.getSFXVolume() * 11) > 10 ? 10 : (ANC::AudioMgr.getSFXVolume() * 11)); }
		if (string == "VOBar") { level = static_cast<VOLUME_LEVEL>((ANC::AudioMgr.getVOVolume() * 11) > 10 ? 10 : (ANC::AudioMgr.getVOVolume() * 11)); }
		
		switch (level) {
		case VOLUME_LEVEL::ONE:
			if (animator->GetStates().contains("One")) { animator->SwitchAnimationStates("One"); }
			break;

		case VOLUME_LEVEL::TWO:
			if (animator->GetStates().contains("Two")) { animator->SwitchAnimationStates("Two"); }
			break;

		case VOLUME_LEVEL::THREE:
			if (animator->GetStates().contains("Three")) { animator->SwitchAnimationStates("Three"); }
			break;

		case VOLUME_LEVEL::FOUR:
			if (animator->GetStates().contains("Four")) { animator->SwitchAnimationStates("Four"); }
			break;

		case VOLUME_LEVEL::FIVE:
			if (animator->GetStates().contains("Five")) { animator->SwitchAnimationStates("Five"); }
			break;

		case VOLUME_LEVEL::SIX:
			if (animator->GetStates().contains("Six")) { animator->SwitchAnimationStates("Six"); }
			break;

		case VOLUME_LEVEL::SEVEN:
			if (animator->GetStates().contains("Seven")) { animator->SwitchAnimationStates("Seven"); }
			break;

		case VOLUME_LEVEL::EIGHT:
			if (animator->GetStates().contains("Eight")) { animator->SwitchAnimationStates("Eight"); }
			break;

		case VOLUME_LEVEL::NINE:
			if (animator->GetStates().contains("Nine")) { animator->SwitchAnimationStates("Nine"); }
			break;

		case VOLUME_LEVEL::TEN:
			if (animator->GetStates().contains("Ten")) { animator->SwitchAnimationStates("Ten"); }
			break;

		case VOLUME_LEVEL::ZERO: //identical to default state, so fall through
			if (animator->GetStates().contains("SS")) { animator->SwitchAnimationStates("SS"); }
			break;
		default:
			//if (animator->GetStates().contains("SS")) { animator->SwitchAnimationStates("SS"); }
			break;
		}
	}
	//=============================================================================
	//\brief
	//		Shows the confirmation screen and hides all else
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEFINE(ConfirmScreen) {
		(eventHandle);
		(subscriber);
	}

	void ToPrevScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*) {
		SceneMgr.SetLateUpdateLoadScene(SceneMgr.GetPrevScene());
	}

	void ToNextScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*) {
		Camera* cam = ANC::SceneMgr.GetEntityByName("Camera")->GetComponent<Camera>();


		// Grab the entity instances, so as to allow us to manipulate them even when inactive
		auto bg = SceneMgr.GetEntityByName("VictoryBackground");
		auto victoryText = SceneMgr.GetEntityByName("VictoryText");
		auto nextButton = SceneMgr.GetEntityByName("VictoryNextButton");


		//if any item doesn't exist, return
		if (!(!bg || !victoryText || !nextButton))
		{ 
			// Code block only runs when all of them exist
			ANC::SceneMgr.SetGamePaused(false);

			// deactivate all buttons and set to default animation to hide the UI
			bg->GetComponent<Animation>()->SwitchAnimationStates("Default");
			victoryText->GetComponent<Animation>()->SwitchAnimationStates("Default");
			nextButton->GetComponent<Animation>()->SwitchAnimationStates("Default");

			bg->SetActive(false);
			victoryText->SetActive(false);
			nextButton->SetActive(false);
		}


		// Go to next level.
		std::stringstream SceneStream;
		std::string ScnExtension(".scn");
		SceneStream << cam->GetNextScene() << ScnExtension;
		std::string LevelSceneToLoad = SceneStream.str().c_str();
		//ANC_Debug::Debug.Log("%s", LevelSceneToLoad.c_str());
		SceneMgr.SetLateUpdateLoadScene(LevelSceneToLoad);
	}

	////Unused Functions
	//template<const char c[]>
	//void ToScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*subscriber) 
	//{
	//	SceneMgr.SetLateUpdateLoadScene(c);
	//}
}//end of ECS namespace
}//end of ANC namespace
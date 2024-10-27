/******************************************************************************/
/*!
\file		ClickableCompLogic.h
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
#pragma once
//menu logics here
#include "KeywordSubscriber.h"
#include "ECS/Components/Animation.h"
#include <array>

//macro for quick function definiton using this signature
#define FUNCDEF(t) void(t)(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*)


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
	void DebugEvent(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	//=============================================================================
	//\brief
	//		Transitions from main menu scene to the intro cutsscene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	void MainToGame(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	//=============================================================================
	//\brief
	//		Quits the game from the main menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	void QuitGame(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	//=============================================================================
	//\brief
	//		Helper function for controlling volume bar length
	//\param std::string
	//		Contains the name of that specific volume bar entity
	//\param Animation*
	//		Points to that volume bar entity's animation component
	//=============================================================================
	void VolumeSwitch(std::string, Animation*);
	//=============================================================================
	//\brief
	//		Show the main menu panels and hides all others
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(MenuPanelShow);
	//=============================================================================
	//\brief
	//		Shows the first page of the controls screen and hides all other screens
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(ControlsPanelShow);
	//=============================================================================
	//\brief
	//		Shows the second page of the controls screen
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(ControlsPanelShowSecond);
	//=============================================================================
	//\brief
	//		Shows the credits screen
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(CreditsPanelShow);
	//=============================================================================
	//\brief
	//		Shows the pause menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(PausePanelShow);
	//=============================================================================
	//\brief
	//		Hides the pause menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(PausePanelHide);
	//=============================================================================
	//\brief
	//		restarts the current scene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(RestartScene);
	//=============================================================================
	//\brief
	//		Returns from current scene to the main menu
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(BackToMenu);
	//=============================================================================
	//\brief
	//		Skips the current scene
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(SkipScene);
	//=============================================================================
	//\brief
	//		Behaviours for clicking on the skip scene button
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(OnClickButtonSkipScene);
	//=============================================================================
	//\brief
	//		Behaviours for releasing the click on the skip scene button
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(OnClickReleaseSkipScene);
	//=============================================================================
	//\brief
	//		Shows the sound settings screen and hides all else
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	FUNCDEF(ToSettingsMenu);
#if 0
	constexpr std::array<ClickableEventLogic, 10> allClickableEvents
	{ {{"Debug",DebugEvent}
	,{"MM_01",MainToGame}
	,{"MM_04",QuitGame}
	,{"MM_03",CreditsPanelShow}
	,{"MM_02",ControlsPanelShow}
	,{"CP_02",ControlsPanelShowSecond}
	,{"PM_01",PausePanelHide}
	,{"PM_02",RestartScene}
	,{"PM_03",ControlsPanelShow}
	,{"PM_04",BackToMenu}
	,{"BB_01",OnClickButton}}}

#endif
	//attempt to templatize scene loading functions.
	//=============================================================================
	//\brief
	//		Templated scene loading functions
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	template<typename >
	void ToScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	//attempt to templatize scene loading functions.
	template<const char c[]>
	void ToScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*);
	//void(*a)(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*) = ToScene<"LVL1.scn">;

	//Unused Functions
	template<const char c[]>
	void ToScene(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber*)
	{
		SceneMgr.SetLateUpdateLoadScene(c);
	}
	//constexpr char a[] = "LVL1.scn";
	//decltype(ToScene<a>);
	//auto f = get_f(ToScene<"LVL1.scn">);
	//=============================================================================
	//\brief
	//		Templated panel loading function
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	template<const char c[]>
	void TogglePanels(std::pair<std::string const&, BaseComponent*>, KeywordSubscriber* subscriber)
	{
		// Grab the entity instances, so as to allow us to manipulate them even when inactive
		auto bg = SceneMgr.GetEntityByName("DefeatBackground");
		auto defeatText = SceneMgr.GetEntityByName("DefeatText");
		auto restartButton = SceneMgr.GetEntityByName("DefeatRestartButton");
		auto mainMenuButton = SceneMgr.GetEntityByName("DefeatMainMenuButton");


		//if any item doesn't exist, return
		if (!(!bg || !defeatText || !restartButton || !mainMenuButton))
		{ 
			//ANC::SceneMgr.SetGamePaused(false);

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

		auto entity = SceneMgr.GetEntity(subscriber->GetEntityId());
		if (!entity || !entity->HasComponent<Animation>()) { return; }
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains(c))
		{
			animator->SwitchAnimationStates(c); entity->SetActive(true);
		}		//subscriber with the state will be switched to active state
		else
		{
			animator->SwitchAnimationStates("Default"); entity->SetActive(false);
		}	//subscriber without the state will be switched to default state
	}
	
	//=============================================================================
	//\brief
	//		Templated sprite transition function
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	template<const char animState[], bool b = true>
	void TransitSprite(std::pair<std::string const&, BaseComponent*> handle, KeywordSubscriber* subscriber)
	{
		std::shared_ptr<ANC::ECS::Entity> entity;
		//determine if apply to subscribers or caller. At least one subscriber must exits for caller to be updated.
		if constexpr (b) { entity = SceneMgr.GetEntity(subscriber->GetEntityId()); }
		else { entity = SceneMgr.GetEntity(handle.second->GetEntityId()); }

		if (!entity || !entity->HasComponent<Animation>()) { return; }	//one subscriber need not have animation due to this exit clause,
		auto animator = entity->GetComponent<Animation>();
		if (animator->GetStates().contains(animState))
		{
			animator->SwitchAnimationStates(animState);
		}		//button with the state will be switched to active state
	}

	//=============================================================================
	//\brief
	//		ENUM Class for audio purposes
	//=============================================================================
	enum class AUDIO_MUTE_FUNC: char{
		MASTER,
		BGM,
		SFX,
		VO
	};
	//		use <enum, true>
	//=============================================================================
	//\brief
	//		Templated audio muting function
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	template<AUDIO_MUTE_FUNC func>
	void SetAudioFlag(std::pair<std::string const&, BaseComponent*> , KeywordSubscriber* subscriber)
	{
		Animation* animation = SceneMgr.GetEntity(subscriber->GetEntityId())->GetComponent<Animation>();
		if constexpr (func == AUDIO_MUTE_FUNC::MASTER)
		{
			if (ANC::AudioMgr.getMuteMaster()) {
				ANC::AudioMgr.muteMaster(false);
				animation->SwitchAnimationStates("SS");
			} else {
				ANC::AudioMgr.muteMaster(true);
				animation->SwitchAnimationStates("SSCheck");
			}
			ANC_Debug::Debug.Log("Master Flag");
		}
		if constexpr (func == AUDIO_MUTE_FUNC::BGM)
		{

			if (ANC::AudioMgr.getMuteBGM()) {
				ANC::AudioMgr.muteBGM(false);
				animation->SwitchAnimationStates("SS");
			} else {
				ANC::AudioMgr.muteBGM(true);
				animation->SwitchAnimationStates("SSCheck");
			}
			ANC_Debug::Debug.Log("BGM Flag");
		}
		if constexpr (func == AUDIO_MUTE_FUNC::SFX)
		{

			if (ANC::AudioMgr.getMuteSFX()) {
				ANC::AudioMgr.muteSFX(false);
				animation->SwitchAnimationStates("SS");
			} else {
				ANC::AudioMgr.muteSFX(true);
				animation->SwitchAnimationStates("SSCheck");
			}
			ANC_Debug::Debug.Log("SFX Flag");
		}
		if constexpr (func == AUDIO_MUTE_FUNC::VO)
		{

			if (ANC::AudioMgr.getMuteVO()) {
				ANC::AudioMgr.muteVO(false);
				animation->SwitchAnimationStates("SS");
			} else {
				ANC::AudioMgr.muteVO(true);
				animation->SwitchAnimationStates("SSCheck");
			}
			ANC_Debug::Debug.Log("VO Flag");
		}
	}
	//=============================================================================
	//\brief
	//		ENUM Class for audio volume functions
	//=============================================================================
	enum class AUDIO_VOL_FUNC : char {
		MASTER,
		BGM,
		SFX,
		VO
	};
	//=============================================================================
	//\brief
	//		Templated volume control function
	//\param std::pair<std::string const&, BaseComponent*>
	//		Contains the Event Handle to be sent
	//\param KeywordSubscriber*		
	//		Points to the Keyword Subscriber component
	//=============================================================================
	template<AUDIO_VOL_FUNC func, bool b = true>
	void SetVolumeFlag(std::pair < std::string const&, BaseComponent* >, KeywordSubscriber* )
	{
		if constexpr (func == AUDIO_VOL_FUNC::MASTER) {	
			
			if (b) {
				ANC::AudioMgr.stepUpMasterVolume();
			}
			else {
				ANC::AudioMgr.stepDownMasterVolume();
			}
			Animation* animator = entityMgr.GetComponent<Animation>(ANC::SceneMgr.GetEntityByName("MasterBar")->GetID());
			VolumeSwitch("MasterBar", animator);
			ANC_Debug::Debug.Log("Master Volume: %f", ANC::AudioMgr.getMasterVolume());
		}	
		if constexpr (func == AUDIO_VOL_FUNC::BGM) {
			
			if (b) {
				ANC::AudioMgr.stepUpBGMVolume();
			} else {
				ANC::AudioMgr.stepDownBGMVolume();
			}
			Animation* animator = entityMgr.GetComponent<Animation>(ANC::SceneMgr.GetEntityByName("MusicBar")->GetID());
			VolumeSwitch("MusicBar", animator);
			ANC_Debug::Debug.Log("BGM Volume: %f", ANC::AudioMgr.getBGMVolume());
		}
		if constexpr (func == AUDIO_VOL_FUNC::SFX) {

			if (b) {
				ANC::AudioMgr.stepUpSFXVolume();
			} else {
				ANC::AudioMgr.stepDownSFXVolume();
			}
			Animation* animator = entityMgr.GetComponent<Animation>(ANC::SceneMgr.GetEntityByName("SFXBar")->GetID());
			VolumeSwitch("SFXBar", animator);
			ANC_Debug::Debug.Log("SFX Volume: %f", ANC::AudioMgr.getSFXVolume());
		}
		if constexpr (func == AUDIO_VOL_FUNC::VO) {
			
			if (b) {
				ANC::AudioMgr.stepUpVOVolume();
			} else {
				ANC::AudioMgr.stepDownVOVolume();
			}
			Animation* animator = entityMgr.GetComponent<Animation>(ANC::SceneMgr.GetEntityByName("VOBar")->GetID());
			VolumeSwitch("VOBar", animator);
			ANC_Debug::Debug.Log("VO Volume: %f", ANC::AudioMgr.getVOVolume());
		}
	}

	FUNCDEF(ToPrevScene);
	FUNCDEF(ToNextScene);


}//end of ECS namespace
}//end of ANC namespace
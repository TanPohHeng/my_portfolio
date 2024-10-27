#pragma once
/*!************************************************************************
\file SoundManager.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This header file holds the declaration of function(s) related to sound 
management

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "AEEngine.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

/*!***********************************************************************
\namespace Sound
\brief
	Namespace storing the functions and classes for sound management.
*************************************************************************/
namespace Sound{
	/*!***********************************************************************
	\class SoundManager
	\brief
		class made with the goal to of managing AEAudio related items
	*************************************************************************/
	class SoundManager
	{
	private:
		std::vector<AEAudio> audioVec;	//vector of audio files
		enum class ManagerType
		{	//enum for the type of audio the manager is managing
			BGM,	//for background music only
			SFX		//for sound effects only
		} managerType{ ManagerType::BGM };	//by default set it to background music
		std::string name{};	//name of the audio manager
		float volume{1};	//1 is max, 0 is min. stores volume of the manager
	public:
		AEAudioGroup group{};			//audio group of the manager. should be initialized somewhere post construction
		//constructors and destructors for the sound manager
		//type = BGM if true, =SFX if false
		SoundManager(bool type, std::string managerName = "Sound Manager");
		~SoundManager() = default;
		void initSoundGroup();			//intialize sound group
		bool addSound(std::string audioPath, unsigned int& index );	//add sound effects audio 
		bool addMusic(std::string audioPath, unsigned int& index );	//add music audio 
		bool batchAddAudio(std::string batchFilePath);				//batch add audio files into the sound manager
		bool isEmpty();					//check if the manager is empty
		//play the audio with some modifiers
		void playAudio(unsigned int index, AEAudioGroup grp, int timesLooped = 0, f32 volume = 1.0f, f32 pitch = 1.0f);
		void pauseToggleGroup(bool pauseState);						//toggle pause for the group
		void setVolume(float volumePercent);						//set the volume of the group
		void adjustVolume(float volumePercent);						//adjust the volume bassed on the current volume
		float getVolume() { return volume; }						//get the current volume
		void stopAudio() { AEAudioStopGroup(group); }				//stop playing all the current audio in the group
	};
}
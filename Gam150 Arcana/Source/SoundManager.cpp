/*!************************************************************************
\file SoundManager.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This source file holds the definition of function(s) related to sound
management

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "SoundManager.hpp"

/*!***********************************************************************
\namespace Sound
\brief
	Namespace storing the functions and classes for sound management.
*************************************************************************/
namespace Sound 
{
	/*!***********************************************************************
	\brief
		Constructor for SoundManager. sets the type of the manager and it's
		name
	\par [in] type
		the type of the manager. true = background music, false = sound effects
	\par [in] managerName
		name of the sound manager
	*************************************************************************/
	SoundManager::SoundManager(bool type, std::string managerName)
		:group{ },
		managerType{ (type) ? ManagerType::BGM : ManagerType::SFX },
		name{ managerName }
	{	/*empty by design*/
	};

	/*!***********************************************************************
	\brief
		Initialize SoundManager's audio group
	*************************************************************************/
	void SoundManager::initSoundGroup() 
	{	//create the group of the sound manager
		group = AEAudioCreateGroup();	
		//if the group is not valid, print error message
		if (!AEAudioIsValidGroup(group)) { std::cerr << name << "'s Audio Group failed to initialize.\n"; }
	}

	/*!***********************************************************************
	\brief
		check if the sound manager is empty
	\return 
		if the sound manager was empty
	*************************************************************************/
	bool SoundManager::isEmpty() 
	{	//return if the audio vector is empty
		return audioVec.empty();
	}
	
	/*!***********************************************************************
	\brief
		batch load audio from from a file. automatically load audio in the 
		type of the manager
	\param [in] batchFilePath
		the file with the relative path of the audio to batch load
	\return
		if the operation was successful
	*************************************************************************/
	bool SoundManager::batchAddAudio(std::string batchFilePath)
	{
		std::fstream ifs;		//input file stream
		ifs.open(batchFilePath);	//attempt to open file
		if (!ifs.is_open())
		{	//if unable to open, print error and return false
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "failed to load Sound batch path file";
#endif
			return false;
		}
		//create string store for path and size
		std::string pathBuffer;
		unsigned int temp;

		switch (managerType)
		{	//based on the manager type
		case Sound::SoundManager::ManagerType::BGM:	//for background music
			while (std::getline(ifs, pathBuffer))
			{	//while not end of file
				addMusic(pathBuffer, temp);
			}
			break;
		case Sound::SoundManager::ManagerType::SFX:	//for sound effects
			while (std::getline(ifs, pathBuffer))
			{	//while not end of file
				addSound(pathBuffer, temp);
			}
			break;
		}
		ifs.close();	//close the file stream
		return true;	//return operation successful
	}

	/*!***********************************************************************
	\brief
		load audio from from a file for sound effects
	\param [in] audioPath
		the audio file to load
	\param [out] index
		the return by reference index of where the successfully loaded audio
		was stored.
	\return
		if the operation was successful
	*************************************************************************/
	bool SoundManager::addSound(std::string audioPath, unsigned int& index)
	{	//load and store an AEAudio from the path passed by parameters
		AEAudio add{AEAudioLoadSound(audioPath.c_str())};
		if (AEAudioIsValidAudio(add)) {	//if the audio is valid
			audioVec.push_back(add);	//add the audio into the manager
			//return by reference the index the audio was stored at
			index = (unsigned int)audioVec.size() - 1;	
			return true;				//return the operation was successful
		}
		//operation failed, print error and return failed operation
#if defined(DEBUG) | defined(_DEBUG)
		std::cerr << "failed to load sound from path: "<<audioPath<<".\n";
#endif
		return false;
	}

	/*!***********************************************************************
	\brief
		load audio from from a file for background music
	\param [in] audioPath
		the audio file to load
	\param [out] index
		the return by reference index of where the successfully loaded audio
		was stored.
	\return
		if the operation was successful
	*************************************************************************/
	bool SoundManager::addMusic(std::string audioPath, unsigned int& index)
	{	//load and store an AEAudio from the path passed by parameters
		AEAudio add{AEAudioLoadMusic(audioPath.c_str())};
		if (AEAudioIsValidAudio(add)) {//if the audio is valid
			audioVec.push_back(add);	//add the audio into the manager
			//return by reference the index the audio was stored at
			index = (unsigned int)audioVec.size() - 1;
			return true;				//return the operation was successful
		}
		//operation failed, print error and return failed operation
#if defined(DEBUG) | defined(_DEBUG)
		std::cerr << "failed to load music from path: " << audioPath << ".\n";
#endif
		return false;
	}

	/*!***********************************************************************
	\brief
		play an audio loaded in the soundManager.
	\param [in] index
		the index of the audio to be played
	\param [in] grp
		the group to play the Audio in
	\param [in] timesLooped
		the number of times to loop the audio. -1 to loop infinitely
	\param [in] vol
		the volume of the audio played. 1 for max volume, 0 for min volume
	\param [in] pitch
		the pitch of the audio played. 1 for normal pitch
	*************************************************************************/
	void SoundManager::playAudio(unsigned int index, const AEAudioGroup grp, int timesLooped, f32 vol, f32 pitch)
	{	//play the audio file based on parameters passed
		AEAudioPlay(audioVec[index],grp,vol,pitch,timesLooped);
	}
	
	/*!***********************************************************************
	\brief
		sets the group of the sound manager to the boolean passed
	\param [in] pauseState
		a boolean representing if the sound manager should be paused
	*************************************************************************/
	void SoundManager::pauseToggleGroup(bool pauseState) 
	{	//set the pause state of the AEAudioGroup owned by the sound manager
		(pauseState) ?
			AEAudioPauseGroup(SoundManager::group) :
			AEAudioResumeGroup(SoundManager::group);
	}

	/*!***********************************************************************
	\brief
		sets the volume of the sound manager's group
	\param [in] volumePercent
		the volume to be set in terms of percent, with 1 being full volume 
	*************************************************************************/
	void SoundManager::setVolume(float volumePercent)
	{	//set volume of the sound manager's group to a range within 1.0 and 0.0
		volume = (volumePercent > 1.0f || volumePercent < 0.0f) ? volume : volumePercent;
		AEAudioSetGroupVolume(group, volume);
	}

	/*!***********************************************************************
	\brief
		adds a value to the volume of the sound manager's group
	\param [in] volumePercent
		the volume to be added to the current volume in terms of percent,
		with 1 being full volume
	*************************************************************************/
	void SoundManager::adjustVolume(float volumePercent) 
	{	//add to the current volume of the sound manager's group,
		//also ensures the sound manager's group is in a range within 1.0 and 0.0
		volume += volumePercent;
		volume = (volume > 1.0f) ? 1.0f : (volume < EPSILON) ? 0.0f : volume;
		AEAudioSetGroupVolume(group, volume);
	}
}
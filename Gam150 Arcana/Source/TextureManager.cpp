/*!************************************************************************
\file TextureManager.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-25-2023 (mmddyyyy)
\brief
This source file holds the data structure(class) definitions related to 
Texture management.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "TextureManager.hpp"
#include <iostream>			//for std::cerr
#include <fstream>			//for file input

/*!***********************************************************************
\namespace TextureList
\brief
	Namespace storing the functions and classes related to texture manager
*************************************************************************/
namespace TextureList {
	/*!***********************************************************************
	\brief
		Destructor for TextureManager. forcibly unloads all textures currently
		stored in the manager
	*************************************************************************/
	TextureManager::~TextureManager()
	{
		if (isLoaded)
		{	//if the manager is loaded, unload the textures 
			unloadTextures();
		}
	}

	/*!***********************************************************************
	\brief
		Default constructor for TextureManager. Requires initialization post
		construction before first use.
	*************************************************************************/
	TextureManager::TextureManager(){/*empty by design*/}

	/*!***********************************************************************
	\brief
		single param constructor for TextureManager. takes a file path and
		attempts to batch load textures from the file's contents.
	\param [in] texturePathFile
		relative path of file containing the texture file paths
	*************************************************************************/
	TextureManager::TextureManager(const char* texturePathFile)
	{	//attempt to batch load textures
		loadTextures(texturePathFile);
	}

	/*!***********************************************************************
	\brief
		forcibly unloads all textures in the manager
	*************************************************************************/
	void TextureManager::unloadTextures()
	{
		for (AEGfxTexture* t : vectorTextures) 
		{	//for each texture in vector, unload
			AEGfxTextureUnload(t);
		}
		//empty the vector of textures
		vectorTextures.erase(vectorTextures.begin(),vectorTextures.end());
		isLoaded = false;	//reset is loaded to false
#if defined(DEBUG) | defined(_DEBUG)
		std::cout << "Death to the textures\n";	//print debug statement
#endif
	}

	/*!***********************************************************************
	\brief
		batch loads textures from a file containing their path seperated by
		newline characters
	\param [in] texturePathFile
		relative path of file containing the texture file paths
	\return
		if the textures were successfully loaded
	*************************************************************************/
	bool TextureManager::loadTextures(const char* texturePathFile)
	{
		unloadTextures();		//unload old textures first, then start loading new textures
		std::fstream ifs;		//file stream to read file
		ifs.open(texturePathFile);		//open file passed in via param fileName
		if (!ifs.is_open())		//if stream is not open
		{
			isLoaded = false;	//set is loaded to false
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "Texture path file failed to open, no textures were loaded\n";	//error message
#endif
			ifs.close();		//close stream(redundant)
			return isLoaded;	    //end constructor when failed
		}
		char* buffer = new char[MAX_FILE_NAME_LENGTH];	//store texture path spaced by line breaks
		int count = 0;			//count to count number of relative paths of textures in file
		while (ifs.getline(buffer, MAX_FILE_NAME_LENGTH))
		{
			count++;			//get number of texture files to load
		}

		//reset file flags and pointer position
		ifs.clear();
		ifs.seekg(0);

		for (int i = 0; i < count; i++)
		{
			ifs.getline(buffer, MAX_FILE_NAME_LENGTH);	//get file path
			vectorTextures.push_back(AEGfxTextureLoad(buffer));//load file path for textures, then place texture pointer in the vector
#if defined(DEBUG) | defined(_DEBUG)
			if (!vectorTextures.back())
			{	//print to error stream if texture failed to load
				std::cerr << "Texture at slot (" << i << ") failed to load. Replaced with null pointer\n";
				std::cerr << "File attempted to be loaded: [" << buffer << "]\n";
			}
#endif
		}
		isLoaded = true;//set boolean of isLoaded to true
		delete[] buffer;
		ifs.close();	//close file stream.
		return isLoaded;
	}

	/*!***********************************************************************
	\brief
		get a texture pointer given the index of the texture to be retrieved
	\param [in] index
		the index of the texture to be retrieved in the manager
	\return
		a pointer to the texture to be retrieved
	*************************************************************************/
	AEGfxTexture* TextureManager::getTexture(unsigned int index)
	{//check if index within size, then go to positon and return texture pointer
		//if index is out of range, return null pointer
		return (vectorTextures.size() > index) ? vectorTextures[index] : nullptr;
	}

	/*!***********************************************************************
	\brief
		adds a single texture based on the relative path of that texture file name
	\param [in] texturePathFile
		relative path of texture file
	\param [out] index
		return by reference the index the texture was loaded at in the manager.
	\return
		if the texture was successfully loaded by the manager.
	*************************************************************************/
	bool TextureManager::addTextures(const char* texturePathFile, unsigned int& index) 
	{
		bool loadSuccess{ false };	//if loading of texture was successful
		vectorTextures.push_back(AEGfxTextureLoad(texturePathFile));//load file path for textures, then place texture pointer in the vector
		if (!vectorTextures.back())
		{	//print to error stream if texture failed to load
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "Texture at slot (" << vectorTextures.size() << ") failed to load. Replaced with null pointer\n";
#endif
		}
		else
			loadSuccess = true;
		isLoaded = true;	//update the manager's is loaded boolean
		index = static_cast<unsigned int>(vectorTextures.size() - 1);	//update the in/out parameter index with the loaded texture index
		return loadSuccess;		//return if the texture was loaded successfully
	}
}

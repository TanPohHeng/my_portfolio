#pragma once
/*!************************************************************************
\file TextureManager.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-25-2023 (mmddyyyy)
\brief
This header file holds the data structures(class) related to Texture management.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "AEEngine.h"	
#include <vector>		//to store textures

/*!***********************************************************************
\namespace TextureList
\brief
	Namespace storing the functions and classes related to texture manager
*************************************************************************/
namespace TextureList
{
#define MAX_FILE_NAME_LENGTH 100	//define for making sure file names are not above 100 characters

//=========================================================================
//	Note:
//	Texture Manager does not support adding textures when it is loaded by
//	design. Done to prevent any potential loop addition of assets or any
//	potentially costly game run time loading of assets. Unload textures before
//	re-loading them from another file. It is suggested that all textures path
//	for one level be stored in a single file. As of 12/02/2023.
// 
// 	Update:
// 	As of 12/03/2023, Textures can be added to texture managers individually
// 	after main textures are loaded. Textures added this way are unloaded when
// 	TextureManager goes out of scope, or when destructor is called.
//=========================================================================

	/*!***********************************************************************
	\class TextureManager
	\brief
		class made with the goal of storing a group of textures in a single 
		data structure for ease of reference and handling. Also handles loading 
		of textures by batches or by single files. No free calls are required
		due to destructor freeing loaded textures automatically when object
		is destroyed or out of scope
	*************************************************************************/
	class TextureManager
	{
	public:
		//basic texture load and unload functions. 
		//supports batch loading from a file containing all the relative path of the textures to load
		bool loadTextures(const char* texturePathFile);		//batch load textures with this function
		void unloadTextures();								//mass unload all textures currently loaded
		//add a single texture into the manager, index returns by reference the index the loaded texture is stored at
		bool addTextures(const char* texturePathFile, unsigned int& index);	
		//get textures at an index stored in the manager
		AEGfxTexture* getTexture(unsigned int index);
		//check for if the current manager is loaded
		const bool getIsLoaded() const { return isLoaded; }
		
		//constructors for the TextureManager
		TextureManager();
		explicit TextureManager(const char* fileName);
		//destructor for TextureManager
		~TextureManager();

		//ensure Texture manager is non assignable or copyable. Prevent shallow copies
		TextureManager(TextureManager const&) = delete;
		void operator=(TextureManager const&) = delete;
	private:
		bool isLoaded{ false };						//if the manager is loaded with textures
		std::vector<AEGfxTexture*> vectorTextures{};//vector containing pointer to each texture
	};
}

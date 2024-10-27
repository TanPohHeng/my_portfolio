#pragma once
/*!************************************************************************
\file AnimationController.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-09-2023 (mmddyyyy)
\brief
This header file holds the data structures(class) to create Animaations via
mesh UVs through alpha engine, texture atlas and file with atlas path.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "AEEngine.h"
#include "TextureManager.hpp"//for the textureManager dependency
#include "MeshManager.hpp"	//for member variable of Animation
#include <fstream>			//for file reading
#include <string>			//for dealing with input strings
#include <iostream>			//for std::cerr when animation fails to load

namespace Render
{
	/*!***********************************************************************
	\class Animation
	\brief
		Animation class. Holds a reference to the texture manager with the 
		texture atlas for the animation. Also holds meshes with UV offset for 
		each frame of the animation. 
		One animation, One animation Class.
		Use in conjuction with AnimationInstance class to playback animations
		in render. Will crash the game if texture path fails to load due to 
		AEngine receiving a null pointer.
		Note: comments in this class are written above as there is a class member
		variable that greatly benefits from intelisense being able to display comments.
	*************************************************************************/
	class Animation
	{
	private:
		//texture manager reference holding the texture atlas required for animations
		TextureList::TextureManager& textureManager;
		//index of the texture atlas in the texture manager
		unsigned int animationAtlasIndex;
		//mesh manager to hold the frame position UVs
		Mesh::MeshManager framePositionManager;
		//	elements represets:
		// 	atlasColumns:	the number of columns in the texture atlas
		//	atlasRows:	the number of rows in the texture atlas
		//	numFrames:	number of frames in the animation
		unsigned int atlasDetails[3];

	public:
		//constructors for Animation class. As class has dependancies on TextureManager, 
		//no default constructors are allowed. Must be constructed with a refernce to a texture manager
		//texture atlas path file input is optional.
		Animation(const char* textureAtlasDetailsPath, TextureList::TextureManager& refmanager);
		Animation(TextureList::TextureManager& refManager);
		//destructor is defaulted as class member meshManager has its own destructor.
		~Animation() = default;
		//loads a texture atlas from a texture atlas path file and creates meshes with uvs based on the details in the 
		//texture atlas file path
		bool loadAnimationFromFile(const char* textureAtlasDetailsPath, const char delimiter = ' ');
		//loads a texture atlas into the texture manager reference in this class.
		bool loadAnimation(const char* textureAtlasPath, unsigned int columns, unsigned int rows, unsigned int frames);
		//create meshes in member meshManager for each frame of the animation via UVs
		void createAtlasMeshes(const unsigned int col, const unsigned int row, const unsigned int frameCount);
		//get the mesh with UVs for the frame of the animation at the index passed. returns last frame if index is invalid
		AEGfxVertexList* getFrameMesh(unsigned int index);

		//=================================
		//	accessors
		//=================================

		//get the texture manager index of the texture atlas currently loaded
		unsigned int getAtlasIndex() const { return animationAtlasIndex; }
		//get the index of the last frame(the number of frames in the animation)
		unsigned int getLastFrameIndex() const { return atlasDetails[2]; }
	};

}
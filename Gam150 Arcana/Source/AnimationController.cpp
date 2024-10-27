/*!************************************************************************
\file AnimationController.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-09-2023 (mmddyyyy)
\brief
This source file holds the data structures(class) to create Animaations via
mesh UVs through alpha engine, texture atlas and file with atlas path.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "AnimationController.hpp"

/*!***********************************************************************
\namespace Render
\brief
	Namespace storing the functions and classes to streamline the rendering
	process for Alpha Engine
*************************************************************************/
namespace Render 
{
	/*!***********************************************************************
	\brief
		Constructor for Animation. loads a texture atlas path file with details
		of the texture atlas of the animation to use.
	\par [in] textureAtlasPath
		the path of the file with the details and path of the texture atlas
	\par [in] refManager
		the texture manager to store the Animation's texture atlas
	*************************************************************************/
	Animation::Animation(const char* textureAtlasDetailsPath, TextureList::TextureManager& refManager)
		:textureManager{ refManager },
		animationAtlasIndex{ 0 },
		framePositionManager{},
		atlasDetails{ 0, 0, 0 }
	{	//load the file into the load animation file.
		loadAnimationFromFile(textureAtlasDetailsPath);
	}

	/*!***********************************************************************
	\brief
		Constructor for Animation. only requires a reference to a texture manager.
		Animation must be loaded post constructor before first use.
	\par [in] refManager
		the texture manager to store the Animation's texture atlas
	*************************************************************************/
	Animation::Animation(TextureList::TextureManager& refManager)
		:textureManager{refManager},animationAtlasIndex{ 0 },
		framePositionManager{}, atlasDetails{ 0, 0, 0 }
	{/*Empty by design*/ }

	/*!***********************************************************************
	\brief
		Loads details of an Animation through it's path file containing details
		of the texture atlas used for the current animation.
		formating of the file is :
		"column <delimiter> rows <delimiter> number of frames in the animation <delimiter> texture atlas relative path"
	\par [in] textureAtlasPath
		the path of the file with the details and path of the texture atlas
	\par [in] delimiter
		the delimiter character used to seperate individual information in the 
		formating of the texture atlas path file with details.
	*************************************************************************/
	bool Animation::loadAnimationFromFile(const char* textureAtlasDetailsPath, const char delimiter)
	{
		std::fstream ifs;		//input file stream
		ifs.open(textureAtlasDetailsPath);	//attempt to open file
		if (!ifs.is_open())
		{	//if unable to open, print error and return false
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "failed to load texture atlas path file";
#endif
			return false;
		}
		//create string store for path and size
		std::string pathBuffer,
			tempBuffer;

		if (std::getline(ifs, pathBuffer)) 
		{	//for first line, get details of animation.
			std::string::size_type pos{};
			for (unsigned int& i : atlasDetails) 
			{	//load the details of the animation into each element of our atlas details array
				pos = pathBuffer.find_first_of(delimiter);
				tempBuffer = pathBuffer.substr(0, pos);
				i = stoi(tempBuffer);
				pathBuffer.erase(0, pos + 1);
			}
			//load the animation atlas into the texture manager and create the UVs required for the animation
			if (!loadAnimation(pathBuffer.c_str(), atlasDetails[0], atlasDetails[1], atlasDetails[2])) 
			{//file could not be read, close the file stream and return after printing into the error stream
				ifs.close();
#if defined(DEBUG) | defined(_DEBUG)
				std::cerr << "failed to read texture atlas path file";
#endif
				return false;
			}
		}
		else { //file could not be read, close the file stream and return after printing into the error stream
			ifs.close(); 
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "failed to read texture atlas path file";
#endif
			return false;
		}	
		ifs.close();	//close the file stream
		return true;
	}

	/*!***********************************************************************
	\brief
		Loads the animation texture atlas into the texture manager linked to this
		animation
	\par [in] textureAtlasPath
		the path of the texture atlas of the animation
	\par [in] col
		the number of columns in the animation
	\par [in] row
		the number of rows in the animation
	\par [in] frames
		the number of frames in the animation
	\return
		if the animation atlas was successfully loaded
	*************************************************************************/
	bool Animation::loadAnimation(const char* textureAtlasPath, unsigned int col, unsigned int row, unsigned int frames) 
	{	//check if the texture was successfully added
		bool ret{ textureManager.addTextures(textureAtlasPath, animationAtlasIndex) };
		createAtlasMeshes(col, row, frames);	//create corresponding UVs in the mesh manager
		return ret;
	}

	/*!***********************************************************************
	\brief
		Loads the meshes with the right UVs for each frame of the animation
	\par [in] col
		the number of columns in the animation
	\par [in] row
		the number of rows in the animation
	\par [in] frameCount
		the number of frames in the animation
	*************************************************************************/
	void Animation::createAtlasMeshes(const unsigned int col, const unsigned int row, const unsigned int frameCount) 
	{	//if any of the passed variables are 0, do not continue loading
		if (!row || !col||!frameCount) { return; }	
		//if there was a previous animation loaded, unload it's mesh from the mesh manager
		if (framePositionManager.getIsLoaded()) { framePositionManager.unloadMeshes(); }	
		unsigned int count{};	//store the number of mesh created
		f64 uStart{},			//the start of the U axis for the texture
			uEnd{},				//the end of the U axis for the texture
			vStart{},			//the start of the V axis for the texture
			vEnd{};				//the end of the V axis for the texture
		
		for (unsigned int y{}; y < row; ++y)
		{	//for each row in the texture atlas,
			for (unsigned int x{}; x < col; ++x) 
			{	//for each column in the texture atlas,
				//find the UV start and end
				uStart = (1.0f / (f64)col) * (f64)x;
				uEnd = (1.0f / (f64)col) * (f64)(x + 1);
				vStart = (1.0f / (f64)row) * (f64)y;
				vEnd = (1.0f / (f64)row) * (f64)(y + 1);
				//add the mesh for that position to the mesh manager
				framePositionManager.addMesh((f32)uStart, (f32)uEnd, (f32)vStart, (f32)vEnd);
				++count;	//raise the mesh created count
				if (count == frameCount) { return; }	//when the required number of meshes are created, return
			}
		}
	}

	/*!***********************************************************************
	\brief
		get the mesh with UVs corresponding to the frame at index in the animation.
		returns the last frame if index is invalid
	\par [in] index
		the index of the frame to retrieve. index starts at frame 0.
	\return 
		the mesh with UVs corresponding to the frame at index in the animation
	*************************************************************************/
	AEGfxVertexList* Animation::getFrameMesh(unsigned int index)
	{	//if the index is vaild, return the index frame of the animation. Else return the last frame mesh.  
		return (index<atlasDetails[2]) ? framePositionManager.getMesh(index): framePositionManager.getMesh(atlasDetails[2]-1);
	}

}
#pragma once
/*!************************************************************************
\file Render.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 01-05-2023 (mmddyyyy)
\brief
This header file holds the data structures(class) to store values requried
for quick rendering of an object.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "AEEngine.h"
#include "MeshManager.hpp"		//for mesh manager
#include "TextureManager.hpp"	//for texture manager

/*!***********************************************************************
\namespace Render
\brief
	Namespace storing the functions and classes to streamline the rendering 
	process for Alpha Engine
*************************************************************************/
namespace Render
{
	/*!***********************************************************************
	\class RenderObject
	\brief
		class made with the goal to create a single location for developer to
		draw/store most of the information within an object in Alpha Engine that
		needs to be rendered
	*************************************************************************/
	//class RenderObject;	
	class RenderObject
	{
	public:
		//Exposed variables for easier access post inheritance.
		unsigned int textureIndex,	//the index of the texture in the textureManager
			meshIndex;				//the index of the mesh in the meshManager
		f32 positionXAxis,			//the render x axis position of this object in render space
			positionYAxis,			//the render y axis position of this object in render space
			rotationInRadians,		//the rotation of the object in radians
			scaleXAxis,				//the render x axis scale of this object in render space
			scaleYAxis;				//the render y axis scale of this object in render space

		//basic constructor, initialize 
		//default constructor
		RenderObject(f32 = 0.0f,	//x axis position
			f32 = 0.0f,				//y axis position
			f32 = 0.0f,				//rotation in radians
			f32 = 100.0f,			//scale in x
			f32 = 100.0f,			//scale in y
			unsigned int = 0,		//texture index
			unsigned int = 0,		//mesh index
			f32 = 1.0f				//transparency
		);
		//constructor with parameters for texture and mesh manager indexes only, rest are default
		RenderObject(unsigned int texIndex, unsigned meshIndex);

		//defaulted destructor. object does not contain memory allocated objects
		~RenderObject() = default;

		//get and set the transparency of the object
		f32 getAlpha();
		void setAlpha(const f32 alpha = 1.0f);

	private:
		f32 transparency;			//the transparency of the rendered object
	};

	//render function for rendering one object
	void render(float posX, float posY, float rotRad,
		float scaleX, float scaleY, AEGfxVertexList* mesh,
		AEGfxTexture* tex, float alpha = 1.0f);
	
	//render overloads for less parameter calls.
	void render(RenderObject& object, TextureList::TextureManager& textureList, Mesh::MeshManager& meshList);
	void render(RenderObject& object, AEGfxTexture* tex, AEGfxVertexList* mesh);
	
	//alternative render functions for rendering in mesh color.
	void renderInColor(RenderObject& object, Mesh::MeshManager& meshList);	

	//prevent calls to old versions of render functions
	
	//This overload is Depricated, do not use. Any previously used implementations should be re-written using other functions
	void renderInColor(RenderObject& object) = delete;
	//This overload is Depricated, do not use. Any previously used implementations should be re-written using other functions
	void render(RenderObject& object) = delete;
}

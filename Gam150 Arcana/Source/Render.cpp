/*!************************************************************************
\file Render.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 01-05-2023 (mmddyyyy)
\brief
This source file holds the data structures(class) to store values requried
for quick rendering of an object.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "Render.hpp"

/*!***********************************************************************
\brief
	constructor for RenderObject. Use to create a RenderObject a render object
	with a texture index and mesh index.
\par [in] texId
	The index of the object's texture in a TextureList::TextureManager
\par [in] meshId
	The index of the object's mesh in a Mesh::MeshManager
*************************************************************************/
Render::RenderObject::RenderObject(unsigned int texIndex, unsigned meshIndex) :
	positionXAxis{ 0 },
	positionYAxis{ 0 },
	rotationInRadians{ 0 },
	scaleXAxis{ 100.f },
	scaleYAxis{ 100.f },
	textureIndex{ texIndex },
	meshIndex{ meshIndex }
{/*Empty by design*/}

/*!***********************************************************************
\brief
	constructor for RenderObject. Use to create a RenderObject a render object
	with detailed position rotation and scale.
\par [in] xPos
	The X-axis position of the object on screen
\par [in] yPos
	The Y-axis position of the object on screen
\par [in] rad
	The Rotation of the object in radians counter clock wise from upright.
\par [in] xScale
	The scale of the object in the X-Axis
\par [in] scaleY
	The scale of the object in the Y-Axis
\par [in] texId
	The index of the object's texture in a TextureList::TextureManager 
\par [in] meshId
	The index of the object's mesh in a Mesh::MeshManager
*************************************************************************/
Render::RenderObject::RenderObject(f32 xPos, f32 yPos, 
	f32 rad,
	f32 xScale, f32 yScale, 
	unsigned int texId, unsigned int meshId,
	f32 alpha) :
	positionXAxis{ xPos },
	positionYAxis{ yPos },
	rotationInRadians{ rad },
	scaleXAxis{ xScale },
	scaleYAxis{ yScale },
	textureIndex{ texId },
	meshIndex{ meshId },
	transparency{ alpha }
{/*Empty by design*/}

/*!***********************************************************************
\brief
	Get the render transparency of the render object
\return
	the transparency of the object to render
*************************************************************************/
f32 Render::RenderObject::getAlpha() 
{ return transparency; }

/*!***********************************************************************
\brief
	Set the render transparency of the render object
\par [in] alpha
	the transparency of the object to render
*************************************************************************/
void Render::RenderObject::setAlpha(const f32 alpha) 
{ transparency = (alpha>1.0f) ? 1.0f: alpha; }

/*!***********************************************************************
\brief
	Renders a mesh with textures using the Alpha Engine at a transform given
	by the parameters. Use this to render on the fly without using a 
	Render::RenderObject
\par [in] posX
	The X-axis position the mesh should be rendered on screen
\par [in] posY
	The Y-axis position the mesh should be rendered on screen
\par [in] rotRad
	The Rotation of the mesh in radians counter clock wise
\par [in] scaleX
	The scale of the mesh to be rendered(in pixels) in the X-Axis
\par [in] scaleY
	The scale of the mesh to be rendered(in pixels) in the Y-Axis
\par [in] mesh
	A vertex list containing a list of Tris that form a mesh
\par [in] tex
	An AEGfxTexture pointer pointing to a loaded texture file in memory
*************************************************************************/
void Render::render(float posX, float posY, float rotRad,
	float scaleX, float scaleY, AEGfxVertexList* mesh,
	AEGfxTexture* tex, float alpha)
{
	//set the render mode to texture based.
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);	//full colors in all channels
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);			//allow transparency
	AEGfxSetTransparency(alpha);				//set transparency of all render object to passed value.
	// Set the texture to tex and set the texture draw mode
	AEGfxTextureSet(tex, 0, 0);
	AEGfxSetTextureMode(AEGfxTextureMode::AE_GFX_TM_AVERAGE);


	// Create a scale matrix base on passed in parameter
	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, scaleX, scaleY);

	// Create a rotation matrix base on passed in parameter
	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, rotRad);

	// Create a translation matrix base on passed in parameter
	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, posX, posY);

	// Concat the matrices (TRS)
	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	// set the transformation of the next rendered item to the information passed by parameters
	AEGfxSetTransform(transform.m);
	// Draw the mesh based on trangles
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

/*!***********************************************************************
\brief
	Overload of Render::render() function that takes the data from a referenced
	RenderObject to render an object, with the addition of another specified mesh
	and texture
\par [in] object
	a reference used to refer to the object with type RenderObject for use
	in rendering.
\par [in] tex
	An AEGfxTexture pointer pointing to a loaded texture file in memory
\par [in] mesh
	A vertex list containing a list of Tris that form a mesh

*************************************************************************/
void Render::render(RenderObject& object, AEGfxTexture* tex,AEGfxVertexList* mesh)
{	//use parameter object to call base render function.
	render(object.positionXAxis,object.positionYAxis, 
		object.rotationInRadians,
		object.scaleXAxis,object.scaleYAxis,
		mesh, tex, object.getAlpha());
}

/*!***********************************************************************
\brief
	Overload of Render::render() function that takes the data from a referenced
	RenderObject to render an object, along with a texture manager and mesh manager containing
	the texture and mesh for the sprite to be rendered
\par [in] object
	a reference used to refer to the object with type RenderObject for use
	in rendering.
\par [in] textureList
	A reference to the texture manager holding the texture to render at index defined in the object.
\par [in] meshList
	A reference to the mesh manager holding the mesh to render at index defined in the object.
*************************************************************************/
void Render::render(RenderObject& object, TextureList::TextureManager& textureList, Mesh::MeshManager& meshList)
{	//use parameter object to call base render function. Texture and mesh are from their respective manager.
	render(object.positionXAxis, object.positionYAxis,
		object.rotationInRadians,
		object.scaleXAxis, object.scaleYAxis,
		meshList.getMesh(object.meshIndex), textureList.getTexture(object.textureIndex));
}

/*!***********************************************************************
\brief
	Takes the data from a referenced RenderObject to render an object, 
	along with a mesh manager containing the mesh for the sprite to be rendered,
	using the mesh's in built color settings
\par [in] object
	a reference used to refer to the object with type RenderObject for use
	in rendering.
\par [in] meshList
	A reference to the mesh manager holding the mesh to render at index defined in the object.
*************************************************************************/
void Render::renderInColor(RenderObject& object, Mesh::MeshManager& meshList) 
{
	// Set the render mode to Color, and color grading and transparency to normal values
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);	//full colors in all channels
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);			//allow transparency
	AEGfxSetTransparency(1.0f);					//set transparency of all render object to opaque.


	// Create a scale matrix based on object scale.
	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, object.scaleXAxis, object.scaleYAxis);

	// Create a rotation matrix based on object rotation
	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, object.rotationInRadians);

	// Create a translation matrix based on object position on screen
	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, object.positionXAxis, object.positionYAxis);

	// Concat the matrices (TRS)
	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	// set the next render to the object's render position
	AEGfxSetTransform(transform.m);
	// draw the object
	AEGfxMeshDraw(meshList.getMesh(object.meshIndex), AE_GFX_MDM_TRIANGLES);
}
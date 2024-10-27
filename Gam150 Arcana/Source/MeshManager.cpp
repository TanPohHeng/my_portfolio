/*!************************************************************************
\file MeshManager.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This source file holds the definition of class for the storing and management
of meshes(AEVertexList)

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "MeshManager.hpp"
#include <iostream>			

/*!***********************************************************************
\namespace Mesh
\brief
	Namespace storing the functions and classes to Mesh management, creation
	and destruction.
*************************************************************************/
namespace Mesh {
	/*!***********************************************************************
	\brief
		destructor for the mesh manager. unloads meshes if there are meshes in
		the manager
	*************************************************************************/
	MeshManager::~MeshManager()
	{
		if (isLoaded)
		{	//unload meshes if there are meshes to unload
			unloadMeshes();
		}
	}
	/*!***********************************************************************
	\brief
		constructor for mesh manager.
	*************************************************************************/
	MeshManager::MeshManager() : isLoaded{false}
	{/*Empty by design*/}

	/*!***********************************************************************
	\brief
		unload all meshes from the mesh vector
	*************************************************************************/
	void MeshManager::unloadMeshes()
	{
		for (AEGfxVertexList* mesh : vectorMeshes)
		{	//for all mesh pointers in the vector of meshes
			if (mesh)
			{	//free mesh if pointer is valid
				AEGfxMeshFree(mesh);
			}
		}
		//empty the  vector of meshes
		vectorMeshes.erase(vectorMeshes.begin(), vectorMeshes.end());
		isLoaded = false;	//the manager is now empty
	}

	/*!***********************************************************************
	\brief
		get if the manager is loaded with at least 1 mesh
	\return
		if the manager is loaded with at least 1 mesh
	*************************************************************************/
	bool MeshManager::getIsLoaded() { return isLoaded; }

	/*!***********************************************************************
	\brief
		add a normalized card size mesh to the manager.
	\par [in] uStart
		the start of the U axis for the texture UV
	\par [in] uEnd
		the end of the U axis for the texture UV
	\par [in] vStart
		the start of the V axis for the texture UV
	\par [in] vEnd
		the end of the V axis for the texture UV
	\return
		the index at which the created mesh was stored at.
	*************************************************************************/
	unsigned int MeshManager::addCard(f32 uStart, f32 uEnd, f32 vStart, f32 vEnd)
	{	//start making a mesh
		AEGfxMeshStart();
		//create the tris for a card mesh
		AEGfxTriAdd(				// u     v  : u(horizontal) and v(vertical) of the texture to be added			
			-0.5f, -0.7f, 0xFFFF00FF, uStart, vEnd,
			0.5f, -0.7f, 0xFFFFFF00, uEnd, vEnd,
			-0.5f, 0.7f, 0xFF00FFFF, uStart, vStart);
		AEGfxTriAdd(
			0.5f, -0.7f, 0xFFFFFFFF, uEnd, vEnd,
			0.5f, 0.7f, 0xFFFFFFFF, uEnd, vStart,
			-0.5f, 0.7f, 0xFFFFFFFF, uStart, vStart);
		//store the end result
		AEGfxVertexList* mesh = AEGfxMeshEnd();
		//store the end result into a vector
		vectorMeshes.push_back(mesh);
		//make sure to signal that the manager has at least one loaded mesh
		isLoaded = true;
		//return the index at which the new mesh was created
		return static_cast<unsigned int>(vectorMeshes.size()-1);
	}

	/*!***********************************************************************
	\brief
		add a normalized mesh to the manager.
	\par [in] uStart
		the start of the U axis for the texture UV
	\par [in] uEnd
		the end of the U axis for the texture UV
	\par [in] vStart
		the start of the V axis for the texture UV
	\par [in] vEnd
		the end of the V axis for the texture UV
	\return
		the index at which the created mesh was stored at.
	*************************************************************************/
	unsigned int MeshManager::addMesh(f32 uStart, f32 uEnd, f32 vStart, f32 vEnd)
	{
		AEGfxVertexList* mesh;						//to store the mesh for pushing back
		AEGfxMeshStart();							//start making mesh
		//add tris
		AEGfxTriAdd(				// u     v  : u(horizontal) and v(vertical) of the texture to be added			
			-0.5f, -0.5f, 0xFFFF00FF, uStart, vEnd,
			0.5f, -0.5f, 0xFFFFFF00, uEnd, vEnd,
			-0.5f, 0.5f, 0xFF00FFFF, uStart, vStart);
		AEGfxTriAdd(
			0.5f, -0.5f, 0xFFFFFFFF, uEnd, vEnd,
			0.5f, 0.5f, 0xFFFFFFFF, uEnd, vStart,
			-0.5f, 0.5f, 0xFFFFFFFF, uStart, vStart);
		mesh = AEGfxMeshEnd();
		vectorMeshes.push_back(mesh);				//push the mesh into the manager's vector
		isLoaded = true;							//set the manager to is loaded
		return static_cast<unsigned int>(vectorMeshes.size()-1);	//return the index the mesh was stored at
	}

	/*!***********************************************************************
	\brief
		adds a mesh into the manager.
	\par [in] meshToAdd
		a pointer to the mesh to be added into the manger
	\return
		the index at which the created mesh was stored at.
	*************************************************************************/
	unsigned int MeshManager::addMesh(AEGfxVertexList* meshToAdd)
	{
		vectorMeshes.push_back(meshToAdd);	//push back the mesh to be added
		isLoaded = true;					//make the is loaded of this manager true
		return static_cast<unsigned int>(vectorMeshes.size()-1);	//return the index at which the mesh was stored
	}

	/*!***********************************************************************
	\brief
		get the mesh at a certain index of the mesh manager
	\par [in] index
		the index of the mesh stored in the manager to return
	\return
		the mesh stored at the index passed through parameter, or a null pointer 
		if the index is invalid
	*************************************************************************/
	AEGfxVertexList* MeshManager::getMesh(unsigned int index)
	{//check if index within size, then go to positon and return texture pointer
		//if index is out of range, return null pointer
		return (vectorMeshes.size() > index) ? vectorMeshes[index] : nullptr;
	}
}
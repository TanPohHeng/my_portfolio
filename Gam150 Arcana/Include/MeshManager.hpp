#pragma once
/*!************************************************************************
\file MeshManager.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This header file holds the declaration of class for the storing and management
of meshes(AEVertexList)

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "AEEngine.h"
#include <vector>		//for vector

/*!***********************************************************************
\namespace Mesh
\brief
	Namespace storing the functions and classes to Mesh management, creation
	and destruction.
*************************************************************************/
namespace Mesh
{
	/*!***********************************************************************
	\class
		MeshManager class. Handles management, creation and destruction of 
		meshes
	*************************************************************************/
	class MeshManager
	{
	public:
		//add a vertex list as a mesh to the manager 
		unsigned int addMesh(AEGfxVertexList* meshToAdd);
		//add a mesh with UV settings to the manager
		unsigned int addMesh(f32 uStart = 0, f32 uEnd = 1,
							f32 vStart = 0, f32 vEnd = 1);
		//a version of add mesh that creates a mesh of width:1,height:1.4
		unsigned int addCard(f32 uStart = 0, f32 uEnd = 1, f32 vStart = 0, f32 vEnd = 1);
		//unloads all meshes
		void unloadMeshes();
		//check if the manager has at least one mesh
		bool getIsLoaded();
		//get a mesh from the manager at an index
		AEGfxVertexList* getMesh(unsigned int index);
		//constructor and destructors for the mesh manager
		MeshManager();
		~MeshManager();
		
		//ensure mesh manager is non assignable or copyable. Prevent shallow copies
		MeshManager(MeshManager const&) = delete;
		void operator=(MeshManager const&) = delete;
	private:
		bool isLoaded{ false };		//is the current manager loaded with at least one mesh
		std::vector<AEGfxVertexList*> vectorMeshes;	//a vector to hold meshes stored in the manager
	};
}

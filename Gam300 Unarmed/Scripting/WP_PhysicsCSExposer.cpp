/*!************************************************************************
\file WP_PhysicsCSExposer.cpp
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\date 22/11/2024 (dd/mm/yyyy)
\brief
	This source file contains the definitions of functions that act as DLL_API
	for physics system functions. This avoids physics system needing to be a
	DLL_API, thus avoiding the need for JPH::BodyManager to be unmangled and
	allowing the project to build in release mode.
**************************************************************************/

#include <WP_EngineSystem/WP_CSharp/WP_PhysicsCSExposer.h>
#include <WP_EngineSystem/WP_PhysicsSystem.h>

namespace WP_PhysicsCS2CPP
{

	bool Raycast(glm::vec3 _origin, glm::vec3 _direction) 
	{
		[[maybe_unused]] WP_PhysicsSystem::WP_RayResult rResult;
		return Raycast(_origin, _direction, rResult);
	}

	bool Raycast(glm::vec3 _origin, glm::vec3 _direction, std::pair<unsigned, float>& _outResult) 
	{
		return WP_PhysicsSystem::GetInstance()->CastRay(_origin, _direction, _outResult);
	}

	bool Raycast(glm::vec3 _origin, glm::vec3 _direction, std::vector<std::pair<unsigned, float>>& _outResults) 
	{
		return WP_PhysicsSystem::GetInstance()->CastRay(_origin, _direction, _outResults);
	}

	bool RaycastHitObject(unsigned _objectID, glm::vec3 _origin, glm::vec3 _direction) 
	{
		return WP_PhysicsSystem::GetInstance()->CastRayAgainstObject(_objectID, _origin, _direction);
	}
	
	void SetVelocity (unsigned _objectID, glm::vec3 _newVel)	//funnel for both body and character physics types
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return; }
		if (pComp->m_isNPC) 
		{
			WP_PhysicsSystem::GetInstance()->CharacterSetLinearVelocity(_objectID, _newVel);
			return;
		}
		WP_PhysicsSystem::GetInstance()->SetBodyLinearVelocity(_objectID,_newVel);
	}
	void DLL_API AddVelocity(unsigned _objectID, glm::vec3 _newVel)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return; }
		if (pComp->m_isNPC)
		{
			WP_PhysicsSystem::GetInstance()->CharacterAddVelocity(_objectID, _newVel);
			return;
		}
		WP_PhysicsSystem::GetInstance()->AddForceToBody(_objectID, _newVel);
	}

	void DLL_API AddImpulse(unsigned _objectID, glm::vec3 _force)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return; }
		if (pComp->m_isNPC)
		{
			WP_PhysicsSystem::GetInstance()->CharacterAddImpulse(_objectID, _force);
			return;
		}
		WP_PhysicsSystem::GetInstance()->AddImpulseToBody(_objectID, _force);
	}
	
	glm::vec3 DLL_API GetVelocity(unsigned _objectID)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return glm::vec3(); }
		if (pComp->m_isNPC)
		{
			return WP_PhysicsSystem::GetInstance()->CharacterGetLinearVelocity(_objectID);
		}
		return WP_PhysicsSystem::GetInstance()->GetBodyLinearVelocity(_objectID);
	}

	void SetRotation(unsigned _objectID, glm::vec3 _newVel)	//funnel for both body and character physics types
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return; }
		if (pComp->m_isNPC)
		{
			WP_PhysicsSystem::GetInstance()->CharacterSetRotation(_objectID, _newVel);
			return;
		}
		WP_PhysicsSystem::GetInstance()->SetBodyRotation(_objectID, _newVel);
	}
	
	void DLL_API AddRotation(unsigned _objectID, glm::vec3 _newVel)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return; }
		if (pComp->m_isNPC)
		{
			WP_PhysicsSystem::GetInstance()->CharacterAddVelocity(_objectID, _newVel);
			return;
		}
		WP_PhysicsSystem::GetInstance()->AddForceToBody(_objectID, _newVel);
	}

	glm::vec3 DLL_API GetRotation(unsigned _objectID)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp) { return glm::vec3(); }
		if (pComp->m_isNPC)
		{
			return WP_PhysicsSystem::GetInstance()->CharacterGetLinearVelocity(_objectID);
		}
		return WP_PhysicsSystem::GetInstance()->GetBodyLinearVelocity(_objectID);
	}
	bool DLL_API GetIsCharacterGrounded(unsigned _objectID)
	{
		const WP_Physics3D* pComp = (WP_ComponentList<WP_Physics3D>::GetComponentList()->GetComponent(_objectID));
		if (!pComp || !(pComp->m_isNPC)) { return false; }	//no non-character implementation
		return WP_PhysicsSystem::GetInstance()->CharacterGetIsGrounded(_objectID);
	}
}
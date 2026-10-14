#pragma once
/*!************************************************************************
\file WP_PhysicsCSExposer.h
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\date 22/11/2024 (dd/mm/yyyy)
\brief
	This header file contains the declaration of functions that act as DLL_API
	for physics system functions. This avoids physics system needing to be a 
	DLL_API, thus avoiding the need for JPH::BodyManager to be unmangled and
	allowing the project to build in release mode.
**************************************************************************/


namespace WP_PhysicsCS2CPP 
{
	
	/*!***********************************************************************
	\brief
		Casts a ray starting at _origin and ending _origin + _direction.
	\param [in] _origin
		Starting coordinates of the ray to be casted.
	\param [in] _direction
		the orientation/direction of the ray to be casted.
	\return
		if the ray hits a object.
	*************************************************************************/
	bool DLL_API Raycast(glm::vec3 _origin, glm::vec3 _direction);
	/*!***********************************************************************
	\brief
		Casts a ray starting at _origin and ending _origin + _direction.
		return by reference the id and hitfraction of the first object  
		hit by the ray.
	\param [in] _origin
		Starting coordinates of the ray to be casted.
	\param [in] _direction
		the orientation/direction of the ray to be casted.
	\param [out] _outResult
		a pair containing the gameobject id of the hit object, and the 
		hitFraction of the hit. 
		(_origin + _direction * hitFraction = ray hit location)
	\return
		if the ray hits a object.
	*************************************************************************/
	bool DLL_API Raycast(glm::vec3 _origin, glm::vec3 _direction, std::pair<unsigned, float>& _outResult);
	/*!***********************************************************************
	\brief
		Casts a ray starting at _origin and ending _origin + _direction.
		return by reference the id and hitfraction of all game objects'  
		hit by the ray.
	\param [in] _origin
		Starting coordinates of the ray to be casted.
	\param [in] _direction
		the orientation/direction of the ray to be casted.
	\param [out] _outResults
		vector containing pair containing the gameobject id of the hit object, and the 
		hitFraction of all physics enabled game object hit by the ray. 
		(_origin + _direction * hitFraction = ray hit location)
	\return
		if the ray hits any object.
	*************************************************************************/
	bool DLL_API Raycast(glm::vec3 _origin, glm::vec3 _direction, std::vector<std::pair<unsigned, float>>& _out);
	/*!***********************************************************************
	\brief
		Casts a ray starting at _origin and ending _origin + _direction.
	\param [in] _objectID
		the specific object to be raycasted against. All ray hits with physics 
		enabled objects that do not have this id are ignored.
	\param [in] _origin
		Starting coordinates of the ray to be casted.
	\param [in] _direction
		the orientation/direction of the ray to be casted.
	\return
		if the ray hit the specific object.
	*************************************************************************/
	bool DLL_API RaycastHitObject(unsigned _objectID,glm::vec3 _origin, glm::vec3 _direction);

	bool DLL_API SphereCast(float _radius,
		glm::vec3 _center,
		std::vector<unsigned>& _outID);

	bool DLL_API CubeCast(
		glm::vec3 _center,
		glm::quat _globalRotation,
		glm::vec3 _globalScale,
		std::vector<unsigned>& _outID);

	bool DLL_API Shapecast(char _shapeType, glm::vec3 _scale, glm::quat _rotation, glm::vec3 _origin, glm::vec3 _direction, 
		std::vector<unsigned>& _outID, std::vector<glm::vec3>& _outCollisionPoint1,
		std::vector<glm::vec3>& _outCollisionPoint2, std::vector<glm::vec3>& _outCollisionNormal, 
		std::vector<float>& _outCollisionDepth);
	
	bool DLL_API Shapecast(char _shapeType, glm::vec3 _scale, glm::quat _rotation, glm::vec3 _origin, glm::vec3 _direction, 
		std::vector<unsigned>& _outID);

	bool DLL_API ShapecastHitObject(char _shapeType, unsigned _objectID, glm::vec3 _scale, glm::quat _rotation,glm::vec3 _origin, glm::vec3 _direction,
		glm::vec3& _outCollisionPoint1,
		glm::vec3& _outCollisionPoint2, glm::vec3& _outCollisionNormal,
		float& _outCollisionDepth);
	
	bool DLL_API ShapecastHitObject(char _shapeType, unsigned _objectID, glm::vec3 _scale, glm::quat _rotation,glm::vec3 _origin, glm::vec3 _direction);

	bool DLL_API ShapeCollide(char _shapeType, glm::vec3 _scale, glm::quat _rotation, glm::vec3 _origin, glm::vec3 _direction,
		std::vector<unsigned>& _outID, std::vector<glm::vec3>& _outCollisionPoint1,
		std::vector<glm::vec3>& _outCollisionPoint2, std::vector<glm::vec3>& _outCollisionNormal,
		std::vector<float>& _outCollisionDepth);

	bool DLL_API ShapeCollide(char _shapeType, glm::vec3 _scale, glm::quat _rotation, glm::vec3 _origin, glm::vec3 _direction,
		std::vector<unsigned>& _outID);

	void DLL_API SetVelocity(unsigned _objectID, glm::vec3 _newVel);

	void DLL_API AddVelocity(unsigned _objectID, glm::vec3 _newVel);

	void DLL_API SetPosition(unsigned _objectID, glm::vec3 _newVel);

	glm::vec3 DLL_API GetPosition(unsigned _objectID);

	void DLL_API AddImpulse(unsigned _objectID, glm::vec3 _force);

	glm::vec3 DLL_API GetVelocity(unsigned _objectID);

	void DLL_API SetMotionType(unsigned _objectID, unsigned _motionType);

	void DLL_API SetRotation(unsigned _objectID, glm::vec3 _newVel);

	void DLL_API AddRotation(unsigned _objectID, glm::vec3 _newVel);

	glm::vec3 DLL_API GetRotation(unsigned _objectID);

	bool DLL_API GetIsCharacterGrounded(unsigned _objectID);

	constexpr unsigned GetObjectLayerSize = 16; //update according to JPH_OBJECT_LAYER_BITS

	using physicsCharLayerArr = char[GetObjectLayerSize];	//max size char array needed for layer interactions

	void DLL_API SetPhysicsLayerInteraction(unsigned _layer , physicsCharLayerArr& _layerReaction);

	void DLL_API GetPhysicsLayerInteraction(unsigned _layer , physicsCharLayerArr& _layerReaction);

	void DLL_API SetPhysicsLayerCollideWithMoving(unsigned _layer, bool _shouldCollideWithMoving);

	bool DLL_API GetPhysicsLayerCollideWithMoving(unsigned _layer);

	void DLL_API PrintPhysicsLayerInteraction();

	void DLL_API PhysicsMoveKinematic(unsigned _objectID, glm::vec3 _end,float _timeToReachDestination);

	void DLL_API PhysicsMoveKinematic(unsigned _objectID, glm::vec3 _start, glm::vec3 _end,float _timeToReachDestination);
}
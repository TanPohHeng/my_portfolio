/******************************************************************************/
/*!
\file		Transform.h
\author 	Tan Poh Heng, t.pohheng, 620006722
\par    	email: t.pohheng@digipen.edu
\date   	November 21, 2023
\brief		Header file containing transform interface.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once
#include "Pch.h"
#include "ECS/BaseComponent.h"
#include "Maths/Math_Includes.h"

namespace ANC {
namespace ECS 
{
	class Transform_Component : public BaseComponent
	{
	//this tag is here to support previously written code. In the ideal world, this section should be private.
	public:	
		static constexpr COMPONENT_TYPES compType = COMPONENT_TYPES::TRANSFORM;

		
		//if axis cannot be adjusted when adding a vector to position
		bool freezeX{ false },
			freezeY	{ false },
			freezeZ	{ false };

		bool& freeze_position_x{ freezeX },
			& freeze_position_y{ freezeY },
			& freeze_position_z{ freezeZ };

		//default values on reset. reset is used for scene resets.
		Math::Vec3 c_position{};
		Math::Vec2 c_scale{ Math::Vec2{ 1.0f, 1.0f } };
		f64 c_rotation{};
		
		
	public:	//public for now. encapsulation seems slightly redundant and time costly to code around
		f64 rotInRad{};
		f64& rotation{ rotInRad };
		Math::Vec3 position{};
		Math::Vec2 scale{Math::Vec2(1.0f,1.0f)};
		operator Math::Vec2() { return position; };	//quick return for position.
		Math::Vec3 parentOffset{};
		Math::Vec3 parentRotOffset{};
		
		//default constructor
		Transform_Component() = default;
		Transform_Component(Math::Vec3 defaultPos, Math::Vec2 defaultScale = Math::Vec2(1.0f,1.0f), f64 defaultRotation = f64())
			: c_position{ defaultPos }, c_scale{ defaultScale }, c_rotation {defaultRotation},
			rotInRad{ defaultRotation }, position{ defaultPos }, scale {defaultScale}
		{/*Empty by design*/}

		Transform_Component& operator=(const Transform_Component& ref) 
		{
			
			freezeX = ref.freezeX;
			freezeY = ref.freezeY;
			freezeZ = ref.freezeZ;

			c_position = ref.c_position;
			c_scale = ref.c_scale;
			c_rotation = ref.c_rotation;

			rotInRad = ref.rotInRad;
			position = ref.position;
			scale = ref.scale;

			parentOffset.x = ref.parentOffset.x;
			parentOffset.y = ref.parentOffset.y;

			BaseComponent* instance = this;
			*instance = *(reinterpret_cast<const BaseComponent*>(&ref));

			return *this;
		}

		//setters and mutator for position
		void SetPosition(Math::Vec3 const& val) { position = val; }
		void SetPosition(Math::Vec3&& val) { position = std::move(val); }
		void SetOffset(Math::Vec3 const& val) { parentOffset = val; }
		void SetOffset(Math::Vec3&& val) { parentOffset = std::move(val); }
		void SetRotOffset(Math::Vec3 const& val) { parentRotOffset = val; }
		void SetRotOffset(Math::Vec3&& val) { parentRotOffset = std::move(val); }
		void AddToPosition(Math::Vec3 const& val) { position += {(freezeX) ? 0 : val.x, (freezeY) ? 0 : val.y, (freezeZ) ? 0 : val.z}; }
		void SetParentOffSet(Math::Vec2 const& val) { parentOffset = val; }
		//setter for scale
		void SetScale(Math::Vec2 const& val) { scale = val; }
		void SetScale(Math::Vec2&& val) { scale = std::move(val); }
		void ScaleScale(f64 percent) { scale *= percent; }
		//setter for rotation
		void SetRotation(f64 val) { rotInRad = val; }

		//Getters for member variables
		Math::Vec3 GetPosition() const noexcept { return position; }
		Math::Vec3 GetOffset() const noexcept { return parentOffset; }
		Math::Vec3 GetRotOffset() const noexcept { return parentRotOffset; }
		Math::Vec2 GetScale() const noexcept { return scale; }
		f64 GetRotation()	  const noexcept { return rotInRad; }

		void Reset()	//reset function
		{	//reset to default state. only accessible to non-const objects.
			position = c_position;
			scale = c_scale;
			rotInRad = c_rotation;
		}

		void Cache()	//cache function
		{	//set default state. only accessible to non-const objects.
			c_position = position;
			c_scale = scale;
			c_rotation = rotInRad;
		}

		//component type return function. for BaseComponent Class
		COMPONENT_TYPES const GetCompType() override {
			return compType;
		}

		//Additional helper function.
		void MoveTowards(const Math::Vec2& dist) { AddToPosition(dist); }
		
		//friend systems.
		//friend for now until transform System encapsulation functions is enough to power other systems.
		//Ideally remove this by M6 submissions. 


#ifdef _DEBUG
	//Debug And Testing function
	friend void testCollsionUpdate();
#endif // DEBUG		
	};	
	
	
	
}//end of ECS namespace
}//end of ANC namespace

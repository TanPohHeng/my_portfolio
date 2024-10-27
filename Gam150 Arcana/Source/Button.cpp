/*!************************************************************************
\file Button.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-28-2023 (mmddyyyy)
\brief
This header file holds the declaration of class functions for buttons and 
overloads for rendering said class. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "Button.hpp"
#include <iostream>

/*!***********************************************************************
\namespace UI
	namespace UI for storing UI system elements
*************************************************************************/
namespace UI {

	/*!***********************************************************************
	\brief
		single param constructor for Button. parameters besides the first are
		default-able
	\param [in] functionPointer
		a function pointer to the function to call to activate the button.
		non-default-able.
	\param [in] meshID
		the index of mesh of the button in the mesh manager
	\param [in] texID
		the index of texture of the button in the texture manager
	\param [in] positionVector
		the vector with the x and y axis positions
	\param [in] scaleVector
		the vector with the x and y axis scale of the button
	\param [in] hoverSize
		the scale of the button whem hoving over it
	\param [in] clickSize
		the scale of the buttin when clicked
	*************************************************************************/
	Button::Button(FUNC_PTR functionPointer, std::string displayText, s8 fontIndex,
		unsigned int meshID, unsigned int texID,
		AEVec2 positionVector, AEVec2 scaleVector,
		f32 hoverSize, f32 clickSize
	)
		: function{ functionPointer },
		hoverFactor{ hoverSize }, clickFactor{ clickSize },
		text{ displayText }, fontID{ fontIndex },
		clicked{ false }, hovered{ false }, fontSize{ 1 }
		, RenderObject(positionVector.x, positionVector.y, 0,
			scaleVector.x, scaleVector.y, meshID, texID)
	{/*Empty by design*/}

	/*!***********************************************************************
	\brief
		get the scale of the button depending on whether they are hovered or clicked
	\return
		the scale of the button depending on whether they are hovered or clicked
	*************************************************************************/
	const AEVec2 Button::getScale() const
	{
		AEVec2 ret{ this->scaleXAxis , this->scaleYAxis };	//copy the scale of the button

		//find if the button is hovered or clicked, and return the appropriate scale
		(clicked) ?
			AEVec2Scale(&ret, &ret, clickFactor) :
			(hovered) ? AEVec2Scale(&ret, &ret, hoverFactor) :
			AEVec2Set(&ret, ret.x, ret.y);
		return ret;
	}

	/*!***********************************************************************
	\brief
		update the transformation of the button.
	\param [in] posX
		the button's new x-axis position
	\param [in] posY
		the button's new y-axis position
	\param [in] rot
		the button's new rotation in radians
	\param [in] scaleX
		the button's new scale in the x-axis
	\param [in] scaleY
		the button's new scale in the y-axis
	*************************************************************************/
	void Button::updateTransform(f32 posX, f32 posY,
		f32 rot,
		f32 scaleX, f32 scaleY) 
	{	//set the button's transform to the new transform.
		positionXAxis = posX;
		positionYAxis = posY;
		rotationInRadians = rot;
		scaleXAxis = scaleX;
		scaleYAxis = scaleY;
	}
}

namespace Render 
{
	/*!***********************************************************************
	\brief
		render overload for button to render with text
	\param [in] button
		the button to be rendered
	\param [in] fontList
		the fontManager with the fonts for the button text
	\param [in] textureList
		the texture manager with the texture for the button
	\param [in] meshList
		the mesh manager with the mesh for the button
	*************************************************************************/
	void render(UI::Button& button, Fonts::FontManager& fontList,
		TextureList::TextureManager& textureList, Mesh::MeshManager& meshList)
	{
		//render the texture part of the button
		render(button, textureList, meshList);

		//variables for a width and height of the text and font
		f32 w{  }, h{  };

		//get the width and height of the text in the button.
		AEGfxGetPrintSize(fontList.getFont(button.getFontID()), const_cast<char*>(button.getText().c_str()),
			button.getFontSize(), w, h);
		//find the ceter position of the button for text in percent screen space
		f32 bXScreenPosCenter{ button.getPos().x / (AEGetWindowWidth() / 2) },
			bYScreenPosCenter{ button.getPos().y / (AEGetWindowHeight() / 2) };

		//print the font
		AEGfxPrint(fontList.getFont(button.getFontID()), const_cast<char*>(button.getText().c_str()),
			bXScreenPosCenter - w * 0.5f,
			bYScreenPosCenter - h * .5f,
			button.getFontSize(),
			0, 0, 0);

	}
}
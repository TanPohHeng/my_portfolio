#pragma once
/*!************************************************************************
\file Button.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 01-28-2023 (mmddyyyy)
\brief
This header file holds the declaration of class functions for buttons and 
overloads for rendering said class. 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/

#include "AEEngine.h"
#include "Render.hpp"		//for the definition of a RenderObject
#include "FontManager.hpp"	//for the definition of a font manager
#include <string>	

/*!***********************************************************************
\namespace UI
	namespace UI for storing UI system elements
*************************************************************************/
namespace UI {
	//alias for button functions.
	using FUNC_PTR = void(*)();
	/*!***********************************************************************
	\class
		Button class is a child class of RenderObject. Holds additional data
		required to perform the basic function of a button on screen.
	*************************************************************************/
	class Button : public Render::RenderObject
	{
		//member variables
	private:
		const FUNC_PTR function;	//the function called when button is used
		f32 hoverFactor,			//the scale of the button when hovered
			clickFactor;			//the scale of the button when clicked
		std::string text;			//the text on the button
		s8 fontID;					//the id of the font to use for text
		bool clicked,				//is the button clicked?
			hovered;				//is the button hovered?
		f32 fontSize;				//the size of the fonts for text.
	public:

		//member functions and constructors
		//constructor for buttons. forced initialization with at least one parameter
		Button(FUNC_PTR functionPointer, std::string displayText = "Button", s8 fontIndex = 0,
			unsigned int meshID = 0, unsigned int texID = 0,
			AEVec2 positionVector = { 0,0 }, AEVec2 scaleVector = { 10,10 },
			f32 hoverSize = 1.2f, f32 clickSize = 0.8f);
		Button() = delete;	//force no default construction

		//activate the function linked to the button
		void callFunction() { function(); }

		//update the screen transformation of the button
		void updateTransform(f32 posX, f32 posY,
			f32 rot,
			f32 scaleX, f32 scaleY);

		//getter for renderObject variables and booleans
		bool& isClicked() { return clicked; }
		bool& isHovered() { return hovered; };

		//text getter settter
		const std::string getText() const { return text; }
		void setText(std::string newText) { text = newText; }

		//font getter setter
		const s8 getFontID() const { return fontID; }
		void setFontID(s8 newID) { fontID = newID; }

		//texture getter setter
		const unsigned int getTextureID() const { return textureIndex; }
		void setTextureID(const unsigned int newIndex) { textureIndex = newIndex; }

		//mesh getter setter
		const unsigned int getMeshID() const { return meshIndex; }
		void setMeshID(const unsigned int newIndex) { meshIndex = newIndex; }

		//position getter setter
		const AEVec2 getPos() const { AEVec2 ret{positionXAxis,positionYAxis}; return ret; }
		void setPos(AEVec2 newVec) { positionXAxis = newVec.x, positionYAxis = newVec.y; }

		//scale getter setter
		const AEVec2 getScale() const;
		void setScale(AEVec2 newVec) { scaleXAxis = newVec.x, scaleYAxis = newVec.y; }

		//rotation getter setter
		const f32 getRot() { return rotationInRadians; }
		void setRot(f32 rotInRadians) { rotationInRadians = rotInRadians; }

		//font size getter settter
		const f32 getFontSize() { return fontSize; }
		void setFontSize(f32 newSize) { fontSize = newSize; }
	};
}

namespace Render 
{
	//render overload for rendering buttons with texts.
	void render(UI::Button& button, Fonts::FontManager& fontList, TextureList::TextureManager&, Mesh::MeshManager&);
}
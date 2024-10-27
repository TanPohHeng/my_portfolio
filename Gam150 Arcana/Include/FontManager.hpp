#pragma once
/*!************************************************************************
\file FontManager.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This header file holds the data structures(class) designed for the management
of fonts via AEngine functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "AEEngine.h"
#include <vector>

/*!***********************************************************************
\namespace Fonts
\brief
	Namespace storing the functions and classes for font managements with
	AEngine built in font system
*************************************************************************/
namespace Fonts 
{
	/*!***********************************************************************
	\class
		FontManager class. Handles management, creation and destruction of
		fonts. Due to an issue in Alpha Engine, this manager should be global.
	*************************************************************************/
	class FontManager 
	{
	public:
		//constructors and destructors of font manager
		FontManager(const char*);	//create a fontmanager and read a file based on parameter(batch load file only)
		FontManager() = default;	//default constructor
		~FontManager();				//destructor.
		//no copying due to AEngine issues
		FontManager(FontManager const&) = delete;
		FontManager operator=(FontManager const&) = delete;

		//load a font into the manager
		s8 loadFont(const char*, int);
		//batch load fonts from a file
		bool loadFontFromList(const char*);
		//unload all fonts loaded. Do not reload fonts. Only call this function at the end of the app lifetime
		void unloadFonts();
		//get the font index from a storage index
		s8 getFont(unsigned int index);
	private:
		std::vector<s8> fontVector;	//the vector of font indexes
		char delimiter{' '};		//the delimiter used for file reading
	};

}
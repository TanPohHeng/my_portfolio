/*!************************************************************************
\file FontManager.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This source file holds the data structures(class) designed for the management
of fonts via AEngine functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "FontManager.hpp"
#include <iostream>
#include <fstream>
#include <string>

namespace 
{	//define the maximum file length scoped within the file
#define MAX_FILE_LENGTH 100
}
/*!***********************************************************************
\namespace Fonts
\brief
	Namespace storing the functions and classes for font managements with
	AEngine built in font system
*************************************************************************/
namespace Fonts 
{
	/*!***********************************************************************
	\brief
		single param constructor for FontManager. Immediately batch load fonts
		from a file path after construction
	\param [in] filename
		relative path of file containing the font file paths
	*************************************************************************/
	FontManager::FontManager(const char* fileName)
	{	//forced load on construction
		loadFontFromList(fileName);
	}
	/*!***********************************************************************
	\brief
		destructor for fontManager
	*************************************************************************/
	FontManager::~FontManager() 
	{	//forced unload on destruction
		unloadFonts();
	}

	/*!***********************************************************************
	\brief
		load a single font from a ttf file
	\param [in] fontFileName
		relative path of the .ttf file
	\param [in] fontSize
		the size the font should be loaded from the .ttf file
	\return 
		the index of the font in AlphaEngine 
	*************************************************************************/
	s8 FontManager::loadFont(const char* fontFileName, int fontSize) 
	{
		std::fstream ifs;		//input file stream
		ifs.open(fontFileName); //attempt to open file
		if (!ifs.is_open()) 
		{	//if unable to open, print error and return false
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "failed to load font file";
#endif
			return 0;
		}
		fontVector.push_back(AEGfxCreateFont(fontFileName, fontSize));
		ifs.close();
		return fontVector[fontVector.size() - 1];
	}

	/*!***********************************************************************
	\brief
		batch load multiple fonts from a font path file with the formatting 
		of 'filepath'+delimiter+'fontsize'
	\param [in] fontPathFile
		relative path of the file with the relative path of the fonts to batch load
	\return
		if the operation was successful. Does not gurantee that all fonts from
		the batch fonts file was loaded. check for -1 when retreiving font index
		from the manager for fonts that failed to load
	*************************************************************************/
	bool FontManager::loadFontFromList(const char* fontPathFile) 
	{
		std::fstream ifs;		//input file stream
		ifs.open(fontPathFile);	//attempt to open file
		if (!ifs.is_open())
		{	//if unable to open, print error and return false
#if defined(DEBUG) | defined(_DEBUG)
			std::cerr << "failed to load font path file";
#endif
			return false;
		}
		//create string store for path and size
		std::string pathBuffer,
			sizebuffer;
		int size{};			//create store for font size given in file

		while (std::getline(ifs, pathBuffer))
		{	//while not end of file
			//find last part of string meant for size
			std::string::size_type pos{ pathBuffer.find_last_of(delimiter) };
			//increment the postion past the delimiter
			pos += 1;
			//set last portion of string meant for size to another buffer
			sizebuffer = pathBuffer.substr(pos, pathBuffer.size() - pos);
			//clean the path buffer of last portion meant for size AND space used to denote position
			pathBuffer.erase(pos - 1, pathBuffer.size() - (pos - 1));
			//make size the font size based on text
			size = stoi(sizebuffer);
			//push back the font based on file text
			fontVector.push_back(AEGfxCreateFont(pathBuffer.c_str(), size));
#if defined(DEBUG) | defined(_DEBUG)
			//debug message here
			//print if font was unable to be loaded by alpha engine
			if (fontVector[fontVector.size() - 1] == -1) 
			{
				std::cout << "Font from path [" << pathBuffer << "] was unable to be loaded. Access index replaced with -1.\n";
			}
			//print if font was loaded successfully by alpha engine
			std::cout << "Font from path [" << pathBuffer << "] was loaded. size("<<size<<") Access index is"<< fontVector.size()-1 << ".\n";
#endif
		}
		ifs.close();	//close the filestream
		return true;	//return operation successful
	}

	/*!***********************************************************************
	\brief
		unload all loaded fonts. No, do not call this unless at the end of the
		application's life span. When called anywhere else will result in being 
		unable to reload previously freed fonts.
	*************************************************************************/
	void FontManager::unloadFonts() 
	{
		for (s8 i : fontVector) 
		{	//for each non -1 indexed font in our font vector, destroy font
			if(i != -1)	//if the font is valid, free it
				AEGfxDestroyFont(i);
		}
	}

	/*!***********************************************************************
	\brief
		get font index of the font stored in the font manager at passed index.
		returns the last valid font if the passed index is invalid.
	\param [in] index
		the storage index of the font to use stored in the font manager
	\return
		the AEngine index for the font stored at the storage index of the font
		manager.
	*************************************************************************/
	s8 FontManager::getFont(unsigned int index) 
	{	//return
		return ((size_t)index>=fontVector.size())? fontVector[fontVector.size() - 1] : fontVector[index];
	}
}
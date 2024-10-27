//---------------------------------------------------------
// file:	settings.h
// author:	Tan Poh Heng(button and slider logic), Ong You Yang(sprites implementation)
// email:	t.pohheng@digipen.edu ,youyang.o@digipen.edu
//
// brief:	Contains function declarations related settings options
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------


// ---------------------------------------------------------------settings Headers---------------------------------------------------------------
void settings_Init(void);

void settings_Update(void);

void settings_Exit(void);



///===================================================
///	Sound Settings Headers
///===================================================

//forcibly get an X value that is within slider width
float getWithinSliderValue(float const Xpos);

//convert slider value to volume equivalent
float convertSliderValueToVolume(float const Xpos);

//convert volume to slider value equivalent
float convertVolumeToSliderValue(float const Xpos);

//draws the slider range (call before calling drawSliderButton())
void drawSliderbase(float const Ypos);

//draws the slider Button 
double drawSliderButton(float const XPos, float const Ypos);

//set all other volume groups expected to be used to a set volume
void handleVolumeMaster(float const volume);

//mutes all assumed used sound groups
void muteAll(void);

//initialize all volume levels to max, ignore all previous settings
void intiVolume(void);

//draws all sliders
void drawAllSliders(void);

//handles all slider and volume interaction, all without params too :), When sounds implemented, check with sounds as well. Thanks Danish \('<
void handleSliderInteraction(void);
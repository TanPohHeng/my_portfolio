//---------------------------------------------------------
// file:	levelTransition.c
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Contains functions related to level transition animations
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "cprocessing.h"
#include "levelTransition.h"
#define ANIMATION_LENGTH .75	//length of the animation, in seconds


//remeber to free!!
//load image when called by a filepath defined in levelTransition.h
static void initImage(void) 
{
	lvlTransitImage = CP_Image_Load(TRANSITION_IMAGE_PATH);
}

//(IMPORTANT!!) 
//free image to prevent memory shenanigans, use on game_exit if not using the exit animation	(IMPORTANT!!) 
//(IMPORTANT!!) 
void freeTransitionImage(void)
{
	if (lvlTransitImage != NULL)
	{
		CP_Image_Free(&lvlTransitImage);
	}
}

//set the animation to be full screen covered
void setSpriteExtended(void) 
{
	imageXpos = 0 - TRANSITION_IMAGE_WIDTH;

	CP_Settings_Fill(CP_Color_Create(50, 50, 50, 255));

	CP_Graphics_DrawRect
	((float)imageXpos + (float)(TRANSITION_IMAGE_WIDTH - 10), 0,
		(float)CP_System_GetWindowWidth() - (float)(imageXpos - (float)TRANSITION_IMAGE_WIDTH), (float)CP_System_GetWindowHeight());
}

//set the animation to start at outside screen
void setSpriteReseted(void)
{
	imageXpos = 0 - TRANSITION_IMAGE_WIDTH;

}

//animates the sprite from outside the screen on the left to the right, ends with screen covered WIP: make this a pointer style to pass into exitLevelTransition()?
int animateIn()
{	//animate till sprite is out of screen
	if (imageXpos < CP_System_GetWindowWidth() + CP_Image_GetWidth(lvlTransitImage))
	{	//update the xpostion of the the sprite
		imageXpos += ((CP_System_GetWindowWidth() / ANIMATION_LENGTH) +TRANSITION_IMAGE_WIDTH) * CP_System_GetDt();
		return ANIMATION_NOT_DONE;	//not done animating
	}
	return ANIMATION_DONE;
}

//animates the sprite from outside the screen on the left to the right, ends with screen clear WIP: make this a pointer style to pass into exitLevelTransition()?
int animateOut()
{	//animate till sprite is out of screen
	if (imageXpos < CP_System_GetWindowWidth())
	{	//update the xpostion of the the sprite
		imageXpos += ((CP_System_GetWindowWidth() + TRANSITION_IMAGE_WIDTH))/ ANIMATION_LENGTH * CP_System_GetDt();
		return ANIMATION_NOT_DONE;	//not done animating
	}
	return ANIMATION_DONE;
}

//handles the animation logic for level entry, image is preserved for exitLevelTransition(), which is expected to be called  
int initLevelTransition()
{
	if (lvlTransitImage == NULL)
	{
		initImage();
	}
	//set draw position for the image
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	CP_Settings_Fill(CP_Color_Create(50, 50, 50, 255));

	//renders a rect to follow the sprite out of the screen
	CP_Graphics_DrawRect
	((float)imageXpos + (float)(TRANSITION_IMAGE_WIDTH-10), 0,
		(float)CP_System_GetWindowWidth() - (float)(imageXpos - TRANSITION_IMAGE_WIDTH), (float)CP_System_GetWindowHeight());

	//draw sprite at new location
	int a = CP_Image_GetWidth(lvlTransitImage);
	CP_Image_DrawAdvanced(
		lvlTransitImage,
		(float)imageXpos,
		0,
		TRANSITION_IMAGE_WIDTH,
		TRANSITION_IMAGE_HEIGHT,
		255,
		180);
	//update logic for next frame and return if the animation is done
	return animateOut();
}

//handles the animation logic for level exit, as denoted by param isAnimationComplete. 
//return value is 0 when animation is complete, meant to be feed into isAnimation complete parameter
int exitLevelTransition(int isAnimationComplete)
{
	// if there is no image, initialize image
	if (lvlTransitImage == NULL)
	{
		initImage();
	}
	//set draw position for the image
	CP_Settings_ImageMode(CP_POSITION_CORNER);

	//set the fill color for rect
	CP_Settings_Fill(CP_Color_Create(50, 50, 50, 255));

	//renders a rect to follow the sprite to cover the screen
	CP_Graphics_DrawRect
	(0, 0, (float)imageXpos+10.0f, (float)CP_System_GetWindowHeight());

	//draw sprite at new location
	CP_Image_Draw(
		lvlTransitImage,
		(float)imageXpos,
		0,
		TRANSITION_IMAGE_WIDTH,
		TRANSITION_IMAGE_HEIGHT,
		255);

	if (!isAnimationComplete) {
		freeTransitionImage();		//free the image, re init when in new scene
		return ANIMATION_DONE;
	}
	//update logic for next frame and return if the animation is done
	return	animateOut();
}
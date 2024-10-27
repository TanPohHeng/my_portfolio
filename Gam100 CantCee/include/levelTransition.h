#pragma once
//---------------------------------------------------------
// file:	levelTransition.h
// author:	Tan Poh Heng
// email:	t.pohheng@digipen.edu
//
// brief:	Contains function declarations related to level transition animations
//			for use in level scripts. animations are rendered independent of level renders
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2022 DigiPen, All rights reserved.
//---------------------------------------------------------


#ifndef TRANSITION_IMAGE_PATH
#define TRANSITION_IMAGE_PATH "./Assets/LevelTransitionSprite.png"
#define TRANSITION_IMAGE_WIDTH 230
#define TRANSITION_IMAGE_HEIGHT 800
#endif // !IMAGE_PROPERTIES

enum
{
	ANIMATION_DONE = 0,
	ANIMATION_NOT_DONE = 1
};

static CP_Image lvlTransitImage;	//the image for the edge of the transition
static double imageXpos = -230;		//the x position 

//Use this to free the transit image outside leveltransition.c
void freeTransitionImage(void);

//sets situation where screen starts fully covered 
void setSpriteExtended(void);

//sets situation where screen starts uncovered 
void setSpriteReseted(void);

//animates the sprite into the render, assumes sprite starts at 0-sprite width
int animateIn(void );

//animates the sprite out of render, assumes sprite starts at 0-sprite width
int animateOut(void );

//implement level entry into new level animation, returns 0 when the animation is complete, returns 1 when animation is still running
int initLevelTransition(void );

//implement level animation, returns 0 when the animation is complete, returns 1 when animation is still running
int exitLevelTransition(int isAnimationComplete);
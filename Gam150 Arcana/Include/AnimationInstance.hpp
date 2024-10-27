#pragma once
/*!************************************************************************
\file AnimationInstance.hpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This header file holds the declaration of class designed to instance Animations.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "AEEngine.h"
#include "AnimationController.hpp" //for Animations 

/*!***********************************************************************
\namespace Render
\brief
	Namespace storing the functions and classes to streamline the rendering
	process for Alpha Engine
*************************************************************************/
namespace Render
{
	/*!***********************************************************************
	\class
		Animation instance class. Holds the logical implementation for an
		instance of an Animation. Require a reference to the animation to
		play(class Animation(to be renamed Animation)). Something like a 
		Render::RenderObject, but for animations
		One animation instance, one AnimationInstance.
	*************************************************************************/
	class AnimationInstance
	{	
		//the animation to instance
		Animation& animation;			
		unsigned int currentFrame,		//the current frame of this instance 
			lastFrame;					//the last frame of the animation 
		//the length of time for a single frame in seconds
		f32 frameTime;
		//the counter holding the animation time left for the frame
		f32 animationTimeCounter;
		//speed of the current animation during playback
		f32 animationSpeedFactor{ 1.0f };
		//is the current animation a loop
		bool isLoop{ false };
		//is the current animation paused?
		bool isPaused{ false };
		
	public:
		//variables to hold the transform values of this instance
		f32 positionXAxis{ 0 },			//x axis position
			positionYAxis{ 0 },			//y axis position
			rotationInRadians{ 0 },		//rotation in radians
			scaleXAxis{ 500 },			//x axis scale
			scaleYAxis{ 500 };			//y axis scale
	public:
		//forced single param construction, animation instance is dependent on animation
		AnimationInstance(Animation& anim);
		//defaulted destructor
		~AnimationInstance() = default;
		void updateAnimation();				//called to update animation every frame
		void togglePause();					//pauses/unpauses animation from its previous state
		void setLooping(bool b);			//set if this instancce of the animation loops
		AEGfxVertexList* getFrameMesh();	//get the mesh of the current frame of the animation
		void reInstance(
			bool loop = false,
			f32 timePerFrame = 0.3f,
			f32 animationSpeed = 1,
			bool paused = 0,
			unsigned int startingFrame = 0);//(re-)initialize the animation instance details
		void setSpeed(f32 timePerFrame, f32 playbackSpeed = 1);	//set the playback speed and time per frame for the animation instance 
		void setDelay(f32 delayInSeconds);	//set a delay before the instance starts/continues playing 
		void updatePosition(f32 posX, f32 posY) { positionXAxis = posX; positionYAxis = posY; }	//update the position of the instance
		void updateRotation(f32 rotInRad) { rotationInRadians = rotInRad; }		//update the rotation of the instance
		void updateScale(f32 scaleX, f32 scaleY) { scaleXAxis = scaleX; scaleYAxis = scaleY; }	//update the scale of an instance
		bool isAnimationEnded();			//returns if an instance has reached the end of an animation, always false if looping
	};
	void render(AnimationInstance& object, AEGfxTexture* tex);
}
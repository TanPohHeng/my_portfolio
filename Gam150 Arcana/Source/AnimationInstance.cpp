/*!************************************************************************
\file AnimationInstance.cpp
\par Project Name: Arcana
\author Tan Poh Heng
\par DP email: t.pohheng@digipen.edu
\par Course: CSD1451
\date 03-31-2023 (mmddyyyy)
\brief
This source file holds the definition of class designed to instance Animations.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
**************************************************************************/
#include "AnimationInstance.hpp"

/*!***********************************************************************
\namespace Render
\brief
	Namespace storing the functions and classes to streamline the rendering
	process for Alpha Engine
*************************************************************************/
namespace Render {
	/*!***********************************************************************
	\brief
		single parameter constructor for animation instance
	\param [in]
		a reference to the animation this object should be an instance of
	*************************************************************************/
	AnimationInstance::AnimationInstance(Render::Animation& anim) : 
		animation{ anim },
		currentFrame {0},
		lastFrame{ anim.getLastFrameIndex() },
		frameTime{0.3f},
		animationTimeCounter{0.f},
		animationSpeedFactor{ 1.0f },
		isLoop{ false },
		isPaused{ false },
		positionXAxis{ 0 },
		positionYAxis{ 0 },
		rotationInRadians{ 0 },
		scaleXAxis{ 100 },
		scaleYAxis{ 100 }
	{
		//empty by design	
	}

	/*!***********************************************************************
	\brief
		Updates the animation by one frame.
	*************************************************************************/
	void AnimationInstance::updateAnimation()
	{
		if (isPaused) { return; }	//no update if the instance was paused
		//reduce the timer before next frame by the delta time scaled to the animation speed factor
		animationTimeCounter -= (f32)AEFrameRateControllerGetFrameTime() * animationSpeedFactor;
		if (animationTimeCounter <= 0)
		{	//if the timer hits 0, next frame is played
			currentFrame = (currentFrame == (lastFrame)) ?	//increment current frame depending on if it is the last frame
				(isLoop) ? 0 : currentFrame			//check if looping during the last frame
				: ++currentFrame;					//increment normally when not at last frame
			animationTimeCounter = frameTime;		//reset the animation time counter to the frame time
		}
	}
	/*!***********************************************************************
	\brief
		Toggles if the animation is paused
	*************************************************************************/
	void AnimationInstance::togglePause()
	{	//pauses/unpauses animation based on its previous state
		isPaused = !isPaused;
	}	

	/*!***********************************************************************
	\brief
		set the interval of one frame for the instance. Also allows a change
		in playback speed for the instance
	\par [in] timePerFrame
		the interval between 2 frames of an animation
	\par [in] playbackSpeed
		the percent speed at which the instance should play the animation. 
		(2 for this value runs the animation at twice the speed).
	*************************************************************************/
	void AnimationInstance::setSpeed(f32 timePerFrame, f32 playbackSpeed)
	{
		frameTime = timePerFrame;
		animationSpeedFactor = playbackSpeed;
	}

	/*!***********************************************************************
	\brief
		Set if the instance should loop the aniamtion.
	\par [in] b
		if the instance should loop the animation
	*************************************************************************/
	void AnimationInstance::setLooping(bool b) { isLoop = b; }

	/*!***********************************************************************
	\brief
		Delays the next frame of the animation by some time
	\par [in] delayInSeconds
		the amount of delay to for the timer to run before the next frame
	*************************************************************************/
	void AnimationInstance::setDelay(f32 delayInSeconds)
	{
		animationTimeCounter = delayInSeconds;
	}

	/*!***********************************************************************
	\brief
		reistantiate the instance value
	\par [in] loop
		if the instance should loop past the last frame
	\par [in] timePerFrame
		the interval between 2 frames
	\par [in] animationSpeed
		the playback speed of this instance of the animation
	\par [in] paused
		if the instance should update over time
	\par [in] starting frame
		the starting frame of the instance.
	*************************************************************************/
	void AnimationInstance::reInstance(
		bool loop, f32 timePerFrame,
		f32 animationSpeed, bool paused,
		unsigned int startingFrame) 
	{	//set the appropriate member variables to their new numbers
		isLoop = loop;
		frameTime = timePerFrame;
		animationSpeedFactor = animationSpeed;
		isPaused = paused;
		currentFrame = startingFrame;
		lastFrame = animation.getLastFrameIndex();	//ensure any changes to the referenced animation are updated in instance
	}

	/*!***********************************************************************
	\brief
		Get the mesh for the frame of the current frame of the instance
	\return
		the mesh with the UVs for the current frame of this animation
	*************************************************************************/
	AEGfxVertexList* AnimationInstance::getFrameMesh()
	{	//return the animation's mesh for this frame
		return animation.getFrameMesh(currentFrame);
	}

	/*!***********************************************************************
	\brief
		checks if an instance's animation has ended. will always be false if 
		the animation is looping
	\return
		if this instance's animation has ended. will always be false if 
		the animation is looping
	*************************************************************************/
	bool AnimationInstance::isAnimationEnded()
	{
		if (isLoop) { return false; }		//return false if looping
		return (currentFrame == lastFrame);	//return if instance has reached the last frame
	}

	/*!***********************************************************************
	\brief
		render overload for an instance of an animation
	*************************************************************************/
	void render(AnimationInstance& object, AEGfxTexture* tex)
	{
		//set the render mode to texture based.
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxSetTintColor(1.0f, 1.0f, 1.0f, 1.0f);	//full colors in all channels
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);			//allow transparency
		AEGfxSetTransparency(1.0f);
		// Set the texture to tex and set the texture draw mode
		AEGfxTextureSet(tex, 0, 0);
		AEGfxSetTextureMode(AEGfxTextureMode::AE_GFX_TM_AVERAGE);


		// Create a scale matrix based on the info in the instance
		AEMtx33 scale = { 0 };
		AEMtx33Scale(&scale, object.scaleXAxis, object.scaleYAxis);

		// Create a rotation matrix based on the info in the instance
		AEMtx33 rotate = { 0 };
		AEMtx33Rot(&rotate, object.rotationInRadians);

		// Create a translation matrix based on the info in the instance
		AEMtx33 translate = { 0 };
		AEMtx33Trans(&translate, object.positionXAxis, object.positionYAxis);

		// Concat the matrices (TRS)
		AEMtx33 transform = { 0 };
		AEMtx33Concat(&transform, &rotate, &scale);
		AEMtx33Concat(&transform, &translate, &transform);


		// set the transformation based on the info in the instance
		AEGfxSetTransform(transform.m);
		// Draw the mesh for this frame of the instance 
		AEGfxMeshDraw(object.getFrameMesh(), AE_GFX_MDM_TRIANGLES);
		//=========================================
	}


}
/*!
* @file			WP_TimerSystem.h
* @author		Tan Poh Heng
* @author		Emma Natalie Soh
* @par			Poh Heng: t.pohheng\@digipen.edu
* @par			Emma: Email: e.soh\@digipen.edu
* @par			Course: CSD3401 Software Engineering Project 5
* @date			21 October 2024
*
* @brief		This system defines a TimerSystem that manages our delta time.
*
* All Content copyrighted 2024 DigiPen Institute of Technology Singapore.
* All Rights Reserved
*/

/*                                                                     guard
____________________________________________________________________________ */

#ifndef WP_TIMER_SYSTEM_H_
#define WP_TIMER_SYSTEM_H_

/*                                                                  includes
____________________________________________________________________________ */

#include <WP_EngineSystem/WP_EngineSystem.h>
#include <WP_EngineSystem/WP_CSharp/WP_ImportExport.h>
#include <chrono>
#include <forward_list>
#include <functional>

/*                                                                   defines
____________________________________________________________________________ */

/*                                                       foward declarations
____________________________________________________________________________ */

/*                                                                 functions
____________________________________________________________________________ */

/* _________________________________________________________________________ */

/*!
 * @brief	This class manages the delta time for our engine.
 */
class DLL_API WP_TimerSystem : WP_EngineSystem
{
private:

	///////////////////////////////////////////////////////////////////////////
	// proxy types
	///////////////////////////////////////////////////////////////////////////

	using fp_unit = float;

public:

	using clock = std::chrono::steady_clock;

	using callback = std::function<void(void)>;

private:

	WP_TimerSystem();
	~WP_TimerSystem();

	CREATE_ENGINE_INSTANCE_H(WP_TimerSystem)

public:

	///////////////////////////////////////////////////////////////////////////
	// engine overrides
	///////////////////////////////////////////////////////////////////////////

	/*!
	 * @brief	Starts scene. Resets the timescale to 1.f.
	 */
	void OnStartScene() override;

	/*!
	 * @brief	Ends the scene. Resets the timescale to 1.f.
	 */
	void OnEndScene() override;

	/*!
	 * @brief	FPS calculation occurs here.
	 */
	void OnUpdate();

	///////////////////////////////////////////////////////////////////////////
	// functions
	///////////////////////////////////////////////////////////////////////////

public:

	/*!
	 * @brief	Get seconds since last frame.
	 *
	 * @return	The seconds since last frame.
	 */
	fp_unit GetDT() const;

	/*!
	 * @brief	Get seconds since last frame.
	 *
	 * @return	The seconds since last frame.
	 */
	fp_unit GetFixedDT() const;								//

	/*!
	 * @brief	Determine if we are currently zeroing delta. This is meant for
	 *			pausing the gameplay.
	 *
	 * @return	Whether we are zeroing the delta.
	 */
	bool IsZeroingDT() const noexcept;

	/*!
	 * @brief	Set whether we should zero delta. This affets GetDT and
	 *			GetFixedDT calls.
	 *
	 * @param	_is_zeroing_dt
	 *			Specifies whether we shuld zero delta.
	 */
	void SetIsZeroingDT(bool const _is_zeroing_dt) noexcept;

	/*!
	 * @brief	Sets the new time scale of the application.
	 *
	 * @param[in]	_set_time_scale
	 *				The new time scale.
	 *				Negative values will interpreted as their positive
	 *				counterparts.
	 *
	 * @return	The original time scale.
	 */
	float SetTimeScale(float const _set_time_scale) noexcept;

	/*!
	 * @brief Gets the time scale.
	 *
	 * @return	The current time scale.
	 */
	float GetTimeScale() const noexcept;

private:

	void Tick();

public:

	void SetFixedDT(bool _active);

	void SetFixedDTInterval(float _interval);

	void SetFixedDTInterval(std::chrono::seconds _interval);

	void SetFixedDTIntervalByFPS(float _targetFPS);

	void SetFixedDTCallBack(callback const& _callbackFunctionObject);

	bool GetIsUsingFixedDT() const;							//Check if system is using fixed DT

	fp_unit GetFPS() const;								//Get frames per second

	std::chrono::milliseconds GetDTinSeconds() const;			//Get seconds since last frame in chrono duration units.

	size_t	StartTimer();							//return tracking ticketID number. Get timer ticket here, new timer is started for that ID
	//fp_unit LapTimer(size_t _ticketID);			//use ticketID to find time since last lap , add additonal timepoint to m_timer to make this work
	fp_unit SplitTimer(size_t _ticketID);			//use ticketID to find time since start
	fp_unit EndTimer(size_t _ticketID);				//use ticketID to find time since start and end this timer

	bool GetIsFixedDTFrame() const;

private:

	callback m_FixedDTCallback{};							//callback function when fixed DT is hit.

	std::vector<clock::time_point> m_timers{};				//store time points to when timers are started
	std::forward_list<size_t> m_freeTimers;					//store timers that have been ended

	std::chrono::milliseconds
		m_targetDT{},						//DT to hit for Fixed DT
		m_millisecondsPerFrame{};				// 1/fps, stored in std::duration units

	clock::time_point m_frameStart{}, m_lastFrameStart{};	//time capture vars
	clock::time_point m_lastFixedDTFrame{};					//last fixed DT frame

	fp_unit m_FPS{};										//fps var

	/*!
	 * @brief	We will scale DT by this value. Initial value is 1.f.
	 */
	float m_timeScale;

	bool m_useFixedDT{false};
	bool m_fixedDTFrame{ false };
	bool m_isZeroingDT{ false };

};


#endif
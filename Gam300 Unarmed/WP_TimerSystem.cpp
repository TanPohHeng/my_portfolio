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


/*                                                                  includes
____________________________________________________________________________ */

#include <WP_EngineSystem/WP_AssetManager.h>
#include <WP_EngineSystem/WP_TimerSystem.h>

#include <WP_EngineSystem/WP_Event/WP_EventSystem.h>
#include <WP_EngineSystem/WP_Event/payload/WP_TimerSystemOnTimeScaleChange.h>

/*                                                                   defines
____________________________________________________________________________ */

/*                                                       anonymous namespace
____________________________________________________________________________ */

/*                                                                 functions
____________________________________________________________________________ */


/* _________________________________________________________________________ */

CREATE_ENGINE_INSTANCE_CPP(WP_TimerSystem);

/* _________________________________________________________________________ */

//=============================================
// proxy types for quick switch if needed.
//	using	WP_TimerSystem::fp_unit = float;
//=============================================

WP_TimerSystem::WP_TimerSystem() :
	WP_EngineSystem(WP_EngineSystem::s_kSystemFlagsAll, "WP_TimerSystem")
{
	using namespace std::chrono_literals;
	m_targetDT = std::chrono::duration_cast<std::chrono::milliseconds>(16.6666667ms);
	m_frameStart = clock::now();
	m_timeScale = 1.f;
}

/* _________________________________________________________________________ */

WP_TimerSystem::~WP_TimerSystem()
{
}

/* _________________________________________________________________________ */

WP_TimerSystem::fp_unit WP_TimerSystem::GetDT() const
{
	if (m_isZeroingDT)
	{
		return static_cast<fp_unit>(0);
	}

	if (m_useFixedDT)
	{
		return (static_cast<fp_unit>(m_targetDT.count()) / 1000) * m_timeScale;
	}
	return (static_cast<fp_unit>(m_millisecondsPerFrame.count()) / 1000) * m_timeScale;
}

/* _________________________________________________________________________ */

WP_TimerSystem::fp_unit WP_TimerSystem::GetFixedDT() const
{
	if (m_isZeroingDT)
	{
		return static_cast<fp_unit>(0);
	}

	return (static_cast<fp_unit>(m_targetDT.count()) / 1000)  * m_timeScale;
}

/* _________________________________________________________________________ */

bool WP_TimerSystem::IsZeroingDT() const noexcept
{
	return m_isZeroingDT;
}

/* _________________________________________________________________________ */


void WP_TimerSystem::SetIsZeroingDT(bool const _is_zeroing_dt) noexcept
{
	m_isZeroingDT = _is_zeroing_dt;
}

/* _________________________________________________________________________ */

float WP_TimerSystem::SetTimeScale(float const _set_time_scale) noexcept
{
	// save previous time
	float const original = m_timeScale;

	// set the new timescale and then send event to notify
	m_timeScale = std::abs(_set_time_scale);

	WP_TimerSystemOnTimeScaleChange payload
	{
		m_timeScale
	};
	WP_EventSystem::GetInstance()->Notify(
		EventType::kTimeScaleChanged, &payload
	);

	// return the original value
	return original;
}

/* _________________________________________________________________________ */

float WP_TimerSystem::GetTimeScale() const noexcept
{
	return m_timeScale;
}

/* _________________________________________________________________________ */

void WP_TimerSystem::OnUpdate()
{
	m_lastFrameStart = m_frameStart;
	m_frameStart = clock::now();
	m_millisecondsPerFrame = std::chrono::duration_cast<std::chrono::milliseconds>(
		m_frameStart.time_since_epoch() - m_lastFrameStart.time_since_epoch()
	);

	m_FPS = static_cast<WP_TimerSystem::fp_unit>(pow(m_millisecondsPerFrame.count(), -1)) * 1000;

	if (m_FixedDTCallback)
	{
		auto timeSinceLastDTFrame{ (clock::now().time_since_epoch() - m_lastFixedDTFrame.time_since_epoch()) };
		m_fixedDTFrame = false;
		if (timeSinceLastDTFrame > m_targetDT)
		{	//if time since last frame larger than DT interval,
			//LastDTFrame = current time - difference in time since last DT and target DT, should give actual last DT time.
			m_lastFixedDTFrame = clock::now() - (timeSinceLastDTFrame - m_targetDT);
			m_FixedDTCallback();
			m_fixedDTFrame = true;
		}
	}
}

/* _________________________________________________________________________ */

size_t	WP_TimerSystem::StartTimer()
{
	size_t retVal;
	if (m_freeTimers.empty()) {
		m_timers.emplace_back(clock::now());
		retVal = m_timers.size() - 1;
	}
	else
	{
		retVal = m_freeTimers.front();
		m_freeTimers.pop_front();
	}

	return retVal;
}

/* _________________________________________________________________________ */

WP_TimerSystem::fp_unit WP_TimerSystem::SplitTimer(size_t _ticketID)
{
	return static_cast<WP_TimerSystem::fp_unit>(
		std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_timers[_ticketID]).count()) * 1000;	//change to *1000 or seconds will truncate nanoseconds to 0
}

/* _________________________________________________________________________ */

WP_TimerSystem::fp_unit WP_TimerSystem::EndTimer(size_t _ticketID)
{
	m_freeTimers.push_front(_ticketID);
	return SplitTimer(_ticketID);
}

/* _________________________________________________________________________ */

//Getters and Setters

WP_TimerSystem::fp_unit WP_TimerSystem::GetFPS() const
{
	//if (m_useFixedDT)
	//	return 60.0f;
	return m_FPS;
}

/* _________________________________________________________________________ */

std::chrono::milliseconds WP_TimerSystem::GetDTinSeconds() const
{
	return m_millisecondsPerFrame;
}

/* _________________________________________________________________________ */

void  WP_TimerSystem::SetFixedDTIntervalByFPS(float _interval)
{
	assert(_interval > EPSILON);
	m_targetDT = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(1 / _interval));
}

/* _________________________________________________________________________ */

void  WP_TimerSystem::SetFixedDTInterval(std::chrono::seconds _interval)
{
	m_targetDT = _interval;
}

/* _________________________________________________________________________ */

void  WP_TimerSystem::SetFixedDTInterval(float _interval)
{
	m_targetDT = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(_interval));
}

/* _________________________________________________________________________ */

void WP_TimerSystem::OnStartScene()
{
	Tick();
	SetTimeScale(1.f);
}

/* _________________________________________________________________________ */

void WP_TimerSystem::OnEndScene()
{
	SetTimeScale(1.f);
}

/* _________________________________________________________________________ */

void WP_TimerSystem::Tick()
{
	OnUpdate();
}

/* _________________________________________________________________________ */

void WP_TimerSystem::SetFixedDT(bool _active)
{
	m_useFixedDT = _active;
}

/* _________________________________________________________________________ */

void WP_TimerSystem::SetFixedDTCallBack(callback const& _callbackFunctionObject)
{
	m_FixedDTCallback = _callbackFunctionObject;
}

/* _________________________________________________________________________ */

bool WP_TimerSystem::GetIsUsingFixedDT() const
{
	return m_useFixedDT;
}

/* _________________________________________________________________________ */

bool WP_TimerSystem::GetIsFixedDTFrame() const
{
	return m_fixedDTFrame;
}

/* _________________________________________________________________________ */
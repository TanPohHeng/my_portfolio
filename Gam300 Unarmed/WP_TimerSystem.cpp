#include <WP_EngineSystem/WP_AssetManager.h>
#include <WP_EngineSystem/WP_TimerSystem.h>
CREATE_ENGINE_INSTANCE_CPP(WP_TimerSystem);

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
}

WP_TimerSystem::~WP_TimerSystem()
{}

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

WP_TimerSystem::fp_unit WP_TimerSystem::SplitTimer(size_t _ticketID)
{
	return static_cast<WP_TimerSystem::fp_unit>(
		std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_timers[_ticketID]).count()) * 1000;	//change to *1000 or seconds will truncate nanoseconds to 0
}

WP_TimerSystem::fp_unit WP_TimerSystem::EndTimer(size_t _ticketID)
{
	m_freeTimers.push_front(_ticketID);
	return SplitTimer(_ticketID);
}

void WP_TimerSystem::OnUpdate()
{
	m_lastFrameStart = m_frameStart;
	m_frameStart = clock::now();
	m_millisecondsPerFrame = std::chrono::duration_cast<std::chrono::milliseconds>(
		m_frameStart.time_since_epoch() - m_lastFrameStart.time_since_epoch());

	m_FPS = static_cast<WP_TimerSystem::fp_unit>(pow( m_millisecondsPerFrame.count(), -1)) * 1000;
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

//Getters and Setters

WP_TimerSystem::fp_unit WP_TimerSystem::GetFPS() const
{
	//if (m_useFixedDT)
	//	return 60.0f;
	return m_FPS;
}

WP_TimerSystem::fp_unit WP_TimerSystem::GetDT() const
{
	if (m_useFixedDT)
	{return static_cast<fp_unit>(m_targetDT.count())/1000;}
	return static_cast<fp_unit>(m_millisecondsPerFrame.count())/1000;
}

WP_TimerSystem::fp_unit WP_TimerSystem::GetFixedDT() const
{
	return static_cast<fp_unit>(m_targetDT.count()) / 1000;
}

std::chrono::milliseconds WP_TimerSystem::GetDTinSeconds() const
{
	return m_millisecondsPerFrame;
}

void  WP_TimerSystem::SetFixedDTIntervalByFPS(float _interval)
{
	assert(_interval > EPSILON);
	m_targetDT = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(1 / _interval));
}

void  WP_TimerSystem::SetFixedDTInterval(std::chrono::seconds _interval)
{
	m_targetDT = _interval;
}

void  WP_TimerSystem::SetFixedDTInterval(float _interval)
{
	m_targetDT = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<float>(_interval));
}

void WP_TimerSystem::OnStartScene()
{
	Tick();
}

void WP_TimerSystem::Tick()
{
	OnUpdate();
}

void WP_TimerSystem::SetFixedDT(bool _active)
{
	m_useFixedDT = _active;
}

void WP_TimerSystem::SetFixedDTCallBack(callback const& _callbackFunctionObject)
{
	m_FixedDTCallback = _callbackFunctionObject;
}

bool WP_TimerSystem::GetIsUsingFixedDT() const
{
	return m_useFixedDT;
}

bool WP_TimerSystem::GetIsFixedDTFrame() const
{
	return m_fixedDTFrame;
}


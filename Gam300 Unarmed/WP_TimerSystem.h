#pragma once
#include <WP_EngineSystem/WP_EngineSystem.h>
#include <WP_EngineSystem/WP_CSharp/WP_ImportExport.h>
#include <chrono>
#include <forward_list>
#include <functional>

class DLL_API WP_TimerSystem : WP_EngineSystem
{
	using fp_unit = float;							//proxy types for quick switch if needed.
public:
	using clock = std::chrono::steady_clock;
	using callback = std::function<void(void)>;
	void OnStartScene() override;

	void Tick();

	void SetFixedDT(bool _active);
	void SetFixedDTInterval(float _interval);
	void SetFixedDTInterval(std::chrono::seconds _interval);
	void SetFixedDTIntervalByFPS(float _targetFPS);
	void SetFixedDTCallBack(callback const& _callbackFunctionObject);

	bool GetIsUsingFixedDT() const;							//Check if system is using fixed DT
	fp_unit GetFPS() const;								//Get frames per second
	fp_unit GetDT() const;								//Get seconds since last frame.
	fp_unit GetFixedDT() const;								//Get seconds since last frame.
	std::chrono::milliseconds GetDTinSeconds() const;			//Get seconds since last frame in chrono duration units.

	size_t	StartTimer();							//return tracking ticketID number. Get timer ticket here, new timer is started for that ID
	//fp_unit LapTimer(size_t _ticketID);			//use ticketID to find time since last lap , add additonal timepoint to m_timer to make this work
	fp_unit SplitTimer(size_t _ticketID);			//use ticketID to find time since start
	fp_unit EndTimer(size_t _ticketID);				//use ticketID to find time since start and end this timer

	bool GetIsFixedDTFrame() const;
private:
	WP_TimerSystem();
	~WP_TimerSystem();
	CREATE_ENGINE_INSTANCE_H(WP_TimerSystem);

	void OnUpdate();	//fps calculation here
private:
	bool m_useFixedDT{false};
	bool m_fixedDTFrame{ false };

	fp_unit m_FPS{};										//fps var

	std::chrono::milliseconds m_targetDT{},						//DT to hit for Fixed DT
		m_millisecondsPerFrame{};				// 1/fps, stored in std::duration units
	clock::time_point m_frameStart{}, m_lastFrameStart{};	//time capture vars
	clock::time_point m_lastFixedDTFrame{};					//last fixed DT frame

	std::forward_list<size_t> m_freeTimers;					//store timers that have been ended
	std::vector<clock::time_point> m_timers{};				//store time points to when timers are started
	callback m_FixedDTCallback{};							//callback function when fixed DT is hit.
};
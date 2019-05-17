// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEFrameRateController.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/04/27
// Purpose			:	implementation of the frame rate controller
// History			:
// - 2007/04/27		:	- initial implementation
// - 2010/08/17 : Fixed a bug that resulted in the gAEFrameRate variable not
//                being updated. - Dan Weiss
// ---------------------------------------------------------------------------
#include "AEXTime.h"
#include <Windows.h> //QueryPerformance... functions.

// ---------------------------------------------------------------------------
// Defines

#define FRAME_RATE_SYNC_TO_RETRACE 0
namespace AEX
{
	// ---------------------------------------------------------------------------
	// get the current time in seconds

	f64 FRC::GetCPUTime()
	{
		s64 f, t;
		f64 r, r0, r1;

		QueryPerformanceFrequency((LARGE_INTEGER*)(&f));
		QueryPerformanceCounter((LARGE_INTEGER*)(&t));

		//@FIXED - precision warning
		r0 = f64(t / f);
		r1 = (t - ((t / f) * f)) / (f64)(f);
		r = r0 + r1;

		return r;//r0 + r1;
	}

	// ---------------------------------------------------------------------------
	// Functions implementations

	FRC::FRC()
	{}

	bool FRC::Initialize()
	{
		FRC::sFrameCounter = 0;
		FRC::sFrameRateMax = 60;
		FRC::sFrameRate = FRC::sFrameRateMax;
		FRC::sFrameTime = 1.0 / FRC::sFrameRate;
		FRC::sFrameTimeMin = 1.0 / FRC::sFrameRateMax;
		return true;
	}

	// ---------------------------------------------------------------------------

	void FRC::Reset()
	{
		//AE_ASSERT_MESG(gAEFrameRateMax > 0.0, "maximum frame rate MUST be greater than 0");

		FRC::sFrameCounter = 0;
		FRC::sFrameRate = sFrameRateMax;
		FRC::sFrameTime = 1.0 / FRC::sFrameRate;
		FRC::sFrameTimeMin = 1.0 / FRC::sFrameRateMax;
	}
	void FRC::Update()
	{ 
		if (sFrameCounter)
			EndFrame();
		else
			sFrameCounter++;
		StartFrame(); 
	}

	// ---------------------------------------------------------------------------

	void FRC::StartFrame()
	{
		FRC::sFrameTimeStart = FRC::GetCPUTime();
	}

	// ---------------------------------------------------------------------------

	void FRC::EndFrame()
	{

		// if the total time spent is less than the minimum required time to 
		// maintain the maximum frame rate, wait
		do
		{
			FRC::sFrameTimeEnd = GetCPUTime();
			// calculate the amount of time spend this frame
			sFrameTime = sFrameTimeEnd - sFrameTimeStart;
		}
		while (bFrameRateLocked && (sFrameTime) < FRC::sFrameTimeMin);


		//@FIXED - Reset the frame rate variable
		sFrameRate = 1.0 / sFrameTime;

		// increment the total number of counter
		sFrameCounter++;
	}

	f64 FRC::GetMaxFrameRate()
	{
		return sFrameRateMax;
	}
	f64 FRC::GetFrameRate()
	{
		return sFrameRate;
	}
	f64 FRC::GetFrameTime()
	{
		return sFrameTime;
	}
	f64 FRC::GetFrameCounter()
	{
		return sFrameCounter;
	}

	// Setters
	void FRC::SetMaxFrameRate(f64 fps)
	{
		sFrameRateMax = fps;
		FRC::sFrameTimeMin = 1.0 / FRC::sFrameRateMax;
	}
	// ---------------------------------------------------------------------------
	// Static functions implementations

	// ---------------------------------------------------------------------------

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// Timer
	AEXTimer::AEXTimer()
		: startTime_(0.0)
		, timeSinceLastTick_(0.0)
		, timeScale_(1.0f)
		, isPaused_(false)
	{
		Reset();
	}
	f32 AEXTimer::Tick()
	{
		// if the timer is paused, return 0.0 delta time
		if (isPaused_)
		{
			// Update the time stamp to avoid having 
			// a big delta time when the game isn't paused
			timeSinceLastTick_ = FRC::GetCPUTime();
			return 0.0f;
		}

		f64 dt = FRC::GetCPUTime() - timeSinceLastTick_;
		timeSinceLastTick_ = FRC::GetCPUTime();
		return static_cast<f32>(dt)* timeScale_;
	}
	void AEXTimer::Reset()
	{
		// reset the time stamps to the current time
		startTime_ =
			timeSinceLastTick_ = FRC::GetCPUTime();
	}
	void AEXTimer::Start()
	{
		isPaused_ = false;
	}
	void AEXTimer::Pause()
	{
		isPaused_ = true;
	}
	f32 AEXTimer::GetTimeSinceStart()
	{
		f64 total_time = FRC::GetCPUTime() - startTime_;
		return static_cast<f32>(total_time)* timeScale_;
	}
}
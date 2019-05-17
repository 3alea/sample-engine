// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEFrameRateController.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/04/26
// Purpose			:	header file for the frame rate controller
// History			:
// - 2007/04/26		:	- initial implementation
// ---------------------------------------------------------------------------
#ifndef AEX_TIME_H
#define AEX_TIME_H

#include "..\Core\AEXCore.h"
namespace AEX
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// TIME CLASS - Static interface for framerate controller
	class  FRC : public ISystem
	{
		AEX_RTTI_DECL(FRC, ISystem);
		AEX_SINGLETON(FRC);

	public:
		virtual bool Initialize();
		virtual void Update();

		// call between each frame for frame statistics
		void StartFrame();
		void EndFrame();
		void Reset();

		f64 GetMaxFrameRate();
		f64 GetFrameRate();
		f64 GetFrameTime();
		f64 GetFrameCounter();

		// set max frame rate
		void LockFrameRate(bool enabled){ bFrameRateLocked = enabled; }
		inline bool FrameRateLocked(){ return bFrameRateLocked; }
		void SetMaxFrameRate(f64 fps);

		// uses the CPU clock to return a time in seconds.
		static f64 GetCPUTime();

	private:

		bool bFrameRateLocked = true;
		f64	sFrameRateMax;	// clamp the frame rate to at most this number
		f64	sFrameRate;		// the frame rate based on the last frame
		f64	sFrameTime;		// time taken to process the last frame(in seconds)
		u32	sFrameCounter;	// number of frame since the last reset
		f64	sFrameTimeMin;
		f64	sFrameTimeStart;
		f64	sFrameTimeEnd;
	};


	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// TIMER - Simple timer class that makes use of the FRC singleton system.
	struct AEXTimer
	{
		f64 startTime_;
		f64 timeSinceLastTick_;
		f32 timeScale_;
		bool isPaused_;

		// ------------------------
		// METHODS
		AEXTimer();
		f32 Tick();		// Returns the time since last tick. Call each frame.
		void Reset();	// Resets the timer values
		void Start();	// Sets paused to false
		void Pause();	// Sets paused to true
		f32 GetTimeSinceStart(); // returns the time since the last reset
	};
}

// Easy access to singleton
#define aexTime (FRC::Instance())
// ---------------------------------------------------------------------------

#endif // AEX_TIME_H


//Timer.cpp --------------------------------------
#include <windows.h>
#include "Game Timer.h"

GameTimer::GameTimer()
{
	__int64 rate;
	ErrorFlag = false;

	// we need the accuracy
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&rate)) 
	{
	    ErrorFlag = true; //system does not support high-res timer
	}
	
	// usually the rate will be 1193180 (hz)
	if(!rate) 
	{
	    ErrorFlag = true;
	}
	
	RateInv=1.0/(double)rate;
	
	if(!QueryPerformanceCounter((LARGE_INTEGER*)&StartClock)) 
	{
	    ErrorFlag = true; //system does not support high-res timer
	}
    
	FrameStart=(float)GetTime(); // get the first time
    First=FrameStart;
}



double GameTimer::GetTime()
{
	if(ErrorFlag) return FALSE;	//exit out if timer was not initialized
									//correctely
	// by dividing by its rate you get accurate seconds

	__int64 endclock;

	QueryPerformanceCounter((LARGE_INTEGER*)&endclock);

	return (double)(endclock-StartClock)*RateInv;
}



double GameTimer::CalcFrameTime()
{
	if(ErrorFlag) return FALSE;	//exit out if timer was not initialized
							        //correctely

	do {
		// get the time till its not equal to start frame time, it should,
		// never loop at the rate of this timer, but if it does then
		// it will cap to the frame rate to the rate of this timer which
		// is 1193180 hz
		FrameEnd=(double)GetTime();  // get end of frame time
    } while(FrameEnd==FrameStart);


    FrameTime=FrameEnd-FrameStart; // find differnce to get the frametime
    FrameStart=FrameEnd; // make the first time be the next time

    // the inverse of frame time is the frame rate
    Fps=(float)(1.0/FrameTime); // pretty cool huh
	return FrameTime;
}


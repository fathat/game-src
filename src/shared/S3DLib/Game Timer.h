//Timer.h - Use timer to adjust FPS
#pragma once


//////// GameTimer ////////////////////////////
//
// Description:
//
// Use this to figure out how much time has 
// elapsed since the last frame.
//
// Known Problems:
//
// - None
//
//
///////////////////////////////////////////////


class GameTimer
{
protected:
	__int64 StartClock;
	double   FrameTime, FrameStart, FrameEnd;
	float	First, Fps;
	double  RateInv;
public:
	bool ErrorFlag;


public: 
	//Gets time
	double _fastcall GetTime();
	float GetFrameStart() { return (float)FrameStart; }

	//Constructor
	GameTimer();
	
	//Use these
	double CalcFrameTime(); //call this to get frame time
	
	//Information Accessors
	double GetFrameTime() {return FrameTime;}
	float GetFPS() {return Fps;}
	
};

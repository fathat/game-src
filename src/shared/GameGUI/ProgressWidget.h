// I N C L U D E S ///////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "..\S3DLib\Screen.h"


// C L A S S E S /////////////////////////////////////////////

//ProgressWidget
class ProgressWidget
{
	Screen3D* m_screenPtr;
	std::vector< std::string > m_history;
	float m_percent;

	int m_timeOfLastUpdate;
	int m_timeTaskStarted;

	float m_ticks;

	void UpdateScreen();

public:
	ProgressWidget( Screen3D* screen );

	void Reset();

	void  Progress( float val ) ;
	const inline float Progress( ) { return m_percent ; }

	void Tick();

	void NewTask( const char* taskDesc );
};
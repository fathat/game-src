#include <windows.h>
#include <mmsystem.h>
#include "ProgressWidget.h"


void ProgressWidget::UpdateScreen()
{
	if(!m_screenPtr)
		return;
	//draw some stuff

	Font2D* font = m_screenPtr->FM.GetFont( "Tahoma", 8 );

	m_screenPtr->SetClearColor( 0, 0, 0, 0  );
	m_screenPtr->Clear( );
	m_screenPtr->SetRenderState( D3DRS_FOGENABLE, FALSE );
	TextureHandle logo = m_screenPtr->GetTextureHandle( "..\\base\\art\\ui\\loading.jpg" );
	TextureHandle mug  = m_screenPtr->GetTextureHandle( "..\\base\\art\\ui\\beermug.bmp" );
	m_screenPtr->DrawSprite( m_screenPtr->GetWidth()/2-256, m_screenPtr->GetHeight() / 2-256, logo );


	int row = 0;
	int col = 0;
	int maxMugsAcross = m_screenPtr->GetWidth() / 32;
	for( int i=0; i<m_ticks/10; i++ )
	{
		col = i%maxMugsAcross;
		row = i/maxMugsAcross;
		//m_screenPtr->DrawSprite( col*32, m_screenPtr->GetHeight() - (row+1)*32, mug );

		
	}

	//font->Draw( *m_screenPtr, 0, 0, false, D3DCOLOR_ARGB( 255, 255, 255, 255 ), "Loading... %f", m_ticks ) ;

	m_screenPtr->UpdateScreen();
}

ProgressWidget::ProgressWidget( Screen3D* screen )
{
	m_screenPtr = screen;
	Reset();
}

void ProgressWidget::Reset()
{
	m_history.clear();
	m_percent = 0;
	m_ticks = 0;
}

void  ProgressWidget::Progress( float val ) 
{ 
	m_percent = val; 

	if( abs( (int) timeGetTime() - m_timeOfLastUpdate ) > 250)
	{
		UpdateScreen();
		m_timeOfLastUpdate = timeGetTime();
	}	
}

void ProgressWidget::Tick()
{
	m_ticks++;
	UpdateScreen();
}

void ProgressWidget::NewTask( const char* taskDesc )
{
	std::string t = taskDesc;
	m_history.push_back( t );
	m_timeTaskStarted = timeGetTime();
	Progress( 0 );
	
}
#include "global.h"
/*
-----------------------------------------------------------------------------
 Class: LifeMeterBattery

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "LifeMeterBattery.h"
#include "GameState.h"
#include "ThemeManager.h"
#include "Steps.h"
#include "StageStats.h"


const float	BATTERY_X[NUM_PLAYERS]	=	{ -92, +92 };

const float NUM_X[NUM_PLAYERS]		=	{ BATTERY_X[0], BATTERY_X[1] };
const float NUM_Y					=	+2;

const float BATTERY_BLINK_TIME		= 1.2f;


LifeMeterBattery::LifeMeterBattery()
{
	m_iLivesLeft = GAMESTATE->m_SongOptions.m_iBatteryLives;
	m_iTrailingLivesLeft = m_iLivesLeft;

	m_fBatteryBlinkTime = 0;

	
	m_soundGainLife.Load( THEME->GetPathToS("LifeMeterBattery gain") );
	m_soundLoseLife.Load( THEME->GetPathToS("LifeMeterBattery lose"),true );
}

void LifeMeterBattery::Load( PlayerNumber pn )
{
	LifeMeter::Load( pn );

	bool bPlayerEnabled = GAMESTATE->IsPlayerEnabled(pn);

	m_sprFrame.Load( THEME->GetPathToG("LifeMeterBattery frame") );
	this->AddChild( &m_sprFrame );

	m_sprBattery.Load( THEME->GetPathToG("LifeMeterBattery lives 1x4") );
	m_sprBattery.StopAnimating();
	if( bPlayerEnabled )
		this->AddChild( &m_sprBattery );

	m_textNumLives.LoadFromNumbers( THEME->GetPathToN("LifeMeterBattery lives") );
	m_textNumLives.SetDiffuse( RageColor(1,1,1,1) );
	m_textNumLives.SetShadowLength( 0 );
	if( bPlayerEnabled )
		this->AddChild( &m_textNumLives );

	m_sprFrame.SetZoomX( pn==PLAYER_1 ? 1.0f : -1.0f );
	m_sprBattery.SetZoomX( pn==PLAYER_1 ? 1.0f : -1.0f );
	m_sprBattery.SetX( BATTERY_X[pn] );
	m_textNumLives.SetX( NUM_X[pn] );
	m_textNumLives.SetY( NUM_Y );

	if( bPlayerEnabled )
	{
		m_Percent.SetName( "LifeMeterBattery Percent" );
		m_Percent.Load( pn, &g_CurStageStats );
		this->AddChild( &m_Percent );
	}

	Refresh();
}

void LifeMeterBattery::OnSongEnded()
{
	if( g_CurStageStats.bFailedEarlier[m_PlayerNumber] )
		return;

	if( m_iLivesLeft < GAMESTATE->m_SongOptions.m_iBatteryLives )
	{
		m_iTrailingLivesLeft = m_iLivesLeft;
		m_iLivesLeft += ( GAMESTATE->m_pCurNotes[m_PlayerNumber]->GetMeter()>=8 ? 2 : 1 );
		m_iLivesLeft = min( m_iLivesLeft, GAMESTATE->m_SongOptions.m_iBatteryLives );
		m_soundGainLife.Play();
	}

	Refresh();
}


void LifeMeterBattery::ChangeLife( TapNoteScore score )
{
	if( g_CurStageStats.bFailedEarlier[m_PlayerNumber] )
		return;

	switch( score )
	{
	case TNS_MARVELOUS:
	case TNS_PERFECT:
	case TNS_GREAT:
		break;
	case TNS_GOOD:
	case TNS_BOO:
	case TNS_MISS:
		m_iTrailingLivesLeft = m_iLivesLeft;
		m_iLivesLeft--;
		m_soundLoseLife.Play();

		m_textNumLives.SetZoom( 1.5f );
		m_textNumLives.BeginTweening( 0.15f );
		m_textNumLives.SetZoom( 1.0f );

		Refresh();
		m_fBatteryBlinkTime = BATTERY_BLINK_TIME;
		break;
	default:
		ASSERT(0);
	}
	if( m_iLivesLeft == 0 )
		g_CurStageStats.bFailedEarlier[m_PlayerNumber] = true;
}

void LifeMeterBattery::ChangeLife( HoldNoteScore score, TapNoteScore tscore )
{
	switch( score )
	{
	case HNS_OK:
		break;
	case HNS_NG:
		ChangeLife( TNS_MISS );		// NG is the same as a miss
		break;
	default:
		ASSERT(0);
	}
}

void LifeMeterBattery::ChangeLife( float fDeltaLifePercent )
{
}

void LifeMeterBattery::ChangeLifeMine()
{
	ChangeLife( TNS_MISS );		// same as a miss
}

void LifeMeterBattery::OnDancePointsChange()
{
}


bool LifeMeterBattery::IsInDanger() const
{
	return false;
}

bool LifeMeterBattery::IsHot() const
{
	return false;
}

bool LifeMeterBattery::IsFailing() const
{
	return g_CurStageStats.bFailedEarlier[m_PlayerNumber];
}

float LifeMeterBattery::GetLife() const
{
	if( !GAMESTATE->m_SongOptions.m_iBatteryLives )
		return 1;

	return float(m_iLivesLeft) / GAMESTATE->m_SongOptions.m_iBatteryLives;
}

void LifeMeterBattery::Refresh()
{
	if( m_iLivesLeft <= 4 )
	{
		m_textNumLives.SetText( "" );
		m_sprBattery.SetState( max(m_iLivesLeft-1,0) );
	}
	else
	{
		m_textNumLives.SetText( ssprintf("x%d", m_iLivesLeft-1) );
		m_sprBattery.SetState( 3 );
	}
}

void LifeMeterBattery::Update( float fDeltaTime )
{
	LifeMeter::Update( fDeltaTime );

	if( m_fBatteryBlinkTime > 0 )
	{
		m_fBatteryBlinkTime -= fDeltaTime;
		int iFrame1 = m_iLivesLeft-1;
		int iFrame2 = m_iTrailingLivesLeft-1;
		
		int iFrameNo = (int(m_fBatteryBlinkTime*15)%2) ? iFrame1 : iFrame2;
		CLAMP( iFrameNo, 0, 3 );
		m_sprBattery.SetState( iFrameNo );

	}
	else
	{
		m_fBatteryBlinkTime = 0;
		int iFrameNo = m_iLivesLeft-1;
		CLAMP( iFrameNo, 0, 3 );
		m_sprBattery.SetState( iFrameNo );
	}
}

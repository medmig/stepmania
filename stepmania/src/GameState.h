#ifndef GAMESTATE_H
#define GAMESTATE_H
/*
-----------------------------------------------------------------------------
 Class: GameState

 Desc: Holds game data that is not saved between sessions.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "GameConstantsAndTypes.h"
#include "PlayerOptions.h"
#include "SongOptions.h"
#include "Game.h"
#include "Style.h"
#include "Grade.h"
#include "Attack.h"
#include "RageTimer.h"

#include <map>
#include <deque>

class Song;
class Steps;
class Course;
class Trail;
class GameDef;
class StyleDef;
class NoteFieldPositioning;
class Character;
class TimingData;
struct StageStats;

class GameState
{
public:
	GameState();
	~GameState();
	void Reset();
	void ApplyCmdline(); // called by Reset
	void BeginGame();	// called when first player joins
	void JoinPlayer( PlayerNumber pn );
	void PlayersFinalized();	// called after a style is chosen, which means the number of players is finalized
	void EndGame();	// called on ScreenGameOver, ScreenMusicScroll, ScreenCredits
	void SaveCurrentSettingsToProfile( PlayerNumber pn ); // called at the beginning of each stage

	void Update( float fDelta );


	//
	// Main state info
	//
	Game				m_CurGame;
	Style				m_CurStyle;
	bool				m_bSideIsJoined[NUM_PLAYERS];	// left side, right side
	bool				m_bPlayersFinalized;
	PlayMode			m_PlayMode;			// many screens display different info depending on this value
	int					m_iCoins;			// not "credits"
	PlayerNumber		m_MasterPlayerNumber;	// used in Styles where one player controls both sides
	bool				m_bIsOnSystemMenu; // system screens will not be effected by the operator key -- Miryokuteki
	CourseDifficulty	m_PreferredCourseDifficulty[NUM_PLAYERS]; // used in nonstop
	bool DifficultiesLocked();
	bool ChangeDifficulty( PlayerNumber pn, Difficulty dc );
	bool ChangeDifficulty( PlayerNumber pn, int dir );
	bool ChangeCourseDifficulty( PlayerNumber pn, int dir );
	bool IsCourseDifficultyShown( CourseDifficulty cd );
	Difficulty GetEasiestNotesDifficulty() const;
	RageTimer			m_timeGameStarted;	// from the moment the first player pressed Start

	/* This is set to a random number per-game/round; it can be used for a random seed. */
	int				m_iGameSeed, m_iRoundSeed;

	bool			PlayersCanJoin() const;	// true if it's not too late for a player to join
	bool			EnoughCreditsToJoin() const;	// true if an unjoined player can join by pressint start
	int				GetNumSidesJoined() const;

	GameDef*	GetCurrentGameDef();
	const StyleDef*	GetCurrentStyleDef() const;

	void GetPlayerInfo( PlayerNumber pn, bool& bIsEnabledOut, bool& bIsHumanOut );
	bool IsPlayerEnabled( PlayerNumber pn ) const;
	bool IsPlayerEnabled( int p ) const { return IsPlayerEnabled( (PlayerNumber)p ); };
	int	GetNumPlayersEnabled() const;
	bool PlayerUsingBothSides() const;

	bool IsHumanPlayer( PlayerNumber pn ) const;
	bool IsHumanPlayer( int p ) const { return IsHumanPlayer( (PlayerNumber)p ); };
	int GetNumHumanPlayers() const;
	PlayerNumber GetFirstHumanPlayer() const;
	bool IsCpuPlayer( PlayerNumber pn ) const;
	bool IsCpuPlayer( int p ) const { return IsCpuPlayer( (PlayerNumber)p ); };
	bool AnyPlayersAreCpu() const;

	void GetCharacters( vector<Character*> &apCharactersOut );
	Character* GameState::GetRandomCharacter();
	Character* GameState::GetDefaultCharacter();

	PlayerController	m_PlayerController[NUM_PLAYERS];
	
	// Used in Battle and Rave
	int					m_iCpuSkill[NUM_PLAYERS];	// only used when m_PlayerController is PC_CPU
	// Used in Rave
	float				m_fSuperMeterGrowthScale[NUM_PLAYERS];


	bool IsCourseMode() const;
	bool IsBattleMode() const; /* not Rave */

	bool ShowMarvelous() const;

	CString			m_sLoadingMessage;	// used in loading screen
	CString			m_sPreferredGroup;	// GROUP_ALL_MUSIC denotes no preferred group
	bool			m_bChangedFailType;	// true if FailType was changed in the song options screen
	Difficulty		m_PreferredDifficulty[NUM_PLAYERS];
	SortOrder	m_SortOrder;			// used by MusicWheel
	bool			m_bEditing;			// NoteField does special stuff when this is true
	bool			m_bDemonstrationOrJukebox;	// ScreenGameplay does special stuff when this is true
	bool			m_bJukeboxUsesModifiers;
	int				m_iNumStagesOfThisSong;
	int				m_iCurrentStageIndex;

	int				GetStageIndex() const;
	void			BeginStage();
	void			CancelStage();
	void			FinishStage();
	int				GetNumStagesLeft() const;
	bool			IsFinalStage() const;
	bool			IsExtraStage() const;
	bool			IsExtraStage2() const;
	CString			GetStageText() const;
	void			GetAllStageTexts( CStringArray &out ) const;
	int				GetCourseSongIndex() const;
	CString			GetPlayerDisplayName( PlayerNumber pn ) const;


	//
	// State Info used during gameplay
	//
	Song*		m_pCurSong;
	Steps*		m_pCurSteps[NUM_PLAYERS];
	Course*		m_pCurCourse;
	Course*		m_pCurTrail[NUM_PLAYERS];


	//
	// Music statistics:  Arcade: the current stage (one song).  Oni/Endles: a single song in a course
	//
	// Let a lot of classes access this info here so the don't have to keep their own copies.
	//
	float		m_fMusicSeconds;	// time into the current song
	float		m_fSongBeat;
	float		m_fCurBPS;
	bool		m_bFreeze;	// in the middle of a freeze
	RageTimer	m_LastBeatUpdate; // time of last m_fSongBeat, etc. update
	bool		m_bPastHereWeGo;
	float		m_fLastDrawnBeat[NUM_PLAYERS]; // set by NoteField

	map<float,CString> m_BeatToNoteSkin[NUM_PLAYERS];
	int			m_BeatToNoteSkinRev; /* hack: incremented whenever m_BeatToNoteSkin changes */
	void ResetNoteSkins();
	void ResetNoteSkinsForPlayer( PlayerNumber pn );
	void GetAllUsedNoteSkins( vector<CString> &out ) const;

	static const float MUSIC_SECONDS_INVALID;

	void ResetMusicStatistics();	// Call this when it's time to play a new song.  Clears the values above.
	void UpdateSongPosition( float fPositionSeconds, const TimingData &timing, const RageTimer &timestamp = RageZeroTimer );
	float GetSongPercent( float beat ) const;

	enum HealthState { HOT, ALIVE, DANGER, DEAD };
	HealthState	m_HealthState[NUM_PLAYERS];
	bool AllAreInDangerOrWorse() const;
	bool AllAreDead() const;
	bool OneIsHot() const;

	// used in PLAY_MODE_BATTLE and PLAY_MODE_RAVE
	AttackArray	m_ActiveAttacks[NUM_PLAYERS];
	
	// Attacks take a while to transition out of use.  Account for this in PlayerAI
	// by still penalizing it for 1 second after the player options are rebuilt.
	int		m_iLastPositiveSumOfAttackLevels[NUM_PLAYERS];
	float	m_fSecondsUntilAttacksPhasedOut[NUM_PLAYERS];	// positive means PlayerAI is still affected

	vector<Attack>	m_ModsToApply[NUM_PLAYERS];

	void SetNoteSkinForBeatRange( PlayerNumber pn, CString sNoteSkin, float StartBeat, float EndBeat );

	// used in PLAY_MODE_BATTLE
	Attack	m_Inventory[NUM_PLAYERS][NUM_INVENTORY_SLOTS];
	float	m_fOpponentHealthPercent;

	// used in PLAY_MODE_RAVE
	float	m_fTugLifePercentP1;
	float	m_fSuperMeter[NUM_PLAYERS];	// between 0 and NUM_ATTACK_LEVELS
	
	bool	m_bAttackBeganThisUpdate[NUM_PLAYERS];	// flag for other objects to watch (play sounds)
	bool	m_bAttackEndedThisUpdate[NUM_PLAYERS];	// flag for other objects to watch (play sounds)
	void GetUndisplayedBeats( PlayerNumber pn, float TotalSeconds, float &StartBeat, float &EndBeat ) const; // only meaningful when a NoteField is in use
	void LaunchAttack( PlayerNumber target, Attack aa );
	void RebuildPlayerOptionsFromActiveAttacks( PlayerNumber pn );
	void RemoveAllActiveAttacks();	// called on end of song
	void RemoveActiveAttacksForPlayer( PlayerNumber pn, AttackLevel al=NUM_ATTACK_LEVELS /*all*/ );
	void RemoveAllInventory();
	int GetSumOfActiveAttackLevels( PlayerNumber pn ) const;
	PlayerNumber GetBestPlayer() const;
	StageResult GetStageResult( PlayerNumber pn ) const;

	void ResetStageStatistics();	// Call this when it's time to play a new stage.
	void GetFinalEvalStatsAndSongs( StageStats& statsOut, vector<Song*>& vSongsOut ) const;	// shown on arcade final evaluation


	//
	// Options stuff
	//

	PlayerOptions	m_CurrentPlayerOptions[NUM_PLAYERS];    // current approaches destination
	PlayerOptions	m_PlayerOptions[NUM_PLAYERS];			// change this, and current will move gradually toward it
	PlayerOptions   m_StoredPlayerOptions[NUM_PLAYERS];	// user's choices on the PlayerOptions screen
	SongOptions		m_SongOptions;
	SongOptions		m_StoredSongOptions;

	void ApplyModifiers( PlayerNumber pn, CString sModifiers );
	void StoreSelectedOptions();
	void RestoreSelectedOptions();

	bool IsDisqualified( PlayerNumber pn );

	void AdjustFailType();

	// character stuff
private:
	vector<Character*> m_pCharacters;

public:
	Character* m_pCurCharacters[NUM_PLAYERS];

	void ReloadCharacters();

	


	bool HasEarnedExtraStage() const;
	bool m_bAllow2ndExtraStage; //only used when "Allow Selection of Extra Stage is on"


	//
	// Ranking Stuff
	//
	struct RankingFeat
	{
		enum { SONG, COURSE, CATEGORY } Type;
		Song* pSong;		// valid if Type == SONG
		Steps* pSteps;		// valid if Type == SONG
		Course* pCourse;	// valid if Type == COURSE
		Grade grade;
		int iScore;
		float fPercentDP;
		CString Banner;
		CString Feat;
		CString *pStringToFill;
	};

	void GetRankingFeats( PlayerNumber pn, vector<RankingFeat> &vFeatsOut ) const;
	void StoreRankingName( PlayerNumber pn, CString name );	// Called by name entry screens
	vector<CString*> m_vpsNamesThatWereFilled;	// filled on StoreRankingName, 


	//
	// Award stuff
	//
	// lowest priority in front, highest priority at the back.
	deque<PerDifficultyAward> m_vLastPerDifficultyAwards[NUM_PLAYERS];
	deque<PeakComboAward> m_vLastPeakComboAwards[NUM_PLAYERS];


	//
	// Arrow positioning
	//
	NoteFieldPositioning *m_pPosition;

	//
	// Attract stuff
	//
	int m_iNumTimesThroughAttract;	// negative means play regardless of m_iAttractSoundFrequency setting
	bool IsTimeToPlayAttractSounds();
};


extern GameState*	GAMESTATE;	// global and accessable from anywhere in our program

#endif

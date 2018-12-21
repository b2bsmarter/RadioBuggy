//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************
#pragma once

// The IGameUIControl interface provides an abstraction to the GameMain class
// for controlling UI elements on the screen in response to game state.  It helps to
// provide separation between the game control and how the UI is displayed to the user.
//    SetGameLoading - is called to have the UI indicate the game is currently loading
//        resources for the game or a particular level.
//    SetGameStats - is called to have the UI indicate the status of the last completed game.
//    SetGameOver - is called to have the UI indicate this iteration of the game as finished.
//    SetLevelStart - is called to have the UI indicate the level is ready to begin.  This includes
//        providing the objectives for the level and other current data.
//    SetPause - is called to have the UI indicate the game is currently in the Paused state.
//    ShowTooSmall - this method indicates the UI should display to the user that the current
//        display configuration is too small for the game to be played.
//    HideTooSmall - this method inticates the UI should stop displaying to the user that the
//        current display configuration is too small for game play.
//    SetAction - is called to have the UI indicate the next action the user should take.
//    ShowGameInfoOverlay - is called to have the UI display to the user the current state
//        as defined by the other interface calls above.
//    HideGameInfoOverlay - is called to have the UI remove the display of the current state
//        when the game transitions into active game play.

namespace Simple3DGame
{
	public enum class GameInfoOverlayCommand
	{
		None,
		TapToContinue,
		PleaseWait,
		PlayAgain,
	};

	public interface class IGameUIControl
	{
		void SetTitle(Platform::String^ objective);
		void SetScore(int points);
		void SetPause();
		void ForcePause(bool pause_to);
		void SetRockets(int rockets);
		void SetRings(int rings_at, int rings_to);
		void SetTimeFloat(float time);
		void SetCountDown(int time);
		void SetCountDown(Platform::String^ objective);
		void SetCountDownScore(int score, int col);
		void SetCountParameters(float scale, float opaicity, bool b_visible);

		void SetLevelList();

		void AddLevelListItem(Platform::String^ Image, Platform::String^ Name, int Score, bool enabled, int _level_no);
		void ClearLevelList();
		void ScrollToLevel(int l);

		void ShowStartScreen();
		void ShowGameScreen();
		void ShowNameChoose();
		void ShowNameEntry();

		void SetPlayerList();
		void AddPlayerListItem(int index, Platform::String^ Name, int _score, int _level);
		void ClearPlayerList();

		void SetFadeOp(float val);
		void StopFade();

		void HideHUD();
		void ShowHUD();

		void SetEditMode();

		void SmallEverything();

		void SetScreenControls(int mode);
/*
		void SetGameLoading();w
		void SetGameStats(int maxLevel, int hitCount, int shotCount);
		void SetGameOver(bool win, int maxLevel, int hitCount, int shotCount, int highScore);
		void SetLevelStart(int level, Platform::String^ objective, float timeLimit, float bonusTime);
		void SetPause(int level, int hitCount, int shotCount, float timeRemaining);
		void ShowTooSmall();
		void HideTooSmall();
		void SetAction(GameInfoOverlayCommand action);
		void HideGameInfoOverlay();
		void ShowGameInfoOverlay();
		*/
	};
};


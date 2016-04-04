#include "stdafx.h"
#include "Simulation.h"

BOOL GameInitialize(HINSTANCE hInstance)
{
	_pGame = new GameEngine(hInstance, IDI_ICON, IDI_ICON_SM);
	if (NULL == _pGame)
	{
		return FALSE;
	}

	return TRUE;
}

void GameStart(HWND hWindow)
{
	// Seed the random number generator.
	srand(GetTickCount());
	_hWindow = _pGame->GetWindow();

	_pGame->ConfigureGame("PredatorPrey.txt");
	_iWidth = _pGame->GetWidth();
	_iHeight = _pGame->GetHeight();

	_pGame->CreateObstacles();
	_pGame->CreatePredators();
	_pGame->CreatePrey();
	_pGame->OriginalObstacles();
	Reset();
}

void GameCycle()
{
	if (!startSimulation)
	{
		return;
	}

	_obstacles = _pGame->GetObstacles();
	_predators = _pGame->GetPredators();
	_preys = _pGame->GetPreys();

	// Iterate predators.
	const size_t ulCount1 = _predators.size();
	for ( size_t ulActor1 = 0; ulActor1 < ulCount1; ++ulActor1 )
	{
		Actor* _predator = _predators[ulActor1];
		BOOL process = _predator->ProcessActor();
		if (process)
		{
			_predator->Update(_preys);
			_predator->Move(_obstacles, _iWidth, _iHeight);
		}
	}

	// Iterate prey.
	const size_t ulCount2 = _preys.size();
	for ( size_t ulActor2 = 0; ulActor2 < ulCount2; ++ulActor2 )
	{
		Actor* _prey = _preys[ulActor2];
		BOOL process = _prey->ProcessActor();
		if (process)
		{
			_prey->Update(_predators);
			_prey->Move(_obstacles, _iWidth, _iHeight);
		}
	}

	InvalidateRect(_hWindow, NULL, FALSE);
}

void GameEnd()
{
	// Cleanup any game variables.
	_pGame->DeInitialize();

	// Cleanup the game engine.
	if (NULL != _pGame) { delete _pGame; }
}

void GamePaint(HDC hDC)
{
	_pGame->DrawWindow();
}

BOOL HandleKeys()
{
	BOOL quit = FALSE;
	if (GetAsyncKeyState(VK_ESCAPE) < 0)
	{ 
		quit = TRUE;
	}
	if (GetAsyncKeyState(VK_RETURN) < 0)
	{
		Reset();
	}
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		if (startSimulation) { startSimulation = FALSE; }
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		if (!startSimulation) { startSimulation = TRUE; }
	}
	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		startSimulation = !startSimulation;
	}
	return quit;
}

void Reset()
{
	_pGame->Reset();
	startSimulation = FALSE;
	InvalidateRect(_hWindow, NULL, FALSE);
}
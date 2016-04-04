#pragma once

#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "Actor.h"
#include "Predator.h"
#include "Prey.h"

using namespace std;
typedef WORD	CONFIGURATION;
typedef WORD	TILESTATE;


extern BOOL GameInitialize(HINSTANCE hInstance);
extern void GameStart(HWND hWindow);
extern void GameEnd();
extern void GamePaint(HDC hDC);
extern void GameCycle();
extern BOOL HandleKeys();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);


class GameEngine
{
protected:
	// Member variables.
	static GameEngine*	m_pGameEngine;
	HINSTANCE			m_hInstance;
	HWND				m_hWindow;
	WORD				m_wIcon, m_wSmallIcon;
	int					m_iWidth, m_iHeight;
	int					m_iCanvasWidth, m_iCanvasHeight;
	int					m_iFrameDelay;
	int					m_iConfiguration[50];
	int*				m_pbObstacleMap;
	int*				m_pbObstacleStartMap;
	vector<Actor*>		m_vPredator;
	vector<Actor*>		m_vPrey;

public:
	// Constructor / Destructor.
	GameEngine(HINSTANCE hInstance, WORD wIcon, WORD wSmallIcon);
	virtual ~GameEngine() {}

	// General methods.
	static GameEngine*	GetEngine() { return m_pGameEngine; }
	BOOL				Initialize(int iCmdShow);
	void				DeInitialize();
	LRESULT				HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);
	void				ConfigureGame(string filename);
	void				CreateObstacles();
	void				CreatePredators();
	void				CreatePrey();
	void				OriginalObstacles();
	void				Reset();
	void				DrawWindow();

	// Accessor methods.
	HINSTANCE			GetInstance() { return m_hInstance; }
	HWND				GetWindow() { return m_hWindow; }
	void				SetWindow(HWND hWindow) { m_hWindow = hWindow; }
	int					GetFrameDelay() const { return m_iFrameDelay; }
	int					GetCanvasWidth() const { return m_iCanvasWidth; }
	int					GetCanvasHeight() const { return m_iCanvasHeight; }
	int					GetWidth() const { return m_iWidth; }
	int					GetHeight() const { return m_iHeight; }
	int*				GetObstacles() const { return m_pbObstacleMap; }
	vector<Actor*>		GetPredators() const { return m_vPredator; }
	vector<Actor*>		GetPreys() const { return m_vPrey; }

private:
	void				SetFrameRate(int iFrameRate) { m_iFrameDelay = 1000 / iFrameRate; }
};



const CONFIGURATION
	MAX_NUMBER_GAMELOOPS= 0x0000L,
	WIDTH_ENVIRONMENT	= 0x0001L,
	HEIGHT_ENVIRONMENT	= 0x0002L,
	WIDTH_OBSTACLE		= 0x0003L,
	HEIGHT_OBSTACLE		= 0x0004L,
	NUMBER_OBSTACLES	= 0x0005L,
	NUMBER_PREDATORS	= 0x0006L,
	NUMBER_PREY			= 0x0007L,
	PREDATOR_HEARING	= 0x0008L,
	PREDATOR_VISION		= 0x0009L,
	PREDATOR_MOVEMENT	= 0x000AL,
	PREDATOR_MEMORY		= 0x000BL,
	PREY_HEARING		= 0x000CL,
	PREY_VISION			= 0x000DL,
	PREY_MOVEMENT		= 0x000EL;


const TILESTATE
	NONE				= 0x0000L,
	OBSTACLE			= 0x0001L,
	PREDATOR			= 0x0002L,
	PREY				= 0x0003L;
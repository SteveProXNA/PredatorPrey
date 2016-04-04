#include "stdafx.h"
#include "GameEngine.h"

GameEngine* GameEngine::m_pGameEngine = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	static int iTickTrigger = 0;
	int iTickCount;

	BOOL temp = GameInitialize(hInstance);
	if (!temp)
	{
		// End the game.
		GameEnd();
		return TRUE;
	}

	// Initialize the game engine.
	temp = GameEngine::GetEngine()->Initialize(iCmdShow);
	if (!temp)
	{
		return FALSE;
	}

	// Enter the main message loop.
	while (TRUE)
	{
		temp = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (temp)
		{
			// Process the message
			if (WM_QUIT == msg.message)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Check the tick count to see if a game cycle has elapsed.
			iTickCount = GetTickCount();
			if (iTickCount > iTickTrigger)
			{
				int iFrameDelay = GameEngine::GetEngine()->GetFrameDelay();
				iTickTrigger = iTickCount + iFrameDelay;
				BOOL quit = HandleKeys();
				if (quit)
				{
					break;
				}
				GameCycle();
			}
		}
	}

	GameEnd();
	return msg.wParam;
}

LRESULT CALLBACK CustomWndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Route all Windows messages to the game engine.
	return GameEngine::GetEngine()->HandleEvent(hWindow, msg, wParam, lParam);
}

// Constructor / Destructor.
GameEngine::GameEngine(HINSTANCE hInstance, WORD wIcon, WORD wSmallIcon) :
	m_hInstance(hInstance),
	m_wIcon(wIcon),
	m_wSmallIcon(wSmallIcon)
{
	// Set the member variables for the game engine.
	m_pGameEngine = this;
	m_hWindow = NULL;
	m_iWidth		= 1;
	m_iHeight		= 1;
	m_iCanvasWidth	= 512;
	m_iCanvasHeight = 512;
	m_iFrameDelay	= 50;		// 20fps
	m_pbObstacleMap = NULL;
	m_pbObstacleStartMap = NULL;
}

// General methods.
BOOL GameEngine::Initialize(int iCmdShow)
{
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = CustomWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(m_wIcon));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("PredatorPrey");
	if (!RegisterClass(&wndClass))
	{
		return FALSE;
	}

	/// create window
	RECT size;
	size.top = 0;
	size.left = 0;
	size.bottom = m_iCanvasWidth;
	size.right = m_iCanvasHeight;
	AdjustWindowRectEx( &size, WS_CAPTION | WS_BORDER, false, WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW  );
	int iNeededWindowWidth = size.right  - size.left;
	int iNeededWindowHeight =size.bottom - size.top;

	m_hWindow = CreateWindow(
		TEXT("PredatorPrey"),				// window class name
		TEXT("Predator Prey Demo"),			// window caption
		WS_CAPTION | WS_OVERLAPPEDWINDOW,	// window style
		CW_USEDEFAULT,						// initial x position
		CW_USEDEFAULT| WS_VISIBLE,			// initial y position
		iNeededWindowWidth,					// initial x size
		iNeededWindowHeight,				// initial y size
		NULL,								// parent window handle
		NULL,								// window menu handle
		m_hInstance,						// program instance handle
		NULL								// creation parameters
		);

	// finally we can show the window
	ShowWindow(m_hWindow, iCmdShow);
	UpdateWindow(m_hWindow);

	return TRUE;
}

void CleanupActors(vector<Actor*> actors)
{
	const size_t ulCount = actors.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		Actor* pActor = actors[ulActor];
		delete pActor;
	}
	actors.clear();
}

void GameEngine::DeInitialize()
{
	// Cleanup obstacles.
	delete[] m_pbObstacleMap;
	delete[] m_pbObstacleStartMap;
	m_pbObstacleMap = NULL;
	m_pbObstacleStartMap = NULL;

	// Cleanup predators and prey.
	CleanupActors(m_vPredator);
	CleanupActors(m_vPrey);
}

LRESULT GameEngine::HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Route Windows messages to game engine member functions.
	switch (msg)
	{
	case WM_CREATE:
		// Set the game window and start the game.
		SetWindow(hWindow);
		GameStart(hWindow);
		return 0;

	case WM_PAINT:
		DrawWindow();
		break;

	case WM_SIZE:
		m_iCanvasWidth = LOWORD(lParam);
		m_iCanvasHeight = HIWORD(lParam);
		break;

	case WM_DESTROY:
		// Quit the application.
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWindow, msg, wParam, lParam);
}

void GameEngine::ConfigureGame(string filename)
{
	// Hard code data if no config file.
	m_iConfiguration[MAX_NUMBER_GAMELOOPS]	= 20;
	m_iConfiguration[WIDTH_ENVIRONMENT]		= 16;
	m_iConfiguration[HEIGHT_ENVIRONMENT]	= 16;
	m_iConfiguration[WIDTH_OBSTACLE]		= 1;
	m_iConfiguration[HEIGHT_OBSTACLE]		= 1;
	m_iConfiguration[NUMBER_OBSTACLES]		= 50;
	m_iConfiguration[NUMBER_PREDATORS]		= 20;
	m_iConfiguration[NUMBER_PREY]			= 20;
	m_iConfiguration[PREDATOR_HEARING]		= 7;
	m_iConfiguration[PREDATOR_VISION]		= 7;
	m_iConfiguration[PREDATOR_MOVEMENT]		= 5;
	m_iConfiguration[PREDATOR_MEMORY]		= 15;
	m_iConfiguration[PREY_HEARING]			= 7;
	m_iConfiguration[PREY_VISION]			= 7;
	m_iConfiguration[PREY_MOVEMENT]			= 5;

	std::ifstream myfile;
	myfile.open(filename.c_str(), std::ifstream::in);
	
	if(!myfile.fail())
	{
		char buffer[256];
		int cnt = 0;

		while (!myfile.eof())
		{
			myfile.getline (buffer,100);
			int temp = atoi(buffer);
			m_iConfiguration[cnt++] = temp;
		}

		myfile.close();
	}
	else
	{
		myfile.clear(std::ios::failbit);
	}

	int iFrameRate = m_iConfiguration[MAX_NUMBER_GAMELOOPS];
	SetFrameRate(iFrameRate);
	m_iWidth = m_iConfiguration[WIDTH_ENVIRONMENT];
	m_iHeight = m_iConfiguration[HEIGHT_ENVIRONMENT];
}

void GameEngine::CreateObstacles()
{
	m_pbObstacleMap = new int[m_iWidth * m_iHeight];
	m_pbObstacleStartMap = new int[m_iWidth * m_iHeight];
	memset(m_pbObstacleMap, 0, sizeof(int) * m_iWidth * m_iHeight);
	memset(m_pbObstacleStartMap, 0, sizeof(int) * m_iWidth * m_iHeight);

	int iWidth = m_iWidth;		//m_iWidth  - WIDTH_OBSTACLE;
	int iHeight = m_iHeight;	//m_iHeight - HEIGHT_OBSTACLE;
	int numberObstacles = m_iConfiguration[NUMBER_OBSTACLES];

	for (int i=0; i < numberObstacles; ++i)
	{
		int x = rand() % iWidth;
		int y = rand() % iHeight;

		while (m_pbObstacleMap[x + y * iWidth] != NONE)
		{
			x = rand() % iWidth;
			y = rand() % iHeight;
		}

		m_pbObstacleMap[x + y * iWidth] = OBSTACLE;
	}
}

void GameEngine::CreatePredators()
{
	int numberPredators = m_iConfiguration[NUMBER_PREDATORS];
	m_vPredator.reserve(numberPredators);

	int hearing = m_iConfiguration[PREDATOR_HEARING];
	int vision = m_iConfiguration[PREDATOR_VISION];
	int movement = m_iConfiguration[PREDATOR_MOVEMENT];
	int mem = m_iConfiguration[PREDATOR_MEMORY];

	for (int i=0; i < numberPredators; ++i)
	{
		int x = rand() % m_iWidth;
		int y = rand() % m_iHeight;

		while (m_pbObstacleMap[x + y * m_iWidth] != NONE)
		{
			x = rand() % m_iWidth;
			y = rand() % m_iHeight;
		}

		m_pbObstacleMap[x + y * m_iWidth] = PREDATOR;
		int dir = rand() % 4;

		Actor* predator = new Predator(x, y, (Actor::eDirection)dir, hearing, vision, movement, mem);
		m_vPredator.push_back(predator);
	}
}

void GameEngine::CreatePrey()
{
	int numberPrey = m_iConfiguration[NUMBER_PREY];
	m_vPrey.reserve(numberPrey);

	int hearing = m_iConfiguration[PREY_HEARING];
	int vision = m_iConfiguration[PREY_VISION];
	int movement = m_iConfiguration[PREY_MOVEMENT];

	for (int i=0; i < numberPrey; ++i)
	{
		int x = rand() % m_iWidth;
		int y = rand() % m_iHeight;

		while (m_pbObstacleMap[x + y * m_iWidth] != NONE)
		{
			x = rand() % m_iWidth;
			y = rand() % m_iHeight;
		}

		m_pbObstacleMap[x + y * m_iWidth] = PREY;
		int dir = rand() % 4;

		Actor* prey = new Prey(x, y, (Actor::eDirection)dir, hearing, vision, movement);
		m_vPrey.push_back(prey);
	}
}

void GameEngine::OriginalObstacles()
{
	// Original obstacles.
	for (int x=0; x < m_iWidth; ++x)
	{
		for (int y=0; y < m_iHeight; ++y)
		{
			int index = x + y * m_iWidth;
			m_pbObstacleStartMap[index] = m_pbObstacleMap[index];
		}
	}
}

void GameEngine::Reset()
{
	// Reset obstacles.
	for (int x=0; x < m_iWidth; ++x)
	{
		for (int y = 0; y < m_iHeight; ++y)
		{
			int index = x + y * m_iWidth;
			m_pbObstacleMap[index] = m_pbObstacleStartMap[index];
		}
	}

	// Reset actors.
	size_t ulCount = m_vPredator.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		Actor* pActor = m_vPredator[ulActor];
		pActor->Reset();
		((Predator*)pActor)->SetPredatorState(Predator::ePredatorState::SEARCH);
		((Predator*)pActor)->SetTargetPrey(NULL);
	}
	ulCount = m_vPrey.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		Actor* pActor = m_vPrey[ulActor];
		pActor->Reset();
		((Prey*)pActor)->SetPreyState(Prey::ePreyState::IDLE);
		((Prey*)pActor)->SetTargetPredator(NULL);
	}
}

void DrawActor(vector<Actor*> actors, HDC hdc, HPEN pen, float fCellWidth, float fCellHeight, int iActorBorder, int iLineLenX, int iLineLenY)
{
	size_t ulCount = actors.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		const Actor* pActor = actors[ ulActor ];

		int a = int(pActor->GetX() * fCellWidth + iActorBorder);
		int b = int(pActor->GetY() * fCellHeight + iActorBorder);

		int c = int(a + fCellWidth - iActorBorder*2);
		int d = int(b + fCellHeight - iActorBorder*2);

		SelectObject( hdc, pen );
		Ellipse( hdc, a,b,c,d );

		int iCenterX = (a+c)/2;
		int iCenterY = (b+d)/2;
		MoveToEx( hdc, iCenterX , iCenterY , NULL );

		Actor::eDirection eDirection = pActor->GetDirection();
		switch ( eDirection )
		{
		case Actor::eDirection::DIR_LEFT:
			LineTo( hdc, iCenterX-iLineLenX, iCenterY );
			break;
		case Actor::eDirection::DIR_RIGHT:
			LineTo( hdc, iCenterX+iLineLenX, iCenterY );
			break;
		case Actor::eDirection::DIR_UP:
			LineTo( hdc, iCenterX, iCenterY-iLineLenY );
			break;
		case Actor::eDirection::DIR_DOWN:
			LineTo( hdc, iCenterX, iCenterY+iLineLenY );
			break;
		}

		/// draw information text
		std::string strInfoText = pActor->GetInfoText();
		if ( !strInfoText.empty() )
		{
			SetTextColor( hdc, RGB( 0,128,0 ) );
			TextOut( hdc, iCenterX + iLineLenX + 4, iCenterY, strInfoText.c_str(), strInfoText.length() );
		}
	}
}
void GameEngine::DrawWindow()
{
	PAINTSTRUCT		paint;
	HDC				hdcFinal			= BeginPaint( m_hWindow, &paint );
	static HBRUSH	hbrBkgnd			= CreateSolidBrush( RGB( 255,255,255 ) ); 
	static HBRUSH	hbrBkgndObstacle	= CreateSolidBrush( RGB( 32,32,32 ) ); 
	static HBRUSH	hbrBkgndActor		= CreateSolidBrush( RGB( 128,128,128 ) ); 
	static HPEN		blackPen			= CreatePen( PS_SOLID, 3, RGB( 0,0,0 ) );
	static HPEN		redPen				= CreatePen( PS_SOLID, 2, RGB( 255,0,0 ) );
	static HPEN		bluePen				= CreatePen( PS_SOLID, 2, RGB( 0,0,255 ) );
	static HPEN		greenPen			= CreatePen( PS_SOLID, 2, RGB( 0,0,255 ) );

	HDC		hdc		= CreateCompatibleDC(hdcFinal);
	HBITMAP hbmMem	= CreateCompatibleBitmap(hdcFinal, m_iCanvasWidth, m_iCanvasHeight);
	HANDLE  hOld    = SelectObject(hdc, hbmMem);


	int iCanvasWidth	= m_iCanvasWidth;
	int iCanvasHeight	= m_iCanvasHeight;

	//////////////////////////////////////////////////////////////////////////
	/// background
	SelectObject( hdc, hbrBkgnd );
	Rectangle( hdc, 0,0,iCanvasWidth,iCanvasHeight );

	//////////////////////////////////////////////////////////////////////////
	///  black lines
	SelectObject( hdc, blackPen );

	float fCellWidth		= (float)iCanvasWidth / (float)m_iWidth;
	float fCellHeight		= (float)iCanvasHeight / (float)m_iHeight;

	for (int y = 0; y <= m_iHeight; ++y )
	{
		int iCoord = int( y * fCellHeight );
		MoveToEx( hdc, 0, iCoord, NULL );
		LineTo( hdc, iCanvasWidth , iCoord );
	}

	for (int x = 0; x <= m_iWidth; ++x )
	{
		int iCoord = int( x * fCellWidth);
		MoveToEx( hdc, iCoord, 0, NULL );
		LineTo( hdc, iCoord , iCanvasWidth );
	}


	//////////////////////////////////////////////////////////////////////////
	/// obstacles
	SelectObject( hdc, hbrBkgndObstacle );
	for (int y = 0; y < m_iHeight; ++y )
	{
		for (int x = 0; x < m_iWidth; ++x )
		{
			if (OBSTACLE == m_pbObstacleMap[x + y * m_iWidth])
			{
				int a = int( x * fCellWidth );
				int b = int( y * fCellHeight );
				Rectangle( hdc, a,b, int(a + fCellWidth), int(b + fCellHeight) );
			}
		}
	}

	SelectObject( hdc, hbrBkgndActor );

	int iActorBorder = 4;

	int iLineLenX = int(fCellWidth/2.f);
	int iLineLenY = int(fCellHeight/2.f);

	//////////////////////////////////////////////////////////////////////////
	/// actors
	DrawActor(m_vPredator, hdc, redPen, fCellWidth, fCellHeight, iActorBorder, iLineLenX, iLineLenY);
	DrawActor(m_vPrey, hdc, bluePen, fCellWidth, fCellHeight, iActorBorder, iLineLenX, iLineLenY);

	// blit the offscreen buffer
	BitBlt(hdcFinal, 0, 0, m_iCanvasWidth, m_iCanvasHeight, hdc, 0, 0, SRCCOPY);
	// Free-up the off-screen DC
	SelectObject(hdc, hOld);
	DeleteObject(hbmMem);
	DeleteDC (hdc);
	EndPaint( m_hWindow, &paint );
}
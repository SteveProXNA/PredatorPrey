#include		"resource.h"
#include		"GameEngine.h"

HINSTANCE		_hInstance;
HWND			_hWindow;
GameEngine*		_pGame;
int*			_obstacles;
vector<Actor*>	_predators;
vector<Actor*>	_preys;
int				_iWidth, _iHeight;

BOOL			startSimulation;
void			Reset();
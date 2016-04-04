#pragma once

#include "stdafx.h"
#include "GameEngine.h"

class Actor
{
public:
	enum eDirection
	{
		DIR_LEFT = 0,
		DIR_RIGHT = 1,
		DIR_UP = 2,
		DIR_DOWN = 3
	};

	Actor(int x, int y, eDirection dir, int hearing, int vision, int movement) :
		m_iXstart(x),
		m_iYstart(y),
		m_eStartDirection(dir),
		m_iHearing(hearing),
		m_iVision(vision),
		m_iMovement(movement)
		{}
	virtual ~Actor() {}

	virtual void Update(std::vector<Actor*> actors) = 0;
	virtual void Move(int* obstacles, int iWidth, int iHeight) = 0;

	void Reset();

	int GetX() const { return m_iX; }
	int GetY() const { return m_iY; }
	Actor::eDirection GetDirection() const { return m_eDirection; }
	void SetDirection(Actor::eDirection eDirection) { m_eDirection = eDirection; }
	std::string GetInfoText() const { return m_strInfoText; }
	int GetHearing() const { return m_iHearing; }
	int GetVision() const { return m_iVision; }
	BOOL ProcessActor();

protected:
	int				m_iX, m_iY;
	int				m_iXstart, m_iYstart;
	eDirection		m_eDirection;
	eDirection		m_eStartDirection;
	int				m_iHearing;
	int				m_iVision;
	int				m_iMovement;
	int				m_iCounter;
	std::string		m_strInfoText;
	BOOL			m_bLeft, m_bRight, m_bUp, m_bDown;

	void			CheckDirections(int* obstacles, int iWidth, int iHeight);
	void			MoveDirection();
};
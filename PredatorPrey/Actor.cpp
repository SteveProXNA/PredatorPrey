#include "stdafx.h"
#include "GameEngine.h"

void Actor::Reset()
{
	m_iX = m_iXstart;
	m_iY = m_iYstart;
	m_eDirection = m_eStartDirection;
	m_iCounter = 0;
	m_bLeft = m_bRight = m_bUp = m_bDown = FALSE;
}

BOOL Actor::ProcessActor()
{
	m_iCounter++;
	if (m_iCounter < m_iMovement)
	{
		return FALSE;
	}

	m_iCounter = 0;
	return TRUE;
}

void Actor::CheckDirections(int* obstacles, int iWidth, int iHeight)
{
	int index	= m_iX + m_iY * iWidth;
	m_bLeft = m_bRight = m_bUp = m_bDown = FALSE;

	int iLeft	= (m_iX-1) + m_iY * iWidth;
	int iRight	= (m_iX+1) + m_iY * iWidth;
	int iUp		= m_iX + (m_iY-1) * iWidth;
	int iDown	= m_iX + (m_iY+1) * iWidth;

	m_bLeft		= m_iX > 0			&& obstacles[iLeft] == NONE;
	m_bRight	= m_iX < iWidth-1	&& obstacles[iRight] == NONE;
	m_bUp		= m_iY > 0			&& obstacles[iUp] == NONE;
	m_bDown		= m_iY < iHeight-1	&& obstacles[iDown] == NONE;
}

void Actor::MoveDirection()
{
	if (m_eDirection == Actor::eDirection::DIR_LEFT)	{ m_iX -= 1; }
	if (m_eDirection == Actor::eDirection::DIR_RIGHT)	{ m_iX += 1; }
	if (m_eDirection == Actor::eDirection::DIR_UP)		{ m_iY -= 1; }
	if (m_eDirection == Actor::eDirection::DIR_DOWN)	{ m_iY += 1; }
}
#include "stdafx.h"
#include "GameEngine.h"

void Prey::Update(std::vector<Actor*> actors)
{
	if (m_ePreyState == Prey::ePreyState::IDLE)
	{
		// Look for the nearest predator.
		Actor* targetPredator = IdleTargetPredator(actors);
		if (NULL != targetPredator)
		{
			// If found then update predator and prey.
			m_pTarget = targetPredator;
			m_ePreyState = Prey::ePreyState::FLEE;

			((Predator*)targetPredator)->SetTargetPrey(this);
			((Predator*)targetPredator)->SetPredatorState(Predator::ePredatorState::HUNT);
		}
	}
	else if (m_ePreyState == Prey::ePreyState::FLEE)
	{
		// Check current predator is still in range.
		Actor* targetPredator = FleeTargetPredator();
		if (NULL == targetPredator)
		{
			((Predator*)m_pTarget)->SetTargetPrey(NULL);
			((Predator*)m_pTarget)->SetPredatorState(Predator::ePredatorState::SEARCH);

			m_ePreyState = Prey::ePreyState::IDLE;
			m_pTarget = NULL;
		}
	}
	else if (m_ePreyState == Prey::ePreyState::HIDE)
	{
		// TO DO.
	}
}


void Prey::Move(int* obstacles, int iWidth, int iHeight)
{
	if (m_ePreyState != Prey::ePreyState::FLEE)
	{
		return;
	}

	Actor::eDirection dirBefore = m_eDirection;
	int index = m_iX + m_iY * iWidth;

	CheckDirections(obstacles, iWidth, iHeight);
	if (!m_bLeft && !m_bRight && !m_bUp && !m_bDown)
	{
		return;
	}

	int cnt = 0;
	int dirs[4];

	// Flee predator.
	if (NULL == m_pTarget)
	{
		m_ePreyState = Prey::ePreyState::IDLE;
		return;
	}

	int iTargetX = m_pTarget->GetX();
	int iTargetY = m_pTarget->GetY();

	if (m_iX < iTargetX && m_bLeft)		{ dirs[cnt++] = Actor::eDirection::DIR_LEFT; }
	if (m_iX > iTargetX && m_bRight)	{ dirs[cnt++] = Actor::eDirection::DIR_RIGHT; }
	if (m_iY < iTargetY && m_bUp)		{ dirs[cnt++] = Actor::eDirection::DIR_UP; }
	if (m_iY > iTargetY && m_bDown)		{ dirs[cnt++] = Actor::eDirection::DIR_DOWN; }

	// can't move anywhere.
	if (0 == cnt)
	{
		return;
	}

	int rnd = rand() % cnt;
	Actor::eDirection dirAfter = (Actor::eDirection)dirs[rnd];
	m_eDirection = dirAfter;
	if (dirBefore != dirAfter)
	{
		return;
	}

	// Move.
	obstacles[index] = NONE;
	MoveDirection();
	index = m_iX + m_iY * iWidth;
	obstacles[index] = PREY;
}


Actor* Prey::IdleTargetPredator(std::vector<Actor*> actors)
{
	Actor* targetPredator = NULL;
	float maxDistance = m_iHearing;

	const size_t ulCount = actors.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		Predator* predator = (Predator*)actors[ulActor];
		Actor* predatorTargetPrey = predator->GetTargetPrey();
		if (NULL == predatorTargetPrey)
		{
			int xTarget = predator->GetX();
			int yTarget = predator->GetY();
			int xDist = m_iX - xTarget;
			int yDist = m_iY - yTarget;
			float input = (float)(xDist * xDist + yDist * yDist);
			float distance = sqrt(input);

			if (distance <= maxDistance)
			{
				targetPredator = predator;
				maxDistance = distance;
			}
		}
	}

	return targetPredator;
}

Actor* Prey::FleeTargetPredator()
{
	if (NULL == m_pTarget)
	{
		return NULL;
	}

	Predator* prey = (Predator*)m_pTarget;
	int xTarget = prey->GetX();
	int yTarget = prey->GetY();
	int xDist = m_iX - xTarget;
	int yDist = m_iY - yTarget;
	float input = (float)(xDist * xDist + yDist * yDist);
	float distance = sqrt(input);

	if (distance > m_iHearing)
	{
		return NULL;
	}

	return m_pTarget;
}
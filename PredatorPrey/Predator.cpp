#include "stdafx.h"
#include "GameEngine.h"

void Predator::Update(std::vector<Actor*> actors)
{
	if (m_ePredatorState == Predator::ePredatorState::SEARCH)
	{
		// Look for the nearest prey.
		Actor* targetPrey = SearchTargetPrey(actors);
		if (NULL != targetPrey)
		{
			// If found then update predator and prey.
			m_pTarget = targetPrey;
			m_ePredatorState = Predator::ePredatorState::HUNT;

			((Prey*)targetPrey)->SetTargetPredator(this);
			((Prey*)targetPrey)->SetPreyState(Prey::ePreyState::FLEE);
		}
	}
	else if (m_ePredatorState == Predator::ePredatorState::HUNT)
	{
		// Ensure current prey is still in range.
		Actor* targetPrey = HuntTargetPrey();
		if (NULL == targetPrey)
		{
			((Prey*)m_pTarget)->SetTargetPredator(NULL);
			((Prey*)m_pTarget)->SetPreyState(Prey::ePreyState::IDLE);
			
			m_ePredatorState = Predator::ePredatorState::SEARCH;
			m_pTarget = NULL;
		}
	}
	else if (m_ePredatorState == Predator::ePredatorState::EAT)
	{
		// TO DO.
	}
}


void Predator::Move(int* obstacles, int iWidth, int iHeight)
{
	Actor::eDirection dirBefore = m_eDirection;
	int index = m_iX + m_iY * iWidth;

	CheckDirections(obstacles, iWidth, iHeight);
	if (!m_bLeft && !m_bRight && !m_bUp && !m_bDown)
	{
		return;
	}

	int cnt = 0;
	int dirs[4];
	if (m_ePredatorState == Predator::ePredatorState::SEARCH)
	{
		// Wander.
		if (m_bLeft) { dirs[cnt++] = Actor::eDirection::DIR_LEFT; }
		if (m_bRight) { dirs[cnt++] = Actor::eDirection::DIR_RIGHT; }
		if (m_bUp) { dirs[cnt++] = Actor::eDirection::DIR_UP; }
		if (m_bDown) { dirs[cnt++] = Actor::eDirection::DIR_DOWN; }

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
		obstacles[index] = PREDATOR;
	}
	else if (m_ePredatorState == Predator::ePredatorState::HUNT)
	{
		// Hunt prey.
		if (NULL == m_pTarget)
		{
			m_ePredatorState = Predator::ePredatorState::SEARCH;
			return;
		}

		int iTargetX = m_pTarget->GetX();
		int iTargetY = m_pTarget->GetY();

		if (m_iX > iTargetX && m_bLeft)		{ dirs[cnt++] = Actor::eDirection::DIR_LEFT; }
		if (m_iX < iTargetX && m_bRight)	{ dirs[cnt++] = Actor::eDirection::DIR_RIGHT; }
		if (m_iY > iTargetY && m_bUp)		{ dirs[cnt++] = Actor::eDirection::DIR_UP; }
		if (m_iY < iTargetY && m_bDown)		{ dirs[cnt++] = Actor::eDirection::DIR_DOWN; }
		
		// can't move anywhere.
		if (0 == cnt)
		{
			((Prey*)m_pTarget)->SetTargetPredator(NULL);
			((Prey*)m_pTarget)->SetPreyState(Prey::ePreyState::IDLE);

			m_ePredatorState = Predator::ePredatorState::SEARCH;
			m_pTarget = NULL;
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
		obstacles[index] = PREDATOR;
	}
}

 Actor* Predator::SearchTargetPrey(std::vector<Actor*> actors)
{
	Actor* targetPrey = NULL;
	float maxDistance = m_iHearing;

	const size_t ulCount = actors.size();
	for ( size_t ulActor = 0; ulActor < ulCount; ++ulActor )
	{
		Prey* prey = (Prey*)actors[ulActor];
		Actor* preyTargetPredator = prey->GetTargetPredator();
		if (NULL == preyTargetPredator)
		{
			int xTarget = prey->GetX();
			int yTarget = prey->GetY();
			int xDist = m_iX - xTarget;
			int yDist = m_iY - yTarget;
			float input = (float)(xDist * xDist + yDist * yDist);
			float distance = sqrt(input);

			if (distance <= maxDistance)
			{
				targetPrey = prey;
				maxDistance = distance;
			}
		}
	}

	return targetPrey;
}

 Actor* Predator::HuntTargetPrey()
 {
	if (NULL == m_pTarget)
	{
		return NULL;
	}

	Prey* prey = (Prey*)m_pTarget;
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
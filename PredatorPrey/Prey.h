#pragma once

#include "stdafx.h"
#include "GameEngine.h"

class Prey : public Actor
{
public:
	enum ePreyState
	{
		HIDE,
		IDLE,
		FLEE
	};

	Prey(int x, int y, Actor::eDirection dir, int hearing, int vision, int movement) :
		Actor(x, y, dir, hearing, vision, movement),
		m_ePreyState(ePreyState::IDLE)
		{}
	virtual ~Prey() {}

	virtual void Update(std::vector<Actor*> actors);
	virtual void Move(int* obstacles, int iWidth, int iHeight);

	Prey::ePreyState GetPreyState() const { return m_ePreyState; }
	void SetPreyState(Prey::ePreyState state) { m_ePreyState = state; }
	Actor* GetTargetPredator() const { return m_pTarget; }
	void SetTargetPredator(Actor* predator) { m_pTarget = predator; }

private:
	ePreyState		m_ePreyState;
	Actor*			m_pTarget;

	Actor*			IdleTargetPredator(std::vector<Actor*> actors);
	Actor*			FleeTargetPredator();
};
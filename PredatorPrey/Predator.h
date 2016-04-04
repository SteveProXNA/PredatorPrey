#pragma once

#include "stdafx.h"
#include "GameEngine.h"

class Predator : public Actor
{
public:
	enum ePredatorState
	{
		SEARCH,
		HUNT,
		EAT
	};

	Predator(int x, int y, Actor::eDirection dir, int hearing, int vision, int movement, int mem) :
		Actor(x, y, dir, hearing, vision, movement),
		m_ePredatorState(ePredatorState::SEARCH),
		m_iMemory(mem)
		{}
	virtual ~Predator() {}

	virtual void Update(std::vector<Actor*> actors);
	virtual void Move(int* obstacles, int iWidth, int iHeight);

	Predator::ePredatorState GetPredatorState() const { return m_ePredatorState; }
	void SetPredatorState(Predator::ePredatorState state) { m_ePredatorState = state; }
	Actor* GetTargetPrey() const { return m_pTarget; }
	void SetTargetPrey(Actor* prey) { m_pTarget = prey; }

private:
	ePredatorState	m_ePredatorState;
	int				m_iMemory;
	Actor*			m_pTarget;

	Actor*			SearchTargetPrey(std::vector<Actor*> actors);
	Actor*			HuntTargetPrey();
};
// Steinbach, Aymeric - 1DAE08

#pragma once
#include "Room.h"
#include "Entity.h"
#include <set>

class Enemy;

class Avatar : public Entity
{
public:
	Avatar( Room* pRoom, SoundManager* pSoundManager, const std::string& spriteFileName, const Index& spriteColsRows, const Vector2f& colVect, float colWidth, float colHeight, const Index& startPos );

	void Draw() const override final;
	void Update(float elapsedSec) override final;

	void EnemyCollision( Enemy* pEnemy );
	bool IsKnockedOut() const;
	void AddInventory( int itemID );
	bool HasItem( int itemID );
	bool HasKey() const;
	int GetNrCoins() const;
	void ToggleControl( bool b );

	const int m_MaxCoins{ 3 };

private:
	static int m_EntityId;

	const float m_JumpHeight;
	const Vector2f m_JumpAcceleration;
	const Vector2f m_KnockAcceleration;
	const double m_KnockDuration;
	const double m_PushDuration;
	const float m_PushLength;
	bool m_IsJumpEnabled;
	bool m_IsDashEnabled;
	bool m_IsKnocked;
	bool m_StopState;
	bool m_HasControl;
	
	int m_NrCoins;
	std::set<int> m_Items;

	void HandleStates();
	void HandleInputs();
	void HandlePhysics(float elapsedSec);

	enum class State{ wait, move, crouch, slide, dash, stomp, pushed, knocked };
	State m_State;
	void SetState( State state );

	void Waits();
	void Moves();
	void Dashes();
	void Pushed();
	void KnockedOut();
};
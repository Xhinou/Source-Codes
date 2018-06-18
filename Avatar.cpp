// Steinbach, Aymeric - 1DAE08

#include "stdafx.h"
#include "Engine.h"
#include "Game.h"
#include "Avatar.h"
#include "Tile.h"
#include "Enemy.h"
#include "utils.h"
#include <functional>
#include <chrono>
#include <algorithm>

int Avatar::m_EntityId{ 0 };

Avatar::Avatar( Room* pRoom, SoundManager* pSoundManager, const std::string& spriteFileName, const Index& spriteColsRows, const Vector2f& colVect, float colWidth, float colHeight, const Index& startPos )
	: Entity{ pRoom, pSoundManager, spriteFileName, spriteColsRows, colVect, colWidth, colHeight, startPos, 90.0f * Engine::m_Scale }
	, m_JumpHeight{ 640.0f }
	, m_JumpAcceleration{ 0.0f, 640.0f }
	, m_KnockAcceleration{ 500.0f, 100.0f }
	, m_KnockDuration{ 1.8 }
	, m_PushLength { 500.0f }
	, m_PushDuration{ 0.2 }
	, m_IsJumpEnabled{ true }
	, m_IsDashEnabled{ true }
	, m_StopState{ false }
	, m_HasControl{ true }
	, m_NrCoins{}
{
	m_Dir = Direction::right;
	SetState( State::wait );
}

void Avatar::Draw() const
{
	float offset{ 128.0f };
	Rectf destRect{ m_Shape };
	if (m_Dir == Direction::left)
	{
		Rectf mirrorShape{ -destRect.left - destRect.width, destRect.bottom, destRect.width, destRect.height };
		glPushMatrix();
		glScalef(-1.0f, 1.0f, 1.0f);
		m_Sprite.Draw(mirrorShape);
		glPopMatrix();
	}
	else
		m_Sprite.Draw(m_Shape);
	if (Engine::m_ShowColliders)
	{
		utils::SetColor( Color::blue );
		utils::DrawRect( m_Shape );
		utils::SetColor(Color::red);
		utils::DrawCollider(m_Collider);
	}
}

void Avatar::Update(float elapsedSec)
{
	m_Clock.Update( elapsedSec );
	m_Sprite.Update(elapsedSec);
	HandleStates();
	HandleInputs();
	if ( !m_IsKinematic )
		HandlePhysics( elapsedSec );
}

void Avatar::HandleStates()
{
	m_StateManager.Update();

	if( !m_IsOnGround && m_State != State::knocked  && m_State != State::pushed )
	{
		m_Sprite.SetSpriteClip( Index{ 0, 4 }, 1 );
		if( m_State == State::dash )
		{
			m_Sprite.SetSpriteClip( Index{ 0, 7 }, 1 );
		}
		m_IsJumpEnabled = false;
	}
}

void Avatar::HandleInputs()
{
	if( m_HasControl && m_State != State::knocked && m_State != State::pushed )
	{
		if( Game::m_Controller.up && m_IsOnGround && m_IsJumpEnabled )
		{
			m_IsJumpEnabled = false;
			m_Sprite.SetSpriteClip( Index{ 0, 4 }, 1 );
			m_Velocity += m_JumpAcceleration;
			m_IsOnGround = false;
			m_pSoundManager->PlaySound( "jump", 0 );
		}
		else if( !Game::m_Controller.up && m_IsOnGround )
		{
			if ( !m_IsJumpEnabled )
				m_pSoundManager->PlaySound( "land", 0 );
			m_IsJumpEnabled = true;
		}
		if( m_IsOnGround && Game::m_Controller.b && m_IsDashEnabled )
		{
			m_IsDashEnabled = false;
			SetState( State::dash );
		}
		else if( !Game::m_Controller.b )
		{
			m_IsDashEnabled = true;
		}
	}
}

void Avatar::HandlePhysics(float elapsedSec)
{
	m_Velocity += m_Acceleration * elapsedSec;
	if( m_Velocity.y < m_Acceleration.y )
		m_Velocity.y = m_Acceleration.y;
	m_pRoom->HandleCollisions( this );
	Translate(m_Velocity * elapsedSec);
}

void Avatar::EnemyCollision( Enemy* pEnemy )
{
	if( utils::IsOverlapping( m_Collider.bottom, pEnemy->GetCollider().GetShape() ) )
	{
		m_Velocity.y = m_PushLength;
	}
	else if( !( ( m_Dir == pEnemy->GetDirection() ) &&
		( ( m_Shape.left > pEnemy->GetPos().x && m_Dir == Direction::left )
			|| ( m_Shape.left < pEnemy->GetPos().x && m_Dir == Direction::right ) ) ) )
	{
		m_Clock.StartTimer( m_KnockDuration );
		m_Velocity = { 0, m_KnockAcceleration.y * float( m_Clock.GetRemainingTime() ) };
		m_Dir = Direction( -int( pEnemy->GetDirection() ) );
		m_Velocity.x += m_KnockAcceleration.x * int( pEnemy->GetDirection() );
		SetState( State::knocked );
	}
	else if( m_State == State::dash )
	{
		pEnemy->m_IsDestroyed = true;
		SetState( State::pushed );
	}
	else if( m_IsOnGround )
	{
		SetState( State::pushed );
	}
	m_IsJumpEnabled = false;
}

void Avatar::ToggleControl( bool b )
{
	m_HasControl = b;
}

int Avatar::GetNrCoins() const
{
	return m_NrCoins;
}

bool Avatar::IsKnockedOut() const
{
	return m_IsKnocked;
}

void Avatar::AddInventory( int itemID )
{
	m_Items.insert( itemID );
	int keyID{ 0 };
	if( itemID != keyID ) ++m_NrCoins;
}

bool Avatar::HasItem( int itemID )
{
	return m_Items.find( itemID ) != m_Items.cend();
}

bool Avatar::HasKey() const
{
	int keyID{ 0 };
	return m_Items.count( keyID );
}

/// STATES

void Avatar::Waits()
{
	m_Sprite.SetSpriteClip( Index{ 0, 0 }, 1 );
	m_Velocity.x = 0;

	if( m_HasControl && ( Game::m_Controller.left || Game::m_Controller.right ) )
	{
		SetState( State::move );
	}
}

void Avatar::Moves()
{
	m_Sprite.SetSpriteClip(Index{ 0, 2 }, 4);

	if ( m_IsOnGround )
		m_pSoundManager->PlaySound( "walk", 0 );

	if ( !m_HasControl )
		SetState( State::wait );

	if (Game::m_Controller.left && !Game::m_Controller.right)
	{
		m_Dir = Direction::left;
		m_Velocity.x = -m_MoveSpeed;
	}
	else if (Game::m_Controller.right && !Game::m_Controller.left)
	{
		m_Dir = Direction::right;
		m_Velocity.x = m_MoveSpeed;
	}

	if (!Game::m_Controller.IsKeyPressed())
	{
		SetState( State::wait );
	}
}
void Avatar::Dashes()
{
	float dashSpeed{ 1.4f };
	m_Sprite.SetSpriteClip( Index{ 0, 6 }, 8, 60 );
	m_pSoundManager->PlaySound( "dash", 0 );

	if( m_Dir == Direction::right )
	{
		if( Game::m_Controller.left )
		{
			SetState( State::move );
		}
		m_Velocity.x = m_MoveSpeed * dashSpeed;
		std::vector<Tile*> pTiles = m_pRoom->GetCollidedTiles( m_Collider.right );
		for( Tile* pTile : pTiles )
		{
			if( pTile->GetHardness() == Tile::Hardness::breakable )
			{
				m_pSoundManager->PlaySound( "break", 0 );
				m_pRoom->PlayAnim( pTile->GetShape().GetCenter(), "wl3_spritesheet_fragments.png", Index{ 1, 5 }, 5 );
				m_pRoom->DeleteTile( pTile );
				SetState( State::pushed );
			}
		}
	}
	else
	{
		if( Game::m_Controller.right )
		{
			SetState( State::move );
		}
		m_Velocity.x = -m_MoveSpeed * dashSpeed;
		std::vector<Tile*> pTiles = m_pRoom->GetCollidedTiles( m_Collider.left );
		for (Tile* pTile : pTiles )
		{
			if( pTile->GetHardness() == Tile::Hardness::breakable )
			{
				m_pSoundManager->PlaySound( "break", 0 );
				m_pRoom->PlayAnim( pTile->GetShape().GetCenter(), "wl3_spritesheet_fragments.png", Index{ 1, 5 }, 5 );
				m_pRoom->DeleteTile( pTile );
				SetState( State::pushed );
			}
		}
	}

	const float epsilon{ 0.000001f };
	if( m_Clock.IsActive() && !m_IsOnGround )
	{
		m_Clock.EndTimer();
	}
	else if( !m_Clock.IsActive() && m_IsOnGround )
	{
		SetState( State::wait );
	}
	else if( ( m_Dir == Direction::left && m_pRoom->CheckCollisionWith( m_Collider.left ) )
		|| ( m_Dir == Direction::right && m_pRoom->CheckCollisionWith( m_Collider.right ) ) )
	{
		SetState( State::pushed );
	}
}

void Avatar::Pushed()
{
	m_Sprite.SetSpriteClip(Index{ 0, 5 }, 1);
	m_Velocity.x *= 0.75f;

	if (!m_Clock.IsActive())
	{
		SetState( State::wait );
	}
}

void Avatar::KnockedOut()
{
	m_Sprite.SetSpriteClip( Index{ 3, 9 }, 5 );

	m_IsKnocked = true;
	if( m_pRoom->IsSideColliding( m_Collider.bottom ) )
	{
		m_Velocity.y += m_KnockAcceleration.y * float( m_Clock.GetRemainingTime() );
	}
	if( !m_Clock.IsActive() )
	{
		SetState( State::wait );
	}
}

void Avatar::SetState( State state )
{
	m_State = state;
	switch( state )
	{
		case State::wait:
			m_Velocity.x = 0;
			m_IsKnocked = false;
			m_IsJumpEnabled = true;
			m_StateManager.SetState( std::bind( &Avatar::Waits, this ) );
			break;
		case State::move:
			m_StateManager.SetState( std::bind( &Avatar::Moves, this ) );
			break;
		case State::dash:
			m_Clock.StartTimer( 0.8 );
			m_StateManager.SetState( std::bind( &Avatar::Dashes, this ) );
			break;
		case State::pushed:
			m_pSoundManager->PlaySound( "pushed", 0 );
			m_Clock.StartTimer( m_PushDuration );
			m_Velocity.x = m_PushLength * -int( m_Dir );
			m_StateManager.SetState( std::bind( &Avatar::Pushed, this ) );
			break;
		case State::knocked:
			m_pSoundManager->PlaySound( "knocked", 0 );
			m_StateManager.SetState( std::bind( &Avatar::KnockedOut, this ) );
			break;
	}
}
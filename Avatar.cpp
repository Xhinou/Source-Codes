// Steinbach, Aymeric - 1DAE08

#include "stdafx.h"
#include "Game.h"
#include "Engine.h"
#include "Avatar.h"
#include "utils.h"
#include <functional>
#include <chrono>

int Avatar::m_EntityId{ 0 };

Avatar::Avatar(std::string fileName, Room* pRoom, const Rectf& shape)
	: Entity{ fileName, 32, 32, pRoom, shape, 360.0f }
	, m_JumpHeight{ 640.0f }
	, m_JumpAcceleration{ 0.0f, 640.0f }
	, m_KnockAcceleration{ 500.0f, 100.0f }
	, m_KnockDuration{ 1.8 }
	, m_PushLength { 500.0f }
	, m_PushDuration{ 0.2 }
	, m_IsJumpEnabled{ true }
{
	m_StateManager.SetState(std::bind(&Avatar::Wait, this));
}

Avatar::Avatar(std::string fileName, Room* pRoom, const Point2f& pos, float width, float height)
	: Avatar(fileName, pRoom, Rectf{ pos.x, pos.y, width, height })
{
}

void Avatar::Draw() const
{
	float offset{ 128.0f };
	Rectf destRect{ m_Shape.left - (offset - m_Shape.width) / 2, m_Shape.bottom, offset, offset };
	if (m_Dir == Direction::left)
	{
		Rectf mirrorShape{ -destRect.left - destRect.width, destRect.bottom, destRect.width, destRect.height };
		glPushMatrix();
		glScalef(-1.0f, 1.0f, 1.0f);
		m_Sprite.Draw(mirrorShape);
		glPopMatrix();
	}
	else
		m_Sprite.Draw(destRect);
	if (Engine::m_ShowColliders)
	{
		utils::SetColor(Color::red);
		utils::DrawCollider(m_Collider);
	}
}

void Avatar::Update(float elapsedSec)
{
	m_Clock.Update();
	m_Sprite.Update(elapsedSec);
	m_StateManager.Update();
	HandleInputs();
	HandlePhysics(elapsedSec);
}

void Avatar::HandleInputs()
{
	if (!m_IsKnocked)
	{
		if (Game::m_Controller.up && m_IsOnGround && m_IsJumpEnabled)
		{
			m_IsJumpEnabled = false;
			m_Sprite.SetSpriteClip(Index{ 4, 0 }, 1);
			m_Velocity += m_JumpAcceleration;
			m_IsOnGround = false;
		}
		else if (!Game::m_Controller.up && m_IsOnGround)
		{
			m_IsJumpEnabled = true;
		}

		if (!m_IsOnGround)
		{
			m_Sprite.SetSpriteClip(Index{ 4, 0 }, 1);
			m_IsJumpEnabled = false;
		}
	}
}

void Avatar::HandlePhysics(float elapsedSec)
{
	m_Velocity += m_Acceleration * elapsedSec;
	m_Collider.DefineFromShape(m_Shape);
	if (m_pRoom->HandleCollisions(m_Shape, m_Velocity, m_IsOnGround, m_Collider))
		m_Collider.DefineFromShape(m_Shape);
	Translate(m_Velocity * elapsedSec);
}

void Avatar::SetCurrentRoom(Room* room)
{
	m_pRoom = room;
}

void Avatar::EnemyCollision(bool isKnocked, const Vector2f& pushDir)
{
	if (isKnocked)
	{
		m_Clock.StartTimer(m_KnockDuration);
		m_Velocity = { 0, m_KnockAcceleration.y * float(m_Clock.GetRemainingTime()) };
		if (m_Dir == Direction::left)
		{
			m_Velocity.x += m_KnockAcceleration.x;
		}
		else
		{
			m_Velocity.x -= m_KnockAcceleration.x;
		}
		m_IsJumpEnabled = false;
		m_StateManager.SetState(std::bind(&Avatar::KnockedOut, this));
	}
	else if (m_IsOnGround)
	{
		m_Clock.StartTimer(m_PushDuration);
		if (m_Dir == Direction::left)
		{
			m_Velocity.x = m_PushLength;
		}
		else
		{
			m_Velocity.x = -m_PushLength;
		}
		m_StateManager.SetState(std::bind(&Avatar::Pushed, this));
	}
	else
	{
		m_Velocity.y = m_PushLength;
	}
	m_IsJumpEnabled = false;
}

bool Avatar::IsKnockedOut() const
{
	return m_IsKnocked;
}

void Avatar::Wait()
{
	m_Sprite.SetSpriteClip(Index{ 0, 0 }, 1);
	m_Velocity.x = 0;

	if (Game::m_Controller.left || Game::m_Controller.right)
	{
		m_StateManager.SetState(std::bind(&Avatar::Move, this));
	}
	else if (Game::m_Controller.a)
	{
		EnemyCollision(true);
	}
}

void Avatar::Move()
{
	m_Sprite.SetSpriteClip(Index{ 2, 0 }, 4);

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
	else
	{

	}

	if (!Game::m_Controller.IsKeyPressed())
	{
		m_Velocity.x = 0;
		m_StateManager.SetState(std::bind(&Avatar::Wait, this));
	}
}

void Avatar::Crouch()
{

}

void Avatar::Slide()
{

}

void Avatar::Dash()
{

}

void Avatar::Stomp()
{
	//jump disabled
}

void Avatar::Pushed()
{
	m_Sprite.SetSpriteClip(Index{ 5, 0 }, 1);
	m_Velocity.x *= 0.75f;

	if (!m_Clock.IsActive())
	{
		m_IsJumpEnabled = true;
		m_StateManager.SetState(std::bind(&Avatar::Wait, this));
	}
}

void Avatar::KnockedOut()
{
	m_Sprite.SetSpriteClip(Index{ 9, 3 }, 5);

	m_IsKnocked = true;
	if (m_pRoom->IsSideColliding(m_Collider.bottom))
	{
		m_Velocity.y += m_KnockAcceleration.y * float(m_Clock.GetRemainingTime());
	}

	if (!m_Clock.IsActive())
	{
		m_IsKnocked = false;
		m_IsJumpEnabled = true;
		m_StateManager.SetState(std::bind(&Avatar::Wait, this));
	}
}
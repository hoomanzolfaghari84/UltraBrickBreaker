#pragma once

#include "../../Core/Scene.hpp"
#include "../../Utils/Constants.hpp"
#include "../GameEvents.hpp"

class GamePlayScene : public Scene
{
public:
	GamePlayScene()
	{
	}

	~GamePlayScene()
	{
		Close();
	}

	// Inherited via Scene
	void Initialize() override;

	void Update(float dt) override;

	void Render(sf::RenderWindow& window) override;

	void Close() override;

	/*void HandleEvent(IEvent event) override;*/

private:

	float m_BrickSpeed = INITIAL_BRICK_SPEED;
	float m_BrickSpawnTimer = 0.f;

	float m_BallSpeed = INITIAL_BALL_SPEED;

	Entity m_Paddle = -1;
	float m_PaddleSpeed = INITIAL_PADDLE_SPEED;
	
	int m_Score = 0;

	Entity m_Ground = -1;
	
	sf::Font m_Font;
	sf::Text* m_ScoreText = nullptr;
	sf::RectangleShape* m_TopBar = nullptr;



	//void onBallHitWall(const BallHitWallEvent& event);

	void onBallHitBrick(const BallHitBrickEvent& event);

	void onBallOutOfBounds(const BallOutOfBoundsEvent& event);

	void onBallHitPaddle(const BallHitPaddleEvent& event);

};

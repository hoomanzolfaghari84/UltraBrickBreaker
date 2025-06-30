#pragma once

#include "Scene.hpp"
#include "../../Utils/Constants.hpp"

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

	//void HandleEvent(IEvent event) override;

private:

	float m_BrickSpeed = INITIAL_BRICK_SPEED;
	float m_BrickSpawnTimer = 0.f;

	float m_BallSpeed = INITIAL_BALL_SPEED;
	
	Entity m_Paddle = -1;
	
	int m_Score = 0;
	
	sf::Font m_Font;
	sf::Text* m_ScoreText = nullptr;
	sf::RectangleShape* m_TopBar = nullptr;


	void SMovement(float dt);
	void SBrickSpawner(float dt);
	void SCollisions();

};

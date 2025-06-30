#include <SFML/Graphics.hpp>
#include "memory"
#include "../Core/ECS.hpp"


class Game
{
public:



	Game();
	~Game() = default;

	void Start();
	
private:
	std::unique_ptr<EntityManager> m_EntityManager;
	void SetupGame();
	void Update(float dt);
	void Render();

	void SMovement(float dt);
	void SBrickSpawner(float dt);
	void SCollisions();
	void SInput();
	void HandleEvents();

	Entity m_Player;
	sf::RenderWindow m_Window;
	int m_Score = 0;
	sf::Font m_Font;
	float m_BrickSpawnTimer = 3.f;
	float m_BrickSpawnInterval = 3.f;

};

void SpawnBrickRow();

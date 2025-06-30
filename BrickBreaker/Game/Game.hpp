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
	void SCollisions();
	void SInput();
	void HandleEvents();

	Entity m_Player;
	sf::RenderWindow m_Window;

};
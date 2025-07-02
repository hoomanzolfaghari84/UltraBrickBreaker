#pragma once

#include <SFML/Graphics.hpp>
#include "memory"
#include "../Core/ECS.hpp"
#include "../Core/Scene.hpp"


class Game
{
public:



	Game();
	~Game() = default;

	void Start();
	
private:
	sf::RenderWindow m_Window;
	std::vector<std::unique_ptr<Scene>> m_Scenes;

	std::size_t m_CurrentSceneIndex = -1;


	void AddScene(std::unique_ptr<Scene> scene) {
		m_Scenes.push_back(std::move(scene));
	}

	void UpdateAllScenes(float dt) {
		for (auto& scene : m_Scenes) {
			if(scene->IsUpdatable())
				scene->Update(dt);
		}
	}

	void RenderAllScenes() {
		for (auto& scene : m_Scenes) {
			if(scene->IsRenderable())
				scene->Render(m_Window);
		}
	}

	void SetupGame();
	void Update(float dt);
	void Render();


	void HandleInput(const std::optional<sf::Event> event);
	void HandleEvents();

	

};


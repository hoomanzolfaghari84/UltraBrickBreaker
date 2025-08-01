#include "Game.hpp"
#include "../Utils/Constants.hpp"
#include "../Utils/Vec2.hpp"
#include "../Core/Components.hpp"
#include <iostream>
#include "GamePlayScene/GamePlayScene.hpp"

Game::Game()
{
	
	SetupGame();

}



void Game::Start()
{
    m_Window = sf::RenderWindow(sf::VideoMode({ 
        static_cast<unsigned int>(WINDOW_WIDTH),
        static_cast<unsigned int>(WINDOW_HEIGHT)
        }), "Brick Breaker Game");
    m_Window.setFramerateLimit(60);

    sf::Clock clock;
    while (m_Window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.1f);

        while (const std::optional event = m_Window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_Window.close();
            }
            else if (event->is<sf::Event::KeyPressed>() || event->is<sf::Event::KeyReleased>()) {
                HandleInput(event);
            }
        }

        Update(dt);

        m_Window.clear();
        Render();
        m_Window.display();
    }
   
}


void Game::SetupGame()
{
    auto gamePlayScene = std::make_unique<GamePlayScene>();
    gamePlayScene->Initialize();
    AddScene(std::move(gamePlayScene));
}

void Game::Update(float dt)
{
    HandleEvents();
    
    UpdateAllScenes(dt);
}

void Game::Render()
{
    RenderAllScenes();
}


void Game::HandleInput(const std::optional<sf::Event> event)
{

    if (auto keyPressedEvent = event->getIf<sf::Event::KeyPressed>()) {
        
        for (auto& scene : m_Scenes)
        {
            scene->HandleEvent<KeyPressedEvent>(KeyPressedEvent(Key::fromSFMLKey(keyPressedEvent->code)));
        }
     
    }
    else if (auto keyPressedEvent = event->getIf<sf::Event::KeyReleased>()) {
        for (auto& scene : m_Scenes)
        {
            scene->HandleEvent<KeyReleasedEvent>(KeyReleasedEvent(Key::fromSFMLKey(keyPressedEvent->code)));
        }
    }
       /* if ( m_EntityManager->hasComponent<Velocity>(m_Player)) {
            auto& vel = m_EntityManager->getComponent<Velocity>(m_Player);
            vel.velocity = { 0.f, 0.f };
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                vel.velocity.x = -PADDLE_SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                vel.velocity.x = PADDLE_SPEED;
        }*/
}

void Game::HandleEvents()
{

}

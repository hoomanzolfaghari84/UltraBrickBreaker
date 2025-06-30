#include "Game.hpp"
#include "../Utils/Constants.hpp"
#include "../Utils/Vec2.hpp"
#include "../Core/Components.hpp"
#include <iostream>

Game::Game()
{
	m_EntityManager = std::make_unique<EntityManager>();
	SetupGame();
}

void Game::Start()
{
    m_Window = sf::RenderWindow(sf::VideoMode({ static_cast<unsigned int>(WINDOW_WIDTH),
    static_cast<unsigned int>(WINDOW_HEIGHT)
        }), "Brick Breaker ECS");
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
        }

        Update(dt);

        m_Window.clear();
        Render();
        m_Window.display();
    }
   
}

void Game::SetupGame()
{

    m_EntityManager->registerComponent<Transform>();
    m_EntityManager->registerComponent<Velocity>();
    m_EntityManager->registerComponent<Renderable>();
    m_EntityManager->registerComponent<Tag>();

    // Paddle
    Entity paddle = m_EntityManager->createEntity();
    m_Player = paddle;
    m_EntityManager->addComponent<Transform>(paddle, {
        {WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 40},
        {PADDLE_WIDTH, PADDLE_HEIGHT},
        0.f
        });

    std::cout << "Added Transform Component\n";

    m_EntityManager->addComponent<Velocity>(paddle, {});
    m_EntityManager->addComponent<Renderable>(paddle, { sf::Color::White });
    m_EntityManager->addComponent<Tag>(paddle, {"paddle"});

    std::cout << "Created Paddle\n";


    // Ball
    Entity ball = m_EntityManager->createEntity();
    m_EntityManager->addComponent<Transform>(ball, {
        {WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f},
        {BALL_SIZE, BALL_SIZE},
        0.f
        });
    m_EntityManager->addComponent<Velocity>(ball, { Vec2(1.f, -1.f).normalized() * BALL_SPEED });
    m_EntityManager->addComponent<Renderable>(ball, { sf::Color::Yellow });
    m_EntityManager->addComponent<Tag>(ball, {"ball"});

    std::cout << "Created Ball\n";

    // Bricks
    float startX = 50;
    float startY = 50;
    for (int row = 0; row < BRICK_ROWS; ++row) {
        for (int col = 0; col < BRICK_COLS; ++col) {
            Entity brick = m_EntityManager->createEntity();
            m_EntityManager->addComponent<Transform>(brick, {
                {startX + col * (BRICK_WIDTH + BRICK_SPACING),
                 startY + row * (BRICK_HEIGHT + BRICK_SPACING)},
                {BRICK_WIDTH, BRICK_HEIGHT},
                0.f
                });
            m_EntityManager->addComponent<Renderable>(brick, { sf::Color::Red });
            m_EntityManager->addComponent<Tag>(brick, {"brick"});
        }
    }

    std::cout << "Created Bricks\n";
}

void Game::Update(float dt)
{
    SInput();
    SMovement(dt);
    SCollisions();
    HandleEvents();
}

void Game::Render()
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (m_EntityManager->hasComponent<Transform>(e) && m_EntityManager->hasComponent<Renderable>(e)) {
            const auto& tr = m_EntityManager->getComponent<Transform>(e);
            const auto& rend = m_EntityManager->getComponent<Renderable>(e);

            sf::RectangleShape shape;
            shape.setSize({ tr.size.x, tr.size.y });
            shape.setOrigin({ tr.size.x / 2.f, tr.size.y / 2.f });
            shape.setPosition({ tr.position.x, tr.position.y });
          
            //shape.rotate(tr.rotation);
            shape.setFillColor(rend.color);

            m_Window.draw(shape);
        }
    }
}

void Game::SMovement(float dt)
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (m_EntityManager->hasComponent<Transform>(e) && m_EntityManager->hasComponent<Velocity>(e)) {
            auto& tr = m_EntityManager->getComponent<Transform>(e);
            const auto& vel = m_EntityManager->getComponent<Velocity>(e);
            tr.position += vel.velocity * dt;
        }
    }
}

void Game::SCollisions()
{
}

void Game::SInput()
{
        if ( m_EntityManager->hasComponent<Velocity>(m_Player)) {
            auto& vel = m_EntityManager->getComponent<Velocity>(m_Player);
            vel.velocity = { 0.f, 0.f };
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                vel.velocity.x = -PADDLE_SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                vel.velocity.x = PADDLE_SPEED;
        }
}

void Game::HandleEvents()
{
}

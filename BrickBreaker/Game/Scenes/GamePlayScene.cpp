#include "GamePlayScene.hpp"
#include "../../Core/Components.hpp"
#include <iostream>


Entity SpawnBrick(EntityManager& entityManager, float speed, float startX, int col, float startY = 50, int row = 0)
{
    Entity brick = entityManager.createEntity();
    entityManager.addComponent<Transform>(brick, {
        { startX + col * (BRICK_WIDTH + BRICK_SPACING), startY + row * (BRICK_HEIGHT + BRICK_SPACING) },
        { BRICK_WIDTH, BRICK_HEIGHT },
        0.f
        });
    entityManager.addComponent<Velocity>(brick, { { 0.f, speed } });
    entityManager.addComponent<Renderable>(brick, { sf::Color::Red });
    entityManager.addComponent<Tag>(brick, { "brick" });

    return brick;
}


void GamePlayScene::Initialize()
{
    m_EntityManager->registerComponent<Transform>();
    m_EntityManager->registerComponent<Velocity>();
    m_EntityManager->registerComponent<Renderable>();
    m_EntityManager->registerComponent<Tag>();

    // Paddle
    m_Paddle = m_EntityManager->createEntity();
    
    m_EntityManager->addComponent<Transform>(m_Paddle, {
        {WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 40},
        {PADDLE_WIDTH, PADDLE_HEIGHT},
        0.f
        });

    std::cout << "Added Transform Component\n";

    m_EntityManager->addComponent<Velocity>(m_Paddle, {});
    m_EntityManager->addComponent<Renderable>(m_Paddle, { sf::Color::White });
    m_EntityManager->addComponent<Tag>(m_Paddle, { "paddle" });

    std::cout << "Created Paddle\n";


    // Ball
    Entity ball = m_EntityManager->createEntity();
    m_EntityManager->addComponent<Transform>(ball, {
        {WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f},
        {BALL_SIZE, BALL_SIZE},
        0.f
        });
    m_EntityManager->addComponent<Velocity>(ball, { Vec2(1.f, -1.f).normalized() * m_BallSpeed });
    m_EntityManager->addComponent<Renderable>(ball, { sf::Color::Yellow });
    m_EntityManager->addComponent<Tag>(ball, { "ball" });

    std::cout << "Created Ball\n";

    // Bricks
    float startX = 50;
    float startY = 50;
    for (int row = 0; row < BRICK_ROWS; ++row) {
        for (int col = 0; col < BRICK_COLS; ++col) {
            SpawnBrick(*m_EntityManager, m_BrickSpeed, startX, col, startY, row);
        }
    }

    m_TopBar = new sf::RectangleShape();
    m_TopBar->setSize({ WINDOW_WIDTH, 50.f });
    m_TopBar->setPosition({ 0.f, 0.f });
    m_TopBar->setFillColor(sf::Color::Black);

    if (!m_Font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font!\n";
    }

    m_ScoreText = new sf::Text(m_Font);
    m_ScoreText->setCharacterSize(24);
    m_ScoreText->setFillColor(sf::Color::White);
    m_ScoreText->setPosition({ 50.f, 20.f });
    m_Score = 0;

    m_ScoreText->setString("Score: " + std::to_string(m_Score));
}



void GamePlayScene::Update(float dt)
{
    SMovement(dt);
    SBrickSpawner(dt);
    SCollisions();
   
}

sf::RectangleShape DrawRectangle(const Transform& tr, const Renderable& rend) {
    sf::RectangleShape shape;
    shape.setSize({ tr.size.x, tr.size.y });
    shape.setOrigin({ tr.size.x / 2.f, tr.size.y / 2.f });
    shape.setRotation(sf::degrees(tr.rotation));
    shape.setPosition({ tr.position.x, tr.position.y });
    shape.rotate(sf::degrees(tr.rotation));
    shape.setFillColor(rend.color);

    return shape;
}

void GamePlayScene::Render(sf::RenderWindow& window)
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (m_EntityManager->hasComponent<Transform>(e) && m_EntityManager->hasComponent<Renderable>(e)) {
            const auto& tr = m_EntityManager->getComponent<Transform>(e);
            const auto& rend = m_EntityManager->getComponent<Renderable>(e);
            if (e == m_Paddle) {
                
                auto shape = DrawRectangle(tr, rend);
                window.draw(shape);

            }
            else if (m_EntityManager->hasComponent<Tag>(e)) {
                const auto& tag = m_EntityManager->getComponent<Tag>(e);
                if (tag.name == "brick") {
                    
                    auto shape = DrawRectangle(tr, rend);
                    window.draw(shape);

                }
                else if (tag.name == "ball") {
                    sf::CircleShape shape;
                    shape.setRadius(tr.size.x);
                    shape.setOrigin({ tr.size.x / 2.f, tr.size.y / 2.f });
                    shape.setPosition({ tr.position.x, tr.position.y });
                    shape.setFillColor(rend.color);
                    window.draw(shape);
                }
            }
        }



    }
    
    window.draw(*m_TopBar);
    window.draw(*m_ScoreText);

    /*sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(0.f, 50.f), sf::Color::White),
        sf::Vertex(sf::Vector2f(WINDOW_WIDTH, 50.f), sf::Color::White)
    };

    window.draw(line, 2, sf::PrimitiveType::Lines);*/
}

void GamePlayScene::Close()
{
}

/*void GamePlayScene::HandleEvent(IEvent event)
{
}*/



void GamePlayScene::SMovement(float dt)
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (m_EntityManager->hasComponent<Transform>(e) && m_EntityManager->hasComponent<Velocity>(e)) {
            auto& tr = m_EntityManager->getComponent<Transform>(e);
            const auto& vel = m_EntityManager->getComponent<Velocity>(e);
            tr.position += vel.velocity * dt;
        }
    }
}

bool IsColliding(const Transform& a, const Transform& b) {
    return (
        std::abs(a.position.x - b.position.x) < (a.size.x + b.size.x) / 2.f &&
        std::abs(a.position.y - b.position.y) < (a.size.y + b.size.y) / 2.f
        );
}


void GamePlayScene::SBrickSpawner(float dt) {

    m_BrickSpawnTimer += dt;
    /*std::cout << "Brick Spawn Timer " << m_BrickSpawnTimer << std::endl;
    std::cout << "Brick Spawn Interval " << (BRICK_HEIGHT + BRICK_SPACING / 2) / m_BrickSpeed << std::endl;*/
    if (m_BrickSpawnTimer >= (BRICK_HEIGHT + BRICK_SPACING / 2) / m_BrickSpeed ) {
        m_BrickSpawnTimer = 0.f;

        float startX = 50.f;

        for (int col = 0; col < BRICK_COLS; ++col) {
            if (std::rand() % 3 == 0) continue; // skip some for pattern

            SpawnBrick(*m_EntityManager, m_BrickSpeed, startX, col);

        }

        std::cout << "Spawned new brick row.\n";

    }
}

void GamePlayScene::SCollisions()
{

    Entity ball = -1;
    Transform* ballTr = nullptr;
    Velocity* ballVel = nullptr;

    // Find ball entity
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!m_EntityManager->isValid(e)) continue;
        if (!m_EntityManager->hasComponent<Tag>(e)) continue;

        auto& tag = m_EntityManager->getComponent<Tag>(e);
        if (tag.name == "ball") {
            ball = e;
            ballTr = &m_EntityManager->getComponent<Transform>(e);
            ballVel = &m_EntityManager->getComponent<Velocity>(e);
            break;
        }
    }

    if (!ballTr || !ballVel) return;

    // 1. Wall and Ceiling Collisions
    if (ballTr->position.x - ballTr->size.x / 2.f < 0.f || ballTr->position.x + ballTr->size.x / 2.f > WINDOW_WIDTH)
        ballVel->velocity.x *= -1.f;
    if (ballTr->position.y - ballTr->size.y / 2.f < 0.f)
        ballVel->velocity.y *= -1.f;

    // 2. Ball Out of Bounds (bottom)
    if (ballTr->position.y - ballTr->size.y / 2.f > WINDOW_HEIGHT) {
        // Reset ball
        ballTr->position = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
        ballVel->velocity = Vec2(1.f, -1.f).normalized() * m_BallSpeed;
        return;
    }

    // 3. Paddle Collision
    auto& paddleTr = m_EntityManager->getComponent<Transform>(m_Paddle);

    // Clamp paddle inside
    paddleTr.position.x = std::clamp(paddleTr.position.x, paddleTr.size.x / 2.f, WINDOW_WIDTH - paddleTr.size.x / 2.f);

    if (IsColliding(*ballTr, paddleTr)) {
        float hitX = (ballTr->position.x - paddleTr.position.x) / (paddleTr.size.x / 2.f);
        float angle = hitX * 75.f; // max 75 deg deviation
        ballVel->velocity = Vec2(std::sin(angle * DEG2RAD), -std::cos(angle * DEG2RAD)) * m_BallSpeed;

        // Push ball up slightly to prevent re-collision
        ballTr->position.y = paddleTr.position.y - paddleTr.size.y / 2.f - ballTr->size.y / 2.f - 1.f;
    }

    // 4. Brick Collisions
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (!m_EntityManager->isValid(e)) continue;
        if (!m_EntityManager->hasComponent<Tag>(e)) continue;

        auto& tag = m_EntityManager->getComponent<Tag>(e);
        if (tag.name != "brick") continue;

        auto& brickTr = m_EntityManager->getComponent<Transform>(e);
        if (IsColliding(*ballTr, brickTr)) {
            // Invert Y velocity for simplicity
            ballVel->velocity.y *= -1.f;
            m_EntityManager->destroyEntity(e); // remove brick
            m_Score += 100;

            //break; // one collision per frame
        }
    }

    // 5. Normalize velocity to prevent buildup
    ballVel->velocity = ballVel->velocity.normalized() * m_BallSpeed;
}
#include "GamePlayScene.hpp"
#include "../../Core/Components.hpp"
#include "../../Core/Phyisics.hpp"
#include <iostream>
#include "../GameEvents.hpp"


//Entity SpawnBrick(EntityManager& entityManager, float speed, float startX, int col, float startY = 50, int row = 0)
//{
//    Entity brick = entityManager.createEntity();
//    entityManager.addComponent<Transform>(brick, {
//        { startX + col * (BRICK_WIDTH + BRICK_SPACING), startY + row * (BRICK_HEIGHT + BRICK_SPACING) },
//        { BRICK_WIDTH, BRICK_HEIGHT },
//        0.f
//        });
//    //entityManager.addComponent<Kinematics>(brick, { { 0.f, speed } });
//    entityManager.addComponent<Renderable>(brick, { sf::Color::Red });
//    entityManager.addComponent<Tag>(brick, { "brick" });
//
//    return brick;
//}


void GamePlayScene::Initialize()
{
    m_EntityManager->registerComponent<Transform>();
    m_EntityManager->registerComponent<Kinematics>();
    m_EntityManager->registerComponent<RenderCircle>();
    m_EntityManager->registerComponent<RenderRectangle>();
    m_EntityManager->registerComponent<Tag>();
    m_EntityManager->registerComponent<Collider>();

    // Paddle
    m_Paddle = m_EntityManager->createEntity();
    auto k = Kinematics();
    k.position = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - (PADDLE_HEIGHT * 1.5f) };
    k.SetMass(100);
    k.isStatic = false;

    m_EntityManager->addComponent<Kinematics>(m_Paddle, k);
    auto r = RenderRectangle();
    r.position = k.position;
    r.size = { PADDLE_WIDTH, PADDLE_HEIGHT };
    r.color = Color(sf::Color::White.r, sf::Color::White.g, sf::Color::White.b, sf::Color::White.a);
    m_EntityManager->addComponent<RenderRectangle>(m_Paddle, r);

    auto col = Collider();
    col.type = ColliderShapeType::Rectangle;
    col.position = k.position;
    auto cs = RectangleCollisionShape();
    cs.halfsize = r.size * 0.5f;
    col.shape = cs;
    col.isTrigger = false;
    m_EntityManager->addComponent<Collider>(m_Paddle, col);

    m_EntityManager->addComponent<Tag>(m_Paddle, { "paddle" });

    std::cout << "Created Paddle\n";


    // Ball
    Entity ball = m_EntityManager->createEntity();
    auto kc = Kinematics();
    kc.position = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
    kc.velocity = { 0.f, INITIAL_BALL_SPEED};

    m_EntityManager->addComponent<Kinematics>(ball, kc);
    RenderCircle rc = RenderCircle();
    rc.position = kc.position;
    rc.radius = 10.f;
    rc.color = Color(sf::Color::Green.r, sf::Color::Green.g, sf::Color::Green.b, sf::Color::Green.a);
    m_EntityManager->addComponent<RenderCircle>(ball, rc);

    auto colc = Collider();
    colc.type = ColliderShapeType::Circle;
    colc.position = kc.position;
    auto csc = CircleCollisionShape();
    csc.radius = rc.radius;
    colc.shape = csc;
    colc.isTrigger = false;
    m_EntityManager->addComponent<Collider>(ball, colc);

    m_EntityManager->addComponent<Tag>(ball, { "ball" });

    std::cout << "Created Ball\n";

    // Ground

    Entity ground = m_EntityManager->createEntity();
    m_Ground = ground;
    auto kg = Kinematics();
    kg.position = { WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT - (PADDLE_HEIGHT * 0.5f) + EPSILON };
    kg.isStatic = true;
    kg.SetMass(1000);
    m_EntityManager->addComponent<Kinematics>(ground, kg);

    RenderRectangle rg = RenderRectangle();
    rg.position = kg.position;
    rg.size = {WINDOW_WIDTH, PADDLE_HEIGHT};
    rg.color = Color(sf::Color::Magenta.r, sf::Color::Magenta.g, sf::Color::Magenta.b, sf::Color::Magenta.a);
    m_EntityManager->addComponent<RenderRectangle>(ground, rg);

    auto colg = Collider();
    colg.type = ColliderShapeType::Rectangle;
    colg.position = kg.position;
    auto csg = RectangleCollisionShape();
    csg.halfsize = rg.size * 0.5f;
    colg.shape = csg;
    colg.isTrigger = false;
    m_EntityManager->addComponent<Collider>(ground, colg);

    m_EntityManager->addComponent<Tag>(ground, { "ground" });

    std::cout << "Created Ball\n";

    //// Bricks
    //float startX = 50;
    //float startY = 50;
    //for (int row = 0; row < BRICK_ROWS; ++row) {
    //    for (int col = 0; col < BRICK_COLS; ++col) {
    //        SpawnBrick(*m_EntityManager, m_BrickSpeed, startX, col, startY, row);
    //    }
    //}


    // Top Bar
    m_TopBar = new sf::RectangleShape();
    m_TopBar->setSize({ WINDOW_WIDTH, 50.f });
    m_TopBar->setPosition({ 0.f, 0.f });
    m_TopBar->setFillColor(sf::Color::Black);

    if (!m_Font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font!\n";
    }

    // Score Text
    m_ScoreText = new sf::Text(m_Font);
    m_ScoreText->setCharacterSize(24);
    m_ScoreText->setFillColor(sf::Color::White);
    m_ScoreText->setPosition({ 50.f, 20.f });
    m_Score = 0;

    m_ScoreText->setString("Score: " + std::to_string(m_Score));

    // Event Handlers
    m_EventBus->subscribe<ScoreChangedEvent>([this](const ScoreChangedEvent& e) {
        m_Score += e.delta;
        this->m_ScoreText->setString("Score: " + std::to_string(this->m_Score));
        }
    );

    m_EventBus->subscribe<KeyPressedEvent>([this](const KeyPressedEvent& e) {
        if (e.GetKeyCode() == Key::KeyCode::Left)
        {
            this->HandleEvent<PaddleLeft>(PaddleLeft());
        }
        else if (e.GetKeyCode() == Key::KeyCode::Right) {
             this->HandleEvent<PaddleRight>(PaddleRight());
        }
        }
    );

    m_EventBus->subscribe<KeyReleasedEvent>([this](const KeyReleasedEvent& e) {
        if (e.GetKeyCode() == Key::KeyCode::Left || e.GetKeyCode() == Key::KeyCode::Right)
        {
            this->HandleEvent<PaddleStop>(PaddleStop());
        }
        }
    );

    m_EventBus->subscribe<PaddleLeft>([this](const PaddleLeft& e) {
        this->m_EntityManager->getComponent<Kinematics>(m_Paddle).velocity.x = -m_PaddleSpeed;
        }
    );

    m_EventBus->subscribe<PaddleRight>([this](const PaddleRight& e) {
        this->m_EntityManager->getComponent<Kinematics>(m_Paddle).velocity.x = m_PaddleSpeed;
        }
    );
    m_EventBus->subscribe<PaddleStop>([this](const PaddleStop& e) {
        this->m_EntityManager->getComponent<Kinematics>(m_Paddle).velocity.x = 0.f;
        }
    );

    /*m_EventBus->subscribe<CollisionEvent>([this](const CollisionEvent& e) {
        if (e.entityA == this->m_Ground) {

        }
        else if (e.entityB == this->m_Ground) {

        }

        }
    );*/

    //Systems
    auto collisionSystem = std::make_unique<CollisionSystem>(*m_EventBus);
    m_Systems.push_back(std::move(collisionSystem));

    auto physicsSystem = std::make_unique<PhysicsSystem>(*m_EventBus);
    m_Systems.push_back(std::move(physicsSystem));

}


void GamePlayScene::Update(float dt)
{
    //SMovement(dt);
    //SBrickSpawner(dt);
    //SCollisions();
    for (auto& sys : m_Systems) {
        sys->update(*m_EntityManager, dt);
    }
   
}

void DrawRectangle(const RenderRectangle& rend, sf::RectangleShape& shape) {
    
    shape.setSize({ rend.size.x, rend.size.y });
    shape.setOrigin({ rend.size.x / 2.f, rend.size.y / 2.f });
    shape.setRotation(sf::degrees(rend.rotation));
    shape.setPosition({ rend.position.x, rend.position.y });
    shape.setFillColor(sf::Color(rend.color.r, rend.color.g, rend.color.b, rend.color.a));
}

void GamePlayScene::Render(sf::RenderWindow& window)
{
    for (Entity e = 0; e < MAX_ENTITIES; ++e) {
        if (m_EntityManager->hasComponent<RenderRectangle>(e)) {
            sf::RectangleShape shape;
            const RenderRectangle& c = m_EntityManager->getComponent<RenderRectangle>(e);
            DrawRectangle(c, shape);
            window.draw(shape);
            //std::cout << "Drew Rectangle\n";
        }
        else if (m_EntityManager->hasComponent<RenderCircle>(e)) {
            auto& tr = m_EntityManager->getComponent<RenderCircle>(e);
            sf::CircleShape shape;
            shape.setRadius(tr.radius);
            shape.setOrigin({ tr.radius / 2.f, tr.radius / 2.f });
            shape.setPosition({ tr.position.x, tr.position.y });
            shape.setFillColor(sf::Color(tr.color.r, tr.color.g, tr.color.b, tr.color.a));
            window.draw(shape);
            //std::cout << "Drew Circle\n";
        }


    }
    
    window.draw(*m_TopBar);
    //std::cout << "Drew TopBar\n";
    window.draw(*m_ScoreText);
    //std::cout << "Drew Score Text\n";

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

//void GamePlayScene::HandleEvent(IEvent event)
//{
//}





//void GamePlayScene::SBrickSpawner(float dt) {
//
//    m_BrickSpawnTimer += dt;
//    /*std::cout << "Brick Spawn Timer " << m_BrickSpawnTimer << std::endl;
//    std::cout << "Brick Spawn Interval " << (BRICK_HEIGHT + BRICK_SPACING / 2) / m_BrickSpeed << std::endl;*/
//    if (m_BrickSpawnTimer >= (BRICK_HEIGHT + BRICK_SPACING / 2) / m_BrickSpeed ) {
//        m_BrickSpawnTimer = 0.f;
//
//        float startX = 50.f;
//
//        for (int col = 0; col < BRICK_COLS; ++col) {
//            if (std::rand() % 3 == 0) continue; // skip some for pattern
//
//            SpawnBrick(*m_EntityManager, m_BrickSpeed, startX, col);
//
//        }
//
//        std::cout << "Spawned new brick row.\n";
//
//    }
//}

//void GamePlayScene::onBallHitWall(const BallHitWallEvent& event) {
//    auto& vel = m_EntityManager->getComponent<Velocity>(event.ball);
//    if (event.side == BallHitWallEvent::Side::LeftRight)
//        vel.velocity.x *= -1.f;
//    else
//        vel.velocity.y *= -1.f;
//}

void GamePlayScene::onBallHitBrick(const BallHitBrickEvent& event) {
   /* em.destroyEntity(event.brick);
    score += 100;*/

    // Maybe play sound or particles
}

void GamePlayScene::onBallOutOfBounds(const BallOutOfBoundsEvent& event) {
    /*auto& tr = em.getComponent<Transform>(event.ball);
    auto& vel = em.getComponent<Velocity>(event.ball);
    tr.position = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
    vel.velocity = Vec2(1.f, -1.f).normalized() * speed;*/
}

void GamePlayScene::onBallHitPaddle(const BallHitPaddleEvent& event) {
   /* auto& ballTr = em.getComponent<Transform>(event.ball);
    auto& ballVel = em.getComponent<Velocity>(event.ball);
    auto& paddleTr = em.getComponent<Transform>(event.paddle);

    float hitX = (ballTr.position.x - paddleTr.position.x) / (paddleTr.size.x / 2.f);
    float angle = hitX * 75.f;
    ballVel.velocity = Vec2(std::sin(angle * DEG2RAD), -std::cos(angle * DEG2RAD)) * speed;

    ballTr.position.y = paddleTr.position.y - paddleTr.size.y / 2.f - ballTr.size.y / 2.f - 1.f;*/
}




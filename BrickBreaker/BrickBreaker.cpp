// BrickBreaker.cpp : Defines the entry point for the application.
//

#include "BrickBreaker.h"
#include <SFML/Graphics.hpp>
#include "Core/ECS.hpp"
#include "Core/Components.hpp"
#include "Core/System.hpp"
#include "Core/EventBus.hpp"
#include "Events.hpp"



class RenderSystem {
public:
    void render(EntityManager& em, sf::RenderWindow& window) {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (em.hasComponent<Transform>(e) && em.hasComponent<Renderable>(e)) {
                auto& tr = em.getComponent<Transform>(e);
                auto& rend = em.getComponent<Renderable>(e);

                sf::RectangleShape shape;
                shape.setSize({ tr.size.x, tr.size.y });
                shape.setOrigin({ tr.size.x / 2.f, tr.size.y / 2.f });
                shape.setPosition({tr.position.x, tr.position.y});
                //shape.setRotation( tr.rotation );
                shape.setFillColor(rend.color);

                window.draw(shape);
            }
        }
    }
};

class OutOfBoundsSystem {
public:
    OutOfBoundsSystem(float screenWidth, float screenHeight, EventBus& bus)
        : width(screenWidth), height(screenHeight), eventBus(bus) {
    }

    void update(EntityManager& em) {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (em.hasComponent<Transform>(e)) {
                const auto& tr = em.getComponent<Transform>(e);
                if (tr.position.x > width) {
                    BoxOutOfBoundsEvent ev(e, tr.position);
                    eventBus.emit(ev);
                }
            }
        }
    }

private:
    float width, height;
    EventBus& eventBus;
};


int main()
{

    auto window = sf::RenderWindow(sf::VideoMode({ 600u, 800u }), "CMake SFML Project");
    window.setFramerateLimit(144);

    EventBus& eventBus = *new EventBus();

    EntityManager& em = *new EntityManager();
    em.registerComponent<Transform>();
    em.registerComponent<Velocity>();
    em.registerComponent<Renderable>();

    // Create moving box
    Entity box = em.createEntity();
    em.addComponent<Transform>(box, { {0.f, 300.f}, {50.f, 50.f}, 0.f });
    em.addComponent<Velocity>(box, { {120.f, 0.f} });  // Moving right
    em.addComponent<Renderable>(box, { sf::Color::Green });

    // Setup systems
    MovementSystem movement;
    OutOfBoundsSystem boundsCheck(800.f, 600.f, eventBus);
    RenderSystem renderer;

    // Subscribe to out-of-bounds events
    eventBus.subscribe<BoxOutOfBoundsEvent>([&](const BoxOutOfBoundsEvent& evt) {
        std::cout << "Entity " << evt.entity << " out of bounds at " << evt.position.x << ", wrapping.\n";
        if (em.hasComponent<Transform>(evt.entity)) {
            auto& tr = em.getComponent<Transform>(evt.entity);
            tr.position.x = -tr.size.x / 2.f;  // Wrap to left side
        }
        });

    sf::Clock clock;


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {

                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        movement.update(em, dt);
        boundsCheck.update(em);

        window.clear();
        renderer.render(em, window);
        window.display();

    }


    delete& em;
    delete& eventBus;
    
}

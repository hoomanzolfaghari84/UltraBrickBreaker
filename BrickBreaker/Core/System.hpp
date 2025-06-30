#include "ECS.hpp"
#include "Components.hpp"

class System {
public:
    virtual void update(EntityManager& em, float dt) = 0;
    virtual ~System() = default;
};

class MovementSystem : public System {
public:
    void update(EntityManager& em, float dt) override {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (em.hasComponent<Transform>(e) && em.hasComponent<Velocity>(e)) {
                auto& tr = em.getComponent<Transform>(e);
                const auto& vel = em.getComponent<Velocity>(e);
                tr.position += vel.velocity * dt;
            }
        }
    }
};

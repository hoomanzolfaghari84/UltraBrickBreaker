#pragma once

#include "../../Core/System.hpp"
#include "../../Core/EventBus.hpp"


class BrickSpawnSystem : public System {
public:
	BrickSpawnSystem(EventBus& eventBus) : m_EventBus(eventBus) {

	}

	void update(EntityManager& em, float dt) override {
		
	}

private:
	float m_BrickSpawnTimer = 0.f;
	EventBus& m_EventBus;

};
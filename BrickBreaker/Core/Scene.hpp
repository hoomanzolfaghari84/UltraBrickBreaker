#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "ECS.hpp"
#include "EventBus.hpp"
#include "System.hpp"


class Scene
{
public:
	virtual ~Scene() = default;

	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(sf::RenderWindow& window) = 0;
	virtual void Close() = 0;

	template<typename EventType>
	void HandleEvent(const EventType& event) const {
		m_EventBus->emit<EventType>(event);
	}

	void SetUpdatable(bool value) {
		m_Updatable = value;
	}

	const bool IsUpdatable() const{
		return m_Updatable;
	}

	void SetRenderable(bool value) {
		m_Renderable = value;
	}

	const bool IsRenderable() const {
		return m_Renderable;
	}

protected:
	std::unique_ptr<EntityManager> m_EntityManager = std::make_unique<EntityManager>();
	std::unique_ptr<EventBus> m_EventBus = std::make_unique<EventBus>();
	//std::vector<std::unique_ptr<Scene>> m_Scenes;
	std::vector<std::unique_ptr<System>> m_Systems;

	bool m_Updatable = true;
	bool m_Renderable = true;

};

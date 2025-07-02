#pragma once

#include <optional>

#include "ECS.hpp"
#include "../Utils/Vec2.hpp"
#include "../Utils/KeyCodes.hpp"


struct CollisionEvent {
    Entity entityA;
    Entity entityB;

    Vec2 contactPoint;       // Approximate point of collision
    Vec2 contactNormal;      // Normal vector (unit vector perpendicular to surface)
    Vec2 penetrationVector;  // MTV: How to resolve the collision
    float penetrationDepth;          // Scalar length of overlap

    bool isTrigger; // If either collider is a trigger, no resolution
};

class KeyEvent : public IEvent
{
public:
	Key::KeyCode GetKeyCode() const { return m_KeyCode; }

protected:
	KeyEvent(const Key::KeyCode keycode)
		: m_KeyCode(keycode) {
	}

	Key::KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const Key::KeyCode keycode, bool isRepeat = false)
		: KeyEvent(keycode), m_IsRepeat(isRepeat) {
	}

	bool IsRepeat() const { return m_IsRepeat; }

	

private:
	bool m_IsRepeat;
};

class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(const Key::KeyCode keycode)
		: KeyEvent(keycode) {
	}

};

//class KeyTypedEvent : public KeyEvent
//{
//public:
//	KeyTypedEvent(const Key::KeyCode keycode)
//		: KeyEvent(keycode) {
//	}
//
//	std::string ToString() const //override
//	{
//		std::stringstream ss;
//		ss << "KeyTypedEvent: " << m_KeyCode;
//		return ss.str();
//	}
//
//	EVENT_CLASS_TYPE(KeyTyped)
//};

//  KeyPressed, KeyReleased, KeyTyped,
//  MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
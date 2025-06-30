#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <cassert>

// === Base Event Interface ===
class IEvent {
public:
    virtual ~IEvent() = default;
};

// === Type-Erased Listener Base ===
class IListenerList {
public:
    virtual ~IListenerList() = default;
};

template<typename EventType>
class ListenerList : public IListenerList {
public:
    void add(std::function<void(const EventType&)> listener) {
        listeners.push_back(std::move(listener));
    }

    void dispatch(const EventType& event) const {
        for (const auto& listener : listeners)
            listener(event);
    }

private:
    std::vector<std::function<void(const EventType&)>> listeners;
};

//// === EventBus Class ===
//class EventBus {
//public:
//    template<typename EventType>
//    void subscribe(std::function<void(const EventType&)> listener) {
//        auto& list = getOrCreate<EventType>();
//        list.add(std::move(listener));
//    }
//
//    template<typename EventType>
//    void emit(const EventType& event) const {
//        auto it = listeners.find(std::type_index(typeid(EventType)));
//        if (it != listeners.end()) {
//            auto* base = it->second.get();
//            auto* list = static_cast<ListenerList<EventType>*>(base);
//            list->dispatch(event);
//        }
//    }
//
//private:
//    std::unordered_map<std::type_index, std::unique_ptr<IListenerList>> listeners;
//
//    template<typename EventType>
//    ListenerList<EventType>& getOrCreate() {
//        auto type = std::type_index(typeid(EventType));
//        auto it = listeners.find(type);
//        if (it == listeners.end()) {
//            auto list = std::make_unique<ListenerList<EventType>>();
//            auto* raw = list.get();
//            listeners[type] = std::move(list);
//            return *raw;
//        }
//        return *static_cast<ListenerList<EventType>*>(listeners[type].get());
//    }
//};

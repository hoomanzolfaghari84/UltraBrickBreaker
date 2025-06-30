#pragma once

#include <array>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <typeindex>
#include <cassert>
#include <cstdint>

// Constants
constexpr std::size_t MAX_ENTITIES = 5000;
constexpr std::size_t MAX_COMPONENTS = 64;

// Type aliases
using Entity = std::uint32_t;
using ComponentType = std::uint8_t;
using Signature = std::bitset<MAX_COMPONENTS>;

// Forward declaration
class EntityManager;

// Base ComponentStorage interface
struct IComponentStorage {
    virtual ~IComponentStorage() = default;
    virtual void remove(Entity entity) = 0;
    virtual bool has(Entity entity) const = 0;
};

// Templated component storage
template<typename T>
class ComponentStorage : public IComponentStorage {
public:
    void insert(Entity entity, T component) {
        assert(entityToIndex.find(entity) == entityToIndex.end());
        size_t index = components.size();
        entityToIndex[entity] = index;
        indexToEntity[index] = entity;
        components.emplace_back(component);
    }

    /*
    Using vector.erase() would be slow for large vectors (linear time) because it shifts all elements after the erased one.
    This implementation avoids that by doing a swap-and-pop in constant time (O(1)).
    */
    void remove(Entity entity) override {
        auto it = entityToIndex.find(entity);
        if (it != entityToIndex.end()) {
            size_t index = it->second;
            Entity lastEntity = indexToEntity[components.size() - 1];
            components[index] = components.back();
            entityToIndex[lastEntity] = index;
            indexToEntity[index] = lastEntity;

            components.pop_back();
            entityToIndex.erase(it);
            indexToEntity.erase(components.size());
        }
    }

    bool has(Entity entity) const override {
        return entityToIndex.find(entity) != entityToIndex.end();
    }

    T& get(Entity entity) {
        assert(has(entity));
        return components[entityToIndex[entity]];
    }

private:
    std::vector<T> components;
    std::unordered_map<Entity, size_t> entityToIndex;
    std::unordered_map<size_t, Entity> indexToEntity;
};

// Entity Manager
class EntityManager {
public:
    EntityManager() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            availableEntities.push(entity);
        }
    }

    Entity createEntity() {
        assert(livingEntityCount < MAX_ENTITIES);
        Entity id = availableEntities.front();
        availableEntities.pop();
        ++livingEntityCount;
        return id;
    }

    void destroyEntity(Entity entity) {
        assert(entity < MAX_ENTITIES);
        signatures[entity].reset();
        for (auto& [typeIdx, storage] : componentStores) {
            storage->remove(entity);
        }
        availableEntities.push(entity);
        --livingEntityCount;
    }

    template<typename T>
    void registerComponent() {
        const std::type_index typeIdx = std::type_index(typeid(T));
        assert(componentTypes.find(typeIdx) == componentTypes.end());
        componentTypes[typeIdx] = nextComponentType++;
        componentStores[typeIdx] = std::make_unique<ComponentStorage<T>>();
    }

    template<typename T>
    void addComponent(Entity entity, T component) {
        getComponentStorage<T>()->insert(entity, component);
        signatures[entity].set(getComponentType<T>());
    }

    template<typename T>
    void removeComponent(Entity entity) {
        getComponentStorage<T>()->remove(entity);
        signatures[entity].reset(getComponentType<T>());
    }

    template<typename T>
    bool hasComponent(Entity entity) const {
        return getComponentStorage<T>()->has(entity);
    }

    template<typename T>
    T& getComponent(Entity entity) {
        return getComponentStorage<T>()->get(entity);
    }

    Signature getSignature(Entity entity) const {
        return signatures[entity];
    }

    void clear() {
        for (auto& [_, storage] : componentStores) {
            storage.reset();
        }
        componentStores.clear();
        componentTypes.clear();
        signatures.fill(Signature{});
        livingEntityCount = 0;
        std::queue<Entity> empty;
        std::swap(availableEntities, empty);
        for (Entity e = 0; e < MAX_ENTITIES; ++e)
            availableEntities.push(e);
    }

private:
    std::queue<Entity> availableEntities;
    std::array<Signature, MAX_ENTITIES> signatures{};
    std::unordered_map<std::type_index, ComponentType> componentTypes;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> componentStores;
    ComponentType nextComponentType = 0;
    std::uint32_t livingEntityCount = 0;

    template<typename T>
    ComponentType getComponentType() const {
        auto it = componentTypes.find(std::type_index(typeid(T)));
        assert(it != componentTypes.end());
        return it->second;
    }

    template<typename T>
    ComponentStorage<T>* getComponentStorage() const {
        auto it = componentStores.find(std::type_index(typeid(T)));
        assert(it != componentStores.end());
        return static_cast<ComponentStorage<T>*>(it->second.get());
    }
};

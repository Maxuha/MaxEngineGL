//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_GAMEOBJECT_H
#define MAXENGINE_GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include <concepts>
#include <algorithm>
#include "../components/Transform.h"

class Component;
class Transform;

class GameObject {
public:
    explicit GameObject(const std::string &name = "GameObject");
    virtual ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    [[nodiscard]] Transform* GetTransform() { return transform; }
    [[nodiscard]] const Transform* GetTransform() const { return transform; }

    [[nodiscard]] const std::string& GetName() const { return name; }
    void SetName(std::string newName) { name = std::move(newName); }

    virtual void Start();
    virtual void Update(float deltaTime);


    template<typename T, typename... Args>
        requires std::derived_from<T, Component>
    T* AddComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = component.get();

        rawPtr->Attach(*this);

        _components.push_back(std::move(component));
        return rawPtr;
    }

    template<typename T>
        requires std::derived_from<T, Component>
    void RemoveComponent() {
        auto it = std::remove_if(_components.begin(), _components.end(), [](const auto& comp) {
            return dynamic_cast<T*>(comp.get()) != nullptr;
        });
        _components.erase(it, _components.end());
    }

    template<typename T>
        requires std::derived_from<T, Component>
    [[nodiscard]] T* GetComponent() {
        for (const auto& comp : _components) {
            if (auto* target = dynamic_cast<T*>(comp.get())) {
                return target;
            }
        }
        return nullptr;
    }

    template<typename T>
        requires std::derived_from<T, Component>
    [[nodiscard]] const T* GetComponent() const {
        for (const auto& comp : _components) {
            if (const auto* target = dynamic_cast<const T*>(comp.get())) {
                return target;
            }
        }
        return nullptr;
    }

    template<typename T>
        requires std::derived_from<T, Component>
    [[nodiscard]] std::vector<T*> GetComponentsInChildren() {
        std::vector<T*> foundComponents;
        GetComponentsInChildrenInternal<T>(foundComponents);
        return foundComponents;
    }

private:
    template<typename T>
    void GetComponentsInChildrenInternal(std::vector<T*>& outComponents) {
        if (auto* currentComponent = GetComponent<T>()) {
            outComponents.push_back(currentComponent);
        }

        for (const auto* child : transform->children) {
            if (!child) continue;

            if (GameObject* childGO = child->GetGameObject()) {
                childGO->GetComponentsInChildrenInternal<T>(outComponents);
            }
        }
    }

    std::string name;

    std::vector<std::unique_ptr<Component>> _components;

    Transform* transform = nullptr;
};

#endif //MAXENGINE_GAMEOBJECT_H
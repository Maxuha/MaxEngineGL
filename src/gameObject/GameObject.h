//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_GAMEOBJECT_H
#define MAXENGINE_GAMEOBJECT_H
#include <map>
#include <string>
#include <typeindex>
#include "../components/Component.h"
#include "../components/Transform.h"

class GameObject {
public:
    GameObject();

    virtual ~GameObject() = default;

    Transform* GetTransform() const;

    virtual void Start();

    virtual void Update(float delta_time);

    template<typename T>
    requires std::derived_from<T, Component>
    T* AddComponent() {
        auto component = new T();
        component->Attach(*this);
        components.insert(std::make_pair(std::type_index(typeid(T)), component));
        _components.push_back(component);
        return component;
    }

    template<typename T>
    requires std::derived_from<T, Component>
    void RemoveComponent() {
        auto typeId = std::type_index(typeid(T));
        auto component = components.find(typeId);
        if (component == components.end()) return;
        delete component->second;
        components.erase(typeId);
    }

    template<typename T>
    requires std::derived_from<T, Component>
    T* GetComponent() {
        // auto typeId = std::type_index(typeid(T));
        // auto component = components.find(typeId);
        // if (component == components.end()) return nullptr;
        // return static_cast<T*>(component->second);

        for (auto component : _components) {
            T* target = dynamic_cast<T*>(component);

            if (target != nullptr) {
                return target;
            }
        }
        return nullptr;
    }

    template<typename T>
    requires std::derived_from<T, Component>
    std::vector<T*> GetComponentsInChildren() {
        std::vector<T*> _components;
        for (const auto child : transform->children) {
            auto component = child->GetGameObject()->GetComponent<T>();
            _components.push_back(component);
        }
        return static_cast<std::vector<T*>>(_components);
    }

private:
    std::string name;

    std::multimap<std::type_index, Component*> components;
    std::vector<Component*> _components;

    Transform* transform;
};



#endif //MAXENGINE_GAMEOBJECT_H

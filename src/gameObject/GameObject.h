//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_GAMEOBJECT_H
#define MAXENGINE_GAMEOBJECT_H
#include <map>
#include <string>
#include <typeindex>
#include "../components/Component.h"

class GameObject {
public:
    std::string name;

    std::multimap<std::type_index, Component*> components;

    virtual void Start();

    virtual void Update(float delta_time);

    template<typename T>
    requires std::derived_from<T, Component>
    T* AddComponent(){
        auto component = new T();
        components.insert(std::make_pair(std::type_index(typeid(T)), component));
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
        auto typeId = std::type_index(typeid(T));
        auto component = components.find(typeId);
        if (component == components.end()) return nullptr;
        return static_cast<T*>(component->second);
    }

private:
    void InternalUpdate();
};



#endif //MAXENGINE_GAMEOBJECT_H

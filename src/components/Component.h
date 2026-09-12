//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_COMPONENT_H
#define MAXENGINE_COMPONENT_H


class GameObject;

class Component {
public:
    virtual ~Component() = default;

    Component();

    explicit Component(GameObject* gameObject);

    void Attach(GameObject& _gameObject);

    GameObject* GetGameObject() const;

    virtual void Start();
    virtual void Update(float delta_time);

private:
    GameObject* gameObject = nullptr;
};


#endif //MAXENGINE_COMPONENT_H
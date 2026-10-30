//
// Created by zykov on 10/28/2026.
//

#ifndef MAXENGINE_SPHERE_H
#define MAXENGINE_SPHERE_H


class Sphere : public GameObject {
public:
    static Sphere *BuildSphere();

    Transform *transform;

    void Start() override;

    void Update(float delta_time) override;
};


#endif //MAXENGINE_SPHERE_H

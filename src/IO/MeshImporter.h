//
// Created by zykov on 3/30/2026.
//

#ifndef MAXENGINE_MESHIMPORTER_H
#define MAXENGINE_MESHIMPORTER_H


class Mesh;

class MeshImporter {
    public:
    virtual ~MeshImporter() = default;

    virtual Mesh* Import() = 0;
};

#endif //MAXENGINE_MESHIMPORTER_H

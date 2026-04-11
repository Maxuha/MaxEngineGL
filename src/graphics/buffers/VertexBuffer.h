//
// Created by zykov on 4/3/2026.
//

#ifndef MAXENGINE_VERTEXBUFFEROBJECT_H
#define MAXENGINE_VERTEXBUFFEROBJECT_H

class VertexBuffer {
public:
    VertexBuffer(const void* vertecies, const unsigned size);

    ~VertexBuffer();

    unsigned int id{};

    void Bind();

    void Unbind();
};

#endif //MAXENGINE_VERTEXBUFFEROBJECT_H

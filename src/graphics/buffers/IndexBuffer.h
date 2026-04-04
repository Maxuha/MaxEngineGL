//
// Created by zykov on 4/3/2026.
//

#ifndef MAXENGINE_INDEXBUFFER_H
#define MAXENGINE_INDEXBUFFER_H
#include "glad/glad.h"

class IndexBuffer {
private:
    unsigned int id{};
    unsigned int count{};
public:
    IndexBuffer(const void* indices, const unsigned count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline GLuint GetCount() const {
        return count;
    }
};

#endif //MAXENGINE_INDEXBUFFER_H

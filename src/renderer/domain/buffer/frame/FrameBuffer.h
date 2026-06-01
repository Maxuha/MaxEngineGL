//
// Created by zykov on 5/11/2026.
//

#ifndef MAXENGINE_FRAMEBUFFER_H
#define MAXENGINE_FRAMEBUFFER_H
#include "../../GLTexture.h"
#include "glad/glad.h"


namespace Rendering {
    class FrameBuffer{
    public:
        FrameBuffer();

        size_t GetWidth() const;

        size_t GetHeight() const;

        void BindTexture(const GLTexture& texture);

        void Bind() const;

        void Unbind() const;

        GLuint GetId() const;

    private:
        size_t width{};
        size_t height{};

        GLuint id{};

    };
}


#endif //MAXENGINE_FRAMEBUFFER_H

//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"

#include "components/MeshRenderer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

GLuint createShaderProgram() {
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;

        out vec3 Normal;
        out vec3 FragPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;

            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core

        in vec3 Normal;
        in vec3 FragPos;

        out vec4 FragColor;

        uniform vec3 color;

        uniform vec3 lightDirection;
        uniform vec3 lightColor;
        uniform float lightIntensity;

        void main()
        {
            vec3 norm = normalize(Normal);

            // light comes from opposite direction
            vec3 lightDir = normalize(-lightDirection);

            float diff = max(dot(norm, lightDir), 0.0);

            vec3 diffuse = diff * lightColor * color * lightIntensity;

            FragColor = vec4(diffuse, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Scene::Init() {
    shaderProgram = createShaderProgram();

    light = new DirectionalLight();

    camera = new Camera();
    camera->fov = 45;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = aspectRatio;

    light = new DirectionalLight();

    gameObject = { Cube::BuildCube() };
    for (GameObject *obj : gameObject) {
        obj->Start();
        obj->GetComponent<MeshRenderer>()->shaderProgram = shaderProgram;
    }
}

void Scene::Render() const {
    glUseProgram(shaderProgram);

    GLint vLocation = glGetUniformLocation(shaderProgram, "view");
    GLint pLocation = glGetUniformLocation(shaderProgram, "projection");
    GLint lightDirection = glGetUniformLocation(shaderProgram, "lightDirection");
    GLint lightColor = glGetUniformLocation(shaderProgram, "lightColor");
    GLint lightIntensity = glGetUniformLocation(shaderProgram, "lightIntensity");

    glm::vec3 lightDir(light->direction.x, light->direction.y, light->direction.z);
    glm::vec3 lightCol(light->color.x, light->color.y, light->color.z);
    glm::float32 lightInt(light->intensity);

    glUniformMatrix4fv(vLocation, 1, GL_FALSE, glm::value_ptr(camera->ViewMatrix()));
    glUniformMatrix4fv(pLocation, 1, GL_FALSE, glm::value_ptr(camera->ProjectionMatrix()));
    glUniform3fv(lightDirection, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightColor, 1, glm::value_ptr(lightCol));
    glUniform1f(lightIntensity, lightInt);

    for (GameObject *obj : gameObject) {
        obj->Update();
    }
}

void Scene::Cleanup() {
}

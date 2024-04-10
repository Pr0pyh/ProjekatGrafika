#ifndef PROJECT_BASE_GAME_H
#define PROJECT_BASE_GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Game {
private:
    static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
    {
        glViewport(0, 0, width, height);
    }
public:
    Game(){}

    GLFWwindow* Initialize(const int windowWidth, const int windowHeight, const char* title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);
        if(window == NULL)
        {
            std::cout << "failed to make a window" << std::endl;
            glfwTerminate();
            return NULL;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "failed to initialize GLAD" << std::endl;
            return NULL;
        }
        return window;
    }

    void Input(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void Update();

    void Draw(GLFWwindow* window)
    {
        glClearColor(0.1f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Deinitialize()
    {
        glfwTerminate();
    }
};

#endif //PROJECT_BASE_GAME_H

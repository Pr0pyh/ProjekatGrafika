#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <MojeKlase/Game.h>

#include <iostream>

const int windowWidth = 800;
const int windowHeight = 600;

int main()
{
    Game game;
    GLFWwindow* window = game.Initialize(windowWidth, windowHeight, "projekat");

    while(!glfwWindowShouldClose(window))
    {
        game.Input(window);

        game.Draw(window);
    }

    game.Deinitialize();

    return 0;
}
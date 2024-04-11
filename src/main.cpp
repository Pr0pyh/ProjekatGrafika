#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <MojeKlase/Game.h>

#include <iostream>

const int windowWidth = 1980;
const int windowHeight = 1485;

int main()
{
    Game game;
    GLFWwindow* window = game.Initialize(windowWidth, windowHeight, "projekat");

    game.shaderInitialization();
    game.arrayAndBufferInitialization();
//    game.textureInitialization();
    game.modelInitialization();
    game.skyboxInitialization();

    while(!glfwWindowShouldClose(window))
    {
        game.ScreenSettings();
        game.Input(window);
        game.DrawSkybox();
        game.Update();
        game.Draw(window);
    }

    game.Deinitialize();

    return 0;
}
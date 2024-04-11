#ifndef PROJECT_BASE_GAME_H
#define PROJECT_BASE_GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <MojeKlase/Shader.h>
#include <MojeKlase/Camera.h>
#include <MojeKlase/Model.h>

#include <iostream>

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX;
float lastY;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 snapshotPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.2f,  5.0f,  1.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -8.0f)
};

class Game {
private:
    Shader *shader;
    Shader *skyboxShader;
    Shader *lightShader;
    Model *room;
    Model *lamp;
    unsigned int texture0;
    unsigned int texture1;
    unsigned int skyboxTexture;
    unsigned int VAO, VBO;
    unsigned int skyboxVAO, skyboxVBO;

    static void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
        glViewport(0, 0, width, height);
    }

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        camera.processMouseMovement(xoffset, yoffset);
    }

    unsigned int loadSkyboxTexture(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "cubemap texture failed: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

public:
    Game() {}

    GLFWwindow *Initialize(const int windowWidth, const int windowHeight, const char *title) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);
        if (window == NULL) {
            std::cout << "failed to make a window" << std::endl;
            glfwTerminate();
            return NULL;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "failed to initialize GLAD" << std::endl;
            return NULL;
        }

        lastX = windowWidth / 2.0f;
        lastY = windowHeight / 2.0f;

        return window;
    }

    void shaderInitialization() {
        shader = new Shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
        skyboxShader = new Shader("resources/shaders/skyboxShader.vs", "resources/shaders/skyboxShader.fs");
        lightShader = new Shader("resources/shaders/lightShader.vs", "resources/shaders/lightShader.fs");
    }

    void arrayAndBufferInitialization() {
        float vertices[] = {
                // positions          // normals           // texture coords
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    void textureInitialization() {
        glBindVertexArray(VAO);
        glGenTextures(1, &skyboxTexture);
        glBindTexture(GL_TEXTURE_2D, texture0);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load("resources/textures/container2.png", &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        data = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
        if (data) {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        shader->use();
        shader->setInt("texture0", 0);
        shader->setInt("texture1", 1);
    }

    void skyboxInitialization()
    {
        float skyboxVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };
        glGenBuffers(1, &skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        std::vector<std::string> faces
                {
                        "resources/textures/mySkybox2/right.png",
                        "resources/textures/mySkybox2/left.png",
                        "resources/textures/mySkybox2/top.png",
                        "resources/textures/mySkybox2/bottom.png",
                        "resources/textures/mySkybox2/front.png",
                        "resources/textures/mySkybox2/back.png"
                };
        skyboxTexture = loadSkyboxTexture(faces);
        skyboxShader->use();
        skyboxShader->setInt("skybox", 0);
    }

    void modelInitialization()
    {
        room = new Model("resources/objects/SobaProzor3/roomWindow.obj");
    }

    void Input(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(FORWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(BACKWARD, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(LEFT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard(RIGHT, deltaTime);

        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            snapshotPosition = camera.Front;
    }

    void ScreenSettings()
    {
        glEnable(GL_DEPTH_TEST);
        //blending
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        glBlendEquation(GL_FUNC_ADD);

        glClearColor(0.1f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DrawSkybox()
    {
        glDepthMask(GL_FALSE);
        skyboxShader->use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewmatrix()));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }

    void Update()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewmatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);


        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        shader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        shader->setInt("material.diffuse", 0);
        shader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader->setFloat("material.shininess", 32.0f);

        shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader->setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        shader->setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
        shader->setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

        // point light 1
        shader->setVec3("pointLights[0].position", pointLightPositions[0]);
        shader->setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.0f);
        shader->setVec3("pointLights[0].diffuse", 1.0f, 0.6f, 0.0f);
        shader->setVec3("pointLights[0].specular", 1.0f, 0.6f, 0.0f);
        shader->setFloat("pointLights[0].constant", 1.0f);
        shader->setFloat("pointLights[0].linear", 0.07f);
        shader->setFloat("pointLights[0].quadratic", 0.017f);
        // point light 2
        shader->setVec3("pointLights[1].position", pointLightPositions[1]);
        shader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader->setFloat("pointLights[1].constant", 1.0f);
        shader->setFloat("pointLights[1].linear", 0.09f);
        shader->setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        shader->setVec3("pointLights[2].position", pointLightPositions[2]);
        shader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader->setFloat("pointLights[2].constant", 3.0f);
        shader->setFloat("pointLights[2].linear", 0.09f);
        shader->setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        shader->setVec3("pointLights[3].position", pointLightPositions[3]);
        shader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader->setFloat("pointLights[3].constant", 1.0f);
        shader->setFloat("pointLights[3].linear", 0.09f);
        shader->setFloat("pointLights[3].quadratic", 0.032f);

        shader->setVec3("spotLight.position", camera.Position);
        shader->setVec3("spotLight.direction", camera.Front);
        shader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader->setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);
        shader->setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
        shader->setFloat("spotLight.constant", 1.0f);
        shader->setFloat("spotLight.linear", 0.09f);
        shader->setFloat("spotLight.quadratic", 0.032f);
        shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        shader->setVec3("cameraPos", snapshotPosition);
        shader->setVec3("viewPos", camera.Position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        shader->setMat4("model", model);

        lightShader->use();
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        lightShader->setMat4("view", view);
        lightShader->setMat4("projection", projection);
        lightShader->setMat4("model", model);
    }

    void Draw(GLFWwindow* window)
    {
        //face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        shader->use();
        room->Draw(shader);
        //lightShader
        lightShader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Deinitialize()
    {
        shader->deleteProgram();
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &skyboxVBO);
        glDeleteVertexArrays(1, &skyboxVAO);
        glfwTerminate();
    }
};

#endif //PROJECT_BASE_GAME_H

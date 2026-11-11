#include <iostream>
#include <vector>

// Подключение GLAD и GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Подключение GLM для матриц (у вас настроено в CMake)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Подключение PhysX
#include <PxPhysicsAPI.h>

using namespace physx;

// Физические константы
const PxVec3 G_GRAVITY(0.0f, -9.81f, 0.0f);
const PxReal TIME_STEP = 1.0f / 60.0f;

// Глобальные объекты PhysX
PxFoundation*           gFoundation = nullptr;
PxPhysics*              gPhysics    = nullptr;
PxScene*                gScene      = nullptr;
PxDefaultCpuDispatcher* gDispatcher = nullptr;
PxMaterial*             gMaterial   = nullptr;
PxRigidDynamic*         gDynamicBox = nullptr;

// Шейдеры (Modern OpenGL)
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main() {
        FragColor = vec4(color, 1.0);
    }
)";

// Инициализация PhysX
void InitPhysics() {
    static PxDefaultAllocator gAllocator;
    static PxDefaultErrorCallback gErrorCallback;

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    PxTolerancesScale scale;
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, scale);

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = G_GRAVITY;
    gDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.2f);

    // Статическая плоскость (земля)
    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);

    // Динамический куб (размер 1х1х1, полуэкстенты 0.5)
    PxTransform boxTransform(PxVec3(0.0f, 10.0f, 0.0f));
    PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
    gDynamicBox = PxCreateDynamic(*gPhysics, boxTransform, boxGeometry, *gMaterial, 10.0f);
    gScene->addActor(*gDynamicBox);
}

void CleanupPhysics() {
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    PX_RELEASE(gFoundation);
}

// Вспомогательная функция для компиляции шейдеров
GLuint CompileShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
int main() {
    std::cout << "Starting application..." << std::endl;

    if (!glfwInit()) {
        std::cerr << "CRITICAL ERROR: Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "PhysX + GLFW Core Profile", NULL, NULL);
    if (!window) {
        std::cerr << "CRITICAL ERROR: Failed to create GLFW window! (Your GPU might not support OpenGL 3.3 Core)" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "CRITICAL ERROR: Failed to initialize GLAD!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    std::cout << "Initializing PhysX..." << std::endl;
    InitPhysics();
    std::cout << "PhysX initialized successfully!" << std::endl;

    GLuint shaderProgram = CompileShaders();

    // Вершины каркасного куба (Wireframe)
    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f 
    };
    unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7 
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 4.0f, 15.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    std::cout << "Entering main loop..." << std::endl;

    while (!glfwWindowShouldClose(window)) {
        gScene->simulate(TIME_STEP);
        gScene->fetchResults(true);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        PxTransform pose = gDynamicBox->getGlobalPose();
        PxMat44 pxMatrix(pose);
        glm::mat4 model = glm::make_mat4(pxMatrix.front());

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    CleanupPhysics();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

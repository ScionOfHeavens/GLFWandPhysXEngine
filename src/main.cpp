#include <glm/glm.hpp>

#include "physics.hpp"
#include "shader_program_builder.hpp"
#include "model.hpp"
#include "window.hpp"

int main()
{
	Window::init();
	Window::setWindowSize(800, 600);
	Window::setWindowTitle("Java Core project");
    Window::buildContext();
    Physics::initPhysics();
    Physics::CreatePlane();
    Physics::CreateCube();

    ShaderProgramBuilder builder;
	ShaderProgram ourShader = builder.withVertexShader("assets/shaders/shader.vs")
									 .withFragmentShader("assets/shaders/shader.fs")
									 .build();

    Model ourModel("assets/meshes/Cube/Cube.obj"); 

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Window::setRenderFunc([&ourShader, &ourModel](Camera camera, float screenWidth, float screenHeight){
        
		ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::make_mat4(Physics::stepPhysics().front());
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // смещаем вниз чтобы быть в центре сцены
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// объект слишком большой для нашей сцены, поэтому немного уменьшим его
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
	});

	Window::run();

    glfwTerminate();
    Physics::CleanupPhysics();
    return 0;
}
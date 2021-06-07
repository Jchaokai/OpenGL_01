#include "camera.h"
#include "shader_m.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

/// <summary>
/// 错误处理
/// </summary>
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) glClearError();\
	x;\
	ASSERT(glLogCall(#x,__FILE__,__LINE__))


static void glClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool glLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[opengl:error] : " << error << "\nfile: " << file << "\nfunction():" << function <<
            "\nline: " << line << "\n------------------------------" << std::endl;
        return false;
    }
    return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(lightColor * objectColor, 1.0);\n"
"}\n";

const char* vertexShaderSource2 = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0);\n"
"}\n";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //glfwSwapInterval(3);

    if (glewInit() != GLEW_OK) {
        std::cout << "error" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    unsigned int vertex1  = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment1 = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(vertex1, 1, &vertexShaderSource, 0));
    GLCall(glShaderSource(fragment1, 1, &fragmentShaderSource, 0));
    GLCall(glCompileShader(vertex1));
    // check shader complie error
    int success;
    char infoLog[512];
    glGetShaderiv(vertex1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::FAILED\n" << infoLog << std::endl;
    }
    GLCall(glCompileShader(fragment1));
    // check shader complie error
    glGetShaderiv(fragment1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::FAILED\n" << infoLog << std::endl;
    }
    unsigned int lightingShader = glCreateProgram();

    GLCall(glAttachShader(lightingShader, vertex1));
    GLCall(glAttachShader(lightingShader, fragment1));
    GLCall(glLinkProgram(lightingShader));
    glGetShaderiv(lightingShader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(lightingShader, 512, NULL, infoLog);
        std::cout << "shader program link FAILED\n" << infoLog << std::endl;
    }
    GLCall(glDeleteShader(vertex1));
    GLCall(glDeleteShader(fragment1));

    unsigned int vertex2 = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment2 = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(vertex2, 1, &vertexShaderSource2, 0));
    GLCall(glShaderSource(fragment2, 1, &fragmentShaderSource2, 0));
    glCompileShader(vertex2);
    // check shader complie error
    glGetShaderiv(vertex2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::FAILED\n" << infoLog << std::endl;
    }
    glCompileShader(fragment2);
    // check shader complie error
    glGetShaderiv(fragment2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::FAILED\n" << infoLog << std::endl;
    }
    unsigned int  lightCubeShader = glCreateProgram();
    GLCall(glAttachShader(lightCubeShader, vertex2));
    GLCall(glAttachShader(lightCubeShader, fragment2));
    GLCall(glLinkProgram(lightCubeShader));
    glGetShaderiv(lightingShader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(lightingShader, 512, NULL, infoLog);
        std::cout << "shader program link FAILED\n" << infoLog << std::endl;
    }
    GLCall(glDeleteShader(vertex2));
    GLCall(glDeleteShader(fragment2));


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    GLCall(glGenVertexArrays(1, &cubeVAO));
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GLCall(glBindVertexArray(cubeVAO));
    
    // position attribute
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    GLCall(glGenVertexArrays(1, &lightCubeVAO));
    GLCall(glBindVertexArray(lightCubeVAO));
    
    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
       

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        GLCall(glUseProgram(lightingShader));
        GLCall(glUniform3fv(glGetUniformLocation(lightingShader, "objectColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f))));
        GLCall(glUniform3fv(glGetUniformLocation(lightingShader, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))));
       

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader, "view"), 1, GL_FALSE, &view[0][0]);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.2f)); 
        glUniformMatrix4fv(glGetUniformLocation(lightingShader, "model"), 1, GL_FALSE, &model[0][0]);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        GLCall(glUseProgram(lightCubeShader));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "view"), 1, GL_FALSE, &view[0][0]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
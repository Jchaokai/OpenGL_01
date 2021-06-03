#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"uniform float offest;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x+offest, -aPos.y, aPos.z, 1.0);\n"
	"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(3);

	if (glewInit() != GLEW_OK) {
		std::cout << "error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;


	// vertex shader
	unsigned int vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexshader);	// check shader complie error

	// fragment shader
	unsigned int fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentshader);	// check shader complie error

	//link  shader 
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexshader);
	glAttachShader(shaderProgram, fragmentshader);
	glLinkProgram(shaderProgram);	// check link error

	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);

	float vertices[] = {
		// first triangle
		-0.9f, -0.5f, 0.0f,  // left 
		-0.0f, -0.5f, 0.0f,  // right
		-0.45f, 0.5f, 0.0f,  // top 
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// uniform
	int gl_uniform_location = glGetUniformLocation(shaderProgram, "offest");
	if (gl_uniform_location == -1) {
		std::cout << "glGetUniformLocation() error" << std::endl;
	}

	float offest = 0.0f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//
		glUseProgram(shaderProgram);
	
		glUniform1f(gl_uniform_location, offest);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		offest += 0.02f;
		if (offest >= 1) offest = 0.0f;


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
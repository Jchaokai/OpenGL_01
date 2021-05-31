#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


struct shaderPorgramSource
{
	std::string VertexSource;
	std::string FragmentSource;

};
static shaderPorgramSource pardeShader(const std::string filepath) {

	std::ifstream stream(filepath);

	enum class shaderType {
		NONE = 0,
		VERTEX = 1,
		FRAGMENT = 2,
	};

	std::string line;
	std::stringstream ss[3];
	shaderType type = shaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = shaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = shaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[1].str(),ss[2].str() };
}

unsigned int compileShader(unsigned int shader_type, const std::string& source) {
	unsigned int id = glCreateShader(shader_type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//todo compile failed
	int  success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR SHADER: " << (shader_type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return id;
}

static unsigned int createProgram(const std::string& vertexShader, const std::string& fragShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragShader);


	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(840, 510, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);


	if (glewInit() != GLEW_OK) {
		std::cout << "error" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	// 
	float vertices[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f,  0.5f,
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//告诉opengl,VBO（缓存）怎么布局（layout)的
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);



	shaderPorgramSource sources = pardeShader("res/shaders/basic.shader");
	unsigned int shader = createProgram(sources.VertexSource, sources.FragmentSource);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		glDrawArrays(GL_TRIANGLES, 0, 3);



		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
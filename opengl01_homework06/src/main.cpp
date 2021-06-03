#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "main.h"


struct shaderProgramSource{
	std::string vertexSource;
	std::string fragmentSource;

};

static shaderProgramSource parseSource(std::string filepath) {
	std::ifstream file(filepath);
	enum class shaderType
	{
		VERTEX = 0,
		FRAGMENT = 1,
		NONE = 2,
	};

	std::string line;
	std::stringstream ss[3];
	shaderType curtype = shaderType::NONE;

	while (getline(file, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				curtype = shaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				curtype = shaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)curtype] << line << "\n";
		}
	}
	std::cout << ss[0].str() << std::endl;
	std::cout << ss[1].str() << std::endl;
	return { ss[0].str(),ss[1].str() };
}

unsigned int compileShader(unsigned int shader_type,const std::string& source) {
	unsigned int  shader1 = glCreateShader(shader_type);
	const char* src = source.c_str();
	glShaderSource(shader1, 1,&src, nullptr);
	glCompileShader(shader1);	//check compile error
	return shader1;
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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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


	shaderProgramSource shaderSources =  parseSource("res/basic.shader");
	unsigned int shaderprogram = createProgram(shaderSources.vertexSource, shaderSources.fragmentSource);
	
	float vertices[] = {
		// 位置              // 颜色
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//顶点位置layout
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	//顶点颜色layout
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);




	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//
		glUseProgram(shaderprogram);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


/// <summary>
/// ????????
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



const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aColor;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos, 1.0);\n"
"	ourColor = aColor;\n"
"	TexCoord = aTexCoord;\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform float mixvalue;\n"
"\n"
"uniform sampler2D texture1;\n"
"uniform sampler2D texture2;\n"
"void main()\n"
"{\n"
"	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixvalue);\n"
"}\n";


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(700, 560, "Hello World", NULL, NULL);
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

	//shader program
	unsigned int vertexshader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexshader, 1, &vertexShaderSource, nullptr);
	glShaderSource(fragmentshader, 1, &fragmentShaderSource, nullptr);
	GLCall(glCompileShader(vertexshader));
	GLCall(glCompileShader(fragmentshader));


	unsigned int shaderprogram = glCreateProgram();
	GLCall(glAttachShader(shaderprogram, vertexshader));
	GLCall(glAttachShader(shaderprogram, fragmentshader));
	GLCall(glLinkProgram(shaderprogram));	//check link error

	GLCall(glDeleteShader(vertexshader));
	GLCall(glDeleteShader(fragmentshader));

	float vertices[] = {
		//     ---- λ?? ----       ---- ??ɫ ----     - ???????? -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ????
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ????
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ????
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ????
	};
	unsigned int  indexs[] = {
		0, 1, 3, // first triangle
		1, 2, 3
	};

	unsigned int VAO, VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);
	//position layout 
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	//color layout
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));
	//texture layout
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));


	// texture1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// ???û??ơ????˷?ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//??????????ʾ??Χ?ĵ???????????ȥ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//?Ŵ???С????ɫ?ͽ?ȡֵ?????????ص㣬
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//????ͼƬ????
	int width, height, nrchannels;
	unsigned char* data = stbi_load("images/container.jpg", &width, &height, &nrchannels, 0);
	if (data) {
		//????????
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
		std::cout << "load \"images/container.jpg\" error" << std::endl;
	stbi_image_free(data);

	// texture2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// ???û??ƣ????˷?ʽ
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	stbi_set_flip_vertically_on_load(true);

	//????ͼƬ???? & ????????
	data = stbi_load("images/awesomeface.png", &width, &height, &nrchannels, 0);
	if (data) {
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else {
		std::cout << "load \"images/awesomeface.png\" error " << std::endl;
	}
	stbi_image_free(data);

	// get uniform 
	// ??????uniform????ǰ ??????ɫ?????? ??????
	glUseProgram(shaderprogram);
	glUniform1i(glGetUniformLocation(shaderprogram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderprogram, "texture2"), 1);
	
	float visiable = 0.1f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			visiable += 0.0005f; // change this value accordingly (might be too slow or too fast based on system hardware)
			if (visiable >= 1.0f)
				visiable = 1.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			visiable -= 0.0005f; // change this value accordingly (might be too slow or too fast based on system hardware)
			if (visiable <= 0.0f)
				visiable = 0.0f;
		}

		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		

		//??????Ԫ
		//??????????????Ԫ??Ĭ?ϵ?һ??????
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, texture2));

		//
		GLCall(glUniform1f(glGetUniformLocation(shaderprogram, "mixvalue"), visiable));
		
		glUseProgram(shaderprogram);

		GLCall(glBindVertexArray(VAO));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
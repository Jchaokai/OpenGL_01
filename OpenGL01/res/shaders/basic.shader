#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
};

#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.6f, 0.8f, 0.5f, 1.0f);
};
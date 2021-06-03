#shader vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
out vec3 mypos;
void main()
{
	gl_Position = vec4(pos, 1.0f);
	mypos = pos;
};

#shader fragment
#version 330 core
out vec4 fragcolor;
in vec3 mypos;

void main()
{
	fragcolor = vec4(mypos, 1.0f);
}
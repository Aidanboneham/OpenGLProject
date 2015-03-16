#version 410

layout(location=0) in vec4 pos;
layout(location=1) in vec2 uv;

out vec2 texcoord;

void main()
{
	gl_Position = pos;
	texcoord = uv;
}
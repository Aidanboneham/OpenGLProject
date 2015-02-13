#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;

out vec4 out_color;

uniform mat4 projection_view;
uniform float timer;

void main()
{
	out_color = color;
	vec4 pos = position;
	pos.y = sin(pos.x+timer) + sin(pos.z+timer);
	gl_Position = projection_view * pos;
}
#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 frag_normal;
out vec4 frag_position;

uniform mat4 projection_view;
uniform mat4 world;

void main()
{
	frag_position = position;
	frag_normal = world * vec4(normal.xyz, 0);
	gl_Position = projection_view * world * position;
}

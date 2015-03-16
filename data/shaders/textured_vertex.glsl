#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_coord;

out vec4 reflected_screen_pos; //added this

uniform mat4 projection_view; 
uniform mat4 projection_view_reflected; //added this

void main()
{
	//added this
	reflected_screen_pos = projection_view_reflected * position;


	gl_Position = projection_view * position;
}
#version 410

in vec2 frag_tex_coord;

out vec4 frag_color;

uniform sampler2D diffuse;

void main()
{
	frag_color = texture(diffuse, frag_tex_coord);
}
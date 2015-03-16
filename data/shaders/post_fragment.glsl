#version 410

in vec2 texcoord;

out vec4 fragcolor;

uniform sampler2D target;

void main()
{
	fragcolor = texture(target, texcoord);
}
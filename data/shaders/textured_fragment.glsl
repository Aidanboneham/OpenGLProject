#version 410

//ADDED THIS
in vec4 reflected_screen_pos;

out vec4 frag_color;
uniform sampler2D diffuse;

void main()
{
	//ADDED THIS
	vec4 uv_position = reflected_screen_pos / reflected_screen_pos.w;
	uv_position = (uv_position + 1) * 0.5f;
	
	//ADDED THIS
	frag_color = texture(diffuse, uv_position.xy);
}
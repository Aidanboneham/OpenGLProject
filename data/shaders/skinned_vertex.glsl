#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 tex_coord;
layout(location=3) in vec4 bone_indices;
layout(location=4) in vec4 bone_weights;

out vec2 frag_tex_coord;
out vec4 vert_normal;

uniform mat4 projection_view;
uniform mat4 world;
uniform mat4 inv_transpose_world;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	frag_tex_coord = tex_coord;

	ivec4 indices = ivec4(bone_indices);
	
	vec4 final_position = vec4(0,0,0,0);

	final_position += bones[indices.x] * position * bone_weights.x;
	final_position += bones[indices.y] * position * bone_weights.y;
	final_position += bones[indices.z] * position * bone_weights.z;
	final_position += bones[indices.w] * position * bone_weights.w;
	final_position.w = 1;

	vec4 final_normal = vec4(0,0,0,0);
	final_normal += bones[indices.x] * normal * bone_weights.x;
	final_normal += bones[indices.y] * normal * bone_weights.y;
	final_normal += bones[indices.z] * normal * bone_weights.z;
	final_normal += bones[indices.w] * normal * bone_weights.w;
	final_normal.w = 0;

	vert_normal = inv_transpose_world * final_normal;

	gl_Position = projection_view * world * final_position;
}
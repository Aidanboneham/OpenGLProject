//GEOMETRY SHADER
#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 geo_position[];
in float geo_lifetime[];
in float geo_lifespan[];

out vec4 frag_color;

uniform mat4 projection_view;
uniform mat4 camera_world;

uniform float start_size;
uniform float end_size;

uniform vec4 start_color;
uniform vec4 end_color;

void main()
{
	//calculate a 0-1 value for the interpolation
	//of start/end pairs
	float t = geo_lifetime[0] / geo_lifespan[0];
	//interpolate to find the correct color
	frag_color = mix(start_color, end_color, t);

	//half the size so that the total width of the 
	//quad will be correct
	float half_size = mix(start_size, end_size, t) * 0.5f;

	//create scaled local-space vertices
	vec3 corners[4];
	corners[0] = vec3(half_size, -half_size, 0);
	corners[1] = vec3(half_size, half_size, 0);
	corners[2] = vec3(-half_size, -half_size, 0);
	corners[3] = vec3(-half_size, half_size, 0);

	//build the 3 axies of the rotation matrix
	vec3 forward = normalize(camera_world[3].xyz - geo_position[0]);
	vec3 right = cross(camera_world[1].xyz, forward);
	vec3 up = cross(forward, right);
	//combine axies into matrix
	mat3 billboard_rot = mat3(right, up, forward);
	
	//rotate the verts
	corners[0] = billboard_rot * corners[0];
	corners[1] = billboard_rot * corners[1];
	corners[2] = billboard_rot * corners[2];
	corners[3] = billboard_rot * corners[3];
	
	//translate the verts
	corners[0] += geo_position[0];
	corners[1] += geo_position[0];
	corners[2] += geo_position[0];
	corners[3] += geo_position[0];
	
	//transform verts by the projection view and output them
	gl_Position = projection_view * vec4(corners[0], 1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[1], 1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[2], 1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[3], 1);
	EmitVertex();
}
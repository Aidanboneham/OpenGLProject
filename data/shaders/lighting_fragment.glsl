#version 410

in vec4 frag_normal;
in vec4 frag_position;

out vec4 frag_color;

uniform vec3 ambient_light;

uniform vec3 light_dir;
uniform vec3 light_color;

uniform vec3 material_color;

uniform vec3 eye_pos;
uniform float specular_power;

void main()
{
	vec3 N = normalize(frag_normal.xyz);
	vec3 L = normalize(light_dir);

	vec3 ambient = material_color * ambient_light;

	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * light_color * material_color;

	vec3 E = normalize(eye_pos - frag_position.xyz);
	vec3 R = reflect(L, N);


	float s = max(0, dot(R, E));



	s = pow(s, specular_power);
	vec3 specular = vec3(s) * light_color * material_color;

	frag_color = vec4(frag_normal.xyz, 1);//vec4(ambient + diffuse + specular, 1);
}
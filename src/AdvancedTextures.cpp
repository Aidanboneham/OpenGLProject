#include "AdvancedTextures.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h" 
#include "Utility.h"

#include "stb_image.h"

bool AdvancedTextures::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	generateQuad(5.0f);
	loadTextures();

	LoadShader("./shaders/normal_mapped_vertex.glsl", 
				"./shaders/normal_mapped_fragment.glsl", &m_program_id);
	
	
	m_ambient_light = vec3(0.1f);
	m_light_dir = glm::normalize(vec3(-5, -1, 0));
	m_light_color = vec3(0.7f);
	m_specular_power = 5.0f;

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	return true;
}

void AdvancedTextures::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool AdvancedTextures::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
			i == 10 ? white : black);
	}

	m_light_dir = ( glm::rotate(dt, vec3(0, 1, 0)) * vec4(m_light_dir, 0) ).xyz;


	Gizmos::addLine(vec3(0), m_light_dir * 10, vec4(1, 1, 1, 1));

	m_camera.update(dt);

	return true;
}

void AdvancedTextures::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int proj_view_uniform = 
		glGetUniformLocation(m_program_id, "projection_view");

	glUniformMatrix4fv(proj_view_uniform, 1, 
						GL_FALSE, (float*)&m_camera.view_proj);

	int ambient_uniform =
		glGetUniformLocation(m_program_id, "ambient_light");
	int light_dir_uniform =
		glGetUniformLocation(m_program_id, "light_dir");
	int light_color_uniform =
		glGetUniformLocation(m_program_id, "light_color");
	int eye_pos_uniform =
		glGetUniformLocation(m_program_id, "eye_pos");
	int specular_uniform =
		glGetUniformLocation(m_program_id, "specular_power");

	glUniform3fv(ambient_uniform, 1, (float*)&m_ambient_light);

	m_light_dir = m_camera.world[2].xyz * -1;
	glUniform3fv(light_dir_uniform, 1, (float*)&m_light_dir);


	glUniform3fv(light_color_uniform, 1, (float*)&m_light_color);
	vec3 camera_pos = m_camera.world[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, m_specular_power);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	int diffuse_location =
		glGetUniformLocation(m_program_id, "diffuse_tex");
	int normal_location =
		glGetUniformLocation(m_program_id, "normal_tex");
	int specular_locatoin =
		glGetUniformLocation(m_program_id, "specular_tex");

	glUniform1i(diffuse_location, 0);
	glUniform1i(normal_location, 1);
	glUniform1i(specular_locatoin, 2);


	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, m_quad.m_index_count, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera.proj, m_camera.view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void AdvancedTextures::loadTextures()
{
	int width, height, channels;

	unsigned char *data;

	data = stbi_load("./textures/rock_diffuse.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_diffuse_texture);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);


	data = stbi_load("./textures/rock_normal.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_normal_texture);
	glBindTexture(GL_TEXTURE_2D, m_normal_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);


	data = stbi_load("./textures/rock_specular.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_specular_texture);
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}


void AdvancedTextures::generateQuad(float size)
{
	VertexNormal vertex_data[4];

	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);

	vertex_data[0].normal = vec4(0, 1, 0, 0);
	vertex_data[1].normal = vec4(0, 1, 0, 0);
	vertex_data[2].normal = vec4(0, 1, 0, 0);
	vertex_data[3].normal = vec4(0, 1, 0, 0);

	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[1].tangent = vec4(1, 0, 0, 0);
	vertex_data[2].tangent = vec4(1, 0, 0, 0);
	vertex_data[3].tangent = vec4(1, 0, 0, 0);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };
	m_quad.m_index_count = 6;

	glGenVertexArrays(1, &m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal) * 4,
		vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6,
		index_data, GL_STATIC_DRAW);









	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normal
	glEnableVertexAttribArray(2); //tangent
	glEnableVertexAttribArray(3); //tex coord


	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexNormal), 0); //position

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
		sizeof(VertexNormal), (void*)(sizeof(vec4) * 1)); //normal

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE,
		sizeof(VertexNormal), (void*)(sizeof(vec4) * 2));//tangent

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexNormal), (void*)(sizeof(vec4) * 3));//tex coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#include "Lighting.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "Vertex.h"
#include "Utility.h"

bool Lighting::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	LoadShader("./shaders/lighting_vertex.glsl", 
				"./shaders/lighting_fragment.glsl", &m_program_id);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, 
								"./models/stanford/bunny.obj");
	if (err.size() != 0)
	{
		return false;
	}

	createOpenGLBuffers(shapes);

	m_ambient_light = vec3(0.1f);
	m_light_dir = vec3(0, -1, 0);
	m_light_color = vec3(0.6f, 0, 0);
	m_material_color = vec3(1);
	m_specular_power = 15;

	return true;
}

void Lighting::reloadShader()
{
}

void Lighting::shutdown()
{
	cleanupOpenGLBuffers();
	Gizmos::destroy();
	Application::shutdown();
}

bool Lighting::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
			i == 10 ? white : black);
	}

	if ( glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS )
	{
		glDeleteProgram(m_program_id);

		LoadShader("./shaders/lighting_vertex.glsl",
			"./shaders/lighting_fragment.glsl", &m_program_id);
	}

	m_light_dir = (glm::rotate(dt, glm::normalize(vec3(1, 1, 1))) * vec4(m_light_dir, 0)).xyz;


	m_camera.update(dt);
	
	return true;
}

void Lighting::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&m_camera.view_proj);

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



	int material_color_uniform =
		glGetUniformLocation(m_program_id, "material_color");
	
	glUniform3fv(ambient_uniform, 1, (float*)&m_ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&m_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&m_light_color);
	vec3 camera_pos = m_camera.world[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, m_specular_power);

	glUniform3fv(material_color_uniform, 1, (float*)&m_material_color);

	Gizmos::draw(m_camera.proj, m_camera.view);

	for (unsigned int mesh_index = 0;
		mesh_index < m_gl_data.size();
		++mesh_index)
	{
		glBindVertexArray(m_gl_data[mesh_index].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_data[mesh_index].m_index_count,
							GL_UNSIGNED_INT, 0);
	}

	
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Lighting::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_data.resize(shapes.size());

	for (unsigned int shape_index = 0;
		shape_index < shapes.size();
		++shape_index)
	{
		std::vector<float> vertex_data;

		unsigned int float_count = shapes[shape_index].mesh.positions.size();
		float_count += shapes[shape_index].mesh.normals.size();

		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[shape_index].mesh.positions.begin(),
			shapes[shape_index].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[shape_index].mesh.normals.begin(),
			shapes[shape_index].mesh.normals.end());

		m_gl_data[shape_index].m_index_count = 
							shapes[shape_index].mesh.indices.size();

		glGenVertexArrays(1, &m_gl_data[shape_index].m_VAO);
		glGenBuffers(1, &m_gl_data[shape_index].m_VBO);
		glGenBuffers(1, &m_gl_data[shape_index].m_IBO);

		glBindVertexArray(m_gl_data[shape_index].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_data[shape_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * float_count,
									vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_data[shape_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[shape_index].mesh.indices.size() * sizeof(unsigned int),
			shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float) * shapes[shape_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::cleanupOpenGLBuffers()
{
	glDeleteProgram(m_program_id);

	for (unsigned int i = 0; i < m_gl_data.size(); ++i)
	{
		glDeleteVertexArrays(1, &m_gl_data[i].m_VAO);
		glDeleteBuffers(1, &m_gl_data[i].m_VBO);
		glDeleteBuffers(1, &m_gl_data[i].m_IBO);
	}
}
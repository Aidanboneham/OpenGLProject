#include "RenderingGeometry.h"
#include "glm_header.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Vertex.h"
#include "Gizmos.h"
#include "Utility.h"


bool RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	LoadShader("./shaders/basic_vertex.glsl",
		"./shaders/basic_fragment.glsl", &m_program_id);
	generateGrid(10, 10);

	Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
		
	m_timer = 0;
	return true;
}

void RenderingGeometry::shutdown()
{
	Application::shutdown();
}

bool RenderingGeometry::update()
{
	if (Application::update() == false)
	{
		return false;
	}
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
	float dt = (float)glfwGetTime();
	glfwSetTime(0);

	m_timer += dt;

	m_camera.update(dt);

	return true;
}

void RenderingGeometry::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_id);

	int proj_view_handle = glGetUniformLocation(m_program_id, "projection_view");

	if (proj_view_handle >= 0)
	{
		glUniformMatrix4fv(proj_view_handle, 1, false, (float*)&m_camera.view_proj);
	}

	int timer_handle = glGetUniformLocation(m_program_id, "timer");
	if (timer_handle >= 0)
	{
		glUniform1f(timer_handle, m_timer);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera.view_proj);

	glfwSwapBuffers(m_window);
	glfwPollEvents();

	Application::draw();
}

void 
RenderingGeometry::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[(rows + 1) * (cols + 1)];

	for (unsigned int r = 0; r < rows + 1; ++r)
	{
		for (unsigned int c = 0; c < cols + 1; ++c)
		{
			vec4 pos = vec4((float)c, 0, (float)r, 1);
			vertex_array[c + r * (cols+1)].position = pos;

			vec4 color = vec4((float)c / (cols+1), 0, (float)r / (rows+1), 1);
			vertex_array[c + r * (cols+1)].color = color;
		}
	}

	m_index_count = rows * cols * 6;
	unsigned int *index_array = new unsigned int[m_index_count];

	int index_location = 0;

	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			index_array[index_location + 0] = c + r * (cols+1);
			index_array[index_location + 1] = c + (r+1) * (cols+1);
			index_array[index_location + 2] = (c+1) + r * (cols+1);

			index_array[index_location + 3] = (c + 1) + r * (cols+1);
			index_array[index_location + 4] = c + (r + 1) * (cols+1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (cols+1);

			index_location += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (cols + 1)*(rows + 1)*sizeof(Vertex), vertex_array, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertex_array;
	delete[] index_array;
}


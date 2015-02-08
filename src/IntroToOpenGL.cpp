#include "IntroToOpenGL.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

mat4 BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation)
{
	mat4 result = glm::rotate(orbit_rotation, vec3(0, 1, 0)) *
		glm::translate(vec3(radius, 0, 0)) *
		glm::rotate(local_rotation, vec3(0, 1, 0));

	return result;
}


bool IntroToOpenGL::startup()
{
	if ( Application::startup() == false )
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_camera_x = -5.0f;
	m_camera_z = -5.0f;
	m_timer = 0.0f;
	
	for (int planet_index = 0;
		planet_index < PLANET_COUNT;
		++planet_index)
	{
		m_planet_sizes[planet_index] = ((rand() % 100) / 100.0f) * 5.0f;
	}

	return true;
}

bool IntroToOpenGL::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();

	Gizmos::addTransform(mat4(1), 10);

	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 yellow(1, 1, 0, 1);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	mat4 sun_transform = BuildOrbitMatrix(m_timer, 0, 0);

	Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 15, 15, yellow, &sun_transform);

	for (int planet_index = 0;
		planet_index < 15;
		++planet_index)
	{
		mat4 planet_mat = sun_transform *
			BuildOrbitMatrix(m_timer * (float)planet_index,
			(20 - planet_index) / 2.0f,
			m_timer * (float)planet_index * 0.2f);

		Gizmos::addSphere(planet_mat[3].xyz, 0.2f, 15, 15, blue, &planet_mat);

	}
	
	return true;
}

void IntroToOpenGL::shutdown()
{
	Gizmos::destroy();
}

void IntroToOpenGL::draw()
{
	mat4 view = glm::lookAt(vec3(5, 15 + (sinf(m_timer) * 3.0f), 0), 
								vec3(0, 0, 0), vec3(cosf(m_timer), 
								0,
								sinf(m_timer)));

	Gizmos::draw(m_projection, view);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
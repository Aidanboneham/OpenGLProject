#include "GPUParticles.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

bool GPUParticles::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glfwSwapInterval(0);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	m_emitter.Init(100, 
		vec3(0,0,0),
		0,
		1, 15, 
		0.8f, 5.0f, 
		0.01f, 0.01f, 
		vec4(1, 0, 0, 1), 
		vec4(1, 0.4f, 0, 1));

	m_time = 0; //ADDED THIS

	return true;
}

void GPUParticles::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool GPUParticles::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	static float t = 0;
	static unsigned int f = 0;

	t += dt;
	++f;

	if (t > 1)
	{
		printf("ms/f: %f, fps: %f\n", (t / f) * 1000.0f, (f / t));
		t -= 1.0f;
		f = 0;
	}

	m_time += dt; //ADDED THIS
	m_dt = dt;
	if (m_time > 1)
	{
		m_time -= 1;
	}

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
			i == 10 ? white : black);
	}

	m_camera.update(dt);

	return true;
}

void GPUParticles::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera.proj, m_camera.view);

	m_emitter.Draw(m_time, m_dt, m_camera.world, m_camera.view_proj);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
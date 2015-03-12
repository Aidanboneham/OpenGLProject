#include "Particles.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "Utility.h"

bool Particles::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	LoadShader("./shaders/particle_vertex.glsl",0,
				"./shaders/particle_fragment.glsl", &m_program_id);
	
	m_emitter.Init(10000, 
					vec3(0, 0, 0), 2000, 
					5, 5,
					2.5f, 5.0f,
					1, 0.05f,
					vec4(1, 0, 0, 1),
					vec4(1, 0.4f, 0, 1));

	return true;
}

void Particles::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool Particles::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();

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

	m_emitter.Update(dt);
	m_emitter.UpdateVertexData(m_camera.world[3].xyz, 
								m_camera.world[1].xyz);
	return true;
}

void Particles::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera.proj, m_camera.view);

	glUseProgram(m_program_id);

	int proj_view_uniform = glGetUniformLocation(m_program_id, "projection_view");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&m_camera.view_proj);

	m_emitter.Render();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
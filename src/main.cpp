#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include <cstdio>

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

mat4 BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation)
{
	mat4 result = glm::rotate(orbit_rotation, vec3(0, 1, 0)) *
							glm::translate(vec3(radius, 0, 0)) * 
								glm::rotate(local_rotation, vec3(0, 1, 0));

	return result;
}

int main()
{
	if (glfwInit() == false)
	{
		return -1;
	}

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	
	GLFWwindow* window = glfwCreateWindow(1280, 720,
						"Computer Graphics", nullptr, nullptr);
	if (window == nullptr)
	{
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();
	printf("Successfully loaded OpenGL version %d.%d\n",
							major_version, minor_version);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();


	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	float camera_x = -5.0f;
	float camera_z = -5.0f;

	float timer = 0.0f;


	const int PLANET_COUNT = 15; 

	float planet_sizes[PLANET_COUNT] = {};

	for (int planet_index = 0; 
		planet_index < PLANET_COUNT; 
		++planet_index)
	{
		planet_sizes[planet_index] = (rand() % 100) / 100.0f;
	}

	mat4 test_matrix = glm::translate(vec3(10, 10, 10));

	glfwSetTime(0.0);
	while (glfwWindowShouldClose(window) == false)
	{
		float dt = (float)glfwGetTime();
		glfwSetTime(0.0);
		timer += dt;

		mat4 view = glm::lookAt(vec3(5, 15 + (sinf(timer) * 3.0f), 0), vec3(0, 0, 0), vec3(cosf(timer), 0, sinf(timer)));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		Gizmos::addTransform( mat4(1), 10 );

		vec4 white(1);
		vec4 black(0, 0, 0, 1);
		vec4 red(1, 0, 0, 1);
		vec4 green(0, 1, 0, 1);
		vec4 blue(0, 0, 1, 1);
		vec4 yellow(1, 1, 0, 1);

		for (int i = 0; i <= 20; ++i)
		{
			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
				i == 10 ? white : black);
			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
				i == 10 ? white : black);
		}


		mat4 sun_transform = BuildOrbitMatrix(timer, 0, 0);

		Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 15, 15, yellow, &sun_transform);

		for (int planet_index = 0;
			planet_index < 15;
			++planet_index)
		{
			mat4 planet_mat = sun_transform * 
				BuildOrbitMatrix(timer * (float)planet_index, 
									(20 - planet_index) / 2.0f,
										timer * (float)planet_index * 0.2f);

			Gizmos::addSphere(planet_mat[3].xyz, 0.2f, 15, 15, blue, &planet_mat);

		}


		Gizmos::draw(projection, view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	Gizmos::destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
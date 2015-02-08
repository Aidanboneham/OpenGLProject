#include "Camera.h"

#include "GLFW/glfw3.h"

Camera::Camera(float aspect)
{
	view = glm::lookAt(vec3(0,0,0), vec3(0,0,1), vec3(0, 1, 0));
	world = glm::inverse(view);
	proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
	view_proj = proj * view;
}

void Camera::updateViewProj()
{
	view_proj = proj * view;
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	proj = glm::perspective(fov, aspect, near, far);
	updateViewProj();
}

void Camera::setLookAt(vec3 pos, vec3 center, vec3 up)
{
	view = glm::lookAt(pos, center, up);
	world = glm::inverse(view);
	updateViewProj();
}

void Camera::setPosition(vec3 pos)
{
	world[3] = vec4(pos, 1);
	view = glm::inverse(world);
	updateViewProj();
}


FlyCamera::FlyCamera(float aspect, float new_speed) : Camera(aspect)
{
	this->speed = new_speed;
}

void FlyCamera::update(float dt)
{
	GLFWwindow* curr_window = glfwGetCurrentContext();

	if (glfwGetKey(curr_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		world[3] -= world[2] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		world[3] += world[2] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		world[3] -= world[0] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		world[3] += world[0] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		world[3] -= world[1] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_E) == GLFW_PRESS)
	{
		world[3] += world[1] * speed * dt;
	}
	
	double x_delta , y_delta;

	glfwGetCursorPos(curr_window, &x_delta, &y_delta);
	glfwSetCursorPos(curr_window, 1280.f / 2.f, 720.f / 2.f);

	x_delta -= (1280.f / 2.f);
	y_delta -= (720.f / 2.f);

	x_delta /= (1280.f / 2.f);
	y_delta /= (720.f / 2.f);

	x_delta *= -sensitivity;
	y_delta *= -sensitivity;

	if (glfwGetMouseButton(curr_window, 1))
	{
		vec3 camera_right = (vec3)world[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		world[0] = rot * world[0];
		world[1] = rot * world[1];
		world[2] = rot * world[2];
	}

	view = glm::inverse(world);
	updateViewProj();
}

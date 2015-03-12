#include "Quaternions.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

bool Quaternions::startup()
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

	float PI = 3.14159f;

	m_hip_frames[0].position = vec3(0, 5, 0);
	m_hip_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_knee_frames[0].position = vec3(0, -2.5, 0);
	m_knee_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_ankle_frames[0].position = vec3(0, -2.5, 0);
	m_ankle_frames[0].rotation = glm::quat(vec3(-1, 0, 0));


	m_hip_frames[1].position = vec3(0, 5, 0);
	m_hip_frames[1].rotation = glm::quat(vec3(1, 0, 0));
	m_knee_frames[1].position = vec3(0, -2.5, 0);
	m_knee_frames[1].rotation = glm::quat(vec3(0, 0, 0));
	m_ankle_frames[1].position = vec3(0, -2.5, 0);
	m_ankle_frames[1].rotation = glm::quat(vec3(0, 0, 0));



	return true;
}

mat4 EvaluateKeyFrames(KeyFrame start, KeyFrame end, float t)
{
	vec3 pos = glm::mix(start.position, end.position, t);
	glm::quat rot = glm::slerp(start.rotation, end.rotation, t);
	mat4 result = glm::translate(pos) * glm::toMat4(rot);

	return result;
}

void Quaternions::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	mat4 test = glm::rotate(1.0f, vec3(1, 1, 1));
	test[3] = vec4(2, 12, 7, 1);
	test = glm::inverse(test);

	test = glm::perspective(3.14159f / 3.0f, 1280.0f / 720.0f, 1.0f, 5000.0f);

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();

	mat4 scale = glm::scale(vec3(2, 2, 2));
	mat4 rot = glm::rotate(25.f, vec3(0, 1, 0));
	mat4 trans = glm::translate(vec3(2, 2, 2));

	mat4 result = trans * rot * scale;


	m_timer += dt * 3.f;
	float sin_wave = sinf(m_timer) * 0.5f + 0.5f;

	m_hip_bone = EvaluateKeyFrames(m_hip_frames[0], m_hip_frames[1], sin_wave);
	m_knee_bone = EvaluateKeyFrames(m_knee_frames[0], m_knee_frames[1], sin_wave);
	m_ankle_bone = EvaluateKeyFrames(m_ankle_frames[0], m_ankle_frames[1], sin_wave);

	mat4 global_hip = m_hip_bone;
	mat4 global_knee = m_hip_bone * m_knee_bone;
	mat4 global_ankle = global_knee * m_ankle_bone;

	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(0, 1, 1, 1), &global_ankle);

	Gizmos::addLine(hip_pos, knee_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(knee_pos, ankle_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));


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

void Quaternions::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera.proj, m_camera.view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
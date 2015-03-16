#include "PostProcess.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Utility.h"


void PostProcess::generateFramebuffer()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_fbo_texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720); //CHANGED THIS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);

	glGenRenderbuffers(1, &m_fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720); //CHANGED THIS
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo_depth);

	GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER DID NOT GENERATE PROPERLY!!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::generateScreenspaceQuad()
{
	vec2 half_texel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertex_data[]
	{
		-1, -1, 0, 1,		half_texel.x, half_texel.y,
		1, -1, 0, 1,		1-half_texel.x, half_texel.y,
		1, 1, 0, 1,			1-half_texel.x, 1-half_texel.y,
		-1, 1, 0, 1,		half_texel.x, 1-half_texel.y,
	};

	unsigned int index_data[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_quad.m_VAO);
	
	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool PostProcess::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	generateScreenspaceQuad();
	generateFramebuffer();

	LoadShader("./shaders/post_vertex.glsl", 0, "./shaders/post_fragment.glsl", &m_post_program_id);

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	return true;
}

void PostProcess::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool PostProcess::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	Gizmos::clear();

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

	m_camera.update(dt);

	return true;
}


void PostProcess::draw()
{

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_camera.proj, m_camera.view);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_post_program_id);

	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
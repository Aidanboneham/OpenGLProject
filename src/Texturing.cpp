#include "Texturing.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "Utility.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Texturing::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	loadTexture("./textures/crate.png");
	generateSphere(5.0f, 20, 80);
	LoadShader("./shaders/textured_vertex.glsl", 0,
			"./shaders/textured_fragment.glsl", &m_program_id);


	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 1;
	
	return true;
}

void Texturing::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool Texturing::update()
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

	m_camera.update(dt);

	return true;
}

void Texturing::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | 
				GL_DEPTH_BUFFER_BIT);


	glUseProgram(m_program_id);

	int view_proj_uniform = glGetUniformLocation(m_program_id, 
												"projection_view");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE,
		(float*)&m_camera.view_proj);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	int diffuse_location = glGetUniformLocation(m_program_id,
														"diffuse");

	glUniform1i(diffuse_location, 0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 80 * 3, GL_UNSIGNED_INT, 0);


	Gizmos::draw(m_camera.proj, 
						m_camera.view);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Texturing::loadTexture(const char * filename)
{
	int width;
	int height;
	int channels;

	unsigned char *data = stbi_load(filename, 
									&width, &height, 
									&channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
									GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Texturing::generateQuad(float size)
{
	VertexTexCoord vertex_data[4];

	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * 4, 
									vertex_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, 
										index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
									sizeof(VertexTexCoord), 0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
					sizeof(VertexTexCoord), (void*)sizeof(vec4)); //tex coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


OpenGLData Texturing::generateSphere(float radius, int rows, int cols)
{
	OpenGLData result = {};
	
	VertexTexCoord * verts = new VertexTexCoord[cols + 1];

	verts[0].position = vec4(0, 0, 0, 1);
	verts[0].tex_coord = vec2(0.5, 0.5);

	for (int i = 0; i < cols; ++i)
	{
		verts[i + 1].position = vec4(sinf((i / (float)cols) * 2.f * 3.14159f), 
								 	 0, 
									 cosf((i / (float)cols) * 2.f * 3.14159f), 
									 1);

		verts[i + 1].tex_coord = vec2(verts[i + 1].position.x + 0.5f,
									  verts[i + 1].position.z + 0.5f);

	}








	unsigned int * indices = new unsigned int[3 * cols];

	for (int i = 0; i < cols; ++i)
	{
		indices[i * 3 + 0] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = (i + 2);
	}

	indices[3 * cols - 1] = 1;












	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * (cols + 1),
		verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (cols * 3),
		indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexTexCoord), 0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexTexCoord), (void*)sizeof(vec4)); //tex coord

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	delete[] verts;
	delete[] indices;


	return result;
}

#include "SceneManagement.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "tiny_obj_loader.h"
#include "Utility.h"


void RenderPlane(vec4 plane)
{
	vec3 up = vec3(0, 1, 0);
	if (plane.xyz() == vec3(0, 1, 0))
	{
		up = vec3(1, 0, 0);
	}

	vec3 tangent = glm::normalize(glm::cross(plane.xyz(), up));
	vec3 bitangent = glm::normalize(glm::cross(plane.xyz(), tangent));

	vec3 p = plane.xyz() * plane.w;

	vec3 v0 = p + tangent + bitangent;
	vec3 v1 = p + tangent - bitangent;
	vec3 v2 = p - tangent - bitangent;
	vec3 v3 = p - tangent + bitangent;

	Gizmos::addTri(v0, v1, v2, vec4(1, 1, 0, 1));
	Gizmos::addTri(v0, v2, v3, vec4(1, 1, 0, 1));

	Gizmos::addLine(p, p + plane.xyz() * 2, vec4(0,1,1,1));
}

AABB GenerateAABB(vec3* positions, 
					unsigned int count,
					unsigned int stride = 0)
{
	AABB result = {};

	if (stride == 0)
	{
		stride = sizeof(vec3);
	}

	result.min = positions[0];
	result.max = positions[0];

	for (unsigned int pos_index = 0;
		pos_index < count;
		++pos_index)
	{
		if (positions->x < result.min.x) result.min.x = positions->x;
		if (positions->y < result.min.y) result.min.y = positions->y;
		if (positions->z < result.min.z) result.min.z = positions->z;

		if (positions->x > result.max.x) result.max.x = positions->x;
		if (positions->y > result.max.y) result.max.y = positions->y;
		if (positions->z > result.max.z) result.max.z = positions->z;
		
		positions = (vec3*)((char*)positions + stride);
	}

	return result;
}

void RenderAABB(AABB aabb, mat4 transform)
{
	vec3 center = ((aabb.min + transform[3].xyz()) + (aabb.max + transform[3].xyz())) * 0.5f;
	vec3 extents = ((aabb.max + transform[3].xyz()) - (aabb.min + transform[3].xyz())) * 0.5f;

	Gizmos::addAABB(center, extents, vec4(1, 1, 1, 1), &transform);
}

//IMPORTANT(aidan): the planes array needs to be 6 elements big
void GetFrustumPlanes(mat4 view_proj, vec4* planes)
{
	// right side
	planes[0] = vec4(view_proj[0][3] - view_proj[1][0],
		view_proj[1][3] - view_proj[1][0],
		view_proj[2][3] - view_proj[2][0],
		view_proj[3][3] - view_proj[3][0]);
	// left side
	planes[1] = vec4(view_proj[0][3] + view_proj[0][0],
		view_proj[1][3] + view_proj[1][0],
		view_proj[2][3] + view_proj[2][0],
		view_proj[3][3] + view_proj[3][0]);
	// top
	planes[2] = vec4(view_proj[0][3] - view_proj[0][1],
		view_proj[1][3] - view_proj[1][1],
		view_proj[2][3] - view_proj[2][1],
		view_proj[3][3] - view_proj[3][1]);
	// bottom
	planes[3] = vec4(view_proj[0][3] + view_proj[0][1],
		view_proj[1][3] + view_proj[1][1],
		view_proj[2][3] + view_proj[2][1],
		view_proj[3][3] + view_proj[3][1]);
	// far
	planes[4] = vec4(view_proj[0][3] - view_proj[0][2],
		view_proj[1][3] - view_proj[1][2],
		view_proj[2][3] - view_proj[2][2],
		view_proj[3][3] - view_proj[3][2]);
	// near
	planes[5] = vec4(view_proj[0][3] + view_proj[0][2],
		view_proj[1][3] + view_proj[1][2],
		view_proj[2][3] + view_proj[2][2],
		view_proj[3][3] + view_proj[3][2]);

	for (int i = 0; i < 6; i++)
		planes[i] = glm::normalize(planes[i]);
}


bool OnPositivePlaneSide(vec4 plane, AABB aabb, mat4 transform)
{
	vec3 points[8];

	vec3 center = (aabb.min + aabb.max) * 0.5f;
	vec3 extents = (aabb.max - aabb.min) * 0.5f;

	for (unsigned int point = 0;
		point < 8;
		++point)
	{
		points[point] = center;
	}

	points[0] += vec3(extents.x, extents.y, extents.z);
	points[1] += vec3(-extents.x, extents.y, extents.z);
	points[2] += vec3(-extents.x, -extents.y, extents.z);
	points[3] += vec3(extents.x, -extents.y, extents.z);
	points[4] += vec3(extents.x, extents.y, -extents.z);
	points[5] += vec3(-extents.x, extents.y, -extents.z);
	points[6] += vec3(-extents.x,-extents.y, -extents.z);
	points[7] += vec3(extents.x, -extents.y, -extents.z);

	bool result = false;

	for (unsigned int point = 0;
		point < 8;
		++point)
	{

		vec4 transformed_point = transform * vec4(points[point], 1);
		float d = glm::dot(plane.xyz(), transformed_point.xyz()) + plane.w;
		
		if (d > 0)
		{
			result = true;
		}
	}

	return result;
}


void SceneManagement::LoadMesh(char* obj_filename)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	tinyobj::LoadObj(shapes, materials, obj_filename);

	unsigned int mesh_count = m_meshes.size();

	m_meshes.resize(m_meshes.size() + shapes.size());

	for (unsigned int shape_index = 0;
		shape_index < shapes.size();
		++shape_index)
	{
		unsigned int mesh_index = mesh_count + shape_index;

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

		m_meshes[mesh_index].m_index_count =
			shapes[shape_index].mesh.indices.size();
		
		glGenVertexArrays(1, &m_meshes[mesh_index].m_vao);
		glGenBuffers(1, &m_meshes[mesh_index].m_vbo);
		glGenBuffers(1, &m_meshes[mesh_index].m_ibo);

		glBindVertexArray(m_meshes[mesh_index].m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[mesh_index].m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * float_count,
			vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[mesh_index].m_ibo);
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

		m_meshes[mesh_index].m_aabb = GenerateAABB(
								(vec3*)&shapes[shape_index].mesh.positions[0],
								shapes[shape_index].mesh.positions.size() / 3);
	}
}

void SceneManagement::DrawMesh(MeshObject mesh)
{
	glBindVertexArray(mesh.m_vao);
	glDrawElements(GL_TRIANGLES, mesh.m_index_count, GL_UNSIGNED_INT, 0);
}

bool SceneManagement::startup()
{

	if (Application::startup() == false)
	{
		return false;
	}
	
	LoadShader("./shaders/lighting_vertex.glsl", 0,
		"./shaders/lighting_fragment.glsl", &m_program_id);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_camera.sensitivity = 3;

	//loading meshes here
	LoadMesh("./models/stanford/bunny.obj");

	float extents = 1000.0f;

	for (unsigned int mesh_index = 0;
		mesh_index < 1000;
		++mesh_index)
	{
		m_meshes.push_back(m_meshes[0]);
		m_meshes.back().m_transform = 
			glm::translate(vec3(glm::linearRand(-extents, extents), glm::linearRand(-extents, extents), glm::linearRand(-extents, extents))) *
			glm::rotate(glm::linearRand(-6.f, 6.f), glm::normalize(vec3(glm::linearRand(-10, 10), glm::linearRand(-10, 10), glm::linearRand(-10, 10))));
	}

	m_ambient_light = vec3(0.1f);
	m_light_dir = vec3(0, -1, 0);
	m_light_color = vec3(0.6f, 0, 0);
	m_material_color = vec3(1);
	m_specular_power = 15;

	return true;
}

void SceneManagement::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool SceneManagement::update()
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

void SceneManagement::draw()
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
	int world_unifrom =
		glGetUniformLocation(m_program_id, "world");

	glUniform3fv(ambient_uniform, 1, (float*)&m_ambient_light);
	glUniform3fv(light_dir_uniform, 1, (float*)&m_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&m_light_color);
	vec3 camera_pos = m_camera.world[3].xyz;
	glUniform3fv(eye_pos_uniform, 1, (float*)&camera_pos);
	glUniform1f(specular_uniform, m_specular_power);
	glUniform3fv(material_color_uniform, 1, (float*)&m_material_color);

	vec4 planes[6];
	GetFrustumPlanes(m_camera.view_proj, planes);

	for (unsigned int mesh_index = 0;
		mesh_index < m_meshes.size();
		++mesh_index)
	{
		bool is_in_frustum = true;

		for (unsigned int plane_index = 0;
			plane_index < 6;
			++plane_index)
		{
			if (!OnPositivePlaneSide(planes[plane_index], 
									m_meshes[mesh_index].m_aabb, 
									m_meshes[mesh_index].m_transform))
			{
				is_in_frustum = false;
				break;
			}
		}

		if (is_in_frustum)
		{
			glUniformMatrix4fv(world_unifrom, 1, GL_FALSE, (float*)&m_meshes[mesh_index].m_transform);
			DrawMesh(m_meshes[mesh_index]);
		}
	}

	Gizmos::draw(m_camera.proj, m_camera.view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
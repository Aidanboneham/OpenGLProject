#include "Emitter.h"
#include "gl_core_4_4.h"

Emitter::Emitter() : 
	m_particles(nullptr), m_max_particles(0), m_alive_count(0),
	m_vertex_data(nullptr), m_index_data(nullptr)
{
	m_buffers = {};
}

Emitter::~Emitter()
{
	delete[] m_particles;
	delete[] m_vertex_data;
	delete[] m_index_data;

	glDeleteVertexArrays(1, &m_buffers.m_VAO);
	glDeleteBuffers(1, &m_buffers.m_VBO);
	glDeleteBuffers(1, &m_buffers.m_IBO);

}

void Emitter::Init(unsigned int a_max_particles, vec3 a_position,
	float a_emit_rate,
	float a_min_lifespan, float a_max_lifespan,
	float a_min_velocity, float a_max_velocity,
	float a_start_size, float a_end_size,
	vec4 a_start_color, vec4 a_end_color)
{
	m_position = vec4(a_position, 1);

	m_lifespan_min = a_min_lifespan;
	m_lifespan_max = a_max_lifespan;
	m_velocity_min = a_min_velocity;
	m_velocity_max = a_max_velocity;
	m_start_size = a_start_size;
	m_end_size = a_end_size;
	m_start_color = a_start_color;
	m_end_color = a_end_color;

	m_emit_rate = 1.0f / a_emit_rate;

	m_max_particles = a_max_particles;
	m_alive_count = 0;

	m_particles = new Particle[m_max_particles];
	m_vertex_data = new VertexParticle[m_max_particles * 4];

	m_index_data = new unsigned int[m_max_particles * 6];
	
	for (unsigned int i = 0 ;
		i < m_max_particles;
		++i)
	{
		unsigned int start = 4 * i;

		m_index_data[i * 6 + 0] = start + 0;
		m_index_data[i * 6 + 1] = start + 1;
		m_index_data[i * 6 + 2] = start + 2;

		m_index_data[i * 6 + 3] = start + 0;
		m_index_data[i * 6 + 4] = start + 2;
		m_index_data[i * 6 + 5] = start + 3;
	}

	glGenVertexArrays(1, &m_buffers.m_VAO);
	glGenBuffers(1, &m_buffers.m_VBO);
	glGenBuffers(1, &m_buffers.m_IBO);

	glBindVertexArray(m_buffers.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_max_particles * 4 * sizeof(VertexParticle),
												m_vertex_data, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_max_particles * 6 * sizeof(unsigned int),
														m_index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), (void*)sizeof(vec4));


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Emitter::EmitParticles()
{
	unsigned int particles_to_emit = 
		(unsigned int)(m_emit_timer / m_emit_rate);
	m_emit_timer -= particles_to_emit * m_emit_rate;

	for (unsigned int i = 0;
		i < particles_to_emit && m_alive_count < m_max_particles;
		++i)
	{
		m_particles[m_alive_count].position = m_position;
		m_particles[m_alive_count].lifetime = 0;
		m_particles[m_alive_count].lifespan = 
			glm::linearRand(m_lifespan_min, m_lifespan_max);

		m_particles[m_alive_count].color = m_start_color;
		m_particles[m_alive_count].size = m_start_size;

		float velocity_length = 
			glm::linearRand(m_velocity_min, m_velocity_max);

		m_particles[m_alive_count].velocity.xyz = 
			glm::sphericalRand(velocity_length);
		m_particles[m_alive_count].velocity.w = 0;

		++m_alive_count;
	}
}

void Emitter::Update(float a_dt)
{
	//clean up the dead particles
	for (unsigned int i = 0;
		i < m_alive_count;
		++i)
	{
		m_particles[i].lifetime += a_dt;

		if (m_particles[i].lifetime > m_particles[i].lifespan)
		{
			--m_alive_count;
			m_particles[i] = m_particles[m_alive_count];
			--i;
		}
	}


	m_emit_timer += a_dt;
	EmitParticles();

	//move all the alive particles
	for (unsigned int i = 0;
		i < m_alive_count;
		++i)
	{
		m_particles[i].position += a_dt * m_particles[i].velocity;

		float t = m_particles[i].lifetime / m_particles[i].lifespan;
		m_particles[i].color = glm::mix(m_start_color, m_end_color, t);
		m_particles[i].size = glm::mix(m_start_size, m_end_size, t);
	}
}

void Emitter::UpdateVertexData(vec3 cam_pos, vec3 cam_up)
{
	for (unsigned int i = 0;
		i < m_alive_count;
		++i)
	{
		mat4 particle_transform(1);

		vec3 to = cam_pos - m_particles[i].position.xyz;
		
		vec3 f = glm::normalize(to);
		vec3 r = glm::cross(cam_up, f);
		vec3 u = glm::cross(f, r);

		f *= m_particles[i].size;
		r *= m_particles[i].size;
		u *= m_particles[i].size;

		particle_transform[0].xyz = r;
		particle_transform[1].xyz = u;
		particle_transform[2].xyz = f;
		particle_transform[3] = m_particles[i].position;

		m_vertex_data[i * 4 + 0].position = particle_transform * vec4(-1, 1, 0, 1);
		m_vertex_data[i * 4 + 1].position = particle_transform * vec4(-1, -1, 0, 1);
		m_vertex_data[i * 4 + 2].position = particle_transform * vec4(1, -1, 0, 1);
		m_vertex_data[i * 4 + 3].position = particle_transform * vec4(1, 1, 0, 1);

		m_vertex_data[i * 4 + 0].color = m_particles[i].color;
		m_vertex_data[i * 4 + 1].color = m_particles[i].color;
		m_vertex_data[i * 4 + 2].color = m_particles[i].color;
		m_vertex_data[i * 4 + 3].color = m_particles[i].color;
	}

}

void Emitter::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_alive_count * 4 * sizeof(VertexParticle), m_vertex_data);

	glBindVertexArray(m_buffers.m_VAO);
	glDrawElements(GL_TRIANGLES, 6 * m_alive_count, GL_UNSIGNED_INT, 0);
}
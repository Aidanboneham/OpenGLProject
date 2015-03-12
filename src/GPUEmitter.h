#ifndef GPU_EMITTER_H_
#define GPU_EMITTER_H_

#include "glm_header.h"

//gpu particle struct
struct GPUParticle
{
	GPUParticle() : lifespan(0), lifetime(1) {}

	vec3 position;
	vec3 velocity;
	float lifetime;
	float lifespan;
};

//gpu emitter class
	//creating gpu buffers
	//setting up stuff for transform feedback
	//creating the shaders
	//actually rendering them
class GPUPointEmitter
{
public:
	GPUPointEmitter();
	~GPUPointEmitter();

	void Init(unsigned int a_max_particles, vec3 a_position,
			float a_emit_rate,
			float a_min_lifespan, float a_max_lifespan,
			float a_min_velocity, float a_max_velocity,
			float a_start_size, float a_end_size,
			vec4 a_start_color, vec4 a_end_color);

	void Draw(float a_curr_time, float a_dt,
					mat4 a_camera_transform,
					mat4 a_projection_view);

	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	GPUParticle* m_particles;
	unsigned int m_max_particles;

	vec3 m_position;

	float m_lifespan_min;
	float m_lifespan_max;

	float m_velocity_min;
	float m_velocity_max;

	float m_start_size;
	float m_end_size;

	vec4 m_start_color;
	vec4 m_end_color;

	unsigned int m_active_buffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];

	unsigned int m_update_shader;
	unsigned int m_draw_shader;

	float m_last_draw_time;
};

#endif //GPU_EMITTER_H_
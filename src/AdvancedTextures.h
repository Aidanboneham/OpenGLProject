#ifndef _ADV_TEX
#define _ADV_TEX

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class AdvancedTextures : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void generateQuad(float size);
	void loadTextures();

	OpenGLData m_quad;

	unsigned int m_program_id;

	unsigned int m_diffuse_texture;
	unsigned int m_normal_texture;
	unsigned int m_specular_texture;
	unsigned int m_displacement_texture;


	vec3 m_ambient_light;
	vec3 m_light_dir;
	vec3 m_light_color;
	vec4 m_background_color;
	float m_specular_power;

	bool m_draw_gizmos;
	float m_fps;

	FlyCamera m_camera;

	TwBar* m_bar;
};




#endif //_ADV_TEX
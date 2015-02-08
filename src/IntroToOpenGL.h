#ifndef _INTRO_OGL_H_
#define _INTRO_OGL_H_

#include "Application.h"


const int PLANET_COUNT = 15;

class IntroToOpenGL : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	mat4 m_projection;

	float m_camera_x;
	float m_camera_z;

	float m_timer;

	float m_planet_sizes[PLANET_COUNT];
};

#endif //_INTRO_OGL_H_
#ifndef RENDER_TARGET_H_
#define RENDER_TARGET_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"


class RenderTargets : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void generateFramebuffer();
	void generatePlane();

	FlyCamera m_camera;

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	OpenGLData	m_plane;

	unsigned int m_program_id; //ADDED THIS

};

#endif //CAM_PROJ_H_
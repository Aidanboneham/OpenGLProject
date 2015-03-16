#ifndef POST_PROCESS_H_
#define POST_PROCESS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"


class PostProcess : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();


	void generateFramebuffer();
	void generateScreenspaceQuad();

	unsigned int m_fbo;
	unsigned int m_fbo_texture;
	unsigned int m_fbo_depth;

	unsigned int m_post_program_id;

	OpenGLData m_quad;







	FlyCamera m_camera;
};

#endif //CAM_PROJ_H_
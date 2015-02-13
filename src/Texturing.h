#ifndef TEXTURING_H_
#define TEXTURING_H_

#include "Application.h"
#include "Camera.h"

class Texturing : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void loadTexture(const char * filename);
	void generateQuad(float size);

	FlyCamera m_camera;

	unsigned int m_texture;
	unsigned int m_program_id;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
};

#endif //CAM_PROJ_H_
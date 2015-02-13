#ifndef RENDERING_GEO_H_
#define RENDERING_GEO_H_

#include "Application.h"
#include "Camera.h"

class RenderingGeometry : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	void generateGrid(unsigned int rows, unsigned int cols);

	void generateShader();
	unsigned int m_program_id;
	unsigned int m_index_count;


	FlyCamera m_camera;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	float m_timer;


};

#endif //RENDERING_GEO_H_
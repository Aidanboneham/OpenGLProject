#ifndef GPU_PARTICLES_H_
#define GPU_PARTICLES_H_

#include "Application.h"
#include "Camera.h"

#include "GPUEmitter.h"

class GPUParticles : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	FlyCamera m_camera;

	GPUPointEmitter m_emitter;

	float m_time; //ADDED THIS
	float m_dt; //ADDED THIS
};

#endif //CAM_PROJ_H_
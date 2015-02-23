#ifndef CAM_PROJ_H_
#define CAM_PROJ_H_

#include "Application.h"
#include "Camera.h"

#include "FBXFile.h"
#include "Vertex.h"


class Animation : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void GenerateGLMeshes(FBXFile* fbx);












	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton *skeleton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);

















	//file object that we load up
	FBXFile*					m_file;

	//opengl handles that opengl need to render stuff
	std::vector<OpenGLData>		m_meshes;

	FlyCamera					m_camera;
	float						m_timer;

	unsigned int				m_program_id;

};

#endif //CAM_PROJ_H_
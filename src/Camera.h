#ifndef CAMERA_H_
#define CAMERA_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;


class Camera
{
public:
	Camera(){}
	Camera(float aspect);

	void updateViewProj();
	void setPosition(vec3 pos);
	void setPerspective(float fov, float aspect, float near, float far);
	void setLookAt(vec3 pos, vec3 center, vec3 up);

	virtual void update(float dt) = 0;

	mat4 world;
	mat4 view;
	mat4 proj;
	mat4 view_proj;
};

class FlyCamera : public Camera
{
public:
	FlyCamera(){}
	FlyCamera(float aspect, float speed);
	virtual void update(float dt);

	float speed;
	float sensitivity;
};




#endif //CAMERA_H_
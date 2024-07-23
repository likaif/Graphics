#ifndef POSE_UTIL_H
#define POSE_UTIL_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

const float THRES_HOLD = acosf(0.5f);

glm::vec3 WorldUp(0.f, 1.f, 0.f);

static glm::mat4 FaceCamera(glm::vec3 position, glm::vec3 cameraPosition, float time = 3.f) {
	glm::vec3 lookAt = glm::normalize(position - cameraPosition);
	float per = glm::dot(lookAt, WorldUp);
	glm::vec3 newUp(glm::normalize(WorldUp - per * lookAt));

	glm::mat4 res(1.f);
	res = glm::translate(res, position);
	res = glm::scale(res, glm::vec3(glm::distance(position, cameraPosition) / time));
	std::cout << "time: " << time << ", scale: " << glm::distance(position, cameraPosition) / time << std::endl;

	// 2 formula
	glm::vec3 right = glm::cross(lookAt, newUp);
	glm::mat4 mat2(glm::mat3(right, newUp, -lookAt));

	//assert(mat == mat2);
	return res * mat2;
}

static glm::mat4 ResetPosePosition(glm::vec3 headPosition, glm::vec3 lookAt, glm::vec3& newPosition, float meters = 2.f) {
	glm::vec3 position = headPosition + meters * glm::normalize(lookAt);
	newPosition = position;

	float per = glm::dot(lookAt, WorldUp);
	float theta = acosf(per);
	if (theta < THRES_HOLD) {
		return FaceCamera(position, headPosition);
	}
	else {
		glm::vec3 newLookAt(glm::normalize(lookAt - WorldUp * per));
		// 2 formula
		glm::vec3 right = glm::cross(newLookAt, WorldUp);
		glm::mat4 mat2(glm::mat3(right, WorldUp, -newLookAt));
		return glm::translate(glm::mat4(1.f) , position) * mat2;
	}
}


#endif
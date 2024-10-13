#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "learnopengl/shader.h"
#include "model.h"

struct Plane {
	glm::vec3 norm;
	float dis = 0.f;

	Plane() = default;

	Plane(const glm::vec3& pl, const glm::vec3& normal) : norm(glm::normalize(normal)), dis(glm::dot(pl, norm)) {};

	void printInfo() {
		std::cout << "norm x: " << norm.x << ", y : " << norm.y << ", z: " << norm.z << ";" << std::endl;
	}
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;

	void printInfo() {
		std::cout << "top: ";
		topFace.printInfo();
		std::cout << "bottom: ";
		bottomFace.printInfo();
		std::cout << "leftFace: ";
		leftFace.printInfo();
		std::cout << "rightFace: ";
		rightFace.printInfo();
		std::cout << "nearFace: ";
		nearFace.printInfo();
		std::cout << "farFace: ";
		farFace.printInfo();
	}
};

Frustum CreateFrustumFromCamera(Camera camera)
{
	Frustum frustum;

	float fovy = glm::radians(camera.Zoom);

	float halfvSide = camera.zFar_ * tanf(fovy / 2.f);
	float halfhSide = halfvSide * camera.aspect_;
	glm::vec3 frontMultFar = camera.Front* camera.zFar_;

	frustum.nearFace = { camera.Position + camera.Front * camera.zNear_, camera.Front};
	frustum.farFace = { camera.Position + camera.Front * camera.zFar_, -camera.Front };
	frustum.leftFace = { camera.Position, glm::cross(frontMultFar - halfhSide * camera.Right, camera.Up)};
	frustum.rightFace = { camera.Position, glm::cross(frontMultFar + halfhSide * camera.Right, -camera.Up) };
	frustum.topFace = { camera.Position, glm::cross(frontMultFar + halfvSide * camera.Up, camera.Right) };
	frustum.bottomFace = { camera.Position, glm::cross(camera.Right, frontMultFar - halfvSide * camera.Up) };
	return frustum;
}

struct Volume
{
	virtual bool isOnFrustum(const Frustum& camFrustum,
		const Transform& modelTransform) const = 0;
};

struct Sphere : public Volume
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	float radius{ 0.f };

	bool isOnFrustum(const Frustum& camFrustum,
		const Transform& modelTransform) const
	{
		return true;
	}
};

struct Transform {
	/*SPACE INFORMATION*/
	// local space info
	glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
	glm::quat quaternion = glm::quat(1.0, 0.0, 0.0, 0.0);
	glm::vec3 scaling = glm::vec3(1.f, 1.f, 1.f);

	// global space info
	glm::mat4 model = glm::mat4(1.0f);

	bool isDirty = false;

	glm::mat4 GetLocalModel() {
		glm::mat4 rotate = glm::toMat4(quaternion);

		// translation * rotation * scale (also know as TRS matrix)
		return glm::translate(glm::mat4(1.0f), pos) *
			rotate *
			glm::scale(glm::mat4(1.0f), scaling);
	}
};

class Entity {
public:
	//Entity(const char* path) : Model(path) {}
	//Entity(Model model) : Model(path) {}

	// constructor, expects a filepath to a 3D model.
	Entity(Model& model) : pModel(&model)
	{
		//boundingVolume = std::make_unique<AABB>(generateAABB(model));
		//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
	}

	template <typename... TArgs>
	void AddChild(TArgs&... args)
	{
		children.emplace_back(std::make_unique<Entity>(args...));
		children.back()->parent = this;
	}

	void updateSelfAndChild()
	{
		if (parent) {
			trans.model = parent->trans.model * trans.GetLocalModel();
		}
		else {
			trans.model = trans.GetLocalModel();
		}

		for (auto&& child : children) {
			child->updateSelfAndChild();
		}
	}

	void drawSelfAndChild(Shader& shader, unsigned int& display, unsigned int& total)
	{
		shader.setMat4("model", trans.model);
		pModel->Draw(shader);
		display++;
		total++;

		for (auto&& child : children) {
			child->drawSelfAndChild(shader, display, total);
		}
	}

	Model* pModel = nullptr;

	Transform trans;

	/*SCENE GRAPH*/
	Entity* parent = nullptr;
	std::list<std::unique_ptr<Entity>> children;
};



#endif
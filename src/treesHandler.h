#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include "planet.h"
#include "cubemap.h"

class Planet;

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();

	void Draw();

private:
	Planet* planet;
	Shader* shader;
	Cubemap* noiseCubemapCPU;
	std::vector<glm::mat4> m_ModelMatrices;

	GLuint VAO, VBO, IBO;

	void SetupBuffers();
	void PlaceTrees(int numPoints);
	void PlaceTreesOnTriangle(int numSubdivisons, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
};
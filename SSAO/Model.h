#pragma once

#include <vector>
#include "Mesh.h"

using namespace DirectX;

struct Model {
	std::vector<Mesh> meshes;
};
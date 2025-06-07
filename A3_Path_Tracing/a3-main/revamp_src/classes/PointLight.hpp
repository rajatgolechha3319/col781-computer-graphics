#pragma once
#include <glm/glm.hpp>
#include "Ray.hpp"
struct PointLight {
    glm::vec3 location; // X,Y,Z
    glm::vec3 intensity; // R,G,B
};

Ray lumos_solem(PointLight source, glm::vec3 destination);

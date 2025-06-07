#pragma once
#include <glm/glm.hpp>

class Ray {
public:
    glm::vec3 o, d;
    Ray(glm::vec3 origin, glm::vec3 direction);
    glm::vec3 at(float t) const;
};
#include "Ray.hpp"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : o(origin), d(direction) {}

glm::vec3 Ray::at(float t) const {
    return o + t * d;
}
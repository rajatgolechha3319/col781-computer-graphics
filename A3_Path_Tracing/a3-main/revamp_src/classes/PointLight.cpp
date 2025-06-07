#include "PointLight.hpp"

Ray lumos_solem(PointLight source, glm::vec3 destination) {
    glm::vec3 direction = glm::normalize(destination - source.location);
    return Ray(source.location, direction);
}
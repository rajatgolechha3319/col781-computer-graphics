#pragma once
#include <glm/glm.hpp>
#include "Material.hpp"

class Material;

struct HitRecord {
    float t_range_min, t_range_max;
    float t;
    glm::vec3 p, n;
    Material* mat;
    int id;
};

HitRecord hit_record_init();
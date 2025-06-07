#include "Material.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

glm::vec3 Lambertian::brdf_compute(const HitRecord& rec, const glm::vec3& l_intensity, const glm::vec3& l, const glm::vec3& v) const {
    // Lambertian BRDF
    // std::cout << "Inside BRDF " << std::endl;
    // std::cout << "Light intensity: " << l_intensity.x << " " << l_intensity.y << " " << l_intensity.z << std::endl;
    glm::vec3 diff = l - rec.p;
    glm::vec3 brdf_constant = albedo / 3.14159265f;
    float cos_theta = glm::max(0.0f, glm::dot(rec.n, glm::normalize(diff)));
    glm::vec3 num = brdf_constant * l_intensity * cos_theta;
    float denom = glm::length2(diff);

    // Specular gloss
    glm::vec3 diff2 = v - rec.p;
    glm::vec3 h = glm::normalize(diff + diff2);
    float cos_alpha = glm::max(0.0f, glm::dot(rec.n, h));
    // Specular power is set to 16
    cos_alpha = glm::pow(cos_alpha, 16);
    glm::vec3 num2 = brdf_constant * l_intensity * cos_alpha;
    
    return ((glm::vec3(1.0f) - metallic_factor) * num / denom ) + ( metallic_factor * num2 / denom );
}

bool Lambertian::reflection(const HitRecord& rec, glm::vec3 v, glm::vec3& r, glm::vec3& kr) const {
    return false;
}

glm::vec3 Lambertian::emission(const HitRecord& rec, glm::vec3 v) const {
    glm::vec3 diff = v - rec.p;
    float cos_theta = glm::max(0.0f,glm::dot(rec.n, glm::normalize(diff)));
    return luminosity * cos_theta;
}
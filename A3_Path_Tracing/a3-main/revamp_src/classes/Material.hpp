#pragma once
#include <glm/glm.hpp>
#include "HitRecord.hpp"
#include <cmath>
#include <iostream>

class HitRecord;
class Material {
    public:
    glm::vec3 metallic_factor;
    glm::vec3 reflectivity;
    glm::vec3 luminosity = glm::vec3(0.0f);
    Material() : metallic_factor(glm::vec3(0.0f)), reflectivity(glm::vec3(0.1f)) {}

    void set_luminosity(glm::vec3 l){
        luminosity = l;
    }
    
    virtual glm::vec3 emission(const HitRecord& rec, glm::vec3 v) const { return glm::vec3(0.0); }
    virtual glm::vec3 brdf_compute(const HitRecord& rec, const glm::vec3& l_intensity, const glm::vec3& l, const glm::vec3& v) const = 0;
    virtual bool reflection(const HitRecord& rec, glm::vec3 v, glm::vec3& r, glm::vec3& kr) const = 0;
    virtual glm::vec3 get_albedo() const { return glm::vec3(0.0f); }
    void set_metallic_factor(glm::vec3 f) { metallic_factor = f; }
    void set_reflectivity(glm::vec3 f) { reflectivity = f; }
};

class Lambertian : public Material {
public:
    glm::vec3 albedo; // In RGB [0,1]



    Lambertian(glm::vec3 albedo) : albedo(albedo) {}

    
    virtual glm::vec3 brdf_compute(const HitRecord& rec, const glm::vec3& l_intensity, const glm::vec3& l,const glm::vec3& v) const override;

    virtual bool reflection(const HitRecord& rec, glm::vec3 v, glm::vec3& r, glm::vec3& kr) const override;

    virtual glm::vec3 emission(const HitRecord& rec, glm::vec3 v) const override;

    virtual glm::vec3 get_albedo() const override { return albedo; }


};
#include "Object.hpp"
#include "HitRecord.hpp"


Object::Object(Shape* shape, Material* mat, glm::mat4 M) : shape(shape), mat(mat) {
    // Set the center of the object
    if(shape != nullptr){
        set_center(shape->get_center_of_jalebi());
    }
}

void Object::set_center (glm::vec3 bindu){
    center_of_jalebi = bindu;
}

glm::vec3 Object::get_center_of_jalebi() const {
    return center_of_jalebi;
}

bool Object::hit(Ray ray, HitRecord& rec) const {
    if(!affine){
        bool hit_val = shape->hit(ray, rec);
        if(hit_val){
            rec.mat = mat;
        }
        return hit_val;
    }
    else{
        // First transform the ray to object space
        glm::vec3 origin = glm::vec3(inv_M * glm::vec4(ray.o, 1.0));
        glm::vec3 direction = glm::vec3(inv_M * glm::vec4(ray.d, 0.0));
        Ray obj_ray(origin, direction);

        // Now let it hit
        bool hit_val = shape->hit(obj_ray, rec);
        if(hit_val){
            rec.mat = mat;
            // Now transform the hit record back to world space
            rec.p = glm::vec3(M * glm::vec4(rec.p, 1.0));
            rec.n = glm::normalize(glm::vec3(inv_T_M * glm::vec4(rec.n, 0.0)));
        }
        return hit_val;
    }
    // Nothing should reach here
    // Add an error message

    // Raise an exception
    throw std::runtime_error("Error in Object::hit: Control should not reach here");
}

void Object::set_transform(glm::mat4 M){
    // True means global, false means local
    this->affine = true;
    this->M = M;
    this->inv_M = glm::inverse(M);
    // this->inv_T_M = glm::transpose(inv_M);
    this->inv_T_M = glm::transpose(glm::mat3(this->inv_M));
}
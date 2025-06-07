#pragma once
#include "Shape.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Object {
private:
    glm::vec3 center_of_jalebi;
    void set_center(glm::vec3 bindu);
    //
    friend class Shape;
public:
    glm::vec3 get_center_of_jalebi() const;
    Shape* shape;
    Material* mat;
    Object(Shape* shape, Material* mat, glm::mat4 M = glm::mat4(1.0));
    bool hit(Ray ray, HitRecord& rec) const;

    // Affine functions for objects implementation
    glm::mat4 M = glm::mat4(1.0); // Transformation matrix
    glm::mat4 inv_M = glm::mat4(1.0); // Inverse of transformation matrix
    glm::mat4 inv_T_M = glm::mat4(1.0); // Inverse of transpose of transformation matrix
    bool affine = false; // Flag to check if object has affine transformation
    void set_transform(glm::mat4 M); // For affine transformations
    
    // For transformation about itself

};
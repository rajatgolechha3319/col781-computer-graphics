#pragma once
#include "Ray.hpp"
#include "HitRecord.hpp"
#include <string>
#include <fstream>
#include <vector>

class Shape {
private:
    virtual glm::vec3 get_center_of_jalebi() const = 0;
    friend class Object;
public:
    virtual bool hit(Ray ray, HitRecord& rec) const = 0;
};

class Sphere : public Shape {
    virtual glm::vec3 get_center_of_jalebi() const override;
public:
    glm::vec3 c;
    float r;
    int id;
    Sphere(glm::vec3 center, float radius, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
};

class Inf_Plane : public Shape {
    virtual glm::vec3 get_center_of_jalebi() const override;
public:
    glm::vec3 n;
    float d1;
    int id;
    Inf_Plane(glm::vec3 normal, float distance, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
};

class AA_BB : public Shape {
    virtual glm::vec3 get_center_of_jalebi() const override;
public:
    // Axis-aligned bounding box
    // Box is specified from 3 pairs of x,y,z ranges
    float x0, x1, y0, y1, z0, z1;
    int id;
    AA_BB(float x0, float x1, float y0, float y1, float z0, float z1, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
};

class Three_Aces : public Shape {
    virtual glm::vec3 get_center_of_jalebi() const override;
public:
    // Three Aces is a general conic section class
    // Input is A1,A2,A3,c
    // Equation of curve is p^T A p + c = 0
    // A = diag(A1,A2,A3)
    // p = (x,y,z)

    glm::vec3 A;
    float c;
    int id;
    Three_Aces(glm::vec3 A, float c, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
};

class Open_Cylinder : public Shape {
    virtual glm::vec3 get_center_of_jalebi() const override;
public:
    // Input will be center, r1,r2, h
    // r1 = outer radius
    // r2 = inner radius
    // h = height
    // Cylinder is open at top and bottom
    // Cylinder is aligned along y-axis

    glm::vec3 c;
    float r1, r2, h;
    int id;
    Open_Cylinder(glm::vec3 center, float r1, float r2, float h, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
};



struct TRIANGLE {
    glm::vec3 v1, v2, v3;
    TRIANGLE() : v1(0.0f), v2(0.0f), v3(0.0f) {}
    TRIANGLE(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : v1(v1), v2(v2), v3(v3) {}
};

struct OBJ_FILE {
    std::string filename;
    std::vector<TRIANGLE> triangles; // A closed mesh

    // Function to load the mesh
    std::pair<glm::vec3,float> load_mesh(const std::string &filepath);
};

class Triangular_Mesh : public Shape {
private:
    std::pair<glm::vec3,float> load_mesh_tri(std::string filepath);
    virtual glm::vec3 get_center_of_jalebi() const override{
        return glm::vec3(0.0f); // No implementation
    };
    Sphere* bounding_sphere;

public:
    std::string filepath;
    int id;
    OBJ_FILE mesh_obj;
    Triangular_Mesh(std::string filepath, int id);
    virtual bool hit(Ray ray, HitRecord& rec) const override;
    // ~Triangular_Mesh() {
    //     delete bounding_sphere; // Avoid memory leaks
    // }

};

// CLASS 9 ACES FOR THE GENERAL CONIC SECTION TO BE ADDED HERE
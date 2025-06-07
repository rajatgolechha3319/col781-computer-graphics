#include "Shape.hpp"
#include <glm/glm.hpp>
#include <cfloat>    
#include <cmath>     
#include <algorithm> 
#include <iostream>

// Constructors
Sphere::Sphere(glm::vec3 center, float radius, int id) : c(center), r(radius), id(id) {}
Inf_Plane::Inf_Plane(glm::vec3 normal, float distance, int id) : n(normal), d1(distance), id(id) {}
AA_BB::AA_BB(float x0, float x1, float y0, float y1, float z0, float z1, int id) : x0(x0), x1(x1), y0(y0), y1(y1), z0(z0), z1(z1), id(id) {}
Three_Aces::Three_Aces(glm::vec3 A, float c, int id) : A(A), c(c), id(id) {}
Open_Cylinder::Open_Cylinder(glm::vec3 center, float r1, float r2, float h, int id) : c(center), r1(r1), r2(r2), h(h), id(id) {}
Triangular_Mesh::Triangular_Mesh(std::string filepath, int id) : id(id), filepath(filepath) {
    std::pair<glm::vec3,float> x = load_mesh_tri(filepath);
    glm::vec3 centroid = x.first;
    float max_dist = x.second;
    bounding_sphere = new Sphere(centroid, max_dist, -1);

    // std::cout << "Loaded mesh with centroid: " << centroid.x << " " << centroid.y << " " << centroid.z << std::endl;
    // std::cout << "Max distance from centroid: " << max_dist << std::endl;
    // std::cout << "Bounding sphere radius: " << max_dist << std::endl;
    // std::cout << "Bounding sphere center: " << centroid.x << " " << centroid.y << " " << centroid.z << std::endl;
    // std::cout << "Number of triangles: " << mesh_obj.triangles.size() << std::endl;

    // // Print the min y coordinate of the triangles
    // float min_y = FLT_MAX;
    // for(auto &t : mesh_obj.triangles){
    //     min_y = std::min(min_y, std::min(t.v1.y, std::min(t.v2.y, t.v3.y)));
    // }
    // std::cout << "Min y coordinate of triangles: " << min_y << std::endl;

}


// String splitter
void split_string(std::string s, std::vector<std::string> &res){
    res.clear();
    std::string my = "";
    int i=0;
    while(i < s.size()){
        if(s[i] == ' ' && my.size() > 0){
            res.push_back(my);
            my = "";
        } else if(s[i] != ' '){
            my += s[i];
        }
        i++;
    }
    if(my.size() > 0){
        res.push_back(my);
    }
}

// OBJ Mesh loader
std::pair<glm::vec3,float> OBJ_FILE::load_mesh(const std::string &filepath){
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::string> temp;
    std::vector<glm::vec3> vertices;
    std::cout << "Parsing obj file" << std::endl;
    glm::vec3 centroid = glm::vec3(0.0f);
    while (std::getline(file, line)) {
        split_string(line, temp);
        if(temp[0] == "v"){
            // Assume that the vertices are in the format v x y z
            vertices.push_back(glm::vec3(std::stof(temp[1]), std::stof(temp[2]), std::stof(temp[3])));
            centroid += glm::vec3(std::stof(temp[1]), std::stof(temp[2]), std::stof(temp[3]));
        }
        else if(temp[0] == "f"){
            // Assuming only triangles
            int idx1, idx2, idx3;
            idx1 = std::stoi(temp[1].substr(0, temp[1].find('/'))) - 1;
            idx2 = std::stoi(temp[2].substr(0, temp[2].find('/'))) - 1;
            idx3 = std::stoi(temp[3].substr(0, temp[3].find('/'))) - 1;
            triangles.push_back(TRIANGLE(vertices[idx1], vertices[idx2], vertices[idx3]));
        }
        // else{
        //     // IGNORE  // PASS // 

        // }
    }
    centroid /= (float) vertices.size();
    float max_dist = 0.0f;
    for(auto &v : vertices){
        max_dist = std::max(max_dist, glm::length(v - centroid));
    }
    return std::make_pair(centroid, max_dist);
    
}

std::pair<glm::vec3,float> Triangular_Mesh::load_mesh_tri(std::string filepath){
    std::pair<glm::vec3,float> x = mesh_obj.load_mesh(filepath);

    // Print the triangles
    // for(auto &t : mesh_obj.triangles){
    //     std::cout << "Triangle: " << t.v1.x << " " << t.v1.y << " " << t.v1.z << " | "
    //               << t.v2.x << " " << t.v2.y << " " << t.v2.z << " | "
    //               << t.v3.x << " " << t.v3.y << " " << t.v3.z << std::endl;
    // }

    return x;
}

// Center functions
glm::vec3 Sphere::get_center_of_jalebi() const { return c; }

glm::vec3 Inf_Plane::get_center_of_jalebi() const {
    if(this->n.x != 0){
        return glm::vec3(this->d1 / this->n.x, 0, 0);
    }
    else if(this->n.y != 0){
        return glm::vec3(0, this->d1 / this->n.y, 0);
    }
    return glm::vec3(0, 0, this->d1 / this->n.z);
}

glm::vec3 AA_BB::get_center_of_jalebi () const {
    return glm::vec3((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2);
}

glm::vec3 Three_Aces::get_center_of_jalebi() const {
    // For now return origin
    return glm::vec3(0.0f);
}

glm::vec3 Open_Cylinder::get_center_of_jalebi() const {
    return c;
}

// Hit functions
bool Sphere::hit(Ray ray, HitRecord& rec) const {
    glm::vec3 co = c - ray.o;
    float t_proj = glm::dot(co, ray.d) / glm::length(ray.d);
    float ray_sphere_distance = glm::length(co - t_proj * ray.d);

    if (ray_sphere_distance > r + 1e-3) return false;

    float t_offset = sqrt(r * r - ray_sphere_distance * ray_sphere_distance);
    float t1 = t_proj - t_offset;
    float t2 = t_proj + t_offset;

    float t_selected = (t1 > rec.t_range_min && t1 < rec.t_range_max) ? t1 : t2;
    if (!(t_selected > rec.t_range_min && t_selected < rec.t_range_max)) return false;

    if (t_selected < rec.t) {
        rec.t = t_selected;
        rec.p = ray.o + t_selected * ray.d;
        // Check if we are inside 
        if(glm::length(ray.o - c) < r){
            rec.n = glm::normalize(c - rec.p);
        }
        else{
            rec.n = glm::normalize(rec.p - c);
        }
        rec.id = id;
        return true;
    }
    return false;
}

bool Inf_Plane::hit(Ray ray,  HitRecord& rec) const {
    // Plane equation: n * (p) = d1
    // Ray equation: p = o + t * d

    float t = (d1 - glm::dot(n, ray.o)) / glm::dot(n, ray.d); // Obvious

    if (t < rec.t_range_min || t > rec.t_range_max) return false;

    if (t < rec.t) {
        rec.t = t;
        rec.p = ray.o + t * ray.d;
        // rec.n = glm::normalize(n);
        if(glm::dot(n, ray.d) > 0){
            rec.n = glm::normalize(-n);
        }
        else{
            rec.n = glm::normalize(n);
        }

        rec.id = id;
        return true;
    }
    return false;
}

bool AA_BB::hit(Ray ray,  HitRecord& rec) const {
    // std::cout << "Checking intersection with AA_BB" << std::endl;
    // My Code
    // AABB is from x0 to x1, y0 to y1, z0 to z1
    glm::vec3 t0,t1;
    if(ray.d.x < 0){
        t0.x = (x1 - ray.o.x) / ray.d.x;
        t1.x = (x0 - ray.o.x) / ray.d.x;
    }
    else{
        t0.x = (x0 - ray.o.x) / ray.d.x;
        t1.x = (x1 - ray.o.x) / ray.d.x;
    }

    if(ray.d.y < 0){
        t0.y = (y1 - ray.o.y) / ray.d.y;
        t1.y = (y0 - ray.o.y) / ray.d.y;
    }
    else{
        t0.y = (y0 - ray.o.y) / ray.d.y;
        t1.y = (y1 - ray.o.y) / ray.d.y;
    }

    if(ray.d.z < 0){
        t0.z = (z1 - ray.o.z) / ray.d.z;
        t1.z = (z0 - ray.o.z) / ray.d.z;
    }
    else{
        t0.z = (z0 - ray.o.z) / ray.d.z;
        t1.z = (z1 - ray.o.z) / ray.d.z;
    }

    float s_close = std::max(std::max(t0.x, t0.y), t0.z);
    float s_far = std::min(std::min(t1.x, t1.y), t1.z);

    if(s_close > s_far){
        return false;
    }

    // Check first if we are inside or not
    if(ray.o.x >= x0 && ray.o.x <= x1){
        if(ray.o.y >= y0 && ray.o.y <= y1){
            if(ray.o.z >= z0 && ray.o.z <= z1){
                // Handling inside part
                // Hit is at s_far when inside
                if(s_far < rec.t_range_min || s_far > rec.t_range_max){
                    return false;
                }

                // Compute normal and record hit
                // Inside normal is opposite than outside
                if(s_far < rec.t){
                    rec.t = s_far;
                    rec.p = ray.o + s_far * ray.d;
                    glm::vec3 normal(0.0f);
                    if(fabs(rec.p.x - x0) < 1.42e-4){
                        normal.x = 1.0f;
                    }else if(fabs(rec.p.x - x1) < 1.12e-4){
                        normal.x = -1.0f;
                    }
                    if(fabs(rec.p.y - y0) < 1.13e-4){
                        normal.y = 1.0f;
                    }else if(fabs(rec.p.y - y1) < 2.13e-4){
                        normal.y = -1.0f;
                    }
                    if(fabs(rec.p.z - z0) < 3.13e-4){
                        normal.z = 1.0f;
                    }else if(fabs(rec.p.z - z1) < 3.43e-4){
                        normal.z = -1.0f;
                    }
                    rec.n = glm::normalize(normal);
                    rec.id = id;
                    return true;
                }
                return false;
            }
        }
    }
        // Handling outside part
        if(s_close > rec.t_range_max || s_close < rec.t_range_min){
            return false;
        }
        if(s_close < rec.t){
            rec.t = s_close;
            rec.p = ray.o + s_close * ray.d;
            glm::vec3 normal(0.0f);
            if(fabs(rec.p.x - x0) < 3.47e-4){
                normal.x = -1.0f;
            }else if(fabs(rec.p.x - x1) < 3.87e-4){
                normal.x = 1.0f;
            }
            if(fabs(rec.p.y - y0) < 1.87e-4){
                normal.y = -1.0f;
            }else if(fabs(rec.p.y - y1) < 1.27e-4){
                normal.y = 1.0f;
            }
            if(fabs(rec.p.z - z0) < 1.29e-4){
                normal.z = -1.0f;
            }else if(fabs(rec.p.z - z1) < 1.69e-4){
                normal.z = 1.0f;
            }
            rec.n = glm::normalize(normal);
            rec.id = id;
            return true;
        }
        return false;
}

bool Three_Aces::hit(Ray ray,  HitRecord& rec) const {
    // Equation of curve is p^T A p + c = 0
    ///
    /// COMMENT : Works with ellipsoid but not hyperboloid
    ///
    /// On solving the above it gives us
    /// t^2 * (a1d1^2 +a2d2^2 + a3d3^2) + t * 2 * (a1d1 * o1 + a2d2 * o2 + a3d3 * o3) + (a1o1^2 + a2o2^2 + a3o3^2 + c) = 0
    ///

    // Let's solve the above quadratic equation
    float quad_a = A.x * ray.d.x * ray.d.x + A.y * ray.d.y * ray.d.y + A.z * ray.d.z * ray.d.z;
    float quad_b = 2 * (A.x * ray.d.x * ray.o.x + A.y * ray.d.y * ray.o.y + A.z * ray.d.z * ray.o.z);
    float quad_c = A.x * ray.o.x * ray.o.x + A.y * ray.o.y * ray.o.y + A.z * ray.o.z * ray.o.z + c;

    float disc = quad_b * quad_b - 4 * quad_a * quad_c;

    if(disc < 0){
        return false;
    }

    // Linear case
    if(quad_a < 1.29e-4){
        float t = -quad_c / quad_b;
        if(t < rec.t_range_min || t > rec.t_range_max){
            return false;
        }
        if(t < rec.t){
            rec.t = t;
            rec.p = ray.o + t * ray.d;
            rec.n = glm::normalize(glm::vec3(2 * A.x * rec.p.x, 2 * A.y * rec.p.y, 2 * A.z * rec.p.z));
            rec.id = id;
            return true;
        }
        return false;
    }


    // Non linear Case
    float t1 = (-quad_b + glm::sqrt(disc)) / (2 * quad_a);
    float t2 = (-quad_b - glm::sqrt(disc)) / (2 * quad_a);

    float t_selected;
    // If both in range select the minimum
    if(t1 > rec.t_range_min && t1 < rec.t_range_max && t2 > rec.t_range_min && t2 < rec.t_range_max){
        t_selected = std::min(t1, t2);
    }
    else if(t1 > rec.t_range_min && t1 < rec.t_range_max){
        t_selected = t1;
    }
    else if(t2 > rec.t_range_min && t2 < rec.t_range_max){
        t_selected = t2;
    }
    else{
        return false;
    }

    if(t_selected < rec.t){
        rec.t = t_selected;
        rec.p = ray.o + t_selected * ray.d;
        rec.n = glm::normalize(glm::vec3(2 * A.x * rec.p.x, 2 * A.y * rec.p.y, 2 * A.z * rec.p.z));
        rec.id = id;
        return true;
    }
    return false;
}

bool Open_Cylinder::hit(Ray ray, HitRecord& rec) const {
    const float EPSILON = 1e-5;
    bool hit = false;
    
    // Top surface (y = c.y + h/2)
    float t_top = (c.y + h / 2 - ray.o.y) / ray.d.y;
    float x_top = ray.o.x + t_top * ray.d.x;
    float z_top = ray.o.z + t_top * ray.d.z;
    float squared_dist_top = (x_top - c.x) * (x_top - c.x) + (z_top - c.z) * (z_top - c.z);
    if(t_top > rec.t_range_min && t_top < rec.t_range_max &&
       squared_dist_top >= (r1 * r1 - EPSILON) && squared_dist_top <= (r2 * r2 + EPSILON)) {
        if(t_top < rec.t) {
            rec.t = t_top;
            rec.p = ray.o + t_top * ray.d;
            rec.n = glm::normalize(glm::vec3(0, 1, 0));
            rec.id = id;
            hit = true;
        }
    }

    // Bottom surface (y = c.y - h/2)
    float t_bottom = (c.y - h / 2 - ray.o.y) / ray.d.y;
    float x_bottom = ray.o.x + t_bottom * ray.d.x;
    float z_bottom = ray.o.z + t_bottom * ray.d.z;
    float squared_dist_bottom = (x_bottom - c.x) * (x_bottom - c.x) + (z_bottom - c.z) * (z_bottom - c.z);
    if(t_bottom > rec.t_range_min && t_bottom < rec.t_range_max &&
       squared_dist_bottom >= (r1 * r1 - EPSILON) && squared_dist_bottom <= (r2 * r2 + EPSILON)) {
        if(t_bottom < rec.t) {
            rec.t = t_bottom;
            rec.p = ray.o + t_bottom * ray.d;
            rec.n = glm::normalize(glm::vec3(0, -1, 0));
            rec.id = id;
            hit = true;
        }
    }

    // Inner cylinder (radius = r1)
    float q1_a = ray.d.x * ray.d.x + ray.d.z * ray.d.z;
    float q1_b = 2 * ((ray.o.x - c.x) * ray.d.x + (ray.o.z - c.z) * ray.d.z);
    float q1_c = (ray.o.x - c.x) * (ray.o.x - c.x) + (ray.o.z - c.z) * (ray.o.z - c.z) - r1 * r1;
    float disc1 = q1_b * q1_b - 4 * q1_a * q1_c;
    if(disc1 >= 0) {
        float sqrt_disc1 = glm::sqrt(disc1);
        float t1 = (-q1_b + sqrt_disc1) / (2 * q1_a);
        float t2 = (-q1_b - sqrt_disc1) / (2 * q1_a);

        float y_t1 = ray.o.y + t1 * ray.d.y;
        float y_t2 = ray.o.y + t2 * ray.d.y;
        if(t1 > rec.t_range_min && t1 < rec.t_range_max &&
           y_t1 >= c.y - h / 2 - EPSILON && y_t1 <= c.y + h / 2 + EPSILON) {
            if(t1 < rec.t) {
                rec.t = t1;
                rec.p = ray.o + t1 * ray.d;
                rec.n = glm::normalize(glm::vec3((rec.p.x - c.x) / r1, 0, (rec.p.z - c.z) / r1));
                rec.id = id;
                hit = true;
            }
        }

        if(t2 > rec.t_range_min && t2 < rec.t_range_max &&
           y_t2 >= c.y - h / 2 - EPSILON && y_t2 <= c.y + h / 2 + EPSILON) {
            if(t2 < rec.t) {
                rec.t = t2;
                rec.p = ray.o + t2 * ray.d;
                rec.n = glm::normalize(glm::vec3((rec.p.x - c.x) / r1, 0, (rec.p.z - c.z) / r1));
                rec.id = id;
                hit = true;
            }
        }
    }

    // Outer cylinder (radius = r2)
    float q2_a = ray.d.x * ray.d.x + ray.d.z * ray.d.z;
    float q2_b = 2 * ((ray.o.x - c.x) * ray.d.x + (ray.o.z - c.z) * ray.d.z);
    float q2_c = (ray.o.x - c.x) * (ray.o.x - c.x) + (ray.o.z - c.z) * (ray.o.z - c.z) - r2 * r2;
    float disc2 = q2_b * q2_b - 4 * q2_a * q2_c;
    if(disc2 >= 0) {
        float sqrt_disc2 = glm::sqrt(disc2);
        float t1 = (-q2_b + sqrt_disc2) / (2 * q2_a);
        float t2 = (-q2_b - sqrt_disc2) / (2 * q2_a);

        float y_t1 = ray.o.y + t1 * ray.d.y;
        float y_t2 = ray.o.y + t2 * ray.d.y;
        if(t1 > rec.t_range_min && t1 < rec.t_range_max &&
           y_t1 >= c.y - h / 2 - EPSILON && y_t1 <= c.y + h / 2 + EPSILON) {
            if(t1 < rec.t) {
                rec.t = t1;
                rec.p = ray.o + t1 * ray.d;
                rec.n = glm::normalize(glm::vec3((rec.p.x - c.x) / r2, 0, (rec.p.z - c.z) / r2));
                rec.id = id;
                hit = true;
            }
        }

        if(t2 > rec.t_range_min && t2 < rec.t_range_max &&
           y_t2 >= c.y - h / 2 - EPSILON && y_t2 <= c.y + h / 2 + EPSILON) {
            if(t2 < rec.t) {
                rec.t = t2;
                rec.p = ray.o + t2 * ray.d;
                rec.n = glm::normalize(glm::vec3((rec.p.x - c.x) / r2, 0, (rec.p.z - c.z) / r2));
                rec.id = id;
                hit = true;
            }
        }
    }

    return hit;
}

bool triangle_hit_help(Ray ray, HitRecord& rec, const TRIANGLE& tri) {
    // Construct the plane from the triangle
    glm::vec3 normal_plane = glm::normalize(glm::cross(tri.v2 - tri.v1, tri.v3 - tri.v1));
    float d_plane = glm::dot(normal_plane, tri.v1);

    // Equation of plane is n * p = d

    float t = (d_plane - glm::dot(normal_plane, ray.o)) / glm::dot(normal_plane, ray.d); // Obvious

    // Check if the point is inside the triangle
    glm::vec3 p = ray.o + t * ray.d;

    // Check if v3, p are on same side of v1,v2
    if(glm::dot(
        glm::cross(tri.v2 - tri.v1, p - tri.v1),
        glm::cross(tri.v2 - tri.v1, tri.v3 - tri.v1)
    ) < 0){
        return false;
    }

    // Check if v1, p are on same side of v2,v3
    if(glm::dot(
        glm::cross(tri.v3 - tri.v2, p - tri.v2),
        glm::cross(tri.v3 - tri.v2, tri.v1 - tri.v2)
    ) < 0){
        return false;
    }

    // Check if v2, p are on same side of v3,v1
    if(glm::dot(
        glm::cross(tri.v1 - tri.v3, p - tri.v3),
        glm::cross(tri.v1 - tri.v3, tri.v2 - tri.v3)
    ) < 0){
        return false;
    }

    // IN TRIANGLE

    if (t < rec.t_range_min || t > rec.t_range_max) return false;

    if (t < rec.t) {
        rec.t = t;
        rec.p = ray.o + t * ray.d;
        
        if(glm::dot(normal_plane, ray.d) > 0){
            rec.n = glm::normalize(-normal_plane);
        }
        else{
            rec.n = glm::normalize(normal_plane);
        }
        // Set id in tri mesh hit
        return true;
    }

    return false;
}

bool Triangular_Mesh::hit(Ray ray,  HitRecord& rec) const {
    bool hit = false;

    // EARLY CHECK //
    // Check if the ray intersects the bounding sphere
    HitRecord bounding_rec = hit_record_init();
    bounding_rec.t_range_min = rec.t_range_min;
    bounding_rec.t_range_max = rec.t_range_max;
    bool hit_check = bounding_sphere->hit(ray, bounding_rec);
    if(!hit_check){
        return false;
    }

    int i = 0;
    int n_tris = mesh_obj.triangles.size();
    while(i < n_tris){
        if(triangle_hit_help(ray, rec, mesh_obj.triangles[i])){
            hit = true;
        }
        i++;
    }
    if(hit){
        rec.id = id;
    }

    return hit;
}
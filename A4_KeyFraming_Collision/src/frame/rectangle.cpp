#include "rectangle.hpp"

glm::mat4 get_rotate_matrix(Quaternion a){
    glm::mat4 rotate_matrix = glm::mat4(1.0f);
    rotate_matrix[0][0] = a.s * a.s + a.v.x * a.v.x - a.v.y * a.v.y - a.v.z * a.v.z;
    rotate_matrix[0][1] = 2 * (a.v.x * a.v.y - a.s * a.v.z);
    rotate_matrix[0][2] = 2 * (a.v.x * a.v.z + a.s * a.v.y);
    rotate_matrix[1][0] = 2 * (a.v.x * a.v.y + a.s * a.v.z);
    rotate_matrix[1][1] = a.s * a.s - a.v.x * a.v.x + a.v.y * a.v.y - a.v.z * a.v.z;
    rotate_matrix[1][2] = 2 * (a.v.y * a.v.z - a.s * a.v.x);
    rotate_matrix[2][0] = 2 * (a.v.x * a.v.z - a.s * a.v.y);
    rotate_matrix[2][1] = 2 * (a.v.y * a.v.z + a.s * a.v.x);
    rotate_matrix[2][2] = a.s * a.s - a.v.x * a.v.x - a.v.y * a.v.y + a.v.z * a.v.z;
    return rotate_matrix;
}

Quaternion Quaternion::mult(Quaternion a, Quaternion b){
    float new_s = a.s * b.s - glm::dot(a.v,b.v);
    glm::vec3 new_v = a.s * b.v + b.s * a.v + glm::cross(a.v,b.v);
    Quaternion res(new_s,new_v);
    return res;
}

void Skeletal_Box::move_object(glm::vec3 translation_vector){
    this->translation_vector = translation_vector;
}

void Skeletal_Box::rotate_object(Quaternion rotation_quaternion){
    this->rotation_quaternion = rotation_quaternion;
}

void Skeletal_Box::compute_call(bool flag){
    if(flag){
        // This is for grandchild
        Skeletal_Box* root = this->parent->parent;
        glm::vec3 translation = root->translation_vector;
        Quaternion rotation = root->rotation_quaternion;

        // PARENT HINGE JOINT
        glm::vec3 parent_hinge_point = this->parent->hinge_joint.point;
        glm::vec3 parent_hinge_dir = this->parent->hinge_joint.direction;
        glm::mat4 translate_hinge_1 = glm::translate(glm::mat4(1.0f), parent_hinge_point);
        glm::mat4 translate_hinge_2 = glm::translate(glm::mat4(1.0f), -parent_hinge_point);
        glm::mat4 rotate_hinge = glm::rotate(glm::mat4(1.0f), this->parent->hinge_joint.angle, parent_hinge_dir);
        glm::mat4 final_hinge_transformation_parent = translate_hinge_1 * rotate_hinge * translate_hinge_2;

        // CHILD HINGE JOINT
        glm::vec3 child_hinge_point = this->hinge_joint.point;
        glm::vec3 child_hinge_dir = this->hinge_joint.direction;
        glm::mat4 translate_hinge_1_child = glm::translate(glm::mat4(1.0f), child_hinge_point);
        glm::mat4 translate_hinge_2_child = glm::translate(glm::mat4(1.0f), -child_hinge_point);
        glm::mat4 rotate_hinge_child = glm::rotate(glm::mat4(1.0f), this->hinge_joint.angle, child_hinge_dir);
        glm::mat4 final_hinge_transformation_child = translate_hinge_1_child * rotate_hinge_child * translate_hinge_2_child;

        glm::vec3 center_world_pos = this->parent->center + this->center + this->parent->parent->center;
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 rotation_matrix = get_rotate_matrix(rotation);
        glm::mat4 final_transformation_matrix = translation_matrix * rotation_matrix;

        // First get vertices in local coordinates
        glm::vec3 lv1 = glm::vec3(-lengths.first.first /2.0f, lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv2 = glm::vec3(-lengths.first.first /2.0f, lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv3 = glm::vec3(lengths.first.first /2.0f, lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv4 = glm::vec3(lengths.first.first /2.0f, lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv5 = glm::vec3(-lengths.first.first /2.0f, -lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv6 = glm::vec3(-lengths.first.first /2.0f, -lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv7 = glm::vec3(lengths.first.first /2.0f, -lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv8 = glm::vec3(lengths.first.first /2.0f, -lengths.first.second / 2.0f, -lengths.second / 2.0f);

        std::vector<glm::vec3> local_vertices = {
            lv1, lv2, lv3, lv4,
            lv5, lv6, lv7, lv8
        };

        int loop_lim = local_vertices.size();
        int i = 0;
        // Apply transformations
        wh2(i, loop_lim){
            glm::vec3 current_vertex = local_vertices[i] + center_world_pos;
            glm::vec4 vertex = glm::vec4(current_vertex, 1.0f);
            vertex = final_hinge_transformation_child * vertex;
            vertex = final_hinge_transformation_parent * vertex;
            vertex = final_transformation_matrix * vertex;
            local_vertices[i] = glm::vec3(vertex);
            i++;
        }

        i = 0;
        glm::vec3 average = glm::vec3(0.0f);
        wh2(i, loop_lim){
            average += local_vertices[i];
            i++;
        }
        average /= local_vertices.size();

        i = 0;
        std::vector<glm::vec3> vertex_normals;
        wh2(i, loop_lim){
            glm::vec3 normal = local_vertices[i] - average;
            vertex_normals.push_back(glm::normalize(normal));
            i++;
        }

        this->vertices = local_vertices;
        this->normals = vertex_normals;

    }
    else{
        // Only root bone is translated or rotated
        Skeletal_Box* root = this;
        while(root->parent != nullptr){
            root = root->parent;
        }
        glm::vec3 translation = root->translation_vector;
        Quaternion rotation = root->rotation_quaternion;

        glm::vec3 center_world_pos = glm::vec3(0.0f);
        Skeletal_Box* current = this;
        while(current != nullptr){
            center_world_pos += current->center;
            current = current->parent;
        }

        // Transformation matrix
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 rotation_matrix = get_rotate_matrix(rotation);

        glm::mat4 final_transformation_matrix = translation_matrix * rotation_matrix;

        // First get vertices in local coordinates
        glm::vec3 lv1 = glm::vec3(-lengths.first.first /2.0f, lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv2 = glm::vec3(-lengths.first.first /2.0f, lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv3 = glm::vec3(lengths.first.first /2.0f, lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv4 = glm::vec3(lengths.first.first /2.0f, lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv5 = glm::vec3(-lengths.first.first /2.0f, -lengths.first.second / 2.0f, -lengths.second / 2.0f);
        glm::vec3 lv6 = glm::vec3(-lengths.first.first /2.0f, -lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv7 = glm::vec3(lengths.first.first /2.0f, -lengths.first.second / 2.0f, lengths.second / 2.0f);
        glm::vec3 lv8 = glm::vec3(lengths.first.first /2.0f, -lengths.first.second / 2.0f, -lengths.second / 2.0f);

        std::vector<glm::vec3> local_vertices = {
            lv1, lv2, lv3, lv4,
            lv5, lv6, lv7, lv8
        };

        int i = 0;
        int loop_lim = local_vertices.size();
        glm::vec3 hinge_point_ = hinge_joint.point;
        glm::vec3 hinge_dir_ = hinge_joint.direction;

        glm::mat4 translate_hinge_1 = glm::translate(glm::mat4(1.0f), hinge_point_);
        glm::mat4 translate_hinge_2 = glm::translate(glm::mat4(1.0f), -hinge_point_);
        glm::mat4 rotate_hinge = glm::rotate(glm::mat4(1.0f), hinge_joint.angle, hinge_dir_);
        glm::mat4 final_hinge_transformation = translate_hinge_1 * rotate_hinge * translate_hinge_2;

        wh2(i, loop_lim){
            glm::vec3 current_vertex = local_vertices[i] + center_world_pos;
            glm::vec4 vertex = glm::vec4(current_vertex, 1.0f);
            vertex = final_hinge_transformation * vertex;
            vertex = final_transformation_matrix * vertex;
            local_vertices[i] = glm::vec3(vertex);
            i++;
        }

        this->vertices = local_vertices;
        // Set the normals as normalized vector from the center to the vertex
        i = 0;
        std::vector<glm::vec3> vertex_normals;
        wh2(i, loop_lim){
            glm::vec3 normal = local_vertices[i] - center_world_pos;
            vertex_normals.push_back(-glm::normalize(normal));
            i++;
        }
        
        this->normals = vertex_normals;
        // Compute the avergae of all vertices
        glm::vec3 average = glm::vec3(0.0f);
        i = 0;
        wh2(i, loop_lim){
            average += local_vertices[i];
            i++;
        }
        average /= local_vertices.size();
    }

    // Can hardcode the normals here from the vertices and the figures.

}

void Skeletal_Box::init(){
    // Set edges wireframe and triangles as mesh
    std::vector<glm::ivec2> edges = {
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7}
    };

    std::vector<glm::ivec3> triangles = {
        {0, 1, 2},
        {0, 2, 3},
        {4, 5, 6},
        {4, 6, 7},
        {0, 1, 5},
        {0, 5, 4},
        {1, 2, 6},
        {1, 6, 5},
        {2, 3, 7},
        {2, 7, 6},
        {3, 0, 4},
        {3, 4, 7}
    };

    this->edges = edges;
    this->triangles = triangles;
    this->vertices = std::vector<glm::vec3>(8, glm::vec3(0.0f));
    this->normals = std::vector<glm::vec3>(8, glm::vec3(0.0f));
}
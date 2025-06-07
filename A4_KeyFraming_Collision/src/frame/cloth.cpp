#include "cloth.hpp"

// INITIALIZATION //
// CLOTH PARAMS
std::pair<float,float> cloth_size = {2.0f,2.0f}; // Change first term to increment length vertically
std::pair<int,int> cloth_division = {13,10};
// SPRING PARAMS
glm::vec3 spring_k(12.25f,8.75f,0.78f); // SPRING - STRUCTURAL - SHEAR - BENDING
glm::vec3 spring_k_damp(1.54f,1.25f,0.13f); // SPRING -> DAMPING CONSTANTS
// GLOBAL PARAMAS
glm::vec3 global_acceleration(0,-9.81f,0); // GLOBAL ACCELERATION
int PBD_iters = 15; // PBD iterations


void particle_matrix::set_value(int i,int j, string mat_name, glm::vec3 value){
    if(mat_name == "location"){
        particle_location_matrix[i][j] = value;
    }
    if(mat_name == "velocity"){
        particle_velocity_matrix[i][j] = value;
    }
    if(mat_name == "force"){
        particle_force_matrix[i][j] = value;
    }
}

void particle_matrix::clear_force_matrix(){
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            particle_force_matrix[i][j] = constants::zeroVec3;
            j++;
        }
        i++;
    }
}

void particle_matrix::init_matrix(){
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    while(i < i_lim){
        std::vector<glm::vec3> temp_location_vector;
        std::vector<glm::vec3> temp_velocity_vector;
        std::vector<glm::vec3> temp_force_vector;
        std::vector<bool> temp_fixed_vector;
        j = 0;
        while(j < j_lim){
            temp_location_vector.push_back(constants::zeroVec3);
            temp_velocity_vector.push_back(constants::zeroVec3);
            temp_force_vector.push_back(constants::zeroVec3);
            temp_fixed_vector.push_back(false);
            j++;
        }
        particle_location_matrix.push_back(temp_location_vector);
        particle_velocity_matrix.push_back(temp_velocity_vector);
        particle_force_matrix.push_back(temp_force_vector);
        is_fixed.push_back(temp_fixed_vector);
        i++;
    }
}

void particle_matrix::initialize_particle_matrix(glm::vec3 initial_cloth_center, float total_cloth_mass){
    mass_per_particle = total_cloth_mass/(cloth_division.first*cloth_division.second);
    init_matrix();
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    glm::vec3 top_left = initial_cloth_center + glm::vec3(-cloth_size.first/2,0,-cloth_size.second/2);
    glm::vec3 temp;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            // Cloth is in x-z plane
            temp = top_left + glm::vec3(i*(cloth_size.first/(cloth_division.first-1)),0,j*(cloth_size.second/(cloth_division.second-1)));
            particle_location_matrix[i][j] = temp;
            particle_velocity_matrix[i][j] = constants::zeroVec3;
            particle_force_matrix[i][j] = constants::zeroVec3;
            j++;
        }
        i++;
    }
    // SPRING INITIALIZATION
    springs_vec.clear();
    // First we will set the structural springs
    // ROWS & COLS adjacency
    i = 0;
    // N_STRUCTURAL = (i_lim-1)*j_lim + (j_lim-1)*i_lim
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            // COLS
            if(i < i_lim-1){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i+1,j)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i+1][j]), 
                    std::make_pair(spring_k.x,spring_k_damp.x)
                )
                );
            }
            // ROWS
            if(j < j_lim-1){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i,j+1)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i][j+1]), 
                    std::make_pair(spring_k.x,spring_k_damp.x)
                )
                );
            }
            j++;
        }   
        i++;
    }
    // Now we will set the shear springs
    i = 0;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            // DIAG - 1
            if(i < i_lim-1 && j < j_lim-1){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i+1,j+1)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i+1][j+1]), 
                    std::make_pair(spring_k.y,spring_k_damp.y)
                )
                );
            }
            // DIAG - 2
            if(i < i_lim-1 && j > 0){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i+1,j-1)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i+1][j-1]), 
                    std::make_pair(spring_k.y,spring_k_damp.y)
                )
                );
            }
            j++;
        }
        i++;
    }

    // Now we will set the bending springs (every 2nd particle)
    i = 0;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            // BEND - ROWS
            if(i < i_lim-2){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i+2,j)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i+2][j]), 
                    std::make_pair(spring_k.z,spring_k_damp.z)
                )
                );
            }
            // BEND - COLS
            if(j < j_lim-2){
                springs_vec.push_back(
                    spring(std::make_pair(std::make_pair(i,j),std::make_pair(i,j+2)), 
                    glm::length(particle_location_matrix[i][j] - particle_location_matrix[i][j+2]), 
                    std::make_pair(spring_k.z,spring_k_damp.z)
                )
                );
            }
            j++;
        }
        i++;
    }

}

std::vector<std::vector<glm::vec3>> particle_matrix::sphere_cloth_collision(sphere &sphere1){
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    std::vector<std::vector<glm::vec3>> particle_force_collision_matrix;
    particle_force_collision_matrix.resize(i_lim);
    glm::vec3 sphere_vel_delta = constants::zeroVec3;
    glm::vec3 sphere_omega_delta = constants::zeroVec3;
    while(i < i_lim){
        particle_force_collision_matrix[i].resize(j_lim);
        j = 0;
        while(j < j_lim){
            particle_force_collision_matrix[i][j] = constants::zeroVec3;

            // Check for collision
            float dist = glm::length(particle_location_matrix[i][j] - sphere1.center);
            if(dist < 1.001 * sphere1.radius && is_fixed[i][j] == false){
                // Handle collision
                float m1 = mass_per_particle;
                float m2 = sphere1.mass;
                glm::vec3 sphere_normal = glm::normalize(particle_location_matrix[i][j] - sphere1.center);
                glm::vec3 normal_impulse = - (1 + COEF_RESTITUTION) * (glm::dot(particle_velocity_matrix[i][j] - sphere1.velocity, sphere_normal)) * sphere_normal / (1.0f/m1 + 1.0f/m2);
                particle_force_collision_matrix[i][j] = normal_impulse;
                sphere_vel_delta -= normal_impulse / m2;
                glm::vec3 v_rel_tangential = (particle_velocity_matrix[i][j] - sphere1.velocity - glm::cross(sphere1.omega, sphere1.radius * sphere_normal)) - glm::dot(particle_velocity_matrix[i][j] - sphere1.velocity, sphere_normal) * sphere_normal;
                glm::vec3 tangential_impulse = - ( v_rel_tangential ) / (1.0f/m1 + 7.0f/(2.0f*m2));
                if(glm::length(tangential_impulse) > COEF_FRICTION * glm::length(normal_impulse)){
                    tangential_impulse = COEF_FRICTION * glm::length(normal_impulse) * glm::normalize(tangential_impulse);
                }
                particle_force_collision_matrix[i][j] += tangential_impulse;
                sphere_omega_delta -= (sphere1.radius * glm::cross(sphere_normal, tangential_impulse)) / (0.4f * m2 * sphere1.radius * sphere1.radius);
            }

            j++;
        }
        i++;
    }
    // Update sphere velocity and omega
    sphere1.velocity += sphere_vel_delta;
    sphere1.omega += sphere_omega_delta;
    return particle_force_collision_matrix;
}

std::vector<std::vector<glm::vec3>> particle_matrix::plane_cloth_collision(plane &plane1){
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    std::vector<std::vector<glm::vec3>> particle_force_collision_matrix;
    particle_force_collision_matrix.resize(i_lim);
    while(i < i_lim){
        particle_force_collision_matrix[i].resize(j_lim);
        j = 0;
        while(j < j_lim){
            particle_force_collision_matrix[i][j] = constants::zeroVec3;
            glm::vec3 plane_n = plane1.plane_desc.first;
            // Check for collision
            float dist = (glm::dot(plane_n,particle_location_matrix[i][j]) - plane1.plane_desc.second) / glm::length(plane_n);
            if(fabs(dist) < 0.01f && is_fixed[i][j] == false){
                float m1 = mass_per_particle;
                glm::vec3 unit_n = glm::normalize(plane_n);
                glm::vec3 v_norm = glm::dot(particle_velocity_matrix[i][j], unit_n) * unit_n;
                glm::vec3 v_tangent = particle_velocity_matrix[i][j] - v_norm;

                glm::vec3 normal_impulse = - (1 + COEF_RESTITUTION) * m1 * v_norm;
                glm::vec3 tangential_impulse = - (m1) * v_tangent;

                if(glm::length(tangential_impulse) > COEF_FRICTION * glm::length(normal_impulse)){
                    tangential_impulse = COEF_FRICTION * glm::length(normal_impulse) * glm::normalize(tangential_impulse);
                }

                particle_force_collision_matrix[i][j] = normal_impulse + tangential_impulse;

            }


            j++;
        }
        i++;
    }

    return particle_force_collision_matrix;

}

void particle_matrix::invocare_forciam(bool pbd_flag){
    if(pbd_flag){
        // PBD 
        clear_force_matrix();
        // Apply gravity to all particles
        int i = 0;
        int i_lim = cloth_division.first;
        int j = 0;
        int j_lim = cloth_division.second;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    particle_force_matrix[i][j] += mass_per_particle*global_acceleration;
                }
                j++;
            }
            i++;
        }
        // Apply spring forces
        int N_STRUCTURAL = (i_lim-1)*j_lim + (j_lim-1)*i_lim;
        i = N_STRUCTURAL;
        i_lim = springs_vec.size();
        while(i < i_lim){
            spring temp_spring = springs_vec[i];
            bool is_fixed_1 = is_fixed[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            bool is_fixed_2 = is_fixed[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second];
            glm::vec3 p21 = particle_location_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_location_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            float p21_mag = glm::length(p21);
            glm::vec3 p21_unit = p21/p21_mag; // Unit vector

            if(p21_mag > EPSILON){
                glm::vec3 v21 = particle_velocity_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_velocity_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
                glm::vec3 f = -temp_spring.spring_constants_pair.first*(p21_mag - temp_spring.rest_length)*p21_unit - temp_spring.spring_constants_pair.second * glm::dot(v21,p21_unit)*p21_unit;
                particle_force_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second] += is_fixed_1 ? constants::zeroVec3 : -f;
                particle_force_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] += is_fixed_2 ? constants::zeroVec3 : f;
            }
            i++;
        }
        // Apply damping for structural springs
        i = 0;
        i_lim = N_STRUCTURAL;
        while(i < i_lim){
            spring temp_spring = springs_vec[i];
            bool is_fixed_1 = is_fixed[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            bool is_fixed_2 = is_fixed[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second];
            glm::vec3 p21 = particle_location_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_location_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            float p21_mag = glm::length(p21);
            glm::vec3 p21_unit = p21/p21_mag; // Unit vector

            if(p21_mag > EPSILON){
                glm::vec3 v21 = particle_velocity_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_velocity_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
                glm::vec3 f = -temp_spring.spring_constants_pair.second * glm::dot(v21,p21_unit)*p21_unit;
                particle_force_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second] += is_fixed_1 ? constants::zeroVec3 : -f;
                particle_force_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] += is_fixed_2 ? constants::zeroVec3 : f;
            }
            i++;
        }
    }
    else{
        clear_force_matrix();
        // Apply gravity to all particles
        int i = 0;
        int i_lim = cloth_division.first;
        int j = 0;
        int j_lim = cloth_division.second;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    particle_force_matrix[i][j] += mass_per_particle*global_acceleration;
                }
                j++;
            }
            i++;
        }
        // Apply spring forces
        i = 0;
        i_lim = springs_vec.size();
        while(i < i_lim){
            spring temp_spring = springs_vec[i];
            bool is_fixed_1 = is_fixed[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            bool is_fixed_2 = is_fixed[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second];
            glm::vec3 p21 = particle_location_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_location_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
            float p21_mag = glm::length(p21);
            glm::vec3 p21_unit = p21/p21_mag; // Unit vector

            if(p21_mag > EPSILON){
                glm::vec3 v21 = particle_velocity_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_velocity_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
                glm::vec3 f = -temp_spring.spring_constants_pair.first*(p21_mag - temp_spring.rest_length)*p21_unit - temp_spring.spring_constants_pair.second * glm::dot(v21,p21_unit)*p21_unit;
                particle_force_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second] += is_fixed_1 ? constants::zeroVec3 : -f;
                particle_force_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] += is_fixed_2 ? constants::zeroVec3 : f;
            }
            i++;
        }
    }
}

void particle_matrix::compute_call(float time_step, bool pbd_flag,plane *plane1, sphere *sphere1, sphere *sphere2){
    // Compute the new positions and velocities of the particles

    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;

    // We need to store the old positions of the particles
    std::vector<std::vector<glm::vec3>> old_particle_location_matrix = particle_location_matrix;

    if(plane1 != nullptr){
        // If we have a plane, we need to apply the collision forces
        std::vector<std::vector<glm::vec3>> particle_force_collision_matrix = plane_cloth_collision(*plane1);
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    particle_force_matrix[i][j] += particle_force_collision_matrix[i][j] / time_step;
                }
                j++;
            }
            i++;
        }
    }

    if(sphere1 != nullptr){
        // If we have a sphere, we need to apply the collision forces
        std::vector<std::vector<glm::vec3>> particle_force_collision_matrix = sphere_cloth_collision(*sphere1);
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    particle_force_matrix[i][j] += particle_force_collision_matrix[i][j] / time_step;
                }
                j++;
            }
            i++;
        }
    }

    if(sphere2 != nullptr){
        // If we have a sphere, we need to apply the collision forces
        std::vector<std::vector<glm::vec3>> particle_force_collision_matrix = sphere_cloth_collision(*sphere2);
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    particle_force_matrix[i][j] += particle_force_collision_matrix[i][j] / time_step;
                }
                j++;
            }
            i++;
        }
    }

    i = 0;

    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            if(!is_fixed[i][j]){
                particle_velocity_matrix[i][j] += time_step*particle_force_matrix[i][j]/mass_per_particle;
                particle_location_matrix[i][j] += time_step*particle_velocity_matrix[i][j];
            }
            j++;
        }
        i++;
    }

    if(pbd_flag){
        // PBD LOOPS
        int loop_iter = 0;
        while(loop_iter < PBD_iters){
            i = 0;
            int N_STRUCTURAL = (i_lim-1)*j_lim + (j_lim-1)*i_lim;
            while(i < N_STRUCTURAL){
                spring temp_spring = springs_vec[i];
                bool is_fixed_1 = is_fixed[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
                bool is_fixed_2 = is_fixed[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second];
                glm::vec3 p21 = particle_location_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] - particle_location_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second];
                float p21_mag = glm::length(p21);
                glm::vec3 p21_unit = p21/p21_mag; // Unit vector

                if(p21_mag > EPSILON){
                    // Calculate the constraint error (actual length - rest length)
                    float correction_total = (p21_mag - temp_spring.rest_length);
                    
                    // Calculate weights based on if particles are fixed
                    float w1 = is_fixed_1 ? 0.0f : 1.0f / mass_per_particle;
                    float w2 = is_fixed_2 ? 0.0f : 1.0f / mass_per_particle;
                
                    if(w1 + w2 > EPSILON){
                        // Calculate position corrections
                        glm::vec3 delta_p1 = is_fixed_1 ? constants::zeroVec3 : (w1/(w1+w2)) * correction_total * p21_unit;
                        glm::vec3 delta_p2 = is_fixed_2 ? constants::zeroVec3 : -(w2/(w1+w2)) * correction_total * p21_unit;
                
                        // Apply corrections - particle 1 moves in direction of particle 2, and vice versa
                        particle_location_matrix[temp_spring.particles_pair.first.first][temp_spring.particles_pair.first.second] += delta_p1;
                        particle_location_matrix[temp_spring.particles_pair.second.first][temp_spring.particles_pair.second.second] += delta_p2;
                    }
                }


                i++;
            }
            loop_iter++;
        }
    }

    // Moving things out of obstacles
    if(sphere1 != nullptr){
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    // Check if collision 
                    glm::vec3 p_center = (particle_location_matrix[i][j] - sphere1->center);
                    if(glm::length(p_center) < (1.000f * sphere1->radius)){
                        // Move point out
                        particle_location_matrix[i][j] = sphere1->center + (1.001f * sphere1->radius) * glm::normalize(p_center);
                    }
                }
                j++;
            }
            i++;
        }

    }

    if(sphere2 != nullptr){
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    // Check if collision 
                    glm::vec3 p_center = (particle_location_matrix[i][j] - sphere2->center);
                    if(glm::length(p_center) < (1.000f * sphere2->radius)){
                        // Move point out
                        particle_location_matrix[i][j] = sphere2->center + (1.001f * sphere2->radius) * glm::normalize(p_center);
                    }
                }
                j++;
            }
            i++;
        }

    }

    if(plane1 != nullptr){
        i = 0;
        while(i < i_lim){
            j = 0;
            while(j < j_lim){
                if(!is_fixed[i][j]){
                    // Check if collision 
                    float dist = (glm::dot(plane1->plane_desc.first,particle_location_matrix[i][j]) - plane1->plane_desc.second) / glm::length(plane1->plane_desc.first);
                    if(fabs(dist) < 0.01f){
                        // Move point out
                        particle_location_matrix[i][j] += -dist * plane1->plane_desc.first;
                    }
                }
                j++;
            }
            i++;
        }
    }

}

mesh particle_matrix::get_mesh(){
    mesh temp_mesh;
    int i = 0;
    int i_lim = cloth_division.first;
    int j = 0;
    int j_lim = cloth_division.second;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            temp_mesh.vertices.push_back(particle_location_matrix[i][j]);
            j++;
        }
        i++;
    }

    // For normals we will use counter clockwise order
    i = 0;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            glm::vec3 n = constants::zeroVec3;
            if(j < j_lim-1 && i > 0){
                n = -glm::normalize(
                    glm::cross(
                        particle_location_matrix[i][j+1] - particle_location_matrix[i][j],
                        particle_location_matrix[i-1][j] - particle_location_matrix[i][j]
                    )
                );
            }
            else{
                if(j > 0 && i < i_lim-1){
                    n = -glm::normalize(
                        glm::cross(
                            particle_location_matrix[i][j-1] - particle_location_matrix[i][j],
                            particle_location_matrix[i+1][j] - particle_location_matrix[i][j]
                        )
                    );
                }
                else if(i == 0){
                    n = glm::normalize(
                        glm::cross(
                            particle_location_matrix[i][j+1] - particle_location_matrix[i][j],
                            particle_location_matrix[i+1][j] - particle_location_matrix[i][j]
                        )
                    );
                }
                else if(i == i_lim-1){
                    n = glm::normalize(
                        glm::cross(
                            particle_location_matrix[i][j-1] - particle_location_matrix[i][j],
                            particle_location_matrix[i-1][j] - particle_location_matrix[i][j]
                        )
                    );
                }
                else{
                    std::cout << "Error in normal calculation" << std::endl;
                    n = constants::zeroVec3;
                }
            }
            temp_mesh.normals.push_back(n);
            j++;
        }
        i++;
    }

    // Now we will create the triangles
    i = 0;
    while(i < i_lim-1){
        j = 0;
        while(j < j_lim-1){
            temp_mesh.triangles.push_back(glm::ivec3(i*cloth_division.second+j,i*cloth_division.second+j+1,(i+1)*cloth_division.second+j));
            temp_mesh.triangles.push_back(glm::ivec3(i*cloth_division.second+j+1,(i+1)*cloth_division.second+j+1,(i+1)*cloth_division.second+j));
            j++;
        }
        i++;
    }

    // Now we will create the edges
    i = 0;
    while(i < i_lim){
        j = 0;
        while(j < j_lim){
            if(i < i_lim-1){
                temp_mesh.edges.push_back(glm::ivec2(i*cloth_division.second+j,(i+1)*cloth_division.second+j));
            }
            if(j < j_lim-1){
                temp_mesh.edges.push_back(glm::ivec2(i*cloth_division.second+j,i*cloth_division.second+j+1));
            }
            j++;
        }
        i++;
    }

    return temp_mesh;
}

std::vector<glm::vec3> plane::plane_boundaries(glm::vec3 plane_center, float plane_size){
    glm::vec3 arbitary_vec = glm::normalize(glm::vec3(1.2f,1.1f,0.8f));
    if(glm::length(glm::normalize(plane_desc.first) - arbitary_vec) < EPSILON){
        arbitary_vec = glm::normalize(glm::vec3(0.8f,1.2f,1.1f));
    }
    glm::vec3 arbitart_tan_1 = glm::normalize(arbitary_vec - glm::dot(arbitary_vec,plane_desc.first)*plane_desc.first);
    glm::vec3 arbitart_tan_2 = glm::normalize(glm::cross(plane_desc.first,arbitart_tan_1));

    std::vector<glm::vec3> plane_boundaries = {
        plane_center + (plane_size) * arbitart_tan_1,
        plane_center - (plane_size) * arbitart_tan_1,
        plane_center + (plane_size) * arbitart_tan_2,
        plane_center - (plane_size) * arbitart_tan_2,
    };

    return plane_boundaries;
}

mesh sphere::get_mesh(){
    mesh temp_mesh;
    int stacks = SPHERE_STACKS;
    int slices = SPHERE_SLICES;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec3> triangles;
    std::vector<glm::ivec2> edges;
    if(setup_sphere == false){
        // CODE FROM MY ASSIGNMENT 2 IMPLEMNTATION
        float radius = this->radius;
        glm::vec3 center = this->center;
        vertices.push_back(glm::vec3(0.0f, 0.0f, radius) + center);
        normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
        vertices.push_back(glm::vec3(0.0f, 0.0f, -radius) + center);
        normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
        // Now add the vertices in the middle
        float theta = 0.0f;
        float phi = 0.0f;
        float d_theta = 3.14f/(float)stacks;
        float d_phi = 2.0f*3.14f/(float)slices;
        int idx1 = 0;
        int idx2 = 0;
        while(idx1 < stacks-1){
            theta += d_theta;
            phi = 0.0f;
            idx2 = 0;
            while(idx2 < slices){
                vertices.push_back(glm::vec3(radius*glm::sin(theta)*glm::cos(phi), radius*glm::sin(theta)*glm::sin(phi), radius*glm::cos(theta)));
                vertices.back() += center;
                normals.push_back(glm::vec3(glm::sin(theta)*glm::cos(phi), glm::sin(theta)*glm::sin(phi), glm::cos(theta)));
                phi += d_phi;
                idx2++;
            }
            idx1++;
        }

        std::vector<std::vector<int>> poly_faces;
        // Pole faces
        // North
        idx1 = 0;
        while(idx1 < slices){
            std::vector<int> pole_face;
            pole_face.push_back(0);
            pole_face.push_back(2 + idx1);
            pole_face.push_back(2 + (idx1+1)%slices);
            poly_faces.push_back(pole_face);
            idx1++;
        }
        // South
        idx1 = 0;
        while(idx1 < slices){
            std::vector<int> pole_face;
            pole_face.push_back(1);
            pole_face.push_back(2 + (stacks-2)*slices + idx1);
            pole_face.push_back(2 + (stacks-2)*slices + (idx1+1)%slices);
            poly_faces.push_back(pole_face);
            idx1++;
        }
        // Now the middle faces
        idx1 = 0;
        while(idx1 < stacks-2){
            idx2 = 0;
            while(idx2 < slices){
                // Make a square and add to poly_faces
                std::vector<int> square;
                square.push_back(2 + idx1*slices + idx2);
                square.push_back(2 + idx1*slices + (idx2+1)%slices);
                square.push_back(2 + (idx1+1)*slices + (idx2+1)%slices);
                square.push_back(2 + (idx1+1)*slices + idx2);
                poly_faces.push_back(square);
                idx2++;
            }
            idx1++;
        }

        std::set<std::pair<int,int>> edges_set;
        // Now we will create the triangles and edges
        for(auto face : poly_faces){
            if(face.size() == 3){
                triangles.push_back(glm::ivec3(face[2],face[1],face[0]));
            }
            else{
                triangles.push_back(glm::ivec3(face[2],face[1],face[0]));
                triangles.push_back(glm::ivec3(face[3],face[2],face[0]));
            }
            for(int i = 0; i < face.size(); i++){
                edges_set.insert(std::make_pair(face[i],face[(i+1)%face.size()]));
            }
        }
        setup_sphere = true;
        triangles_buf = triangles;
        for(auto edge : edges_set){
            edges.push_back(glm::ivec2(edge.first,edge.second));
        }
        edges_buf = edges;

        // Save the vertices and normals
        vertices_buf = vertices;
        normals_buf = normals;
    }
    else{
        vertices = vertices_buf;
        normals = normals_buf;
        triangles = triangles_buf;
        edges = edges_buf;

        // Apply updates from changes in center and angle 
        // First subtract old center
        int i = 0;
        int i_lim = vertices.size();
        glm::vec3 old_center = this->center;
        glm::vec3 new_center = this->center + changes.first;
        glm::vec3 d_theta_rot = changes.second;
        while(i < i_lim){
            vertices[i] -= old_center;
            // Rotate
            vertices[i] = glm::vec3(
                glm::rotate(
                  glm::rotate(
                    glm::rotate(glm::mat4(1.0f), d_theta_rot.x, glm::vec3(1,0,0)),
                                         d_theta_rot.y, glm::vec3(0,1,0)),
                                         d_theta_rot.z, glm::vec3(0,0,1))
                * glm::vec4(vertices[i], 1.0f)
            );
            normals[i] = glm::normalize(vertices[i]);
            // Add new center
            vertices[i] += new_center;
            i++;
        }

        this->center = new_center;
        this->changes = std::make_pair(constants::zeroVec3,constants::zeroVec3);

        vertices_buf = vertices;
        normals_buf = normals;

    }

    temp_mesh.vertices = vertices;
    temp_mesh.normals = normals;
    temp_mesh.triangles = triangles;
    temp_mesh.edges = edges;
    return temp_mesh;

}

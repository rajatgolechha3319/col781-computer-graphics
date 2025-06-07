#include "Scene.hpp"
#include "Camera.hpp"

const glm::vec3 sky_blue = glm::pow(glm::vec3(0.529f, 0.808f, 0.922f), glm::vec3(2.2f));

void save_img_intermediate(HDRImage &image, int k, int sample_rate, int width, int height){
    std::string filename = "out_intermediate_" + std::to_string(k) + ".png";
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    // Create a new image and scale the intensity by sample_rate / (k+1)
    HDRImage new_image(width, height);
    int i = 0;
    while(i<width){
        int j = 0;
        while(j<height){
            new_image.pixel(i, j) = image.pixel(i, j) * (float) sample_rate / (float) (k+1);
            j++;
        }
        i++;
    }
    tonemap(new_image, out, 1, 2.2f);
    IMG_SavePNG(out, filename.c_str());
    SDL_FreeSurface(out);

    std::cout << "Image saved to " << filename << std::endl;
}

glm::vec3 hemisphere_sampler(glm::vec3& n){
    // Sample a random normal 
    // Cosine weighted sampling
    float r1 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    float r2 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    float theta  = glm::acos(glm::sqrt(1.0f - r1));
    float phi = 2.0f * 3.14159265f * r2;
    
    // Convert to cartesian coordinates
    float x = glm::sin(theta) * glm::cos(phi);
    float y = glm::cos(theta);
    float z = glm::sin(theta) * glm::sin(phi);

    // Return new direction according hemisphere
    // First fetch plane
    glm::vec3 n1,n2;
    if (std::fabs(n.x) > std::fabs(n.y)){
        n1 =  glm::vec3(n.z, 0, -n.x) / std::sqrt(n.x * n.x + n.z * n.z);
        n2 = glm::cross(n, n1);
    }
    else{
        n1 = glm::vec3(0, -n.z, n.y) / std::sqrt(n.y * n.y + n.z * n.z);
        n2 = glm::cross(n, n1);
    }
    return x * n1 + y * n + z * n2;
}

glm::vec3 reflection_helper(glm::vec3 n, glm::vec3 v){
    n = glm::normalize(n);
    v = glm::normalize(v);
    return v - 2.0f * glm::dot(v, n) * n;
}

float compute_cos_plane_ray(glm::vec3 n, glm::vec3 v){
    return glm::dot(glm::normalize(n), glm::normalize(-v));
}

Scene::~Scene() {
    delete camera;
    for (Object* obj : objects) delete obj;
    objects.clear();
}

void Scene::start_camera(int height, int width, float fov, glm::vec3 eye, glm::vec3 view_dir, glm::vec3 up) {
    camera = new Camera();
    camera->init_camera(height, width, fov, eye, view_dir, up);
}

glm::vec3 Scene::direct_helper (Ray direction, HitRecord& rec){
    // Now check for direct illumination from light sources
    int n_lights = lights.size();
    int n_objects = objects.size();
    glm::vec3 total_intensity = glm::vec3(0.0f);
    int k = 0;
    while(k < n_lights){
        // Check if in shadow
        bool in_shadow = false;
        HitRecord shadow_rec = hit_record_init();
        // Check if the ray from light source to hit point intersects with any object
        Ray shadow_ray = lumos_solem(lights[k], rec.p);
        int l = 0;
        while(l < n_objects){
            objects[l]->hit(shadow_ray, shadow_rec);
            l++;
        }
        if(shadow_rec.id != rec.id){
            in_shadow = true;
        }

        // Calculate the intensity if not in shadow
        if(!in_shadow){
            glm::vec3 brdf_val = rec.mat->brdf_compute(rec, lights[k].intensity, lights[k].location, direction.o);
            total_intensity += brdf_val;
        }
        k++;
    }
    return total_intensity;
}

glm::vec3 Scene::compute_intensity_sampling(Ray direction, const float cont, int recursion_depth){
    // float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    // std::cout << "Random number: " << r << std::endl;

    glm::vec3 intensity = glm::vec3(0.0f);
    HitRecord rec = hit_record_init();
    bool hit_anything = false;
    int n_objects = objects.size();
    int k = 0;
    while(k<n_objects){
        if (objects[k]->hit(direction, rec)) {
            hit_anything = true;
        }
        k++;
    }

    // If hit, compute direct illumination and specular highlights at the hit 
    if(hit_anything){
        // DIRECT ILLUMINATION
        intensity += direct_helper(direction, rec);

        // OBJECT HIT ILLUMINATION
        intensity += rec.mat->emission(rec, direction.o);

        // Check for continuation
        if(recursion_depth < MAX_REC_DEPTH){
            // Sample a floating point number
            float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            // RUSSIAN ROULETTE //
            if( r < cont){

                // FRESNEL //
                float cos_theta = compute_cos_plane_ray(rec.n, direction.d);
                glm::vec3 new_reflectivity = rec.mat->reflectivity + (glm::vec3(1.0f) - rec.mat->reflectivity) * glm::pow(1.0f - cos_theta, 5.0f);

                // Again sample a random number
                float r2 = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
                glm::vec3 r_dir;
                if(r2 < glm::length(new_reflectivity / 1.732f)){
                    // Reflect the ray
                    r_dir = reflection_helper(rec.n, direction.d);
                }
                else{
                    // Continue the path
                    r_dir = hemisphere_sampler(rec.n);
                    if(glm::length(rec.mat->metallic_factor) > 0.0f){
                        r_dir = glm::normalize(r_dir * (glm::vec3(1.0f) - rec.mat->metallic_factor) + rec.mat->metallic_factor * reflection_helper(rec.n, direction.d));
                    }
                }
                Ray new_ray = Ray(rec.p, r_dir);
                glm::vec3 rec_intensity = compute_intensity_sampling(new_ray, cont, recursion_depth + 1);
                // Assuming Lambertian for now 
                glm::vec3 brdf_constant = rec.mat->get_albedo(); // NO PI AS COSINE SAMPLING REMOVES IT !
                rec_intensity = glm::max(glm::vec3(0.0f), rec_intensity * brdf_constant / cont);
                intensity += rec_intensity;
            }
        }

    }

    else{
        if(recursion_depth == 0){
            // intensity = sky_blue;
            intensity = glm::vec3(0.0f);
        }
    }

    return intensity;

}

glm::vec3 Scene::compute_intensity(Ray direction, int reflections){
    // This function computes the intensity of the ray by recursively tracing the path
    glm::vec3 intensity = glm::vec3(0.0f);
    HitRecord rec = hit_record_init();
    bool hit_anything = false;
    int n_objects = objects.size();
    for(int k=0; k<n_objects; k++){
        if (objects[k]->hit(direction, rec)) {
            hit_anything = true;
        }
    }
    if(hit_anything){
        // Compute direct illumination + Specular highlights
        intensity += direct_helper(direction, rec);

        if(reflections > 1){
            glm::vec3 r = reflection_helper(rec.n, direction.d);
            Ray reflected_ray = Ray(rec.p, r);

            // FRESNEL //
            float cos_theta = compute_cos_plane_ray(rec.n, direction.d);
            glm::vec3 new_kr = rec.mat->reflectivity + (glm::vec3(1.0f) - rec.mat->reflectivity) * glm::pow(1.0f - cos_theta, 5.0f);

            // Recursive call
            intensity += new_kr * compute_intensity(reflected_ray, reflections - 1);

        }

    }
    else{
        intensity = sky_blue;
        intensity = glm::vec3(0.0f);
    }
    return intensity;
}

void Scene::render_scene_raytracer(HDRImage &image, bool parallel){
    if(parallel){
        #pragma omp parallel for schedule(static)
        for (int j = 0; j < camera->buffer_size.first; j++) {
            for (int i = 0; i < camera->buffer_size.second; i++) {
                // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
                // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
    
                // Generate ray from camera
                Ray ray = camera->make_ray(coords.first, coords.second);
    
                glm::vec3 total_intensity = compute_intensity(ray,reflection_depth);

                image.pixel(i, j) = total_intensity;
            }
        }
    }
    else{
        for (int j = 0; j < camera->buffer_size.first; j++) {
            for (int i = 0; i < camera->buffer_size.second; i++) {
                // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
                // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
    
                // Generate ray from camera
                Ray ray = camera->make_ray(coords.first, coords.second);
    
                glm::vec3 total_intensity = compute_intensity(ray,reflection_depth);

                image.pixel(i, j) = total_intensity;
            }
        }
    }
}

void Scene::render_scene_pathtracer(HDRImage &image, bool parallel){
    float cont = 1 - 1.0f / (float) avg_depth;
    if(parallel){
        int k = 0;
        while(k < sample_rate){
        #pragma omp parallel for schedule(static)
            for (int j = 0; j < camera->buffer_size.first; j++) {
                for (int i = 0; i < camera->buffer_size.second; i++) {
                    // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
                    // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                    std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
        
                    // Generate ray from camera
                    Ray ray = camera->make_ray(coords.first, coords.second);
        
                    // glm::vec3 total_intensity = compute_intensity(ray,2);
                    // glm::vec3 total_intensity = compute_direct_illumination(ray);

                    glm::vec3 total_intensity = glm::vec3(0.0f);
                    // SAMPLE MULTIPLE TIMES
                    total_intensity += compute_intensity_sampling(ray, cont, 0);
                    total_intensity /= (float) sample_rate;

        
                    // Store the color in the image
                    image.pixel(i, j) += total_intensity;
                }
            }
        #pragma omp barrier
        // Will save image here
        // save_img_intermediate(image, k, sample_rate, camera->buffer_size.second, camera->buffer_size.first);
        if(k % save_rate == 0){
            std::cout << "Saving image at iteration " << k << std::endl;
            save_img_intermediate(image, k, sample_rate, camera->buffer_size.second, camera->buffer_size.first);
        }
        k++;
        }
    }
    else{
        int k = 0;
        while(k < sample_rate){
            for (int j = 0; j < camera->buffer_size.first; j++) {
                for (int i = 0; i < camera->buffer_size.second; i++) {
                    // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
                    // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                    std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
        
                    // Generate ray from camera
                    Ray ray = camera->make_ray(coords.first, coords.second);
        
                    // glm::vec3 total_intensity = compute_intensity(ray,2);
                    // glm::vec3 total_intensity = compute_direct_illumination(ray);

                    glm::vec3 total_intensity = glm::vec3(0.0f);
                    // SAMPLE MULTIPLE TIMES
                    total_intensity += compute_intensity_sampling(ray, cont, 0);
                    total_intensity /= (float) sample_rate;

        
                    // Store the color in the image
                    image.pixel(i, j) += total_intensity;
                }
            }
        // Will save image here
        // save_img_intermediate(image, k, sample_rate, camera->buffer_size.second, camera->buffer_size.first);
        if(k % save_rate == 0){
            std::cout << "Saving image at iteration " << k << std::endl;
            save_img_intermediate(image, k, sample_rate, camera->buffer_size.second, camera->buffer_size.first);
        }
        k++;
        }
    }
}

void Scene::render_normal_visualization(HDRImage& image, bool parallel){
    if(parallel){
        #pragma omp parallel for schedule(static)
        for (int j = 0; j < camera->buffer_size.first; j++) {
            for (int i = 0; i < camera->buffer_size.second; i++) {
                // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
                // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
                // Generate ray from camera
                Ray ray = camera->make_ray(coords.first, coords.second);
    
                HitRecord rec = hit_record_init();
                bool hit_anything = false;
                int n_objects = objects.size();
                for(int k=0; k<n_objects; k++){
                    if (objects[k]->hit(ray, rec)) {
                        hit_anything = true;
                    }
                }
                if(hit_anything){
                    image.pixel(i, j) = 0.5f * (rec.n + glm::vec3(1.0f));
                }
                else{
                    image.pixel(i, j) = glm::vec3(0.0f);
                }
            }
        }
    }
    else{
        for (int j = 0; j < camera->buffer_size.first; j++) {
            for (int i = 0; i < camera->buffer_size.second; i++) {
                // Convert pixel coordinates to normalized device coordinates (-1 to 1)
                std::pair<float, float> coords = camera->get_camera_plane_coords(i, j);
                // std::cout << "Rendering pixel (" << i << ", " << j << ")" << std::endl;
    
                // Generate ray from camera
                Ray ray = camera->make_ray(coords.first, coords.second);
    
                HitRecord rec = hit_record_init();
                bool hit_anything = false;
                int n_objects = objects.size();
                for(int k=0; k<n_objects; k++){
                    if (objects[k]->hit(ray, rec)) {
                        hit_anything = true;
                    }
                }
                if(hit_anything){
                    image.pixel(i, j) = 0.5f * (rec.n + glm::vec3(1.0f));
                }
                else{
                    image.pixel(i, j) = glm::vec3(0.0f);
                }
            }
        }
    }
}
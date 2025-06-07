#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e6.png";

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Light sources
    // On RIGHT
    // PointLight light_1;
    // light_1.location = glm::vec3(6.5f, 10.5f, 8.0f);
    // light_1.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 250.0f;
    // scene.lights.push_back(light_1);



    // Camera
    // Initialize the camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 3, 8),  // Moved camera 
        glm::vec3(0, 0, -1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    int shape_id = 0;

    // Ground plane
    Inf_Plane* ground_plane = new Inf_Plane(glm::vec3(0, 1, 0), -1, shape_id);
    Material* ground_plane_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* ground_plane_obj = new Object(ground_plane, ground_plane_mat);
    scene.objects.push_back(ground_plane_obj);
    shape_id++;

    // Coloured Sphere
    Sphere* sphere_1 = new Sphere(glm::vec3(4.5, 1.5, -2), 2.5, shape_id);
    Material* sphere_1_mat = new Lambertian(glm::vec3(0.9059, 0.9098, 0.8196));
    // Increase reflectivity
    sphere_1_mat->set_reflectivity(glm::vec3(0.15,0.15,0.15));
    // Increase specular
    sphere_1_mat->set_metallic_factor(glm::vec3(0.9,0.9,0.9));
    Object* sphere_1_obj = new Object(sphere_1, sphere_1_mat);
    scene.objects.push_back(sphere_1_obj);
    shape_id++;

    // A box on left with 0 reflectivity and different colour
    AA_BB* box_1 = new AA_BB(-5,-3,-1,4,-4.5,2.5, shape_id);
    Material* box_1_mat = new Lambertian(glm::vec3(0.8216,0.3137,0.2588));
    box_1_mat->set_reflectivity(glm::vec3(0.0,0.0,0.0));
    Object* box_1_obj = new Object(box_1, box_1_mat);
    scene.objects.push_back(box_1_obj);
    shape_id++;

    // Path Tracing Light source
    // Sphere* light_sphere = new Sphere(glm::vec3(6.5f, 10.5f, 8.0f), 0.5f, shape_id);
    Sphere* light_sphere = new Sphere(glm::vec3(31.5f, 35.5f, 33.f), 25.f, shape_id);
    Material* light_sphere_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    light_sphere_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* light_sphere_obj = new Object(light_sphere, light_sphere_mat);
    scene.objects.push_back(light_sphere_obj);
    shape_id++;


    // Render
    scene.update_avg_depth(4);
    scene.update_sample_rate(300);
    scene.update_save_rate(20);
    scene.render_scene_pathtracer(image, true);

    // Save

    std::cout << "Rendering complete" << std::endl;
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

}
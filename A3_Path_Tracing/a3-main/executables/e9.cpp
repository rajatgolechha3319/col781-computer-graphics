#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e9.png";
    glm::vec3 light_gray = glm::vec3(0.7f, 0.7f, 0.7f);
    // glm::vec3 sun_set = glm::vec3(0.855f, 0.796f, 0.490f);
    // Make bunny pink
    glm::vec3 bunny_color = glm::vec3(1.0f, 0.71f, 0.756f);

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Set up camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 0.75, 2.5),          // Camera position
        glm::vec3(0, 0, -1),             // Look-at point
        glm::vec3(0, 1, 0)               // Up vector
    );

    int shape_id = 0;

    // Add ground plane
    Inf_Plane* base = new Inf_Plane(glm::vec3(0, 1, 0), -0.51f, shape_id++);
    Material* base_material = new Lambertian(light_gray);
    base_material->set_reflectivity(glm::vec3(0.1f, 0.1f, 0.1f));
    Object* base_object = new Object(base, base_material);
    scene.objects.push_back(base_object);

    // Load Bunny
    std::string bunny_file_path = "/Users/darkelixir/Desktop/COL781/A3/graphics_a3_col781/a3-main/obj_files/bunny_1k.obj";
    Triangular_Mesh* bunny_shape = new Triangular_Mesh(bunny_file_path, shape_id++);
    Material* bunny_material = new Lambertian(bunny_color);
    bunny_material->set_metallic_factor(glm::vec3(0.7f, 0.7f, 0.7f));
    bunny_material->set_reflectivity(glm::vec3(0.8f, 0.8f, 0.8f));
    Object* bunny_object = new Object(bunny_shape, bunny_material);
    scene.objects.push_back(bunny_object);

    // Add a light source
    PointLight light;
    light.location = glm::vec3(-5,5,5);
    light.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 150.0f;
    scene.lights.push_back(light);


    // Render the scene
    std::cout << "Rendering..." << std::endl;
    scene.update_avg_depth(5);
    scene.update_sample_rate(500);
    scene.update_save_rate(50);
    scene.render_scene_pathtracer(image, true);

    // Save the output
    std::cout << "Rendering complete" << std::endl;
    SDL_Surface* out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

    return 0;
}
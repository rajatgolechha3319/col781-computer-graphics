#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e10.png";

    glm::vec3 muted_red = glm::vec3(0.886f, 0.447f, 0.357f); // Left wall
    glm::vec3 muted_blue = glm::vec3(0.4f, 0.6f, 0.8f);    // Right wall
    glm::vec3 pale_green = glm::vec3(0.7f, 0.9f, 0.7f);    // Back wall
    glm::vec3 medium_gray = glm::vec3(0.5f, 0.5f, 0.5f);   // Floor
    glm::vec3 pale_yellow = glm::vec3(0.98f, 0.98f, 0.9f); // Ceiling
    glm::vec3 orange = glm::vec3(0.9f, 0.5f, 0.2f);        // Sphere

    // Behind camera wall
    glm::vec3 pale_yellow_2 = glm::vec3(0.8f, 0.7f, 0.3f);

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Add a small point light for global illumination
    PointLight light_1;
    light_1.location = glm::vec3(25.f, 12.5f, -6.25f);
    light_1.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 40.0f;
    scene.lights.push_back(light_1);

    // Camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 12.5f, 27.68034f),  // Moved camera 
        glm::vec3(0, 0, -1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    int shape_id = 0;

    // Left Wall
    Inf_Plane* left_wall = new Inf_Plane(glm::vec3(1, 0, 0), -15.f, shape_id);
    Material* left_wall_mat = new Lambertian(muted_red);
    left_wall_mat->set_reflectivity(glm::vec3(0.05, 0.05, 0.05));
    Object* left_wall_obj = new Object(left_wall, left_wall_mat);
    scene.objects.push_back(left_wall_obj);
    shape_id++;

    // Right Wall
    AA_BB* right_wall = new AA_BB(15.f, 25.f, 0.0f, 25.f, 0.f, 100.f, shape_id);
    Material* right_wall_mat = new Lambertian(muted_blue);
    right_wall_mat->set_reflectivity(glm::vec3(0.05, 0.05, 0.05));
    Object* right_wall_obj = new Object(right_wall, right_wall_mat);
    scene.objects.push_back(right_wall_obj);
    shape_id++;
    
    // Back Wall
    Inf_Plane* back_wall = new Inf_Plane(glm::vec3(0, 0, 1), -12.5f, shape_id);
    Material* back_wall_mat = new Lambertian(pale_green);
    back_wall_mat->set_reflectivity(glm::vec3(0.05, 0.05, 0.05));
    Object* back_wall_obj = new Object(back_wall, back_wall_mat);
    scene.objects.push_back(back_wall_obj);
    shape_id++;

    // Floor
    Inf_Plane* floor = new Inf_Plane(glm::vec3(0, 1, 0), 0.0f, shape_id);
    Material* floor_mat = new Lambertian(medium_gray);
    floor_mat->set_reflectivity(glm::vec3(0.05, 0.05, 0.05));
    Object* floor_obj = new Object(floor, floor_mat);
    scene.objects.push_back(floor_obj);
    shape_id++;

    // Ceiling
    Inf_Plane* ceiling = new Inf_Plane(glm::vec3(0, 1, 0), 25.f, shape_id);
    Material* ceiling_mat = new Lambertian(pale_yellow);
    ceiling_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* ceiling_obj = new Object(ceiling, ceiling_mat);
    scene.objects.push_back(ceiling_obj);
    shape_id++;

    // Behind the camera wall
    Inf_Plane* front_wall = new Inf_Plane(glm::vec3(0, 0, 1), 35.0f, shape_id);
    Material* front_wall_mat = new Lambertian(pale_yellow_2);
    front_wall_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* front_wall_obj = new Object(front_wall, front_wall_mat);
    scene.objects.push_back(front_wall_obj);
    shape_id++;


    // Sphere on the floor near left back corner
    Sphere* sphere_1 = new Sphere(glm::vec3(-5.0f, 5.f, -4.0f), 5.f, shape_id);
    Material* sphere_1_mat = new Lambertian(orange);
    sphere_1_mat->set_reflectivity(glm::vec3(0.3, 0.3, 0.3));
    sphere_1_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* sphere_1_obj = new Object(sphere_1, sphere_1_mat);
    scene.objects.push_back(sphere_1_obj);
    shape_id++;

    // Add RGB Strips on left wall 
    // Red Strip
    AA_BB* left_wall_strip = new AA_BB(-15.f, -14.9f, 18.0f, 18.25f, -12.5f, 100.f, shape_id);
    Material* left_wall_strip_mat = new Lambertian(glm::vec3(0.8f,0.1f,0.1f));
    left_wall_strip_mat->set_luminosity(glm::vec3(0.8f,0.1f,0.1f) * 5.0f);
    Object* left_wall_strip_obj = new Object(left_wall_strip, left_wall_strip_mat);
    scene.objects.push_back(left_wall_strip_obj);
    shape_id++;

    // Green Strip
    AA_BB* left_wall_strip2 = new AA_BB(-15.f, -14.9f, 17.f, 17.25f, -12.5f, 100.f, shape_id);
    Material* left_wall_strip2_mat = new Lambertian(glm::vec3(0.1f,0.8f,0.1f));
    left_wall_strip2_mat->set_luminosity(glm::vec3(0.1f,0.8f,0.1f) * 5.0f);
    Object* left_wall_strip2_obj = new Object(left_wall_strip2, left_wall_strip2_mat);
    scene.objects.push_back(left_wall_strip2_obj);
    shape_id++;

    // Blue Strip
    AA_BB* left_wall_strip3 = new AA_BB(-15.f, -14.9f, 16.f, 16.25f, -12.5f, 100.f, shape_id);
    Material* left_wall_strip3_mat = new Lambertian(glm::vec3(0.1f,0.1f,0.8f));
    left_wall_strip3_mat->set_luminosity(glm::vec3(0.1f,0.1f,0.8f) * 5.0f);
    Object* left_wall_strip3_obj = new Object(left_wall_strip3, left_wall_strip3_mat);
    scene.objects.push_back(left_wall_strip3_obj);
    shape_id++;

    // Add RGB Strips on Back wall
    // Red Strip
    AA_BB* back_wall_strip = new AA_BB(-15.f, 25.f, 18.0f, 18.25f, -12.5f, -12.4f, shape_id);
    Material* back_wall_strip_mat = new Lambertian(glm::vec3(0.8f,0.1f,0.1f));
    back_wall_strip_mat->set_luminosity(glm::vec3(0.8f,0.1f,0.1f) * 5.0f);
    Object* back_wall_strip_obj = new Object(back_wall_strip, back_wall_strip_mat);
    scene.objects.push_back(back_wall_strip_obj);
    shape_id++;

    // Green Strip
    AA_BB* back_wall_strip2 = new AA_BB(-15.f, 25.f, 17.f, 17.25f, -12.5f, -12.4f, shape_id);
    Material* back_wall_strip2_mat = new Lambertian(glm::vec3(0.1f,0.8f,0.1f));
    back_wall_strip2_mat->set_luminosity(glm::vec3(0.1f,0.8f,0.1f) * 5.0f);
    Object* back_wall_strip2_obj = new Object(back_wall_strip2, back_wall_strip2_mat);
    scene.objects.push_back(back_wall_strip2_obj);
    shape_id++;

    // Blue Strip
    AA_BB* back_wall_strip3 = new AA_BB(-15.f, 25.f, 16.f, 16.25f, -12.5f, -12.4f, shape_id);
    Material* back_wall_strip3_mat = new Lambertian(glm::vec3(0.1f,0.1f,0.8f));
    back_wall_strip3_mat->set_luminosity(glm::vec3(0.1f,0.1f,0.8f) * 5.0f);
    Object* back_wall_strip3_obj = new Object(back_wall_strip3, back_wall_strip3_mat);
    scene.objects.push_back(back_wall_strip3_obj);
    shape_id++;


    // Add ceiling lights (unchanged)
    AA_BB* ceiling_light_1 = new AA_BB(-0.25f, 0.25f, 24.5f, 25.0f, 14.5f, 15.0f, shape_id);
    Material* ceiling_light_1_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    ceiling_light_1_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* ceiling_light_1_obj = new Object(ceiling_light_1, ceiling_light_1_mat);
    scene.objects.push_back(ceiling_light_1_obj);
    shape_id++;

    PointLight ceil_light_1;
    ceil_light_1.location = glm::vec3(0.f, 24.4f, 14.75f);
    ceil_light_1.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
    scene.lights.push_back(ceil_light_1);
    
    AA_BB* ceiling_light_2 = new AA_BB(-0.25f, 0.25f, 24.5f, 25.0f, 9.5f, 10.0f, shape_id);
    Material* ceiling_light_2_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    ceiling_light_2_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* ceiling_light_2_obj = new Object(ceiling_light_2, ceiling_light_2_mat);
    scene.objects.push_back(ceiling_light_2_obj);
    shape_id++;

    PointLight ceil_light_2;
    ceil_light_2.location = glm::vec3(0.f, 24.4f, 9.75f);
    ceil_light_2.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
    scene.lights.push_back(ceil_light_2);

    AA_BB* ceiling_light_3 = new AA_BB(-0.25f, 0.25f, 24.5f, 25.0f, 4.5f, 5.0f, shape_id);
    Material* ceiling_light_3_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    ceiling_light_3_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* ceiling_light_3_obj = new Object(ceiling_light_3, ceiling_light_3_mat);
    scene.objects.push_back(ceiling_light_3_obj);
    shape_id++;

    PointLight ceil_light_3;
    ceil_light_3.location = glm::vec3(0.f, 24.4f, 4.75f);
    ceil_light_3.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
    scene.lights.push_back(ceil_light_3);

    AA_BB* ceiling_light_4 = new AA_BB(-0.25f, 0.25f, 24.5f, 25.0f, -0.5f, 0.0f, shape_id);
    Material* ceiling_light_4_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    ceiling_light_4_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* ceiling_light_4_obj = new Object(ceiling_light_4, ceiling_light_4_mat);
    scene.objects.push_back(ceiling_light_4_obj);
    shape_id++;

    PointLight ceil_light_4;
    ceil_light_4.location = glm::vec3(0.f, 24.4f, -0.75f);
    ceil_light_4.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
    scene.lights.push_back(ceil_light_4);

    AA_BB* ceiling_light_5 = new AA_BB(-0.25f, 0.25f, 24.5f, 25.0f, -5.5f, -6.0f, shape_id);
    Material* ceiling_light_5_mat = new Lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    ceiling_light_5_mat->set_luminosity(glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f);
    Object* ceiling_light_5_obj = new Object(ceiling_light_5, ceiling_light_5_mat);
    scene.objects.push_back(ceiling_light_5_obj);
    shape_id++;

    PointLight ceil_light_5;
    ceil_light_5.location = glm::vec3(0.f, 24.4f, -5.75f);
    ceil_light_5.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
    scene.lights.push_back(ceil_light_5);

    // Add a metallic Box like mirror adjacent to the right wall
    AA_BB* right_wall_box = new AA_BB(14.5f, 15.0f, 0.0f, 25.f, 3.0f, 9.0f, shape_id);
    // Metallic gray color
    Material* right_wall_box_mat = new Lambertian(glm::vec3(0.8f, 0.8f, 0.8f));
    right_wall_box_mat->set_reflectivity(glm::vec3(0.9, 0.9, 0.9));
    right_wall_box_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* right_wall_box_obj = new Object(right_wall_box, right_wall_box_mat);
    scene.objects.push_back(right_wall_box_obj);
    shape_id++;

    // Add one more metallic box like mirror adjacent to the left wall
    AA_BB* left_wall_box = new AA_BB(-15.0f, -14.5f, 0.0f, 25.f, 8.0f, 14.0f, shape_id);
    // Metallic gray color
    Material* left_wall_box_mat = new Lambertian(glm::vec3(0.8f, 0.8f, 0.8f));
    left_wall_box_mat->set_reflectivity(glm::vec3(0.9, 0.9, 0.9));
    left_wall_box_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* left_wall_box_obj = new Object(left_wall_box, left_wall_box_mat);
    scene.objects.push_back(left_wall_box_obj);
    shape_id++;

    // Add one more metallic box like mirror adjacent to the back wall
    AA_BB* back_wall_box = new AA_BB(-3.0f, 7.0f, 0.0f, 25.f, -12.5f, -12.0f, shape_id);
    // Metallic gray color
    Material* back_wall_box_mat = new Lambertian(glm::vec3(0.8f, 0.8f, 0.8f));
    back_wall_box_mat->set_reflectivity(glm::vec3(0.9, 0.9, 0.9));
    back_wall_box_mat->set_metallic_factor(glm::vec3(0.9, 0.9, 0.9));
    Object* back_wall_box_obj = new Object(back_wall_box, back_wall_box_mat);
    scene.objects.push_back(back_wall_box_obj);
    shape_id++;

    // Render
    scene.update_avg_depth(5);
    scene.update_sample_rate(250);
    scene.update_save_rate(50);
    scene.render_scene_pathtracer(image, true);

    // Save
    std::cout << "Rendering complete" << std::endl;
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;
}
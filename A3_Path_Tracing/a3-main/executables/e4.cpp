#include "../revamp_src/classes/scene.hpp"
#include <iostream>
#include <omp.h>
#include <vector>

int main() {
    // Window and rendering settings
    const int width = 900;
    const int height = 600;
    const std::string outputFilename = "e4.png";

    // Create image and scene
    HDRImage image(width, height);
    Scene scene;

    // Light sources
    PointLight light1;
    light1.location = (glm::vec3(-5.0f,5.0f,5.0f));
    light1.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 100.0f;
    scene.lights.push_back(light1);

    PointLight light2;
    light2.location = glm::vec3(5.0f,5.0f,5.0f);
    light2.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 100.0f;
    scene.lights.push_back(light2);

    // Back light
    PointLight light3;
    light3.location = glm::vec3(0.0f, 35.0f, -35.0f);
    light3.intensity = glm::vec3(1.0f, 1.0f, 1.0f) * 250.0f;
    scene.lights.push_back(light3);



    // Camera
    // Initialize the camera
    scene.start_camera(height, width, 90.0f,
        glm::vec3(0, 5, 0),  // Moved camera 
        glm::vec3(0, 0, -1),    // Looking direction
        glm::vec3(0, 1, 0)     // Up vector
    );

    int shape_id = 0;

    // Ground plane
    Inf_Plane* ground_plane = new Inf_Plane(glm::vec3(0, 1, 0), 0, shape_id);
    Material* ground_plane_mat = new Lambertian(glm::vec3(1.0,1.0,1.0));
    Object* ground_plane_obj = new Object(ground_plane, ground_plane_mat);
    scene.objects.push_back(ground_plane_obj);
    shape_id++;

    // Ellipsoids
    Sphere* ellipsoid1 = new Sphere(glm::vec3(0.0f, 2.0f, -12.0f), 2.0f, shape_id);
    Material* ellipsoid1_mat = new Lambertian(glm::vec3(0.7, 0.2, 0.5));
    Object* ellipsoid1_obj = new Object(ellipsoid1, ellipsoid1_mat);
    scene.objects.push_back(ellipsoid1_obj);
    shape_id++;

    // Now we will transform this
    glm::mat4 M1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
    scene.objects[shape_id-1]->set_transform(M1);

    Sphere* ellipsoid2 = new Sphere(glm::vec3(4.0f, 2.0f, -8.0f), 2.0f, shape_id);
    Material* ellipsoid2_mat = new Lambertian(glm::vec3(0.5, 0.2, 0.7));
    Object* ellipsoid2_obj = new Object(ellipsoid2, ellipsoid2_mat);
    scene.objects.push_back(ellipsoid2_obj);
    shape_id++;

    // Now we will transform this
    glm::mat4 M2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.5f, 1.0f));
    scene.objects[shape_id-1]->set_transform(M2);

    // Now some boxes on left 
    AA_BB* box1 = new AA_BB(-5,-2, 0, 2, -15, -12, shape_id);
    Material* box1_mat = new Lambertian(glm::vec3(0.8, 0.3, 0.4));
    Object* box1_obj = new Object(box1, box1_mat);
    scene.objects.push_back(box1_obj);
    shape_id++;
    // Now we will transform this
    // Rotate by 45 degrees around the Y axis
    glm::mat4 M3 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0));
    // Also translate it by 3 to right
    M3 = glm::translate(M3, glm::vec3(5.0f, 0.0f, 0.0f));

    scene.objects[shape_id-1]->set_transform(M3);

    // One more box on left
    AA_BB* box2 = new AA_BB(-5, -2, 0, 8, -20, -15, shape_id);
    Material* box2_mat = new Lambertian(glm::vec3(0.8, 0.8, 0.3));
    Object* box2_obj = new Object(box2, box2_mat);
    scene.objects.push_back(box2_obj);
    shape_id++;
    // Now we will transform this
    // Rotate by 75 degrees around the Y axis
    glm::mat4 M4 = glm::rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0, 1, 0));
    // Also translate it by 5 to right
    M4 = glm::translate(M4, glm::vec3(12.0f, 0.0f, 2.0f));
    scene.objects[shape_id-1]->set_transform(M4);

    // One blue box on right
    AA_BB* box3 = new AA_BB(-1, 1, 0, 5, -1, 1, shape_id);
    Material* box3_mat = new Lambertian(glm::vec3(0.2, 0.2, 0.8));
    // box3_mat->set_reflectivity(glm::vec3(0.0, 0.0, 0.0));
    Object* box3_obj = new Object(box3, box3_mat);
    scene.objects.push_back(box3_obj);
    shape_id++;

    // Rotate by 120 degrees around the Y axis
    glm::mat4 M5 = glm::rotate(glm::mat4(1.0f), glm::radians(120.0f), glm::vec3(0, 1, 0));
    // Translate to right of the scene around 8 in X
    M5 = glm::translate(M5, glm::vec3(2.0f, 0.0f, 15.0f));
    scene.objects[shape_id-1]->set_transform(M5);

    scene.camera->rotate_gyro(-7.0f, glm::vec3(0.0f, 1.0f, 0.0f), scene.camera->eye);
    // Render
    scene.update_reflection_depth(3);
    scene.render_scene_raytracer(image, true);

    // Save

    std::cout << "Rendering complete" << std::endl;
    SDL_Surface *out = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    tonemap(image, out, 1, 2.2f);
    IMG_SavePNG(out, outputFilename.c_str());
    SDL_FreeSurface(out);
    std::cout << "Image saved to " << outputFilename << std::endl;

}
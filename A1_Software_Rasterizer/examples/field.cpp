#include "../src/a1.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace R = COL781::Hardware;
// namespace R = COL781::Software;

using namespace glm;

int main() {
    R::Rasterizer r;

    int width = 800, height = 640;

    if (!r.initialize("Example 8", width, height))
        return EXIT_FAILURE;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsNormalTransform(),
        r.fsSpecularLighting()
    );

    // Platform vertices (XZ plane, Y=0)
    float vertices[] = {
        -2.0f, 0.0f, -2.0f, 1.0f,
        2.0f, 0.0f, -2.0f, 1.0f,
        -2.0f, 0.0f, 2.0f, 1.0f,
        2.0f, 0.0f, 2.0f, 1.0f
    };

    // All normals point upward (Y+)
    float normals[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Two triangles forming a quad
    int triangles[] = {
        0, 1, 2,
        1, 3, 2
    };

    R::Object platform = r.createObject();
    r.setVertexAttribs(platform, 0, 4, 4, vertices);
    r.setVertexAttribs(platform, 1, 4, 3, normals);
    r.setTriangleIndices(platform, 2, triangles);

    r.enableDepthTest();

    // Lighting parameters
    vec3 ambientColor(0.2f, 0.2f, 0.3f);
    vec3 specularColor(0.8f, 0.8f, 1.0f);
    vec3 lightColor(1.0f, 1.0f, 1.0f);
    vec3 lightDir;

    // Color definitions:
    vec3 light_color(0.7f, 1.0f, 0.7f);    // Bright mint green
    vec3 dark_color(0.2f, 0.45f, 0.2f);    // Rich emerald green

    // Create view and projection matrices
    mat4 view = lookAt(vec3(3.0f, 2.5f, 3.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), (float)width/height, 0.1f, 100.0f);

    // Declare transformation matrices outside the loop
    mat4 rotation, transform;

    while (!r.shouldQuit()) {
        float currentTime = SDL_GetTicks64() * 1e-3f;
        
        r.clear(vec4(0.1f, 0.1f, 0.1f, 1.0f));
        r.useShaderProgram(program);
        
        // Animate light direction
        lightDir = normalize(vec3(sin(currentTime * 0.5f), 0.8f, cos(currentTime * 0.5f)));
        r.setUniform(program, "lightColor", lightColor);
        r.setUniform(program, "lightDir", lightDir);
        r.setUniform(program, "ambientColor", ambientColor);
        r.setUniform(program, "specularColor", specularColor);
        r.setUniform(program, "viewPos", vec3(3.0f, 2.5f, 3.0f));
        r.setUniform(program, "blinnpow", 16);
        
        // Common rotation for all pieces
        rotation = rotate(mat4(1.0f), radians(currentTime * 10.0f), vec3(0.0f, 1.0f, 0.0f));
        
        // Create 16 smaller squares
        int row = 0;
        while (row < 4) {
            int col = 0;
            while (col < 4) {
                // Calculate position
                float x = -1.5f + col * 1.0f;
                float z = -1.5f + row * 1.0f;
                
                // Alternate colors
                vec3 tileColor = ((row + col) % 2 == 0) ? light_color : dark_color;
                
                // Create transformation matrix
                transform = rotation *
                            translate(mat4(1.0f), vec3(x, 0.0f, z)) *
                            scale(mat4(1.0f), vec3(0.25f, 1.0f, 0.25f));
                
                // Set uniforms and draw
                r.setUniform(program, "transform", projection * view * transform);
                r.setUniform(program, "wsTransform", transform);
                r.setUniform(program, "objectColor", tileColor);
                r.drawObject(platform);
                
                col++;
            }
            row++;
        }
        
        r.show();
    }

    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}

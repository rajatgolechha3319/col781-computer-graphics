#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ctime>

namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

glm::ivec3 getHMS() {
    time_t now = time(0);
    struct tm *ltm = localtime(&now);
    
    int hour = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec;
    
    return glm::ivec3(hour, minute, second);
}

int main() {
    R::Rasterizer r;
    if (!r.initialize("Example 7", 640, 640))
        return EXIT_FAILURE;
    R::ShaderProgram program = r.createShaderProgram(
        r.vsTransform(),
        r.fsConstant()
    );
    float square[] = {
        -0.5,0.5,0.0,1.0,
        0.5,0.5,0.0,1.0,
        0.5,-0.5,0.0,1.0,
        -0.5,-0.5,0.0,1.0
    };
    int triangles[] = {
        0, 1, 2,
        0, 2, 3
    };
    R::Object clock = r.createObject();
    r.setVertexAttribs(clock, 0, 4, 4, square);
    r.setTriangleIndices(clock, 2, triangles);

    mat4 outer_hour_ring = mat4(1.0f);
    mat4 outer_minute_ring = mat4(1.0f);
    mat4 hour_hand = mat4(1.0f);
    mat4 minute_hand = mat4(1.0f);
    mat4 second_hand = mat4(1.0f);
    mat4 clock_centre = mat4(1.0f);
    mat4 clock_bg = mat4(1.0f);
    mat4 scale1 = glm::scale(mat4(1.0f), vec3(0.05f, 0.13f, 1.0f));
    mat4 scale2 = glm::scale(mat4(1.0f), vec3(0.01f, 0.05f, 1.0f));
    mat4 scale3 = glm::scale(mat4(1.0f), vec3(0.03f, 0.4f, 1.0f)); // Hour hand
    mat4 scale4 = glm::scale(mat4(1.0f), vec3(0.02f, 0.70f, 1.0f)); // Minute hand
    mat4 scale5 = glm::scale(mat4(1.0f), vec3(0.008f, 0.52f, 1.0f)); // Second hand
    mat4 scale6 = glm::scale(mat4(1.0f), vec3(0.015f, 0.015f, 1.0f)); // Clock centre
    mat4 scale7 = glm::scale(mat4(1.0f), vec3(0.1f, 0.9f, 1.0f)); // Clock bg
    mat4 shift1 = glm::translate(mat4(1.0f), vec3(0.0f, 0.75f, 0.0f)); // Shift for outer ring Hour
    mat4 shift2 = glm::translate(mat4(1.0f), vec3(0.0f, 0.79f, 0.0f)); // Shift for outer ring Minute

    ivec3 hms_data = getHMS();
    r.enableDepthTest();
    while(!r.shouldQuit()) {
        r.clear(vec4(0.85, 0.8, 0.9, 1.0));
        r.useShaderProgram(program);
        // Drawing the outer ring
        int i = 0;
        while(i < 12){
        outer_hour_ring = mat4(1.0f);
        outer_hour_ring = glm::rotate(outer_hour_ring, radians(30.0f * i), normalize(vec3(0.0f, 0.0f, 1.0f)));
        outer_hour_ring = outer_hour_ring * shift1 * scale1;
        r.setUniform(program, "transform", outer_hour_ring);
        r.setUniform<vec4>(program, "color", vec4(1.0, 1.0, 1.0, 1.0));
        r.drawObject(clock);
        i++;
        }
        i = 0;
        while(i < 60){
        outer_minute_ring = mat4(1.0f);
        outer_minute_ring = glm::rotate(outer_minute_ring, radians(6.0f * i), normalize(vec3(0.0f, 0.0f, 1.0f)));
        outer_minute_ring = outer_minute_ring * shift2 * scale2;
        r.setUniform(program, "transform", outer_minute_ring);
        r.setUniform<vec4>(program, "color", vec4(1.0, 1.0, 1.0, 1.0));
        r.drawObject(clock);
        i++;
        }
        // Fetch time
        hms_data = getHMS();
        // Draw the Hour hand
        hour_hand = mat4(1.0f);
        hour_hand = glm::rotate(hour_hand, radians(-1.0f * (30.0f * hms_data.x + 0.5f * hms_data.y + 0.00833f * hms_data.z)), normalize(vec3(0.0f, 0.0f, 1.0f)));
        hour_hand = hour_hand * scale3;
        hour_hand = glm::translate(hour_hand, vec3(0.0f, 0.2f, 0.0f));
        r.setUniform(program, "transform", hour_hand);
        r.setUniform<vec4>(program, "color", vec4(1.0, 1.0, 1.0, 1.0));
        r.drawObject(clock);

        // Draw the minute hand
        minute_hand = mat4(1.0f);
        minute_hand = glm::rotate(minute_hand, radians(-6.0f * (hms_data.y + 0.01667f * hms_data.z)), normalize(vec3(0.0f, 0.0f, 1.0f)));
        minute_hand = minute_hand * scale4;
        minute_hand = glm::translate(minute_hand, vec3(0.0f, 0.36f, 0.0f));
        r.setUniform(program, "transform", minute_hand);
        r.setUniform<vec4>(program, "color", vec4(1.0, 1.0, 1.0, 1.0));
        r.drawObject(clock);

        // Draw the second hand
        second_hand = mat4(1.0f);
        second_hand = glm::rotate(second_hand, radians(-6.0f * hms_data.z), normalize(vec3(0.0f, 0.0f, 1.0f)));
        second_hand = second_hand * scale5;
        second_hand = glm::translate(second_hand, vec3(0.0f, 0.23f, 0.0f));
        r.setUniform(program, "transform", second_hand);
        r.setUniform<vec4>(program, "color", vec4(0.976, 0.411, 0.055, 1.0));
        r.drawObject(clock);

        // Mark the centre of the clock
        clock_centre = mat4(1.0f);
        clock_centre = clock_centre * scale6;
        r.setUniform(program, "transform", clock_centre);
        r.setUniform<vec4>(program, "color", vec4(0.0, 1.0, 0.0, 0.6));
        r.drawObject(clock);

        i=0;
        while(i < 60){
            clock_bg = mat4(1.0f);
            clock_bg = glm::rotate(clock_bg, radians(6.0f * i), normalize(vec3(0.0f, 0.0f, 1.0f)));
            clock_bg = clock_bg * scale7;
            clock_bg = glm::translate(clock_bg, vec3(0.0f, 0.4f, 0.1f));
            r.setUniform(program, "transform", clock_bg);
            r.setUniform<vec4>(program, "color", vec4(0.0, 0.0, 0.0, 0.65));
            r.drawObject(clock);
            i++;
        }
        r.show();
    }
}

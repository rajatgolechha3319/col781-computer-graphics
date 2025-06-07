#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct time_position{
    float t;
    glm::vec3 P;
};

class part1{
    private:
        std::vector<glm::vec3> tangents;
        int get_interval_index(float time_frame);
    public:
    void initialize(std::vector<time_position> time_positions);
    std::vector<time_position> time_positions;
    glm::vec3 func(float time_frame);
};

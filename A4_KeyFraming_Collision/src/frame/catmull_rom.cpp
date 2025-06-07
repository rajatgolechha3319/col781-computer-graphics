#include "catmull_rom.hpp"


// These are the points that we will use to create the catmull rom spline

// t=0 P=0,0
// t=1 P=1,0.8
// t=1.5 P=1.5,0.97
// t=1.6 P=1.6,0.99
// t=3.14 P=3.14,0

// The spline should be like a sine curve, from which these points are sampled



// First step is to calculate the tangents
// We will use the formula -> m_i = ((t_i+1 - t_i) * (P_i - P_i-1) / (t_i - t_i-1) + (t_i - t_i-1) * (P_i+1 - P_i) / (t_i+1 - t_i)) / (t_i+1 - t_i-1)
// For end points we use one sided version

// We get the following points
// t=0 P=0,0 m=0.8
// t=1 P=1,0.8 m=0.49333333
// t=1.5 P=1.5,0.97 m=0.22333333
// t=1.6 P=1.6,0.99 m=0.14860627
// t=3.14 P=3.14,0 m=-0.64285714

glm::vec3 cubic_interpolate_3D(time_position &P1, time_position &P2, glm::vec3 &m1, glm::vec3 &m2, float t){
    float s = (t - P1.t) / (P2.t - P1.t);
    return (
        (2 * s * s * s - 3 * s * s + 1) * P1.P +
        (s * s * s - 2 * s * s + s) * m1 * (P2.t - P1.t) +
        (-2 * s * s * s + 3 * s * s) * P2.P +
        (s * s * s - s * s) * m2 * (P2.t - P1.t)
    );
}

// HELPER FUNCTIONS
glm::vec3 tan_cal(time_position t_minus_1, time_position t_plus_1, time_position t){
    float del_i = t_plus_1.t - t.t;
    float del_i_minus_1 = t.t - t_minus_1.t;
    return (
        ((del_i * del_i) * (t.P - t_minus_1.P) +
        (del_i_minus_1 * del_i_minus_1) * (t_plus_1.P - t.P))
        / (del_i * del_i_minus_1 * (del_i + del_i_minus_1))
        );
}

void part1::initialize(std::vector<time_position> time_positions){
    this->time_positions = time_positions;
    int n = time_positions.size();
    // Let's compute tangents
    int i;
    // i = 0
    std::vector<glm::vec3> tangents(n);
    glm::vec3 m_0 = (time_positions[1].P - time_positions[0].P) / (time_positions[1].t - time_positions[0].t);
    tangents[0] = m_0;
    // i = n-1
    glm::vec3 m_n = (time_positions[n-1].P - time_positions[n-2].P) / (time_positions[n-1].t - time_positions[n-2].t);
    tangents[n-1] = m_n;

    // i = 1 to n-2
    i = 1;
    while(i < n-1){
        tangents[i] = tan_cal(time_positions[i-1], time_positions[i+1], time_positions[i]);
        i++;
    }

    this->tangents = tangents;
}

int part1::get_interval_index(float time_frame){
    // Assuming time_positions start from 0
    // and are in increasing order
    float EPS = 1e-4;
    float total_time_frame = time_positions[time_positions.size()-1].t;
    time_frame = fmod(fmod(time_frame, total_time_frame) + total_time_frame, total_time_frame);
    int i = 0;
    while(i < time_positions.size()-1){
        if((time_frame >= (time_positions[i].t - EPS)) && (time_frame <= (time_positions[i+1].t + EPS))){
            return i;
        }
        i++;
    }
    throw std::runtime_error("Hallelujah! Error in get_intervul_index");
    return -1;
}

// MAIN FUNCTION
glm::vec3 part1::func(float time_frame){
    int i = get_interval_index(time_frame);

    time_position P1 = time_positions[i];
    time_position P2 = time_positions[i+1];
    glm::vec3 m1 = tangents[i];
    glm::vec3 m2 = tangents[i+1];
    float total_time_frame = time_positions[time_positions.size()-1].t;
    time_frame = fmod(fmod(time_frame, total_time_frame) + total_time_frame, total_time_frame);
    return cubic_interpolate_3D(P1, P2, m1, m2, time_frame);
}


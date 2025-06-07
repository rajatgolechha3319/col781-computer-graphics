#include "HitRecord.hpp"

HitRecord hit_record_init(){
    HitRecord rec;
    rec.t = FLT_MAX;
    rec.t_range_min = 0.001f; // Bias value
    rec.t_range_max = rec.t;
    rec.mat = nullptr;
    rec.id = -1;
    return rec;
}
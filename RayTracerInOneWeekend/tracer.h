#ifndef TRACER_H
#define TRACER_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

//usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utility functions
inline double degrees_to_radians(double degrees){
    return degrees * pi / 180.0;
}

inline double random_double(){
    //return random number from [0, 1)
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max){
    //return random number from [min, max)
    return min + (max - min) * random_double();
}

//common headers
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
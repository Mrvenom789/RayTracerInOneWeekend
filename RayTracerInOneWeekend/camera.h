#ifndef CAMERA_H
#define CAMERA_H

#include "tracer.h"
#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>

class camera {
    public:
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;
        double vfov = 90; //vertical fov
        point3 lookfrom = point3(0, 0, -1);
        point3 lookat = point3(0, 0, 0);
        vec3 vup = vec3(0, 1, 0);

        double defocus_angle = 0;
        double focus_dist = 10;

        void render(const hittable& world) {
            initialize();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; ++j) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; ++i) {
                    color pixel_color(0, 0, 0);
                    for(int k = 0; k < samples_per_pixel; ++k){
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r,max_depth, world);
                    }
                    
                    write_color(std::cout, pixel_color, samples_per_pixel);
                }
            }

            std::clog << "\rDone.                 \n";
        }
    private:
        int image_height;
        point3 center; //camera center
        point3 pixel00_loc; //location of pixel (0, 0)
        vec3 pixel_delta_u; //pixel offset to right
        vec3 pixel_delta_v; //pixel offset down
        vec3 u, v, w;
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;
        void initialize() {
            //calculate image height, make sure it's at least 1
            image_height = static_cast<int>(image_width/aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;
            center = lookfrom;

            //camera
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            //calculate across/down horizontal/vertical viewpoint edges
            auto viewport_u = viewport_width * u;
            auto viewport_v = viewport_height * -v;

            //calculate horizontal/vertical delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            //calculate location of upper left pixel
            auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            //calculate the camera defocus disk basis vectors
            auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = defocus_radius * u;
            defocus_disk_v = defocus_radius * v;
        }

        ray get_ray(int i, int j) const{
            //get a randomly sampled camera ray for pixel at (i, j)
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_sample_square();
            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        vec3 pixel_sample_square() const {
            //return random point in square around pixel in origin
            auto px = -0.5 * random_double();
            auto py = -0.5 * random_double();
            return (px * pixel_delta_u) + (py * pixel_delta_v);
        }

        point3 defocus_disk_sample() const {
            //return random point in the camera defocus disk
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            hit_record rec;

            if(depth <= 0){
                return color(0, 0, 0);
            }

            if(world.hit(r, interval(0.001, infinity), rec)){
                ray scattered;
                color attenuation;
                if(rec.mat->scatter(r, rec, attenuation, scattered)){
                    return attenuation * ray_color(scattered, depth - 1, world);
                }
                return color(0, 0, 0);
            }

            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        }
};

#endif
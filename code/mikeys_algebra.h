#ifndef MIKEYS_ALGEBRA
#define MIKEYS_ALGEBRA

#include <cstdio>
#include <cmath>
using namespace std;

namespace mikeys_algebra {
    struct d_rect {
        double x;
        double y;
        double w;
        double h;
    };

    struct vec2d {
        double x;
        double y;
    };

    struct sec_result {
        bool success = false;
        vec2d sec_point = {0,0};
    };

    // short for slope-intercept form
    class siform {
    private:
        double slope; bool undefined;
        double y_intercept;

        // for when the slope is undefined
        double x_intercept;

    public:
        // slope intercept form
        void set_value(double m, double b) {
            slope = m; undefined = false;
            y_intercept = b;
        }

        // undefined slope
        void set_value(double cept) {
            x_intercept = cept; undefined = true;
        }

        // wrappers for initialization
        siform(double m, double b) {
            set_value(m, b);
        }

        siform(double cept) {
            set_value(cept);
        }

        sec_result intersects(siform& other) {
            sec_result r;

            // no intersection if the lines are parallel
            if ((undefined && other.undefined)
                || (slope == other.slope)) {
                return r;
            }

            if (undefined) {
                r.sec_point.x = x_intercept;
                r.sec_point.y = (r.sec_point.x * other.slope) + other.y_intercept;
            }
            else if (other.undefined) {
                r.sec_point.x = other.x_intercept;
                r.sec_point.y = (r.sec_point.x * slope) + y_intercept;
            }
            else {
                r.sec_point.x = (other.y_intercept - y_intercept) / (slope - other.slope);
                r.sec_point.y = (r.sec_point.x * slope) + y_intercept;
            }
            r.success = true;

            return r;
        }

        void print_eq() {
            if (undefined) {
                printf("x = %f\n", x_intercept);
            }
            else if (y_intercept != 0) {
                if (y_intercept > 0) {
                    printf("y = %fx + %f\n", slope, y_intercept);
                }
                else {
                    printf("y = %fx - %f\n", slope, y_intercept * -1);
                }
            }
            else {
                printf("y = %fx\n", slope);
            }
        }
    };

    siform pts_to_eq(vec2d first, vec2d second) {
        double run = second.x - first.x;
        if (run == 0) {
            return siform(first.x);
        }
        double rise = second.y - first.y;
        double slope = rise / run;
        double y_intercept = first.y - (slope * first.x);
        return siform(slope, y_intercept);
    }

    // normals are calculated on the left side
    vec2d surface_normal(vec2d first, vec2d second) {
        vec2d normal = { -(second.y - first.y),second.x - first.y }; \
            double mag = hypot(normal.x, normal.y);
        normal.x = normal.x / mag;
        normal.y = normal.y / mag;
        return normal;
    }

    double hypot(double a, double b) {
        return sqrt((a * a) + (b * b));
    }

    double dot_product(vec2d a, vec2d b) {
        return (a.x*b.x)+(a.y*b.y);
    }

    vec2d scalar_mult(vec2d vec, double scalar) {
        vec2d resultant = {vec.x*scalar, vec.y*scalar};
        return resultant;
    }

    vec2d add_vec(vec2d a, vec2d b) {
        vec2d c = { a.x + b.x , a.y + b.y };
        return c;
    }

    vec2d subtract_vec(vec2d a, vec2d b) {
        vec2d c = { a.x - b.x , a.y - b.y };
        return c;
    }

    vec2d pts_to_vec(vec2d first, vec2d second){
        vec2d vec = { second.x - first.x , second.y - first.y };
        return vec;
    }

    vec2d normalize(vec2d vec) {
        return scalar_mult(vec,1/hypot(vec.x,vec.y));
    }

    struct line_segment {
        vec2d first;
        vec2d second;
    };
}

#endif

#include <math.h>
#include "image.h"
#include <stdio.h>
#include<unistd.h>

//given 2 linear equations a*new1+b = old1, a*new2+b = old2, solve for a and b
//eg usage: Resizing a 4x4 image to 7x7. Given the x coordinates -.5 -> -.5, 3.5 -> 6.5, 
//solve_system_of_equations(-.5, -.5, 3.5, 6.5);
void solve_system_of_equations(float new1, float old1, float new2, float old2, double* coefficients){
    double determinant = (new2 - new1);
    double a = (1.0 / determinant) * (old2 - old1);
    float b = (1.0 / determinant) * (new2 * old1 - new1 * old2);
    coefficients[0] = a;
    coefficients[1] = b;
}
float nn_interpolate(image im, float x, float y, int c)
{
    int nearest_x = (int) roundf(x);
    int nearest_y = (int) roundf(y);
    if(x < 0.038569 + .001 && x > .035869 - .01 && y < 287.499969 + .001 && y > 287.499969 + .01 && c == 0){
        printf("nearest neighbor to (%f, %f) is (%d, %d)\n", x, y, nearest_x, nearest_y);
    }
    return get_pixel(im, nearest_x, nearest_y, c);
}


image nn_resize(image im, int w, int h)
{
    image resized_img = make_image(w,h,im.c);
    double x_coefficients[2];
    float x_old_top_left = -0.5f;
    float x_new_top_left = -0.5f;
    float x_old_bottom_right = im.w - 0.5f;
    float x_new_bottom_right = resized_img.w - 0.5f;
    solve_system_of_equations(x_new_top_left, x_old_top_left, x_new_bottom_right, x_old_bottom_right, x_coefficients);

    double y_coefficients[2];
    float y_old_top_left = -0.5f;
    float y_new_top_left = -0.5f;
    float y_old_bottom_right = im.h - 0.5f;
    float y_new_bottom_right = resized_img.h - 0.5f;
    solve_system_of_equations(y_new_top_left, y_old_top_left, y_new_bottom_right, y_old_bottom_right, y_coefficients);
    for(int x = 0; x < resized_img.w; ++x){
        for(int y = 0; y < resized_img.h; ++y){
            float old_coordinate_x_f = x_coefficients[0] * x + x_coefficients[1];
            float old_coordinate_y_f = y_coefficients[0] * y + y_coefficients[1];
            for(int c = 0; c < resized_img.c; ++c){
                set_pixel(resized_img, x, y, c, nn_interpolate(im, old_coordinate_x_f, old_coordinate_y_f, c));
            }
        }
    }
    return resized_img;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int left =  (int) floor(x);
    int right = (int) floor(x + 1);
    int top = (int) floor(y);
    int bottom = (int) floor(y + 1);
    //printf("x is %f, y is %f, left is %d, right is %d, top is %d, bottom is %d\n", x, y, left, right, top, bottom);
    //sleep(1);
    float top_left_pixel = get_pixel(im, left, top, c);
    float top_right_pixel = get_pixel(im, right, top, c);
    float bottom_left_pixel = get_pixel(im, left, bottom, c);
    float bottom_right_pixel = get_pixel(im, right, bottom, c);
    float interpolate_horizontal_top = ((x - left) * top_right_pixel) + ((right - x) * top_left_pixel);
    float interpolate_horizontal_bottom = ((x - left) * bottom_right_pixel) + ((right - x) * bottom_left_pixel);
    float interpolate_val = ((y - top) * interpolate_horizontal_bottom) + ((bottom - y) * interpolate_horizontal_top);
    return interpolate_val;
}

image bilinear_resize(image im, int w, int h)
{
    image resized_img = make_image(w, h, im.c);

    double x_coefficients[2];
    float x_old_top_left = -0.5f;
    float x_new_top_left = -0.5f;
    float x_old_bottom_right = im.w - 0.5f;
    float x_new_bottom_right = resized_img.w - 0.5f;
    solve_system_of_equations(x_new_top_left, x_old_top_left, x_new_bottom_right, x_old_bottom_right, x_coefficients);

    double y_coefficients[2];
    float y_old_top_left = -0.5f;
    float y_new_top_left = -0.5f;
    float y_old_bottom_right = im.h - 0.5f;
    float y_new_bottom_right = resized_img.h - 0.5f;
    solve_system_of_equations(y_new_top_left, y_old_top_left, y_new_bottom_right, y_old_bottom_right, y_coefficients);
    for(int x = 0; x < resized_img.w; ++x){
        for(int y = 0; y < resized_img.h; ++y){
            float old_coordinate_x_f = x_coefficients[0] * x + x_coefficients[1];
            float old_coordinate_y_f = y_coefficients[0] * y + y_coefficients[1];
            for(int c = 0; c < resized_img.c; ++c){
                set_pixel(resized_img, x, y, c, bilinear_interpolate(im, old_coordinate_x_f, old_coordinate_y_f, c));
                //printf("setting (%d, %d, %d) to %f\n", x, y, c, bilinear_interpolate(im, old_coordinate_x_f, old_coordinate_y_f, c));
            }
        }
    }
    return resized_img;
}


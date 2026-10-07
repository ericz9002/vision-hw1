#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include <stdbool.h>
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            for(int c = 0; c < im.c; ++c){
                sum += get_pixel(im, x, y, c);
            }
        }
    }
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            for(int c = 0; c < im.c; ++c){
                float pixel_val = get_pixel(im, x, y, c);
                set_pixel(im, x, y, c, pixel_val / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image box_filter = make_image(w, w, 1);
    for(int x = 0; x < box_filter.w; ++x){
        for(int y = 0; y < box_filter.h; ++ y){
            for(int c = 0; c < box_filter.c; ++c){
                set_pixel(box_filter, x, y, c, 1);
            }
        }
    }
    l1_normalize(box_filter);
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == im.c || filter.c == 1);
    assert(filter.h % 2 == 1 && filter.w % 2 == 1);
    image convolved_image;
    if(preserve == 0){
        convolved_image = make_image(im.w, im.h, 1);
    }
    else{
        convolved_image = make_image(im.w, im.h, im.c);
    }
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            float dot_product = 0;
            for(int c = 0; c < im.c; ++c){
                if(preserve == 1){
                    dot_product = 0;
                }
                for(int filter_x = 0; filter_x < filter.w; ++filter_x){
                    for(int filter_y = 0; filter_y < filter.h; ++ filter_y){
                        float image_pixel = get_pixel(im, x - (filter.w / 2) + filter_x, y - (filter.h / 2) + filter_y, c);
                        float filter_pixel;
                        if(filter.c == 1){
                            filter_pixel = get_pixel(filter, filter_x, filter_y, 0);
                        }
                        else{
                            filter_pixel = get_pixel(filter, filter_x, filter_y, c);
                        }
                        dot_product += image_pixel * filter_pixel;
                    }
                }
                if(preserve == 1){
                    set_pixel(convolved_image, x, y, c, dot_product);
                }
            }
            if(preserve == 0){
                set_pixel(convolved_image, x, y, 0, dot_product);
            }
        }
    }
    return convolved_image;
}

image make_highpass_filter()
{
    image highpass_filter = make_image(3,3,1);
    float highpass_vals[9] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
    int highpass_vals_idx = 0;
    for(int x = 0; x < highpass_filter.w; ++x){
        for(int y = 0; y < highpass_filter.h; ++y){
            set_pixel(highpass_filter, x, y, 0, highpass_vals[highpass_vals_idx++]);
        }
    }
    return highpass_filter;
}

image make_sharpen_filter()
{
    image sharpen_filter = make_image(3,3,1);
    float sharpen_vals[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    int sharpen_vals_idx = 0;
    for(int x = 0; x < sharpen_filter.w; ++x){
        for(int y = 0; y < sharpen_filter.h; ++y){
            set_pixel(sharpen_filter, x, y, 0, sharpen_vals[sharpen_vals_idx++]);
        }
    }
    return sharpen_filter;
}

image make_emboss_filter()
{
    image emboss_filter = make_image(3,3,1);
    float emboss_vals[9] = {-2, -1, 0, -1, 1, 1, 0, 1, 2};
    int emboss_vals_idx = 0;
    for(int x = 0; x < emboss_filter.w; ++x){
        for(int y = 0; y < emboss_filter.h; ++y){
            set_pixel(emboss_filter, x, y, 0, emboss_vals[emboss_vals_idx++]);
        }
    }
    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// preserve: box filter, sharpen filter, emboss filter
// no preserve: highpass filter,
// The highpass filter sums to 0, while all the other filters sum to 1. If we set preserve to 1 the filters that sum to 1, then
// the output image usually gets saturated and most of the pixels are white, because it does not normalize the contribution of the 
// r,g,b channel. The highpass filter sums to 0, so it is not affected by this issue. In addition, the highpass filter detects edges,
// and we don't care about the color information of an edge, just that the edge is there.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// We need to clamp the output of all the filters, because it is possible that the output image has pixel values that are less than 0
// or greater than 1.

image make_gaussian_filter(float sigma)
{
    int width = ceil(6 * sigma);
    if(width % 2 == 0){
        width += 1;
    }
    image gaussian_filter = make_image(width, width, 1);
    int img_y = 0;
    for(int y = 0 - floor(width / 2); y <= floor(width / 2); ++y){
        int img_x = 0;
        for(int x = 0 - floor(width / 2); x <= floor(width / 2); ++x){
            float gaussian_val = (1 / (TWOPI * pow(sigma,2))) * (exp(-1 * ((pow(x , 2) + pow(y , 2)) / (2 * pow(sigma, 2)))));
            set_pixel(gaussian_filter, img_x, img_y, 0, gaussian_val);
            img_x += 1;
        }
        img_y += 1;
    }
    return gaussian_filter;
}

image add_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image sum_image = make_image(a.w, a.h, a.c);
    for(int x = 0; x < a.w; ++x){
        for(int y = 0; y < a.h; ++y){
            for(int c = 0; c < a.c; ++c){
                float sum_val = get_pixel(a, x, y, c) + get_pixel(b, x, y, c);
                set_pixel(sum_image, x, y, c, sum_val);
            }
        }
    }
    return sum_image;
}

image sub_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image sub_image = make_image(a.w, a.h, a.c);
    for(int x = 0; x < a.w; ++x){
        for(int y = 0; y < a.h; ++y){
            for(int c = 0; c < a.c; ++c){
                float sub_val = get_pixel(a, x, y, c) - get_pixel(b, x, y, c);
                set_pixel(sub_image, x, y, c, sub_val);
            }
        }
    }
    return sub_image;
}

image make_gx_filter()
{
    image gx_filter = make_image(3,3,1);
    float gx_vals[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int gx_vals_idx = 0;
    for(int x = 0; x < gx_filter.w; ++x){
        for(int y = 0; y < gx_filter.h; ++y){
            set_pixel(gx_filter, x, y, 0, gx_vals[gx_vals_idx++]);
        }
    }
    return gx_filter;
}

image make_gy_filter()
{
    image gy_filter = make_image(3,3,1);
    float gy_vals[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    int gy_vals_idx = 0;
    for(int x = 0; x < gy_filter.w; ++x){
        for(int y = 0; y < gy_filter.h; ++y){
            set_pixel(gy_filter, x, y, 0, gy_vals[gy_vals_idx++]);
        }
    }
    return gy_filter;
}

void feature_normalize(image im)
{
    float min_val = im.data[0];
    float max_val = im.data[0];
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            for(int c = 0; c < im.c; ++c){
                float pixel_val = get_pixel(im, x, y, c);
                if(pixel_val < min_val){
                    min_val = pixel_val;
                }
                if(pixel_val > max_val){
                    max_val = pixel_val;
                }
            }
        }
    }
    float range = max_val - min_val;
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++ y){
            for(int c = 0; c < im.c; ++c){
                float pixel_val = get_pixel(im, x, y, c);
                float new_pixel_val;
                if(range == 0){
                    new_pixel_val = 0;
                }
                else{
                    new_pixel_val = (pixel_val - min_val) / range;
                }
                set_pixel(im, x, y, c, new_pixel_val);
            }
        }
    }
}

image *sobel_image(image im)
{
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image im_dx = convolve_image(im, gx_filter, 0);
    image im_dy = convolve_image(im, gy_filter, 0);
    image im_gradient_magnitude = make_image(im.w, im.h, 1);
    image im_gradient_direction = make_image(im.w, im.h, 1);
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            for(int c = 0; c < 1; ++c){
                float dx_val = get_pixel(im_dx, x, y, c);
                float dy_val = get_pixel(im_dy, x, y, c);
                float magnitude = pow(pow(dx_val, 2) + pow(dy_val, 2), 0.5);
                //tan(theta) = y/x, theta = arctan(y/x) = atan2(y,x)
                float direction = atan2(dy_val, dx_val);
                set_pixel(im_gradient_magnitude, x, y, c, magnitude);
                set_pixel(im_gradient_direction, x, y, c, direction);
            }
        }
    }
    image *sobel_images = calloc(2, sizeof(image));
    sobel_images[0] = im_gradient_magnitude;
    sobel_images[1] = im_gradient_direction;
    return sobel_images;
}

image colorize_sobel(image im)
{
    image* sobel_images = sobel_image(im);
    image colorized_sobel = make_image(im.w, im.h, 3);
    feature_normalize(sobel_images[0]);
    feature_normalize(sobel_images[1]);
    for(int x = 0; x < im.w; ++x){
        for(int y = 0; y < im.h; ++y){
            float angle = get_pixel(sobel_images[1], x, y, 0);
            set_pixel(colorized_sobel, x, y, 0, angle);
            float magnitude = get_pixel(sobel_images[0], x, y, 0);
            set_pixel(colorized_sobel, x, y, 1, .5);
            set_pixel(colorized_sobel, x, y, 2, magnitude);
        }
    }
    hsv_to_rgb(colorized_sobel);
    image emboss_filter = make_emboss_filter();
    colorized_sobel = convolve_image(colorized_sobel, emboss_filter, 1);
    clamp_image(colorized_sobel);
    return colorized_sobel;
}

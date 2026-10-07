#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if(x > im.w - 1){
        x = im.w - 1;
    }
    else if(x < 0){
        x = 0;
    }
    if(y > im.h - 1){
        y = im.h - 1;
    }
    else if(y < 0){
        y = 0;
    }
    int index = im.w * im.h * c + im.w * y + x;
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if(x > im.w - 1 || y > im.h - 1){
        return;
    }
    int index = im.w * im.h * c + im.w * y + x;
    im.data[index] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, sizeof(float) * im.w * im.h * im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    float coefficients[3] = {.299, .587, .114};
    for(int row = 0; row < im.h; ++row){
        for(int column = 0; column < im.w; ++column){
            float grayscale_val = 0;
            for(int channel = 0; channel < im.c; ++channel){
                grayscale_val += coefficients[channel] * get_pixel(im, column, row, channel);
            }
            set_pixel(gray, column, row, 0, grayscale_val);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for(int row = 0; row < im.h; ++row){
        for(int column = 0; column < im.w; ++column){
            float pixel_val = get_pixel(im, column, row, c);
            set_pixel(im, column, row, c, v + pixel_val);
        }
    }
}

void clamp_image(image im)
{
    for(int row = 0; row < im.h; ++ row){
        for(int column = 0; column < im.w; ++column){
            for(int channel = 0; channel < im.c; ++channel){
                if(get_pixel(im, column, row, channel) > 1){
                    set_pixel(im, column, row, channel, 1);
                }
                else if(get_pixel(im, column, row, channel) < 0){
                    set_pixel(im, column, row, channel, 0);
                }
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for(int row = 0; row < im.h; ++row){
        for(int column = 0; column < im.w; ++column){
            float red_val = get_pixel(im, column, row, 0);
            float green_val = get_pixel(im, column, row, 1);
            float blue_val = get_pixel(im, column, row, 2);
            float max_rgb = three_way_max(red_val, green_val, blue_val);
            float min_rgb = three_way_min(red_val, green_val, blue_val);
            float hue = 0;
            float saturation = 0;
            float value = 0;
            value = max_rgb;
            if(max_rgb == 0){
                saturation = 0;
            }
            else{
                saturation = 1.0f - (min_rgb/max_rgb);
            }
            if(max_rgb - min_rgb != 0){
                if(red_val == max_rgb){
                    hue = ((green_val - blue_val) / (max_rgb - min_rgb));
                    if(hue < 0){
                        hue = 6.0f + hue;
                    }
                }
                else if(green_val == max_rgb){
                    hue = (((blue_val - red_val) / (max_rgb - min_rgb)) + 2.0f);
                }
                else{
                    hue = (((red_val - green_val) / (max_rgb - min_rgb)) + 4.0f);
                }
                hue = hue / 6.0f;
            }
            set_pixel(im, column, row, 0, hue);
            set_pixel(im, column, row, 1, saturation);
            set_pixel(im, column, row, 2, value);
        }
    }
    return;
}

void hsv_to_rgb(image im)
{
    for(int row = 0; row < im.h; ++row){
        for(int column = 0; column < im.w; ++column){
            float hue = get_pixel(im, column, row, 0);
            float saturation = get_pixel(im, column, row, 1);
            float val = get_pixel(im, column, row, 2);
            float max_val = val;
            float min_val = (1.0f - saturation) * max_val;
            float scaled_hue = hue * 6;
            float red = 0, green = 0, blue = 0;
            if(scaled_hue < 1.0f){
                red = max_val;
                green = ((max_val - min_val) * scaled_hue) + min_val;
                blue = min_val;
            }
            else if(scaled_hue >= 1.0f && scaled_hue < 2.0f){
                red = max_val - ((max_val - min_val) * (scaled_hue - 1.0f));
                green = max_val;
                blue = min_val;
            }
            else if(scaled_hue >= 2.0f && scaled_hue < 3.0f){
                red = min_val;
                green = max_val;
                blue = ((max_val - min_val) * (scaled_hue - 2.0f)) + min_val;
            }
            else if(scaled_hue >= 3.0f && scaled_hue <= 4.0f){
                red = min_val;
                green = max_val - ((max_val - min_val) * (scaled_hue - 3.0f));
                blue = max_val;
            }
            else if(scaled_hue > 4.0f && scaled_hue <= 5.0f){
                red = ((max_val - min_val) * (scaled_hue - 4.0f)) + min_val;
                green = min_val;
                blue = max_val;
            }
            else if(scaled_hue > 5.0f && scaled_hue <= 6.0f){
                red = max_val;
                green = min_val;
                blue = max_val - ((max_val - min_val) * (scaled_hue - 5.0f));
            }
            set_pixel(im, column, row, 0, red);
            set_pixel(im, column, row, 1, green);
            set_pixel(im, column, row, 2, blue);
        }
    }   
}

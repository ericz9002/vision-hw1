import sys, os
from ctypes import *
import math
import random

lib = CDLL(os.path.join(os.path.dirname(__file__), "libuwimg.so"), RTLD_GLOBAL)

def c_array(ctype, values):
    arr = (ctype*len(values))()
    arr[:] = values
    return arr

class IMAGE(Structure):
    _fields_ = [("w", c_int),
                ("h", c_int),
                ("c", c_int),
                ("data", POINTER(c_float))]
    def __add__(self, other):
        return add_image(self, other)
    def __sub__(self, other):
        return sub_image(self, other)

add_image = lib.add_image
add_image.argtypes = [IMAGE, IMAGE]
add_image.restype = IMAGE

sub_image = lib.sub_image
sub_image.argtypes = [IMAGE, IMAGE]
sub_image.restype = IMAGE

make_image = lib.make_image
make_image.argtypes = [c_int, c_int, c_int]
make_image.restype = IMAGE

free_image = lib.free_image
free_image.argtypes = [IMAGE]

get_pixel = lib.get_pixel
get_pixel.argtypes = [IMAGE, c_int, c_int, c_int]
get_pixel.restype = c_float

set_pixel = lib.set_pixel
set_pixel.argtypes = [IMAGE, c_int, c_int, c_int, c_float]
set_pixel.restype = None

rgb_to_grayscale = lib.rgb_to_grayscale
rgb_to_grayscale.argtypes = [IMAGE]
rgb_to_grayscale.restype = IMAGE

copy_image = lib.copy_image
copy_image.argtypes = [IMAGE]
copy_image.restype = IMAGE

rgb_to_hsv = lib.rgb_to_hsv
rgb_to_hsv.argtypes = [IMAGE]
rgb_to_hsv.restype = None

feature_normalize = lib.feature_normalize
feature_normalize.argtypes = [IMAGE]
feature_normalize.restype = None

clamp_image = lib.clamp_image
clamp_image.argtypes = [IMAGE]
clamp_image.restype = None

hsv_to_rgb = lib.hsv_to_rgb
hsv_to_rgb.argtypes = [IMAGE]
hsv_to_rgb.restype = None

shift_image = lib.shift_image
shift_image.argtypes = [IMAGE, c_int, c_float]
shift_image.restype = None

load_image_lib = lib.load_image
load_image_lib.argtypes = [c_char_p]
load_image_lib.restype = IMAGE

def load_image(f):
    return load_image_lib(f.encode('ascii'))

save_png_lib = lib.save_png
save_png_lib.argtypes = [IMAGE, c_char_p]
save_png_lib.restype = None

def save_png(im, f):
    return save_png_lib(im, f.encode('ascii'))

save_image_lib = lib.save_image
save_image_lib.argtypes = [IMAGE, c_char_p]
save_image_lib.restype = None

def save_image(im, f):
    return save_image_lib(im, f.encode('ascii'))

same_image = lib.same_image
same_image.argtypes = [IMAGE, IMAGE]
same_image.restype = c_int

nn_resize = lib.nn_resize
nn_resize.argtypes = [IMAGE, c_int, c_int]
nn_resize.restype = IMAGE

bilinear_resize = lib.bilinear_resize
bilinear_resize.argtypes = [IMAGE, c_int, c_int]
bilinear_resize.restype = IMAGE

make_sharpen_filter = lib.make_sharpen_filter
make_sharpen_filter.argtypes = []
make_sharpen_filter.restype = IMAGE

make_box_filter = lib.make_box_filter
make_box_filter.argtypes = [c_int]
make_box_filter.restype = IMAGE

make_emboss_filter = lib.make_emboss_filter
make_emboss_filter.argtypes = []
make_emboss_filter.restype = IMAGE

make_highpass_filter = lib.make_highpass_filter
make_highpass_filter.argtypes = []
make_highpass_filter.restype = IMAGE

make_gy_filter = lib.make_gy_filter
make_gy_filter.argtypes = []
make_gy_filter.restype = IMAGE

make_gx_filter = lib.make_gx_filter
make_gx_filter.argtypes = []
make_gx_filter.restype = IMAGE

sobel_image = lib.sobel_image
sobel_image.argtypes = [IMAGE]
sobel_image.restype = POINTER(IMAGE)

colorize_sobel = lib.colorize_sobel
colorize_sobel.argtypes = [IMAGE]
colorize_sobel.restype = IMAGE

make_gaussian_filter = lib.make_gaussian_filter
make_gaussian_filter.argtypes = [c_float]
make_gaussian_filter.restype = IMAGE

convolve_image = lib.convolve_image
convolve_image.argtypes = [IMAGE, IMAGE, c_int]
convolve_image.restype = IMAGE


#resizes images in data folder using nearest neighbor interpolation and saves as jpg. If recompute_files is true,
#it will redo the calculation and save it to the file again even if the file already exists.
def test_nn_resize(image_files, recompute_files = False):
    save_directory = "processed_images/resize/nn_interpolate"
    for file in image_files:
        im_fn, im_extension = os.path.splitext(os.path.basename(file))
        im = load_image(file)

        relative_resized_im_fn = im_fn + "_x4"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = nn_resize(im, im.w * 4, im.h * 4)
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)

        relative_resized_im_fn = im_fn + "_x7"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = nn_resize(im, im.w * 7, im.h * 7)
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)

        relative_resized_im_fn = im_fn + "_x.25"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = nn_resize(im, round(im.w * .25), round(im.h * .25))
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)
        free_image(im)

def test_bl_interpolate(image_files, recompute_files = False):
    save_directory = "processed_images/resize/bl_interpolate"
    for file in image_files:
        im_fn, im_extension = os.path.splitext(os.path.basename(file))
        im = load_image(file)

        relative_resized_im_fn = im_fn + "_x4"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = bilinear_resize(im, im.w * 4, im.h * 4)
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)

        relative_resized_im_fn = im_fn + "_x7"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = bilinear_resize(im, im.w * 7, im.h * 7)
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)

        relative_resized_im_fn = im_fn + "_x.25"
        resized_im_fn = os.path.join(save_directory, relative_resized_im_fn)
        if(recompute_files == True or (recompute_files == False and not os.path.exists(resized_im_fn + '.jpg'))):
            print(f'computing {resized_im_fn}')
            resized_im = bilinear_resize(im, round(im.w * .25), round(im.h * .25))
            save_image(resized_im, resized_im_fn)
            free_image(resized_im)
        free_image(im)

def test_box_filter(image_files, recompute_files = False):
    save_directory = "processed_images/filter/box_filter"
    for file in image_files:
        im_fn, im_extension = os.path.splitext(os.path.basename(file))
        im = load_image(file)
        filtered_im_fn = im_fn + "_box_filter_3x3"
        filtered_im_fn = os.path.join(save_directory, filtered_im_fn)

        filtered_im_fn_no_preserve = im_fn + "_box_filter_3x3_no_preserve"
        filtered_im_fn_no_preserve = os.path.join(save_directory, filtered_im_fn_no_preserve)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn + '.jpg'))):
            box_filter_3x3 = make_box_filter(3)
            filtered_im = convolve_image(im, box_filter_3x3, 1)
            save_image(filtered_im, filtered_im_fn)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn_no_preserve + '.jpg'))):
            box_filter_3x3 = make_box_filter(3)
            filtered_im_no_preserve = convolve_image(im, box_filter_3x3, 0)
            save_image(filtered_im_no_preserve, filtered_im_fn_no_preserve)

        
        relative_filtered_im_fn = im_fn + "_box_filter_7x7"
        filtered_im_fn = os.path.join(save_directory, relative_filtered_im_fn)

        filtered_im_fn_no_preserve = im_fn + "_box_filter_7x7_no_preserve"
        filtered_im_fn_no_preserve = os.path.join(save_directory, filtered_im_fn_no_preserve)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn + '.jpg'))):
            box_filter_7x7 = make_box_filter(7)
            filtered_im = convolve_image(im, box_filter_7x7, 1)
            save_image(filtered_im, filtered_im_fn) 

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn_no_preserve + '.jpg'))):
            box_filter_7x7 = make_box_filter(7)
            filtered_im_no_preserve = convolve_image(im, box_filter_7x7, 0)
            save_image(filtered_im_no_preserve, filtered_im_fn_no_preserve)   

def test_highpass_filter(image_files, recompute_files = False):
    save_directory = "processed_images/filter/highpass_filter"
    for file in image_files:
        im_fn, im_extension = os.path.splitext(os.path.basename(file))
        im = load_image(file)
        filtered_im_fn = im_fn + "_highpass_filter"
        filtered_im_fn = os.path.join(save_directory, filtered_im_fn)

        filtered_im_fn_no_preserve = im_fn + "_highpass_filter_no_preserve"
        filtered_im_fn_no_preserve = os.path.join(save_directory, filtered_im_fn_no_preserve)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn + '.jpg'))):
            highpass_filter = make_highpass_filter()
            filtered_im = convolve_image(im, highpass_filter, 1)
            clamp_image(filtered_im)
            save_image(filtered_im, filtered_im_fn) 

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn_no_preserve + '.jpg'))):
            highpass_filter = make_highpass_filter()
            filtered_im_no_preserve = convolve_image(im, highpass_filter, 0)
            clamp_image(filtered_im_no_preserve)
            save_image(filtered_im_no_preserve, filtered_im_fn_no_preserve) 

def test_sharpen_filter(image_files, recompute_files = False):
    save_directory = "processed_images/filter/sharpen_filter"
    for file in image_files:
        im = load_image(file)
        im_fn, im_extension = os.path.splitext(os.path.basename(file))

        filtered_im_fn = im_fn + "_sharpen_filter"
        filtered_im_fn = os.path.join(save_directory, filtered_im_fn)

        filtered_im_fn_no_preserve = im_fn + "_sharpen_filter_no_preserve"
        filtered_im_fn_no_preserve = os.path.join(save_directory, filtered_im_fn_no_preserve)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn + '.jpg'))):
            sharpen_filter = make_sharpen_filter()
            filtered_im = convolve_image(im, sharpen_filter, 1)
            clamp_image(filtered_im)
            save_image(filtered_im, filtered_im_fn) 

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn_no_preserve + '.jpg'))):
            sharpen_filter = make_sharpen_filter()
            filtered_im_no_preserve = convolve_image(im, sharpen_filter, 0)
            clamp_image(filtered_im_no_preserve)
            save_image(filtered_im_no_preserve, filtered_im_fn_no_preserve) 

def test_emboss_filter(image_files, recompute_files = False):
    save_directory = "processed_images/filter/emboss_filter"
    for file in image_files:
        im_fn, im_extension = os.path.splitext(os.path.basename(file))
        im = load_image(file)

        filtered_im_fn = im_fn + "_emboss_filter"
        filtered_im_fn = os.path.join(save_directory, filtered_im_fn)

        filtered_im_fn_no_preserve = im_fn + "_emboss_filter_no_preserve"
        filtered_im_fn_no_preserve = os.path.join(save_directory, filtered_im_fn_no_preserve)

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn + '.jpg'))):
            emboss_filter = make_emboss_filter()
            filtered_im = convolve_image(im, emboss_filter, 1)
            clamp_image(filtered_im)
            save_image(filtered_im, filtered_im_fn) 

        if(recompute_files == True or (recompute_files == False and not os.path.exists(filtered_im_fn_no_preserve + '.jpg'))):
            emboss_filter = make_emboss_filter()
            filtered_im_no_preserve = convolve_image(im, emboss_filter, 0)
            clamp_image(filtered_im_no_preserve)
            save_image(filtered_im_no_preserve, filtered_im_fn_no_preserve) 


def create_ronboledore():
    dumbledore = load_image("data/dumbledore.png")
    ron = load_image("data/ron.png")
    gaussian_filter = make_gaussian_filter(2)
    low_freq_ron = convolve_image(ron, gaussian_filter, 1)
    high_freq_ron = sub_image(ron, low_freq_ron)
    low_freq_dumbledore = convolve_image(dumbledore, gaussian_filter, 1)
    high_freq_dumbledore = sub_image(dumbledore, low_freq_dumbledore)
    dumbledore_ron = add_image(low_freq_dumbledore, high_freq_ron)
    ron_dumbledore = add_image(low_freq_ron, high_freq_dumbledore)
    clamp_image(dumbledore_ron)
    clamp_image(ron_dumbledore)
    clamp_image(high_freq_ron)
    clamp_image(high_freq_dumbledore)
    save_image(dumbledore_ron, "processed_images/low_freq_dumbledore_high_freq_ron")
    save_image(ron_dumbledore, "processed_images/low_freq_ron_high_freq_dumbledore")
    free_image(dumbledore)
    free_image(ron)
    free_image(gaussian_filter)
    free_image(low_freq_ron)
    free_image(high_freq_ron)
    free_image(low_freq_dumbledore)
    free_image(high_freq_dumbledore)
    free_image(dumbledore_ron)
    free_image(ron_dumbledore)

def create_colorized_sobel():
    im = load_image("data/dog.jpg")
    colorized_sobel = colorize_sobel(im)
    save_image(colorized_sobel, "processed_images/sobel")
    free_image(im)
    free_image(colorized_sobel)


if __name__ == "__main__":
    os.makedirs("processed_images/resize/nn_interpolate", exist_ok = True)
    os.makedirs("processed_images/resize/bl_interpolate", exist_ok = True)
    os.makedirs("processed_images/filter/box_filter", exist_ok = True)
    os.makedirs("processed_images/filter/highpass_filter", exist_ok = True)
    os.makedirs("processed_images/filter/sharpen_filter", exist_ok = True)
    os.makedirs("processed_images/filter/emboss_filter", exist_ok = True)
    image_files = [os.path.join("data", f) for f in os.listdir("data") if os.path.isfile(os.path.join("data", f))]

    recompute = False
    test_nn_resize(image_files, recompute_files = recompute)
    test_bl_interpolate(image_files, recompute_files = recompute)
    test_box_filter(image_files, recompute_files = recompute)
    test_highpass_filter(image_files, recompute_files = recompute)
    test_sharpen_filter(image_files, recompute_files = recompute)
    test_emboss_filter(image_files, recompute_files = recompute)
    create_ronboledore()
    create_colorized_sobel()
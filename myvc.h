#ifndef MYVC
#define MYVC

#include "vc.h"
#include "stdio.h"

typedef enum {
  rgb,
  hsv,
  grayscale,
  binary
} imagetype_e;

#define OUTDIR "out"

#define MAXLABELS 255

typedef struct {
  int x, y, width, height;// Caixa Delimitadora (Bounding Box)
  int area;// Área
  int xc, yc;// Centro-de-massa
  int perimeter;// Perimetro
  int label;// Etiqueta
  float circularity; // Circularidade
} OVC;

int vc_gray_negative(IVC *image);
int vc_test_gray_negative();
int vc_rgb_negative(IVC *image);
int vc_test_rgb_negative();

int vc_rgb_get_red_gray(IVC *image);
int vc_test_rgb_get_red_gray();
int vc_rgb_get_green_gray(IVC *image);
int vc_test_rgb_get_green_gray();
int vc_rgb_get_blue_gray(IVC *image);
int vc_test_rgb_get_blue_gray();

int vc_rgb_to_gray(IVC *rgb, IVC *gray);
int vc_test_rgb_to_gray();

int vc_rgb_to_hsv(IVC *rgb, IVC *hsv);
int vc_test_rgb_to_hsv();

int vc_hsv_get_hue(IVC *hsv, IVC *gray);

int vc_scale_gray_to_rgb(IVC *gray, IVC *rgb);
int vc_test_scale_gray_to_rgb();
int vc_scale_gray_to_rgb2(IVC *gray, IVC *rgb);
int vc_test_scale_gray_to_rgb2();
int vc_gray_to_binary(IVC *gray, IVC *bin);
int vc_test_gray_to_binary();

// Segmentation
int vc_gray_to_binary_fixed(IVC* graysrc, IVC* graydst, int threshold);
int vc_test_gray_to_binary_fixed(int threshold);
int vc_gray_to_binary_global_mean(IVC* graysrc, IVC* graydst);
int vc_test_gray_to_binary_global_mean();
int vc_gray_to_binary_russ(IVC* graysrc, IVC* graydst, float alpha);
int vc_test_gray_to_binary_russ(float alpha);
int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel);
int vc_test_gray_to_binary_midpoint(int kernel);
int vc_gray_to_binary_local_mean(IVC *src, IVC *dst, int kernel);
int vc_test_gray_to_binary_local_mean(int kernel);
int vc_gray_to_binary_bernsen(IVC *src, IVC *dst, int kernel, int c_min);
int vc_test_gray_to_binary_bernsen(int kernel, int c_min);
int vc_gray_to_binary_niblack(IVC *src, IVC *dst, int kernel, float alpha);
int vc_test_gray_to_binary_niblack(int kernel, float alpha);
int vc_gray_to_binary_otsu(IVC *src, IVC *dst);

// Morphological Operators (Binary)
int vc_binary_dilate(IVC *src, IVC* dst, int size);
int vc_test_binary_dilate(int size);
int vc_binary_erode(IVC *src, IVC* dst, int size);
int vc_test_binary_erode(int size);
int vc_binary_open(IVC *src, IVC* dst, int size_erode, int size_dilate);
int vc_test_binary_open(int size_erode, int size_dilate);
int vc_binary_close(IVC *src, IVC* dst, int size_erode, int size_dilate);
int vc_test_binary_close(int size_erode, int size_dilate);

// Morphological Operators (Grayscale)
int vc_gray_dilate(IVC *src, IVC* dst, int size);
int vc_test_gray_dilate(int size);
int vc_gray_erode(IVC *src, IVC* dst, int size);
int vc_test_gray_erode(int size);
int vc_gray_open(IVC *src, IVC* dst, int size_erode, int size_dilate);
int vc_test_gray_open(int size_erode, int size_dilate);
int vc_gray_close(IVC *src, IVC* dst, int size_erode, int size_dilate);
int vc_test_gray_close(int size_erode, int size_dilate);

// Blob labeling
OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);
int vc_binary_blob_print(OVC *blob);
int vc_binary_blob_filter(OVC **blobs, int nblobs, int area);
int vc_draw_boundary_box(IVC *src, int x_min, int x_max, int y_min, int y_max, int color);
int vc_draw_mass_center(IVC *src, int xc, int yc, int color);

// Benchmarking
double benchmark_function(int (*vc_function)(IVC *, IVC *));
double benchmark_average(int (*vc_function)(IVC *, IVC *), size_t times);
void benchmark_average_print(int (*vc_fn)(IVC *, IVC *), char* fn_name, size_t times);
void benchmark_compare_functions(int (*vc_fn1)(IVC *, IVC *), char *fn1_name, int (*vc_fn2)(IVC *, IVC *), char *fn2name, size_t times);

// Testing utilities
int vc_test_binary_function(char* filepath, imagetype_e type, int (*vc_fn)(IVC*, IVC*), char* fn_name);

int vc_histogram_grayscale(IVC *src, long hist[]);

// Filters
int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst);
int vc_gray_lowpass_median_filter(IVC *src, IVC *dst);

// Edge detection
int vc_gray_edge_canny(IVC *src, IVC *dst);

int vc_blob_inside_blob(OVC *b1, OVC *b2);
IVC *vc_gray_fill_holes(IVC *src, OVC **blobs, size_t nblobs);

#endif /* MYVC */

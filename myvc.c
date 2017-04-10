#include "myvc.h"
#include "utils.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int vc_gray_negative(IVC *image) {
  if (!vc_is_grayscale(image))
    return 0;

  long pos;

  for (int x = 0; x < image->width; x++) {
    for (int y = 0; y < image->height; y++) {
      pos = y * image->bytesperline + x;

      image->data[pos] = 255 - image->data[pos];
    }
  }

  return 1;
}

int vc_test_gray_negative() {
  IVC *gray = vc_read_image("Images/FLIR/flir-01.pgm");
  if (vc_gray_negative(gray) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_negative_flir-01.pgm", gray);
  vc_image_free(gray);
  return 1;
}

int vc_rgb_negative(IVC *image) {
  if (!vc_is_rgb(image))
    return 0;

  long pos;

  for (int x = 0; x < image->width; x++) {
    for (int y = 0; y < image->height; y++) {
      pos = y * image->bytesperline + x * 3;

      image->data[pos] = 255 - image->data[pos];
      image->data[pos + 1] = 255 - image->data[pos + 1];
      image->data[pos + 2] = 255 - image->data[pos + 2];
    }
  }

  return 1;
}

int vc_test_rgb_negative() {
  IVC *rgb = vc_read_image("Images/Additional/cablecar.ppm");
  if (vc_rgb_negative(rgb) != 1)
    return 0;
  vc_write_image_info("out/vc_rgb_negative_cablecar.ppm", rgb);
  vc_image_free(rgb);
  return 1;
}

// Get the red component of a rgb image
int vc_rgb_get_red_gray(IVC *image) {
  if (!vc_is_rgb(image))
    return 0;

  long pos = 0;

  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      pos = y * image->bytesperline + x * 3;

      image->data[pos + 1] = image->data[pos];
      image->data[pos + 2] = image->data[pos];
    }
  }

  return 1;
}

int vc_test_rgb_get_red_gray() {
  IVC *rgb = vc_read_image("Images/Classic/lenna.ppm");
  if (vc_rgb_get_red_gray(rgb) != 1)
    return 0;
  vc_write_image_info("out/vc_rgb_get_red_gray_lenna.ppm", rgb);
  vc_image_free(rgb);
  return 1;
}

// Get the green component of a rgb image
int vc_rgb_get_green_gray(IVC *image) {
  if (!vc_is_rgb(image))
    return 0;

  long pos = 0;

  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      pos = y * image->bytesperline + x * 3;

      image->data[pos] = image->data[pos + 1];
      image->data[pos + 2] = image->data[pos + 1];
    }
  }

  return 1;
}

int vc_test_rgb_get_green_gray() {
  IVC *rgb = vc_read_image("Images/Classic/lenna.ppm");
  if (vc_rgb_get_green_gray(rgb) != 1)
    return 0;
  vc_write_image_info("out/vc_rgb_get_green_gray_lenna.ppm", rgb);
  vc_image_free(rgb);
  return 1;
}

// Get the blue component of a rgb image
int vc_rgb_get_blue_gray(IVC *image) {
  if (!vc_is_rgb(image))
    return 0;

  long pos = 0;

  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      pos = y * image->bytesperline + x * 3;

      image->data[pos] = image->data[pos + 2];
      image->data[pos + 1] = image->data[pos + 2];
    }
  }

  return 1;
}

int vc_test_rgb_get_blue_gray() {
  IVC *rgb = vc_read_image("Images/Classic/lenna.ppm");
  if (vc_rgb_get_blue_gray(rgb) != 1)
    return 0;
  vc_write_image_info("out/vc_rgb_get_blue_gray_lenna.ppm", rgb);
  vc_image_free(rgb);
  return 1;
}

// Convert and rgb image to a grayscale image
int vc_rgb_to_gray(IVC *rgb, IVC *gray) {
  if (!vc_is_rgb(rgb))
    return 0;
  if (!vc_is_grayscale(gray))
    return 0;

  /* se os tamanhos das imagens não forem iguais
não podemos fazer a conversão */
  if (rgb->width != gray->width || rgb->height != gray->height)
    return 0;

  long pos_rgb = 0, pos_gray = 0;

  for (int y = 0; y < rgb->height; y++) {
    for (int x = 0; x < rgb->width; x++) {
      pos_rgb = y * rgb->bytesperline + x * 3;
      pos_gray = y * gray->bytesperline + x;

      gray->data[pos_gray] =
          (unsigned char)((float)rgb->data[pos_rgb] * 0.299) +
          ((float)rgb->data[pos_rgb + 1] * 0.587) +
          ((float)rgb->data[pos_rgb + 2] * 0.114);
    }
  }

  return 1;
}

int vc_test_rgb_to_gray() {
  return vc_test_binary_function("Images/Classic/lenna.ppm", grayscale,
                                 vc_rgb_to_gray, "vc_rgb_to_gray");
}

int vc_rgb_to_hsv(IVC *rgb, IVC *hsv) {
  if (!vc_is_rgb(rgb))
    return 0;

  /* como o número de canais e intensidade é o mesmo em RGB e HSV
usamos a função vc_is_rgb para testar se a imagem é HSV */
  if (!vc_is_rgb(hsv))
    return 0;

  /* se os tamanhos das imagens não forem iguais
não podemos fazer a conversão */
  if (rgb->width != hsv->width || rgb->height != hsv->height)
    return 0;

  long pos = 0;
  unsigned char r, g, b;
  unsigned char s, v;
  float h, ampl;

  for (int y = 0; y < rgb->height; y++) {
    for (int x = 0; x < rgb->width; x++) {
      pos = y * rgb->bytesperline + x * rgb->channels;

      r = rgb->data[pos];
      g = rgb->data[pos + 1];
      b = rgb->data[pos + 2];

#ifdef DEBUG
      printf("rgb[%ld]: (%d,%d,%d)\n", pos, r, g, b);
#endif

      unsigned char max = MAX(r, g, b);
#ifdef DEBUG
      printf("max: %d\n", max);
#endif

      if (max == 0) {
        h = 0.0f;
        s = 0;
        v = 0;
      } else {
        ampl = (float)(max - MIN(r, g, b));
#ifdef DEBUG
        printf("min: %d\n", MIN(r, g, b));
        printf("ampl: %.2f\n", ampl);
#endif
        s = (unsigned char)((ampl * 255.0f) / (float)max);
        if (s == 0) {
          h = 0.0f; // hue
        } else {
          if (max == r) {
            if (g >= b) {
              h = (60.f * ((float)(g - b))) / ampl;
            } else if (b > g) {
              h = 360.0f + (60.0f * (((float)(g - b)) / ampl));
            }
          } else if (max == g) {
            h = 120.0f + (60.0f * (((float)(b - r)) / ampl));
          } else if (max == b) {
            h = 240.0f + (60.0f * (((float)(r - g)) / ampl));
          }
        }
        v = max; // set value
      }

#ifdef DEBUG
      printf("hue: %.2f sat: %d val: %d\n", h, s, v);
#endif
      hsv->data[pos] = (unsigned char)((h * 255.0f) / 360.0f);
      hsv->data[pos + 1] = s;
      hsv->data[pos + 2] = v;
    }
  }

  return 1;
}

int vc_test_rgb_to_hsv() {
  return vc_test_binary_function("Images/Classic/airplane.ppm", hsv,
                                 vc_rgb_to_hsv, "vc_rgb_to_hsv");
}

// Convert a grayscale image to a rgb image.
int vc_scale_gray_to_rgb(IVC *gray, IVC *rgb) {
  if (!vc_is_grayscale(gray))
    return 0;
  if (!vc_is_rgb(rgb))
    return 0;

  if ((gray->width != rgb->width) || (gray->height != rgb->height))
    return 0;

  long pos_gray, pos_rgb;
  unsigned char pixel;
  unsigned char r, g, b;
  float slope = 255.0f / 64.0f;

  for (int x = 0; x < gray->width; x++) {
    for (int y = 0; y < gray->height; y++) {
      pos_gray = y * gray->bytesperline + x * gray->channels;
      pos_rgb = y * rgb->bytesperline + x * rgb->channels;

      pixel = gray->data[pos_gray];

      if (pixel <= 64) {
        r = 0;
        g = (unsigned char)(slope * (float)pixel);
        b = 255;
      } else if ((pixel > 64) && (pixel <= 128)) {
        r = 0;
        g = 255;
        b = (unsigned char)((-slope * (float)pixel) + 510.0f);
      } else if ((pixel > 128) && (pixel <= 192)) {
        r = (unsigned char)((slope * (float)pixel) - 510.0f);
        g = 255;
        b = 0;
      } else if (pixel > 192) {
        r = 255;
        g = (unsigned char)((-slope * (float)pixel) + 1020.0f);
        b = 0;
      }

      rgb->data[pos_rgb] = r;
      rgb->data[pos_rgb + 1] = g;
      rgb->data[pos_rgb + 2] = b;
    }
  }

  return 1;
}

int vc_scale_gray_to_rgb2(IVC *gray, IVC *rgb) {
  if (!vc_is_grayscale(gray))
    return 0;
  if (!vc_is_rgb(rgb))
    return 0;

  if ((gray->width != rgb->width) || (gray->height != rgb->height))
    return 0;

  long pos_gray, pos_rgb;
  unsigned char pixel;
  unsigned char r, g, b;

  for (int x = 0; x < gray->width; x++) {
    for (int y = 0; y < gray->height; y++) {
      pos_gray = y * gray->bytesperline + x * gray->channels;
      pos_rgb = y * rgb->bytesperline + x * rgb->channels;

      pixel = gray->data[pos_gray];

      if (pixel < 64) {
        r = 0;
        g = (unsigned char)pixel * 4;
        b = 255;
      } else if ((pixel >= 64) && (pixel < 128)) {
        r = 0;
        g = 255;
        b = (unsigned char)255 - (pixel - 64) * 4;
      } else if ((pixel >= 128) && (pixel < 192)) {
        r = (unsigned char)0 - (pixel - 128) * 4;
        g = 255;
        b = 0;
      } else if (pixel >= 192) {
        r = 255;
        g = (unsigned char)255 - (pixel - 192) * 4;
        b = 0;
      }

      rgb->data[pos_rgb] = r;
      rgb->data[pos_rgb + 1] = g;
      rgb->data[pos_rgb + 2] = b;
    }
  }

  return 1;
}

// Convert a grayscale image to a rgb image.
int vc_test_scale_gray_to_rgb() {
  return vc_test_binary_function("Images/Classic/barbara.pgm", rgb,
                                 vc_scale_gray_to_rgb, "vc_scale_gray_to_rgb");
}

int vc_test_scale_gray_to_rgb2() {
  return vc_test_binary_function("Images/Classic/barbara.pgm", rgb,
                                 vc_scale_gray_to_rgb2,
                                 "vc_scale_gray_to_rgb2");
}

int vc_gray_to_binary(IVC *gray, IVC *bin) {
  if (!vc_is_grayscale(gray))
    return 0;
  if (!vc_is_binary(bin))
    return 0;

  if ((gray->width != bin->width) || (gray->height != bin->height))
    return 0;

  int x, y;
  long pos;

  for (x = 0; x < gray->width; x++) {
    for (y = 0; y < gray->height; y++) {
      pos = y * gray->bytesperline + x;

      if (gray->data[pos] == 255) {
        bin->data[pos] = 1;
      } else if (gray->data[pos] == 0) {
        bin->data[pos] = 0;
      } else {
        fprintf(stderr, "vc_gray_to_binary: can't convert pixel with a value "
                        "of %d, only 0 or 255",
                gray->data[pos]);
        return 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary() {
  IVC *src = vc_read_image("Images/Classic/barbara.pgm");
  IVC *gray = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_global_mean(src, gray) != 1)
    return 0;
  IVC *bin = vc_binary_new(gray->width, gray->height);
  if (vc_gray_to_binary(gray, bin) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_barbara.pbm", bin);
  vc_image_free(src);
  vc_image_free(gray);
  vc_image_free(bin);
  return 1;
}

// Image segmentation by fixed threshold value.
int vc_gray_to_binary_fixed(IVC *graysrc, IVC *graydst, int threshold) {
  if (!vc_is_grayscale(graysrc))
    return 0;
  if (!vc_is_binary(graydst))
    return 0;

  if ((graysrc->width != graydst->width) ||
      (graysrc->height != graydst->height))
    return 0;

  long pos;

  for (int x = 0; x < graysrc->width; x++) {
    for (int y = 0; y < graysrc->height; y++) {
      pos = y * graysrc->bytesperline + x * graysrc->channels;

      if (graysrc->data[pos] > threshold) {
        graydst->data[pos] = 1; // 255
      } else {
        graydst->data[pos] = 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_fixed(int threshold) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_fixed(src, dst, threshold) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_fixed_vc_0005_example1.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

// Image segmentation by global mean value.
int vc_gray_to_binary_global_mean(IVC *graysrc, IVC *graydst) {
  if (!vc_is_grayscale(graysrc))
    return 0;
  if (!vc_is_grayscale(graydst))
    return 0;

  if ((graysrc->width != graydst->width) ||
      (graysrc->height != graydst->height))
    return 0;

  int x, y;
  long pos;
  unsigned long long total = 0;
  int threshold;

  for (x = 0; x < graysrc->width; x++) {
    for (y = 0; y < graysrc->height; y++) {
      pos = y * graysrc->bytesperline + x * graysrc->channels;

      total += graysrc->data[pos];
    }
  }

  threshold = total / (graysrc->width * graysrc->height);

#ifdef DEBUG
  printf("Threshold: %d\n", threshold);
#endif

  for (x = 0; x < graysrc->width; x++) {
    for (y = 0; y < graysrc->height; y++) {
      pos = y * graysrc->bytesperline + x * graysrc->channels;

      if (graysrc->data[pos] > threshold) {
        graydst->data[pos] = 0; // branco
      } else {
        graydst->data[pos] = 1;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_global_mean() {
  return vc_test_binary_function("Images/Extra/cells.pgm", grayscale,
                                 vc_gray_to_binary_global_mean,
                                 "vc_gray_to_binary_global_mean");
}

int vc_gray_to_binary_russ(IVC *graysrc, IVC *graydst, float alpha) {
  if (!vc_is_grayscale(graysrc))
    return 0;
  if (!vc_is_grayscale(graydst))
    return 0;

  if ((graysrc->width != graydst->width) ||
      (graysrc->height != graydst->height))
    return 0;

  int x, y;
  long pos;
  unsigned long long total = 0;
  int mean, threshold;
  float stddev;

  for (x = 0; x < graysrc->width; x++) {
    for (y = 0; y < graysrc->height; y++) {
      pos = y * graysrc->bytesperline + x * graysrc->channels;

      total += graysrc->data[pos];
    }
  }

  mean = total / (graysrc->width * graysrc->height);

#ifdef DEBUG
  printf("mean: %d\n", mean);
#endif

  total = 0;

  for (x = 0; x < graysrc->width; x++) {
    for (y = 0; y < graysrc->height; y++) {
      pos = y * graysrc->bytesperline + x * graysrc->channels;

      total += (graysrc->data[pos] - mean) * (graysrc->data[pos] - mean);
    }
  }

  stddev = sqrt(total / (float)(graysrc->width * graysrc->height));

#ifdef DEBUG
  printf("std. deviation: %.3f\n", stddev);
#endif

  threshold = mean + (int)(alpha * stddev);

#ifdef DEBUG
  printf("threshold: %d\n", threshold);
#endif

  return vc_gray_to_binary_fixed(graysrc, graydst, threshold);
}

int vc_test_gray_to_binary_russ(float alpha) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_russ(src, dst, alpha) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_russ_vc_0005_example1.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_to_binary_midpoint(IVC *src, IVC *dst, int kernel) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if (kernel % 2 != 1) // TODO: check if this is correct
    return 0;

  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;

  long pos, pos_k;
  int threshold, v_min = 255, v_max = 0;
  int x, y, kx, ky;
  int offset = kernel / 2;

#ifdef DEBUG
  printf("kernel: %d offset: %d\n", kernel, offset);
#endif

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
#ifdef DEBUG
      printf("x: %d y: %d\n", x, y);
#endif
      v_max = 0;
      v_min = 255;

      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky > 0) && (y + ky < src->height) && (x + kx > 0) &&
              (x + kx < src->width)) {
            pos_k = ((y + ky) * src->bytesperline) + (x + kx);
#ifdef DEBUG
            printf("kx: %d ky: %d\n", kx, ky);
            printf("pos_k: %ld\n", pos_k);
#endif
            if (src->data[pos_k] > v_max) {
              v_max = src->data[pos_k];
            }
            if (src->data[pos_k] < v_min) {
              v_min = src->data[pos_k];
            }
          }
        }
      }

      threshold = (v_min + v_max) / 2;
      pos = (y * src->bytesperline) + x;
#ifdef DEBUG
      printf("v_min: %d v_max: %d\n", v_min, v_max);
      printf("threshold: %d\n", threshold);
      printf("pos: %ld\n", pos);
#endif
      if (src->data[pos] > threshold) {
        dst->data[pos] = 255;
      } else {
        dst->data[pos] = 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_midpoint(int kernel) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_midpoint(src, dst, kernel) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_midpoint_vc_0005_example1.pgm",
                      dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_to_binary_bernsen(IVC *src, IVC *dst, int kernel, int c_min) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if (kernel % 2 != 1) // TODO: check if this is correct
    return 0;
  if (c_min < 1)
    return 0;

  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;

  long pos, pos_k;
  int threshold, v_min = 255, v_max = 0;
  int x, y, kx, ky;
  int offset = kernel / 2;

#ifdef DEBUG
  printf("kernel: %d offset: %d\n", kernel, offset);
#endif

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {

      v_max = 0;
      v_min = 255;

      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky > 0) && (y + ky < src->height) && (x + kx > 0) &&
              (x + kx < src->width)) {
            pos_k = ((y + ky) * src->bytesperline) + (x + kx);

            if (src->data[pos_k] > v_max) {
              v_max = src->data[pos_k];
            }
            if (src->data[pos_k] < v_min) {
              v_min = src->data[pos_k];
            }
          }
        }
      }

      if ((v_max - v_min) < c_min) {
        threshold = (v_min + v_max) / 2;
      } else {
        threshold = 127; // 255 / 2
      }
      pos = (y * src->bytesperline) + x;
#ifdef DEBUG
      printf("v_min: %d v_max: %d\n", v_min, v_max);
      printf("threshold: %d\n", threshold);
      printf("pos: %ld\n", pos);
#endif
      if (src->data[pos] > threshold) {
        dst->data[pos] = 255;
      } else {
        dst->data[pos] = 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_bernsen(int kernel, int c_min) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_bernsen(src, dst, kernel, c_min) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_bernsen_vc_0005_example1.pgm",
                      dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_to_binary_niblack(IVC *src, IVC *dst, int kernel, float alpha) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if (kernel % 2 != 1) // TODO: check if this is correct
    return 0;

  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;

  long pos, pos_k;
  int tmp, threshold;
  int x, y, kx, ky;
  int offset = kernel / 2;
  unsigned long long total = 0, total_diff = 0;
  unsigned char mean = 0;
  float stdev = 0;

#ifdef DEBUG
  printf("kernel: %d offset: %d\n", kernel, offset);
#endif

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {

      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky > 0) && (y + ky < src->height) && (x + kx > 0) &&
              (x + kx < src->width)) {

            pos_k = ((y + ky) * src->bytesperline) + (x + kx);
            total += src->data[pos_k];
          }
        }
      }

      mean = (unsigned char)(total / kernel);
#ifdef DEBUG
      printf("mean: %d\n", mean);
#endif

      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky > 0) && (y + ky < src->height) && (x + kx > 0) &&
              (x + kx < src->width)) {
            pos_k = ((y + ky) * src->bytesperline) + (x + kx);
            total_diff = (src->data[pos_k] - mean) * (src->data[pos_k] - mean);
          }
        }
      }

      stdev = (float)sqrt(total_diff / (float)kernel);
      tmp = (int)((float)mean + (alpha * stdev));
#ifdef DEBUG
      printf("tmp: %d\n", tmp);
#endif
      if (tmp > 255) {
        threshold = 255;
      } else if (tmp < 0) {
        threshold = 0;
      } else {
        threshold = (unsigned char)tmp;
      }
      pos = (y * src->bytesperline) + x;
#ifdef DEBUG
      printf("stdev: %.2f\n", stdev);
      printf("threshold: %d\n", threshold);
      printf("pos: %ld\n", pos);
#endif
      if (src->data[pos] > threshold) {
        dst->data[pos] = 255;
      } else {
        dst->data[pos] = 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_niblack(int kernel, float alpha) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_niblack(src, dst, kernel, alpha) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_niblack_vc_0005_example1.pgm",
                      dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_to_binary_local_mean(IVC *src, IVC *dst, int kernel) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if (kernel % 2 != 1) // TODO: check if this is correct
    return 0;

  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;

  long pos, pos_k;
  int threshold;
  int x, y, kx, ky;
  int offset = kernel / 2;
  unsigned long long total = 0;

#ifdef DEBUG
  printf("kernel: %d offset: %d\n", kernel, offset);
#endif

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
#ifdef DEBUG
      printf("x: %d y: %d\n", x, y);
#endif
      total = 0;
      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky > 0) && (y + ky < src->height) && (x + kx > 0) &&
              (x + kx < src->width)) {
            pos_k = ((y + ky) * src->bytesperline) + (x + kx);
#ifdef DEBUG
            printf("kx: %d ky: %d\n", kx, ky);
            printf("pos_k: %ld\n", pos_k);
#endif
            total += src->data[pos_k];
          }
        }
      }

      threshold = (int)(total / (unsigned long long)(kernel * kernel));
      pos = (y * src->bytesperline) + x;
#ifdef DEBUG
      printf("total: %lld\n", total);
      printf("threshold: %d\n", threshold);
      printf("pos: %ld\n", pos);
#endif
      if (src->data[pos] > threshold) {
        dst->data[pos] = 255;
      } else {
        dst->data[pos] = 0;
      }
    }
  }

  return 1;
}

int vc_test_gray_to_binary_local_mean(int kernel) {
  IVC *src = vc_read_image("Images/vc_0005_example1.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_local_mean(src, dst, kernel) != 1)
    return 0;
  vc_write_image_info("out/vc_gray_to_binary_local_mean_vc_0005_example1.pgm",
                      dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_histogram_grayscale(IVC *src, long hist[]) {
  if (!src)
    return 0;

  memset(hist, 0, sizeof(long) * 255);

  long i, size = src->width * src->height;
  for (i = 0; i < size; i++) {
    hist[src->data[i]]++;
  }

  return 1;
}

// Otsu's thresholding method using inter-class variance
int vc_gray_to_binary_otsu(IVC *src, IVC *dst) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;

  long int hist[256];
  double prob[256];
  int sumb, sumf;
  double wb, wf, mb, mf, between;
  int ideal_th = 0;
  double max = 0.0;
  int i;
  int threshold;

  // histogram generation
  if (vc_histogram_grayscale(src, hist) != 1)
    return 0;

  // calculation of probability density
  for (i = 0; i < 256; i++) {
    prob[i] = (double)hist[i] / (src->width * src->height);
  }

  for (threshold = 0; threshold < 255; threshold++) {
    sumb = 0;
    sumf = 0;
    wb = 0.0;
    wf = 0.0;
    mb = 0.0;
    mf = 0.0;
    between = 0.0;

    for (i = 0; i < threshold; i++) {
      wb += prob[i];
      mb += i * hist[i];
      sumb += hist[i];
    }
    if (mb != 0)
      mb = mb / sumb;

    for (i = threshold; i < 255; i++) {
      wf += prob[i];
      mf += i * hist[i];
      sumf += hist[i];
    }
    if (mf != 0)
      mf = mf / sumf;

    between = wb * wf * pow(mf - mb, 2);

    if (between > max) {
      max = between;
      ideal_th = threshold;
    }
  }

#ifdef DEBUG
  printf("Otsu's threshold: %d\n", ideal_th);
#endif

  return vc_gray_to_binary_fixed(src, dst, ideal_th);
}

// Morphological Operators (Grayscale)
int vc_binary_dilate(IVC *src, IVC *dst, int size) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  if (size < 1)
    return 0;

  long pos, pos_k;
  int x, y, kx, ky;
  int offset = size / 2;

#ifdef DEBUG
  printf("size: %d offset: %d\n", size, offset);
#endif

  size_t size_data = src->bytesperline * src->height;
  memcpy(dst->data, src->data, sizeof(unsigned char) * size_data);

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
      pos = y * src->bytesperline + x * src->channels;
      if (src->data[pos] == 0) {
        for (kx = -offset; kx <= offset; kx++) {
          for (ky = -offset; ky <= offset; ky++) {
            if ((y + ky >= 0) && (y + ky <= src->height) && (x + kx >= 0) &&
                (x + kx <= src->width)) {
              pos_k = (y + ky) * src->bytesperline + (x + kx) * src->channels;
              dst->data[pos] |= src->data[pos_k];
            }
          }
        }
      }
    }
  }

  return 1;
}

int vc_test_binary_dilate(int size) {
  IVC *src = vc_read_image("Images/FLIR/flir-01.pgm");
  IVC *bin = vc_grayscale_new(src->width, src->height);
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_global_mean(src, bin) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_dilate_flir-01_bin.pgm", bin);
  if (vc_binary_dilate(bin, dst, size) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_dilate_flir-01.pgm", dst);
  vc_image_free(src);
  vc_image_free(bin);
  vc_image_free(dst);
  return 1;
}

int vc_binary_erode(IVC *src, IVC *dst, int size) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  if (size < 1)
    return 0;

  long pos, pos_k;
  int x, y, kx, ky;
  int offset = size / 2;

#ifdef DEBUG
  printf("size: %d offset: %d\n", size, offset);
#endif

  size_t size_data = src->bytesperline * src->height;
  memcpy(dst->data, src->data, sizeof(unsigned char) * size_data);

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
      pos = y * src->bytesperline + x * src->channels;
      if (src->data[pos] == 255) {
        for (kx = -offset; kx <= offset; kx++) {
          for (ky = -offset; ky <= offset; ky++) {
            if ((y + ky >= 0) && (y + ky <= src->height) && (x + kx >= 0) &&
                (x + kx <= src->width)) {
              pos_k = (y + ky) * src->bytesperline + (x + kx) * src->channels;
              dst->data[pos] &= src->data[pos_k];
            }
          }
        }
      }
    }
  }

  return 1;
}

int vc_test_binary_erode(int size) {
  IVC *src = vc_read_image("Images/FLIR/flir-01.pgm");
  IVC *bin = vc_grayscale_new(src->width, src->height);
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_global_mean(src, bin) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_erode_flir-01_bin.pgm", bin);
  if (vc_binary_erode(bin, dst, size) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_erode_flir-01.pgm", dst);
  vc_image_free(src);
  vc_image_free(bin);
  vc_image_free(dst);
  return 1;
}

int vc_binary_open(IVC *src, IVC *dst, int size_erode, int size_dilate) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  IVC *tmp = vc_grayscale_new(src->width, src->height);
  if (vc_binary_erode(src, tmp, size_erode) != 1)
    return 0;
  if (vc_binary_dilate(tmp, dst, size_dilate) != 1)
    return 0;
  return 1;
}

int vc_test_binary_open(int size_erode, int size_dilate) {
  IVC *src = vc_read_image("Images/FLIR/flir-01.pgm");
  IVC *bin = vc_grayscale_new(src->width, src->height);
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_global_mean(src, bin) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_open_flir-01_bin.pgm", bin);
  if (vc_binary_open(bin, dst, size_erode, size_dilate) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_open_flir-01.pgm", dst);
  vc_image_free(src);
  vc_image_free(bin);
  vc_image_free(dst);
  return 1;
}

int vc_binary_close(IVC *src, IVC *dst, int size_erode, int size_dilate) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  IVC *tmp = vc_grayscale_new(src->width, src->height);
  if (vc_binary_dilate(src, tmp, size_dilate) != 1)
    return 0;
  if (vc_binary_erode(tmp, dst, size_erode) != 1)
    return 0;
  return 1;
}

int vc_test_binary_close(int size_erode, int size_dilate) {
  IVC *src = vc_read_image("Images/FLIR/flir-01.pgm");
  IVC *bin = vc_grayscale_new(src->width, src->height);
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_to_binary_global_mean(src, bin) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_close_flir-01_bin.pgm", bin);
  if (vc_binary_close(bin, dst, size_erode, size_dilate) != 1)
    return 0;
  vc_write_image_info("out/vc_test_binary_close_flir-01.pgm", dst);
  vc_image_free(src);
  vc_image_free(bin);
  vc_image_free(dst);
  return 1;
}

// Morphological Operators (Grayscale)

int vc_gray_dilate(IVC *src, IVC *dst, int size) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  if (size < 1)
    return 0;

  long pos, pos_k;
  int x, y, kx, ky;
  int offset = size / 2;
  unsigned char max = 0;

#ifdef DEBUG
  printf("size: %d offset: %d\n", size, offset);
#endif

  size_t size_data = src->bytesperline * src->height;
  memcpy(dst->data, src->data, sizeof(unsigned char) * size_data);

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
      pos = y * src->bytesperline + x * src->channels;
#ifdef DEBUG
      printf("pos: %ld\n", pos);
#endif
      max = 0;
      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky >= 0) && (y + ky <= src->height) && (x + kx >= 0) &&
              (x + kx <= src->width)) {
            pos_k = (y + ky) * src->bytesperline + (x + kx) * src->channels;
#ifdef DEBUG
            printf("src->data[%ld]: %d\n", pos_k, src->data[pos_k]);
#endif
            if (src->data[pos_k] > max) {
              max = src->data[pos_k];
            }
          }
        }
        dst->data[pos] = max;
      }
    }
  }

  return 1;
}

int vc_test_gray_dilate(int size) {
  IVC *src = vc_read_image("Images/Special/testpat.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_dilate(src, dst, size) != 1)
    return 0;
  vc_write_image_info("out/vc_test_gray_dilate_testpat.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_erode(IVC *src, IVC *dst, int size) {
  if (!vc_is_grayscale(src))
    return 0;
  if (!vc_is_grayscale(dst))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  if (size < 1)
    return 0;

  long pos, pos_k;
  int x, y, kx, ky;
  int offset = size / 2;
  unsigned char min = 255;

#ifdef DEBUG
  printf("size: %d offset: %d\n", size, offset);
#endif

  size_t size_data = src->bytesperline * src->height;
  memcpy(dst->data, src->data, sizeof(unsigned char) * size_data);

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
      pos = y * src->bytesperline + x * src->channels;
#ifdef DEBUG
      printf("pos: %ld\n", pos);
#endif
      min = 255;
      for (kx = -offset; kx <= offset; kx++) {
        for (ky = -offset; ky <= offset; ky++) {
          if ((y + ky >= 0) && (y + ky <= src->height) && (x + kx >= 0) &&
              (x + kx <= src->width)) {
            pos_k = (y + ky) * src->bytesperline + (x + kx) * src->channels;
#ifdef DEBUG
            printf("src->data[%ld]: %d\n", pos_k, src->data[pos_k]);
#endif
            if (src->data[pos_k] < min) {
              min = src->data[pos_k];
            }
          }
        }
        dst->data[pos] = min;
      }
    }
  }

  return 1;
}

int vc_test_gray_erode(int size) {
  IVC *src = vc_read_image("Images/Special/testpat.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_erode(src, dst, size) != 1)
    return 0;
  vc_write_image_info("out/vc_test_gray_erode_testpat.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_open(IVC *src, IVC *dst, int size_erode, int size_dilate) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  IVC *tmp = vc_grayscale_new(src->width, src->height);
  if (vc_gray_erode(src, tmp, size_erode) != 1)
    return 0;
  if (vc_gray_dilate(tmp, dst, size_dilate) != 1)
    return 0;
  return 1;
}

int vc_test_gray_open(int size_erode, int size_dilate) {
  IVC *src = vc_read_image("Images/Special/testpat.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_open(src, dst, size_erode, size_dilate) != 1)
    return 0;
  vc_write_image_info("out/vc_test_gray_open_testpat.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_gray_close(IVC *src, IVC *dst, int size_erode, int size_dilate) {
  if ((src->width != dst->width) || (src->height != dst->height))
    return 0;
  IVC *tmp = vc_grayscale_new(src->width, src->height);
  if (vc_gray_dilate(src, tmp, size_dilate) != 1)
    return 0;
  if (vc_gray_erode(tmp, dst, size_erode) != 1)
    return 0;
  return 1;
}

int vc_test_gray_close(int size_erode, int size_dilate) {
  IVC *src = vc_read_image("Images/Special/testpat.pgm");
  IVC *dst = vc_grayscale_new(src->width, src->height);
  if (vc_gray_close(src, dst, size_erode, size_dilate) != 1)
    return 0;
  vc_write_image_info("out/vc_test_gray_close_testpat.pgm", dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

double benchmark_function(int (*vc_function)(IVC *, IVC *)) {
  IVC *gray = vc_read_image("Images/Classic/barbara.pgm");
  IVC *rgb = vc_rgb_new(gray->width, gray->height);
  clock_t start = clock();
  if ((*vc_function)(gray, rgb) != 1)
    return -1.0;
  clock_t end = clock();
  vc_image_free(gray);
  vc_image_free(rgb);
  return (double)(end - start) / CLOCKS_PER_SEC;
}

double benchmark_average(int (*vc_function)(IVC *, IVC *), size_t times) {
  double total = 0.0;
  for (size_t i = 0; i < times; i++) {
    total += benchmark_function((*vc_function));
  }
  return total / (double)times;
}

void benchmark_average_print(int (*vc_fn)(IVC *, IVC *), char *fn_name,
                             size_t times) {
  double elapsed = benchmark_average((*vc_fn), times);
  if (elapsed != 1.0)
    printf("%s took %.5f ms to execute after %zu times\n", fn_name, elapsed,
           times);
}

void benchmark_compare_functions(int (*vc_fn1)(IVC *, IVC *), char *fn1_name,
                                 int (*vc_fn2)(IVC *, IVC *), char *fn2name,
                                 size_t times) {
  double time1 = benchmark_average((*vc_fn1), times);
  double time2 = benchmark_average((*vc_fn2), times);
  double percentage = (time1 * 100.0) / time2;
  printf("%s took %2.f%% of %s\n", fn1_name, percentage, fn2name);
}

int vc_test_binary_function(char *filepath, imagetype_e type,
                            int (*vc_fn)(IVC *, IVC *), char *fn_name) {
  IVC *src = vc_read_image(filepath);
  IVC *dst;
  switch (type) {
  case rgb:
    dst = vc_rgb_new(src->width, src->height);
    break;
  case hsv:
    dst = vc_rgb_new(src->width, src->height);
    break;
  case grayscale:
    dst = vc_grayscale_new(src->width, src->height);
    break;
  case binary:
    dst = vc_binary_new(src->width, src->height);
    break;
  default:
    return 0;
  }
  if ((*vc_fn)(src, dst) != 1)
    return 0;
  // TODO
  char buf[357]; // 255 + 1 + 100 + 1
  switch (type) {
  case rgb:
    snprintf(buf, sizeof(buf), "%s/%s.ppm", OUTDIR, fn_name);
    break;
  case hsv:
    snprintf(buf, sizeof(buf), "%s/%s.ppm", OUTDIR, fn_name);
    break;
  case grayscale:
    snprintf(buf, sizeof(buf), "%s/%s.pgm", OUTDIR, fn_name);
    break;
  case binary:
    snprintf(buf, sizeof(buf), "%s/%s.pbm", OUTDIR, fn_name);
    break;
  default:
    return 0;
  }
  vc_write_image_info(buf, dst);
  vc_image_free(src);
  vc_image_free(dst);
  return 1;
}

int vc_binary_blob_print(OVC *blob) {
  if (!blob)
    return 0;

  printf("label: %d\n", blob->label);
  printf("mass center: (%d:%d)\n", blob->xc, blob->yc);
  printf("area: %d\n", blob->area);
  printf("perimeter: %d\n", blob->perimeter);
  printf("width: %d\theight: %d\n", blob->width, blob->height);
  printf("x: %d\ty: %d\n", blob->x, blob->y);

  return 1;
}

int vc_draw_boundary_box(IVC *src, int x_min, int x_max, int y_min, int y_max,
                         int color) {
  int x, y;
  long pos;

  for (x = x_min; x <= x_max; x++) {
    pos = y_min * src->width + x;
    src->data[pos] = color;
    pos = y_max * src->width + x;
    src->data[pos] = color;
  }

  for (y = y_min; y <= y_max; y++) {
    pos = y * src->width + x_min;
    src->data[pos] = color;
    pos = y * src->width + x_max;
    src->data[pos] = color;
  }

  return 1;
}

int vc_draw_mass_center(IVC *src, int xc, int yc, int color) {
  long pos = yc * src->width + xc;
  long pos_up = (yc - 1) * src->width + xc;
  long pos_down = (yc + 1) * src->width + xc;

  src->data[pos] = color;
  src->data[pos - 1] = color;
  src->data[pos + 1] = color;

  src->data[pos_up] = color;
  src->data[pos_down] = color;

  return 1;
}

unsigned char minimum_non_zero(unsigned char *arr, size_t size) {
  unsigned char min = 255;
  for (size_t i = 0; i < size; i++) {
    if ((arr[i] != 0) && (arr[i] != 255) && (arr[i] < min)) {
      min = arr[i];
    }
  }
  return min;
}

// Etiquetagem de blobs
// src : Imagem binária
// dst : Imagem grayscale (irá conter as etiquetas)
// nlabels : Endereço de memória de uma variável inteira. Recebe o número de
// etiquetas encontradas.
// OVC* : Retorna lista de estruturas de blobs (objectos), com respectivas
// etiquetas. É necessário libertar posteriormente esta memória.
OVC *vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels) {
  unsigned char *datasrc = (unsigned char *)src->data;
  unsigned char *datadst = (unsigned char *)dst->data;
  int width = src->width;
  int height = src->height;
  int bytesperline = src->bytesperline;
  int channels = src->channels;
  int x, y, a, b;
  long int i, size = bytesperline * height;
  long int posX, posA, posB, posC, posD;
  int labeltable[256] = {0};
  int label = 1; // Etiqueta inicial.
  int num;
  // Apontador para lista de blobs (objectos) que será retornada desta função.
  OVC *blobs = NULL;

  // Verificação de erros
  if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) {
    error("vc_binary_blob_labelling: first\n");
    return NULL;
  }
  if ((src->width != dst->width) || (src->height != dst->height) ||
      (src->channels != dst->channels)) {
    error("vc_binary_blob_labelling: tamanhos e canais diferentes\n");
    return NULL;
  }
  if (channels != 1) {
    error("vc_binary_blob_labelling: src->channels != 1\n");
    return NULL;
  }

  // Copia dados da imagem binária para imagem grayscale
  memcpy(datadst, datasrc, size);

  // Todos os pixeis de plano de fundo devem obrigat�riamente ter valor 0
  // Todos os pixeis de primeiro plano devem obrigat�riamente ter valor 255
  // Serão atribuídas etiquetas no intervalo [1,254]
  // Este algoritmo está assim limitado a 254 labels
  for (i = 0; i < size; i++) {
    if (datadst[i] != 0) {
      datadst[i] = 255;
    }
  }

  // Limpa os rebordos da imagem binária
  for (y = 0; y < height; y++) {
    datadst[y * bytesperline + 0 * channels] = 0;
    datadst[y * bytesperline + (width - 1) * channels] = 0;
  }
  for (x = 0; x < width; x++) {
    datadst[0 * bytesperline + x * channels] = 0;
    datadst[(height - 1) * bytesperline + x * channels] = 0;
  }

  // Efectua a etiquetagem
  for (y = 1; y < height - 1; y++) {
    for (x = 1; x < width - 1; x++) {
      // Kernel:
      // A B C
      // D X

      posA = (y - 1) * bytesperline + (x - 1) * channels; // A
      posB = (y - 1) * bytesperline + x * channels;       // B
      posC = (y - 1) * bytesperline + (x + 1) * channels; // C
      posD = y * bytesperline + (x - 1) * channels;       // D
      posX = y * bytesperline + x * channels;             // X

      // Se o pixel foi marcado
      if (datadst[posX] != 0) {
        if ((datadst[posA] == 0) && (datadst[posB] == 0) &&
            (datadst[posC] == 0) && (datadst[posD] == 0)) {
          datadst[posX] = label;
          labeltable[label] = label;
          label++;
        } else {
          num = 255;

          // Se A est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a
          // etiqueta "num"
          if ((datadst[posA] != 0) && (datadst[posA] != 255) &&
              (datadst[posA] < num)) {
            num = datadst[posA];
          }
          // Se B est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a
          // etiqueta "num"
          if ((datadst[posB] != 0) && (datadst[posB] != 255) &&
              (datadst[posB] < num)) {
            num = datadst[posB];
          }
          // Se C est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a
          // etiqueta "num"
          if ((datadst[posC] != 0) && (datadst[posC] != 255) &&
              (datadst[posC] < num)) {
            num = datadst[posC];
          }
          // Se D est� marcado, j� tem etiqueta (j� n�o � 255), e � menor que a
          // etiqueta "num"
          if ((datadst[posD] != 0) && (datadst[posD] != 255) &&
              (datadst[posD] < num)) {
            num = datadst[posD];
          }

          // Actualiza a tabela de etiquetas
          if ((datadst[posA] != 0) && (datadst[posA] != 255)) {
            if (labeltable[datadst[posA]] != labeltable[num]) {
              for (a = 1; a < label; a++) {
                if (labeltable[a] == labeltable[datadst[posA]]) {
                  labeltable[a] = labeltable[num];
                }
              }
            }
          }
          if ((datadst[posB] != 0) && (datadst[posB] != 255)) {
            if (labeltable[datadst[posB]] != labeltable[num]) {
              for (a = 1; a < label; a++) {
                if (labeltable[a] == labeltable[datadst[posB]]) {
                  labeltable[a] = labeltable[num];
                }
              }
            }
          }
          if ((datadst[posC] != 0) && (datadst[posC] != 255)) {
            if (labeltable[datadst[posC]] != labeltable[num]) {
              for (a = 1; a < label; a++) {
                if (labeltable[a] == labeltable[datadst[posC]]) {
                  labeltable[a] = labeltable[num];
                }
              }
            }
          }
          if ((datadst[posD] != 0) && (datadst[posD] != 255)) {
            if (labeltable[datadst[posD]] != labeltable[num]) {
              for (a = 1; a < label; a++) {
                if (labeltable[a] == labeltable[datadst[posD]]) {
                  labeltable[a] = labeltable[num];
                }
              }
            }
          }
          labeltable[datadst[posX]] = num;

          // Atribui a etiqueta ao pixel
          datadst[posX] = num;
        }
      }
    }
  }

  // Volta a etiquetar a imagem
  for (y = 1; y < height - 1; y++) {
    for (x = 1; x < width - 1; x++) {
      posX = y * bytesperline + x * channels; // X

      if (datadst[posX] != 0) {
        datadst[posX] = labeltable[datadst[posX]];
      }
    }
  }

  // Contagem do n�mero de blobs
  // Passo 1: Eliminar, da tabela, etiquetas repetidas
  for (a = 1; a < label - 1; a++) {
    for (b = a + 1; b < label; b++) {
      if (labeltable[a] == labeltable[b])
        labeltable[b] = 0;
    }
  }
  // Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o
  // hajam valores vazios (zero) entre etiquetas
  *nlabels = 0;
  for (a = 1; a < label; a++) {
    if (labeltable[a] != 0) {
      labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
      (*nlabels)++;                         // Conta etiquetas
    }
  }

  // Se não há blobs
  if (*nlabels == 0) {
    return NULL;
  }

#ifdef DEBUG
  printf("nlabels=%d\n", *nlabels);
#endif

  // Cria lista de blobs (objectos) e preenche a etiqueta
  blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
  if (blobs != NULL) {
    for (a = 0; a < (*nlabels); a++) {
      blobs[a].label = labeltable[a];
    }
  } else {
    return NULL;
  }

  return blobs;
}

int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs) {
  unsigned char *data = (unsigned char *)src->data;
  int width = src->width;
  int height = src->height;
  int bytesperline = src->bytesperline;
  int channels = src->channels;
  int x, y, i;
  long int pos;
  int xmin, ymin, xmax, ymax;
  long int sumx, sumy;

  // Verificação de erros
  if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
    return 0;
  if (channels != 1)
    return 0;

  // Conta área de cada blob
  for (i = 0; i < nblobs; i++) {
    xmin = width - 1;
    ymin = height - 1;
    xmax = 0;
    ymax = 0;

    sumx = 0;
    sumy = 0;

    blobs[i].area = 0;

    for (y = 1; y < height - 1; y++) {
      for (x = 1; x < width - 1; x++) {
        pos = y * bytesperline + x * channels;

        if (data[pos] == blobs[i].label) {
          // Área
          blobs[i].area++;

          // Centro de Gravidade
          sumx += x;
          sumy += y;

          // Bounding Box
          if (xmin > x)
            xmin = x;
          if (ymin > y)
            ymin = y;
          if (xmax < x)
            xmax = x;
          if (ymax < y)
            ymax = y;

          // Perímetro
          // Se pelo menos um dos quatro vizinhos não pertence ao mesmo label,
          // entao há um pixel de contorno
          if ((data[pos - 1] != blobs[i].label) ||
              (data[pos + 1] != blobs[i].label) ||
              (data[pos - bytesperline] != blobs[i].label) ||
              (data[pos + bytesperline] != blobs[i].label)) {
            blobs[i].perimeter++;
          }
        }
      }
    }

    // Bounding Box
    blobs[i].x = xmin;
    blobs[i].y = ymin;
    blobs[i].width = (xmax - xmin) + 1;
    blobs[i].height = (ymax - ymin) + 1;

    // Centro de Gravidade
    blobs[i].xc = sumx / (blobs[i].area > 1 ? blobs[i].area : 1);
    blobs[i].yc = sumy / (blobs[i].area > 1 ? blobs[i].area : 1);

    // Circularidade
    blobs[i].circularity = (12.5663706 * blobs[i].area) /
                           (blobs[i].perimeter * blobs[i].perimeter);
  }

  return 1;
}

// Filtra todos os blobs a baixo de `area`.
int vc_binary_blob_filter(OVC **blobs, int nblobs, int area) {
  if (!blobs)
    return -1;
  if (nblobs < 1)
    return -1;

  OVC *in = *blobs;
  int i, j = 0, nfblobs = 0;

  for (i = 0; i < nblobs; i++) {
    if (in[i].area > area) {
      nfblobs++;
    }
  }

  OVC *fblobs = malloc(sizeof(OVC[nfblobs]));
  if (!fblobs)
    return -1;

  for (i = 0; i < nblobs; i++) {
    if (in[i].area > area) {
      fblobs[j] = in[i];
      j++;
    }
  }

  free(*blobs);
  *blobs = fblobs;

  return nfblobs;
}

// Get the H component of a HSV image
int vc_hsv_get_hue(IVC *hsv, IVC *gray) {
  if (!vc_is_rgb(hsv)) {
    return 0;
  }
  if (!vc_is_grayscale(gray)) {
    return 0;
  }
  if ((hsv->width != gray->width) || (hsv->height != gray->height)) {
    return 0;
  }

  long pos_hsv, pos_gray;

  for (int y = 0; y < hsv->height; y++) {
    for (int x = 0; x < hsv->width; x++) {
      pos_hsv = (y * hsv->bytesperline) + (x * 3);
      pos_gray = (y * gray->bytesperline) + x;

      gray->data[pos_gray] = gray->data[pos_hsv];
    }
  }

  return 1;
}

int vc_gray_lowpass_mean_filter(IVC *src, IVC *dst) {
  unsigned char *datasrc = (unsigned char *)src->data;
  unsigned char *datadst = (unsigned char *)dst->data;
  int width = src->width;
  int height = src->height;
  int bytesperline = src->bytesperline;
  int channels = src->channels;
  int x, y;
  long int posX, posA, posB, posC, posD, posE, posF, posG, posH;
  float sum;

  // Verificação de erros
  if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height) ||
      (src->channels != dst->channels))
    return 0;
  if ((src->channels != 1))
    return 0;

  for (y = 1; y < height - 1; y++) {
    for (x = 1; x < width - 1; x++) {
      posA = (y - 1) * bytesperline + (x - 1) * channels;
      posB = (y - 1) * bytesperline + (x)*channels;
      posC = (y - 1) * bytesperline + (x + 1) * channels;
      posD = y * bytesperline + (x - 1) * channels;
      posX = y * bytesperline + (x)*channels;
      posE = y * bytesperline + (x + 1) * channels;
      posF = (y + 1) * bytesperline + (x - 1) * channels;
      posG = (y + 1) * bytesperline + (x)*channels;
      posH = (y + 1) * bytesperline + (x + 1) * channels;

      sum = datasrc[posX];
      sum += datasrc[posA];
      sum += datasrc[posB];
      sum += datasrc[posC];
      sum += datasrc[posD];
      sum += datasrc[posE];
      sum += datasrc[posF];
      sum += datasrc[posG];
      sum += datasrc[posH];

      datadst[posX] = (unsigned char)(sum / 9);
    }
  }

  return 1;
}

int vc_gray_lowpass_median_filter(IVC *src, IVC *dst) {
  unsigned char *datasrc = (unsigned char *)src->data;
  unsigned char *datadst = (unsigned char *)dst->data;
  int width = src->width;
  int height = src->height;
  int bytesperline = src->bytesperline;
  int channels = src->channels;
  int x, y;
  int i, j;
  long int posX, posA, posB, posC, posD, posE, posF, posG, posH;
  unsigned char minvalue, minindex, tmp;
  unsigned char values[9];

  // Verificação de erros
  if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height) ||
      (src->channels != dst->channels))
    return 0;
  if ((src->channels != 1))
    return 0;

  for (y = 1; y < height - 1; y++) {
    for (x = 1; x < width - 1; x++) {
      posA = (y - 1) * bytesperline + (x - 1) * channels;
      posB = (y - 1) * bytesperline + (x)*channels;
      posC = (y - 1) * bytesperline + (x + 1) * channels;
      posD = y * bytesperline + (x - 1) * channels;
      posX = y * bytesperline + (x)*channels;
      posE = y * bytesperline + (x + 1) * channels;
      posF = (y + 1) * bytesperline + (x - 1) * channels;
      posG = (y + 1) * bytesperline + (x)*channels;
      posH = (y + 1) * bytesperline + (x + 1) * channels;

      values[0] = datasrc[posA];
      values[1] = datasrc[posB];
      values[2] = datasrc[posC];
      values[3] = datasrc[posD];
      values[4] = datasrc[posX];
      values[5] = datasrc[posE];
      values[6] = datasrc[posF];
      values[7] = datasrc[posG];
      values[8] = datasrc[posH];

      // Organiza o array[] por ordem crescente
      for (i = 0; i < 9 - 1; i++) {
        minvalue = values[i];
        minindex = i;

        for (j = i + 1; j < 9; j++) {
          if (minvalue > values[j]) {
            minvalue = values[j];
            minindex = j;
          }
        }

        if (i != minindex) {
          tmp = values[i];
          values[i] = values[minindex];
          values[minindex] = tmp;
        }
      }

      // Coloca no pixel de saida, o valor da mediana (values[4])
      datadst[posX] = (unsigned char)values[4];
    }
  }

  return 1;
}

// TODO
int vc_gray_edge_canny(IVC *src, IVC *dst) {
  // Verificação de erros
  if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
    return 0;
  if ((src->width != dst->width) || (src->height != dst->height) ||
      (src->channels != dst->channels))
    return 0;
  if ((src->channels != 1))
    return 0;

  return 1;
}

// Verifica se o blob "b2" está contido dentro do blob "b1".
// TODO: testar
int vc_blob_inside_blob(OVC *b1, OVC *b2) {
  int b1_left = b1->x;
  int b1_right = b1->x + b1->width;
  int b2_left = b2->x;
  int b2_right = b2->x + b2->width;

  int b1_top = b1->y;
  int b1_bottom = b1->y + b1->height;
  int b2_top = b2->y;
  int b2_bottom = b2->y + b2->height;

  // Intersecção:
  // return (b1_left < b2_right && b1_right > b2_left &&
  //        b1_top < b2_bottom && b1_bottom > b2_top);

  return (b1_left < b2_left && b1_right > b2_right && b1_top < b2_top &&
          b1_bottom > b2_bottom);
}

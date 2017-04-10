#include "color.h"

char *vc_color_name(Color color) {
  switch (color) {
  case Red:
    return "Red";
  case Blue:
    return "Blue";
  case Yellow:
    return "Yellow";
  case Green:
    return "Green";
  default:
    return "Unknown color";
  }
}

// hue:0-360; saturation:0-100: value:0-360
// Retorna o número de pixeis dentro dos valores fornecidos.
long vc_rgb_to_hsv_by_color(IVC *src, IVC *dst, int hmin, int hmax, int smin,
                            int smax, int vmin, int vmax) {
  int x, y;
  long pos, counter = 0;
  float rf, gf, bf;
  float hue, saturation, value;
  float max, min;

  // verificação de erros
  if (src->width <= 0 || src->height <= 0 || src->data == NULL)
    return -1;
  if (dst->width != src->width || dst->height != src->height)
    return -1;
  if (src->channels != 3 || dst->channels != 3)
    return -1;

  for (x = 0; x < src->width; x++) {
    for (y = 0; y < src->height; y++) {
      pos = y * src->bytesperline + x * src->channels;

      rf = (float)src->data[pos];
      gf = (float)src->data[pos + 1];
      bf = (float)src->data[pos + 2];

      max = MAX3(rf, gf, bf);
      min = MIN3(rf, gf, bf);

      value = max;
      if (value == 0.0) {
        saturation = 0.0;
        hue = 0.0;
      } else {
        saturation = ((value - min) / value) * 255.0f;
        if (saturation == 0.0) {
          hue = 0.0;
        } else {
          if ((max == rf) && (gf >= bf))
            hue = 60.0f * (gf - bf) / (max - min);
          else if ((max == rf) && (gf < bf))
            hue = 360.0f + 60.0f * (gf - bf) / (max - min);
          else if (max == gf)
            hue = 120.0f + 60.0f * (bf - rf) / (max - min);
          else
            hue = 240.0f + 60.0f * (rf - gf) / (max - min); // max == b
        }
      }

      dst->data[pos] = 0;
      dst->data[pos + 1] = 0;
      dst->data[pos + 2] = 0;

      if (hmin < hmax) {
        // segmentar apenas os pixeis
        if ((hue >= hmin && hue <= hmax) && (saturation * 100 / 255 >= smin &&
                                             saturation * 100 / 255 <= smax) &&
            (value * 100 / 255 >= vmin && value * 100 / 255 <= vmax)) {
          dst->data[pos] = 255;
          dst->data[pos + 1] = 255;
          dst->data[pos + 2] = 255;
          counter += 1;
        }
      } else {
        if ((hue >= hmin || hue <= hmax) && (saturation * 100 / 255 >= smin &&
                                             saturation * 100 / 255 <= smax) &&
            (value * 100 / 255 >= vmin && value * 100 / 255 <= vmax)) {
          dst->data[pos] = 255;
          dst->data[pos + 1] = 255;
          dst->data[pos + 2] = 255;
          counter += 1;
        }
      }
    }
  }

  return counter;
}

long vc_rgb_to_hsv_blue(IVC *src, IVC *dst) {
  return vc_rgb_to_hsv_by_color(src, dst, 210, 260, 20, 100, 16, 80);
}

long vc_rgb_to_hsv_red(IVC *src, IVC *dst) {
  return vc_rgb_to_hsv_by_color(src, dst, 354, 10, 40, 100, 32, 100);
}

long vc_rgb_to_hsv_yellow(IVC *src, IVC *dst) {
  return vc_rgb_to_hsv_by_color(src, dst, 50, 60, 90, 100, 90, 100);
}

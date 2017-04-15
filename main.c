#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "edges.h"
#include "myvc.h"
#include "sign.h"
#include "utils.h"

#define MAXIMAGES 50

// Função auxiliar para comparar dois blobs por area.
int compare_area_desc(const void *a, const void *b) {
  return (((OVC *)b)->area - (((OVC *)a)->area));
}

int compare_perimeter_asc(const void *a, const void *b) {
  return (((OVC *)a)->perimeter - (((OVC *)b)->perimeter));
}

int process_red(IVC *gray, char *filename) {
  // "bin" tem que ser uma imagem grayscale binária (0 e 1)
  IVC *bin = vc_grayscale_new(gray->width, gray->height);

  // converter para imagem binária
  if (!vc_gray_to_binary_global_mean(gray, bin)) {
    error("process_red: vc_gray_to_global_mean failed\n");
  }

#ifdef DEBUG
  if (!vc_write_image_info("out/binary.pbm", bin)) {
    error("process_red: vc_write_image_info failed\n");
  }
#endif

  int area_menor = 800;
  IVC *labeled = vc_grayscale_new(gray->width, gray->height);

  int nblobs = 0; // número de blobs identificados, inicialmente a zero
  OVC *blobs = vc_binary_blob_labelling(bin, labeled, &nblobs);
  if (!blobs) {
    fatal("vc_find_shape: vc_binary_blob_labelling failed\n");
  }

#ifdef DEBUG
  printf("\nFiltering labels by area (> %d).\n", area_menor);
  printf("Number of labels (before filtering): %d\n", nblobs);
#endif

  if (!vc_binary_blob_info(labeled, blobs, nblobs)) {
    fatal("vc_find_shape: vc_binary_blob_info failed\n");
  }

  // apenas blobs com area superior a 800
  nblobs = vc_binary_blob_filter(&blobs, nblobs, area_menor);
  if (nblobs == -1) {
    fatal("vc_find_shape: vc_binary_blob_filter failed\n");
  }

#ifdef DEBUG
  printf("Number of labels (after filtering): %d\n\n", nblobs);
#endif

  for (int i = 0; i < nblobs; i++) {
#ifdef DEBUG
    vc_binary_blob_print(&blobs[i]);
    printf("\n");
#endif
    vc_draw_mass_center(labeled, blobs[i].xc, blobs[i].yc, 255);
    vc_draw_boundary_box(labeled, blobs[i].x, blobs[i].x + blobs[i].width,
                         blobs[i].y, blobs[i].y + blobs[i].height, 255);
  }

#ifdef DEBUG
  char *fname = concat(4, "out/", "blobbed_", filename, ".pgm");
  vc_write_image_info(fname, labeled);
  free(fname);
#endif

  // ordena blobs por area descendente
  qsort(blobs, nblobs, sizeof(OVC), compare_area_desc);

  OVC *largest = &blobs[0];
  // printf("blob com maior área: %d\n", largest->area);
  int insiders = 0;
  for (int i = 1; i < nblobs; i++) {
    if (vc_blob_inside_blob(largest, &blobs[i])) {
      /* printf("blob com label %d está dentro da exterior (maior)\n",
             blobs[i].label); */
      insiders++;
    }
  }

  if (insiders == 1) {
    printf("\nIDENTIFICADO PROIBIDO\n");
  } else if (insiders > 1) {
    printf("\nIDENTIFICADO STOP\n");
  } else {
    error("Sinal vermelho não reconhecido\n");
  }

  vc_image_free(labeled);
  vc_image_free(bin);
  free(blobs);

  return 1;
}

int process_blue(IVC *gray, char *filename) {
  int area_menor = 400;
  IVC *bin      = vc_grayscale_new(gray->width, gray->height);
  IVC *labeled  = vc_grayscale_new(gray->width, gray->height);

  // converter para imagem binária (0 e 1)
  if (!vc_gray_to_binary_global_mean(gray, bin)) {
    error("process_blue: vc_gray_to_global_mean failed\n");
  }

  #ifdef DEBUG
  if (!vc_write_image_info("out/binary.pbm", bin)) {
    error("process_blue: vc_write_image_info failed\n");
  }
  #endif

  int nblobs = 0; // número de blobs identificados, inicialmente a zero
  OVC *blobs = vc_binary_blob_labelling(bin, labeled, &nblobs);
  if (!blobs) {
    fatal("process_blue: vc_binary_blob_labelling failed\n");
  }

#ifdef DEBUG
  printf("\nFiltering labels by area (> %d).\n", area_menor);
  printf("Number of labels (before filtering): %d\n", nblobs);
#endif

  if (!vc_binary_blob_info(labeled, blobs, nblobs)) {
    fatal("process_blue: vc_binary_blob_info failed\n");
  }

  // apenas blobs com area superior a 400
  nblobs = vc_binary_blob_filter(&blobs, nblobs, area_menor);
  if (nblobs == -1) {
    fatal("process_blue: vc_binary_blob_filter failed\n");
  }

#ifdef DEBUG
  printf("Number of labels (after filtering): %d\n\n", nblobs);
#endif

  for (int i = 0; i < nblobs; i++) {
#ifdef DEBUG
    vc_binary_blob_print(&blobs[i]);
    printf("\n");
#endif
    vc_draw_mass_center(labeled, blobs[i].xc, blobs[i].yc, 255);
    vc_draw_boundary_box(labeled, blobs[i].x, blobs[i].x + blobs[i].width,
                         blobs[i].y, blobs[i].y + blobs[i].height, 255);
  }

#ifdef DEBUG
  char *fname = concat(4, "out/", "blobbed_", filename, ".pgm");
  vc_write_image_info(fname, labeled);
  free(fname);
#endif

  // ordena blobs por perimetro ascendente
  qsort(blobs, nblobs, sizeof(OVC), compare_perimeter_asc);

  printf("DEBUG nblobs: %d\n", nblobs);

  OVC *insiders[nblobs];
  int ninsiders = 0;
  OVC *container = NULL;
  for (int i = 1; i < nblobs; i++) { // para cada um dos blobs
    if (container ==
        NULL) { // se o blob exterior ainda não tiver sido encontrado
      for (int j = 0; j < i;
           j++) { // para cada blob com uma area menor do que o atual
        // se o blob atual contiver o mais pequeno
        if (vc_blob_inside_blob(&blobs[i], &blobs[j])) {
          container = &blobs[i];
          insiders[j] = &blobs[j];
          ninsiders++;
        } else {
          insiders[j] = NULL;
        }
      }
    }
  }

/*
  printf("Container: label: %d\n", container->label);

  for (int i = 0; i < ninsiders; i++) {
    // if (insiders[i] == 1) printf("insider label %d ", blobs[i].label);
    if (insiders[i] != NULL)
      printf("insiders[%d]=%d\n", i, insiders[i]->label);
  }
*/

  // preencher os blobs interiores ao exterior a branco
  IVC *filled = vc_gray_fill_holes(gray, insiders, nblobs);
  if (!filled) {
    fatal("process_blue: vc_gray_fill_holes failed\n");
  }

#ifdef DEBUG
  char *fname2 = concat(4, "out/", "filled_", filename, ".pgm");
  vc_write_image_info(fname2, filled);
  free(fname2);
#endif

  vc_gray_negative(filled);
#ifdef DEBUG
  if (!vc_write_image_info("out/inverted_filled.pgm", filled)) {
    error("process_blue: vc_write_image_info failed\n");
  }
#endif

#ifdef DEBUG
  printf("FINAL: container: %d circularity=%.2f\n", container->label,
         container->circularity);
#endif

  if (container->circularity > 0.95) {
    printf("Circulo com %d insiders\n", ninsiders);
    OVC *smallest = insiders[0];
    if ((smallest->width / smallest->height) > 1.0) {
      printf("Seta horizontal\n");
      // calcular o centro de massa "ideal"
      int xm = smallest->x + (smallest->width / 2);
      // se o centro de massa estiver mais para a esquerda então Sinal
      // Esquerda
      if (smallest->xc < xm) {
        printf("\nIDENTIFICADO Sentido Obrigatório (Esquerda)\n");
      } else if (smallest->xc > xm) {
        printf("\nIDENTIFICADO Sentido Obrigatório (Direita)\n");
      }
    } else {
      printf("\nIDENTIFICADO Sentido Obrigatório (Frente)\n");
    }
  } else {
    printf("Rectangulo com %d insiders\n", ninsiders);
    if (ninsiders == 5) {
      printf("\nIDENTIFICADO Autoestrada\n");
    } else {
      printf("\nIDENTIFICADO Via Reservada\n");
    }
  }

  free(blobs);
  vc_image_free(labeled);
  vc_image_free(filled);
  vc_image_free(bin);

  return 1;
}

int process_file(char *path) {
  IVC *src = vc_read_image(path);
  IVC *gray = vc_grayscale_new(src->width, src->height);
  IVC *hsv = vc_rgb_new(src->width, src->height);
  char *filename = get_filename_no_ext(basename(path));

  printf("A identificar `%s'\n", path);

  Color color = vc_find_color(src, hsv);
#ifdef DEBUG
  printf("Cor: %s\n", vc_color_name(color));
  if (!vc_write_image_info("out/color_hsv.ppm", hsv)) {
    error("process_file: vc_write_image_info failed\n");
  }
#endif

  if (!vc_rgb_to_gray(hsv, gray)) {
    error("process_file: convertion to grayscale failed\n");
  }
#ifdef DEBUG
  if (!vc_write_image_info("out/color_segm.pgm", gray)) {
    error("process_file: vc_write_image_info failed\n");
  }
#endif

  if (color == Red) {
    process_red(gray, filename);
  } else if (color == Blue) {
    process_blue(gray, filename);
  } else {
    error("Sinal não reconhecido: \n");
  }

  vc_image_free(src);
  vc_image_free(gray);
  vc_image_free(hsv);
  free(filename);

  return 1;
}

// Imprime informação de utilização
void usage(char *program) {
  fprintf(stderr, "Usage: %s [file or directory]\n", program);
}

char **alloc_images(size_t nimages, size_t nchar) {
  char **images = calloc(nimages, sizeof(char *));
  if (!images)
    return NULL;
  for (size_t i = 0; i < nimages; i++) {
    images[i] = malloc((nchar + 1) * sizeof(char));
  }
  return images;
}

void free_images(char *images[], size_t nimages) {
  for (size_t i = 0; i < nimages; i++) {
    free(images[i]);
  }
  free(images);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Error: not enough arguments\n");
    usage(argv[0]);
    return 1;
  }

  // caminho do ficheiro ou pasta dado pelo utilizador
  char *path = argv[1];

  // testamos se o argumento é um ficheiro
  if (is_regular_file(path)) {
    if (!process_file(path)) {
      fprintf(stderr, "main: process_file failed\n");
      return 1;
    }
    return 0;
  }

  // alocação de memória para MAXIMAGES com 100 caracteres cada
  char **images = alloc_images(MAXIMAGES, 100);
  size_t nimages = 0; // número de imagens encontradas

  // testamos se o argumento é um diretório
  if (is_directory(path)) {
    nimages = get_images_from_dir(path, images, MAXIMAGES);
    printf("Number of images to process: %ld\n", nimages);
    if (nimages < 1) {
      fprintf(stderr, "main: get_images_from_dir found no images\n");
      free_images(images, MAXIMAGES);
      return 1;
    }
    for (size_t i = 0; i < nimages; i++) {
      if (!process_file(images[i])) {
        fprintf(stderr, "main: process_file on image `%s'\n", path);
      }
      getchar();
    }
  }

  // libertar o espaço
  free_images(images, MAXIMAGES);

  return 0;
}

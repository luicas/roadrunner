#include "utils.h"
#include <dirent.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: move to utils.c
char *concat(int count, ...) {
  va_list ap;
  int i;

  // Find required length to store merged string
  int len = 1; // room for NULL
  va_start(ap, count);
  for (i = 0; i < count; i++)
    len += strlen(va_arg(ap, char *));
  va_end(ap);

  // Allocate memory to concat strings
  char *merged = calloc(sizeof(char), len);
  int null_pos = 0;

  // Actually concatenate strings
  va_start(ap, count);
  for (i = 0; i < count; i++) {
    char *s = va_arg(ap, char *);
    strcpy(merged + null_pos, s);
    null_pos += strlen(s);
  }
  va_end(ap);

  return merged;
}

// Verifa se um determinado caminho corresponde a um ficheiro normal.
int is_regular_file(const char *path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0)
    return 0;
  return S_ISREG(path_stat.st_mode);
}

// Verifa se um determinado caminho corresponde a uma directório.
int is_directory(const char *path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0)
    return 0;
  return S_ISDIR(path_stat.st_mode);
}

char *get_filename_no_ext(char *s) {
  char *retstr;
  char *lastdot;
  if (s == NULL)
    return NULL;
  if ((retstr = malloc(strlen(s) + 1)) == NULL)
    return NULL;
  strcpy(retstr, s);
  lastdot = strrchr(retstr, '.');
  if (lastdot != NULL) {
    *lastdot = '\0';
  }
  return retstr;
}

char *get_filename_ext(const char *filename) {
  char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return NULL;
  return dot + 1;
}

int has_netpbm_file_extension(const char *filename) {
  if (!filename)
    return 0;
  const char *extension = get_filename_ext(filename);
  if (!extension)
    return 0;
  // printf("extension: %s\n", extension);
  size_t nextensions = sizeof(extensions) / sizeof(const char *const);
  // para cada extensão de imagem do formato Netpbm
  for (size_t i = 0; i < nextensions; i++) {
    // ver se a extensão do ficheiro é igual a uma extensão Netpbm
    // printf("extensions[i]: %s\n", extensions[i]);
    if (strncmp(extension, extensions[i], 3) == 0)
      return 1;
  }
  return 0;
}

// Retorna o número de imagens encontradas em path
size_t get_images_from_dir(const char *path, char **images, size_t nimages) {
  struct dirent *dir;
  DIR *d = opendir(path);
  if (!d)
    return -1;

  char *filename, *filepath;
  size_t i = 0;

  while (((dir = readdir(d)) != NULL) && (i < nimages)) {
    filename = dir->d_name;
    if (has_netpbm_file_extension(filename) == 1) {
      filepath = concat(3, path, "/", filename);
#ifdef DEBUG
      printf("filepath: %s\n", filepath);
#endif
      strncpy(images[i], filepath, strlen(filepath));
      free(filepath);
      i++;
    }
  }

  closedir(d);

  return i;
}

void fatal(const char *msg) {
  fprintf(stderr, "%s", msg);
  exit(1);
}

void error(const char *msg) { fprintf(stderr, "%s", msg); }

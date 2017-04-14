#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

static const char *const extensions[] = {"pbm", "pgm", "ppm", "pnm"};

void fatal(const char* message);
void error(const char* message);
char *concat(int count, ...);
int is_regular_file(const char *path);
int is_directory(const char *path);
char *get_filename_no_ext(char *s);
char *get_filename_ext(const char *filename);
int has_netpbm_file_extension(const char *filename);
size_t get_images_from_dir(const char *path, char **images, size_t nimages);

#endif /* UTILS_H */

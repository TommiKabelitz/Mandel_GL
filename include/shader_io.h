#include <GLFW/glfw3.h>
#include <stdlib.h>

int read_file(char *filename, char *buffer, size_t buffer_size);

int shader_from_source(char *filename, GLenum shader_type, unsigned int *shader);
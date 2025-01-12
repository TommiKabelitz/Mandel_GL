#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

int read_file(char *filename, char *buffer, size_t buffer_size)
{
    int n_read;

    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("File could not be opened\n");
        return 1;
    }

    n_read = fread(buffer, sizeof(char), buffer_size, fptr);
    if (n_read == buffer_size)
    {
        printf("Buffer could not fit file\n");
        return 1;
    }
    buffer[n_read] = '\0';
    return 0;
}


int shader_from_source(char *filename, GLenum shader_type, unsigned int *shader)
{
    int success;
    char shader_source[4096];

    success = read_file(
        filename,
        shader_source,
        ARRAY_LEN(shader_source));
    if (success != 0)
    {
        return 1;
    }

    // glCreateShader requires a const char for reasons (*shrug*)
    const char *const_source = shader_source;

    char info_log[512];
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &const_source, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(*shader, ARRAY_LEN(info_log), NULL, info_log);
        printf("ERROR:: SHADER COMPILATION FAILED\n");
        printf("%s%s\n", "Shader source:\n", const_source);
        printf("%s\n", info_log);
        return -1;
    }
    return 0;
}

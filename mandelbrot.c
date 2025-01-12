#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <shader_io.h>

#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

const int init_window_width = 1920;
const int init_window_height = 1080;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_escape_key(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

unsigned int get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source)
{
    unsigned int vertex_shader;
    int success;
    char info_log[512];
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, sizeof(info_log) / sizeof(info_log[0]), NULL, info_log);
        printf("ERROR:: VERTEX SHADER COMPILATION FAILED\n");
        printf("%s", info_log);
    }

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, sizeof(info_log) / sizeof(info_log[0]), NULL, info_log);
        printf("ERROR:: FRAGMENT SHADER COMPILATION FAILED\n");
        printf("%s", info_log);
    }

    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, sizeof(info_log) / sizeof(info_log[0]), NULL, info_log);
        printf("ERROR:: ATTACHING SHADERS FAILED\n");
        printf("%s\n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

// Move the centre of the screen towards where the mouse is clicked
// Zoom in/out on left/right click
void handle_cursor_input(GLFWwindow *window, float *centre_x, float *centre_y, float *zoom_factor, float init_x_extent, float init_y_extent)
{
    double cursor_x_pos;
    double cursor_y_pos;
    int left_button_state, right_button_state;
    int width, height;

    left_button_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    right_button_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    // States can only be GLFW_PRESS or GLFW_RELEASE, skip in both cases
    if (left_button_state == right_button_state)
    {
        return;
    }

    glfwGetCursorPos(window, &cursor_x_pos, &cursor_y_pos);
    glfwGetWindowSize(window, &width, &height);

    // In the zoomed coordinate space
    // This is -x0 + x/W*normalisation
    // y has opposing signs as the top left corner is 0,0 for cursor_xy_pos
    float clicked_coord_x = ((*centre_x) - init_x_extent / (*zoom_factor) * 0.5f) + cursor_x_pos / (float)width * init_x_extent / (*zoom_factor);
    float clicked_coord_y = ((*centre_y) + init_y_extent / (*zoom_factor) * 0.5f) - cursor_y_pos / (float)height * init_y_extent / (*zoom_factor);

    float distance_to_new = 0.1f; // Close to 0 => smooth, close to 1 => much less so
    (*centre_x) = (1 - distance_to_new) * (*centre_x) + distance_to_new * clicked_coord_x;
    (*centre_y) = (1 - distance_to_new) * (*centre_y) + distance_to_new * clicked_coord_y;

    if (left_button_state == GLFW_PRESS)
    {
        (*zoom_factor) *= 1.01f;
    }
    else
    {
        (*zoom_factor) *= 0.99f;
    }
}

void draw_set(GLFWwindow *window)
{

    int success;

    char *vertex_shader_filename = "./shaders/simple_vertex_shader.vert\0";
    char *fragment_shader_filename = "./shaders/mandelbrot.frag\0";

    unsigned int vertex_shader, fragment_shader;

    success = shader_from_source(vertex_shader_filename, GL_VERTEX_SHADER, &vertex_shader);
    success = shader_from_source(fragment_shader_filename, GL_FRAGMENT_SHADER, &fragment_shader);
    char info_log[512];

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, sizeof(info_log) / sizeof(info_log[0]), NULL, info_log);
        printf("ERROR:: ATTACHING SHADERS FAILED\n");
        printf("%s\n", info_log);
    }
    glDeleteShader(fragment_shader);

    // Triangle covering full screen
    float vertices[9] = {-1.0f, -1.0f, 0.0f, 3.0f, -1.0f, 0.0f, -1.0f, 3.0f, 0.0f};

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shader_program);

    int u_resolution_loc = glGetUniformLocation(shader_program, "u_resolution");
    if (u_resolution_loc == -1)
    {
        printf("Failed to find a uniform location for u_resolution\n");
        return;
    }

    int window_height, window_width;
    float time;

    int u_centre_loc = glGetUniformLocation(shader_program, "centre");
    if (u_centre_loc == -1)
    {
        printf("Failed to find a uniform location for u_centre\n");
    }

    int u_zoom_factor_loc = glGetUniformLocation(shader_program, "zoom_factor");
    if (u_zoom_factor_loc == -1)
    {
        printf("Failed to find a uniform location for u_zoom_factor\n");
    }

    int u_init_extent_loc = glGetUniformLocation(shader_program, "init_extent");
    if (u_init_extent_loc == -1)
    {
        printf("Failed to find a uniform location for u_init_extent\n");
    }
    // Could use a point struct for a lot of these (x,y) points
    float centre_x = -0.5f;
    float centre_y = 0.0f;
    float zoom_factor = 1.0f;
    float init_x_extent = 3.0f;
    float init_y_extent = 2.0f;
    glUniform2f(u_init_extent_loc, init_x_extent, init_y_extent);

    while (!glfwWindowShouldClose(window))
    {
        process_escape_key(window);

        // Could consider limiting the frame rate here

        // Zoom window based on mouse input
        if (GLFW_HOVERED)
            handle_cursor_input(window, &centre_x, &centre_y, &zoom_factor, init_x_extent, init_y_extent);

        glfwGetWindowSize(window, &window_width, &window_height);
        glUniform2f(u_resolution_loc, (float)window_width, (float)window_height);
        glUniform2f(u_centre_loc, centre_x, centre_y);
        glUniform1f(u_zoom_factor_loc, zoom_factor);
        glDrawArrays(GL_TRIANGLES, 0, 9);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader_program);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(init_window_width, init_window_height, "Mandelbrot", NULL, NULL);
    if (window == NULL)
    {
        printf("The window hates you");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initalize GLAD\n");
        return -1;
    }

    glViewport(0, 0, init_window_width, init_window_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    draw_set(window);

    glfwTerminate();
    return 0;
}

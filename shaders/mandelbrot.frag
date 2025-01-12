#version 460 core

// Could consider trying to implement increased precision version at some point

uniform vec2 u_resolution;
uniform vec2 centre;
uniform float zoom_factor;
uniform vec2 init_extent;
float zoom = zoom_factor;
out vec4 FragColor;

float x_extent = init_extent.x / zoom_factor;
float y_extent = init_extent.y / zoom_factor;

void main()
{
    float xmin = centre.x - x_extent/2.0;
    float xmax = centre.x + x_extent/2.0;
    float ymin = centre.y - y_extent/2.0;
    float ymax = centre.y + y_extent/2.0;

    float max_abs_sq = 2.0 * 2.0;
    float max_iterations = 200.0 * pow(zoom, 0.4);
    float n = 0.0;
    vec2 z = vec2(0.0);
    vec2 c = gl_FragCoord.xy / u_resolution.xy;
    c.x = c.x * x_extent + xmin;
    c.y = c.y * y_extent + ymin;

    for (int i=0; i<max_iterations; i++){
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if (dot(z,z) > (max_abs_sq)) {
            break;
        }
        n += 1.0;
    }


    FragColor = vec4(0.0, pow(n/max_iterations,0.2), 0.0, 1.0);
}





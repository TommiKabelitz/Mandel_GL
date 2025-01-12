# Mandel_GL



Basic code for rendering zoomable Mandelbrot sets using OpenGL with a fragment shader.

The code is functional, but does not include any of the required Glad, GLFW, or OpenGL headers or source.

When running, left click to zoom in, right click to zoom out.

The shader is _currently_ implemented using single precision and quickly shows the limitations of single precision for rendering zoomed Mandelbrot sets. I may consider investigating higher precision either through direct use of doubles or manual float-float precision at a future time.
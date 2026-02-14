#include <glad/glad.h>

#include <X11/Xlib.h>
#include <GL/glx.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const char* shader_source =
"#version 430 core\n"
"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
"layout(std430, binding = 0) buffer my_buffer\n"
"{\n"
"    int data; \n"
"} data;\n"
"\n"
"void main()\n"
"{\n"
"    data.data = 7;\n"
"}\n"
"\n";

void GLAPIENTRY
message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    (void)source; (void)id; (void)length; (void)userParam;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int main(void)
{
    Display* display = XOpenDisplay(NULL);
    assert(display != NULL);
    XVisualInfo* visual_info = XGetVisualInfo(display, 0, NULL, &(int){0});
    GLXContext glx_context = glXCreateContext(display, visual_info, NULL, True);
    assert(glx_context != NULL);
    assert(glXIsDirect(display, glx_context) == True);
    Window root = DefaultRootWindow(display);
    int is_current_context = glXMakeCurrent(display, root, glx_context);
    assert(is_current_context == True);

    int gl_version = gladLoadGL();
    assert(gl_version != 0);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);

    int data = -1;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof data, &data, GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);
    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        char info_log[256];
        glGetShaderInfoLog(shader, sizeof info_log, NULL, info_log);
        puts(info_log);
        exit(EXIT_FAILURE);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glUseProgram(program);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof data, &data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, glx_context);
    XFree(visual_info);
    XCloseDisplay(display);

    printf("Data: %i\n", data);
}

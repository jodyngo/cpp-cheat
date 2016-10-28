#include "common.h"

static const GLuint WIDTH = 500;
static const GLuint HEIGHT = 500;
static const GLchar* vertex_shader_source =
    "#version 120\n"
    "attribute vec2 coord2d;\n"
    "void main(void) {\n"
    "    gl_Position = vec4(coord2d, 0.0, 1.0);\n"
    "}\n";
static const GLchar* fragment_shader_source =
    "#version 120\n"
    "uniform float pi2;\n"
    "uniform float time;\n"
    "uniform float width;\n"
    "uniform float height;\n"
    "uniform float periods_x;\n"
    "uniform float periods_y;\n"
    "void main(void) {\n"
    "    float center_x = width / 2.0;"
    "    float center_y = height / 2.0;"
    "    float x = (gl_FragCoord.x - center_x) * periods_x / width;"
    "    float y = (gl_FragCoord.y - center_y) * periods_y / height;"
    "    gl_FragColor[0] = 0.5 * (1.0 + (sin((pi2 * (sqrt(x*x + y*y) + time)))));\n"
    "    gl_FragColor[1] = 0.0;\n"
    "    gl_FragColor[2] = 0.0;\n"
    "}\n";
static const GLfloat vertices[] = {
    -1.0,  1.0,
     1.0,  1.0,
     1.0, -1.0,
    -1.0, -1.0,
};
static const GLuint indexes[] = {
    0, 2, 1,
    0, 3, 2,
};

int main(void) {
    GLFWwindow *window;
    GLint
        attribute_coord2d,
        ibo_size,
        width_location,
        height_location,
        time_location,
        periods_x_location,
        periods_y_location,
        pi2_location,
        program
    ;
    GLuint ibo, vbo;
    const char *attribute_name = "coord2d";
    const float
        periods_x = 5.0,
        periods_y = 10.0,
        pi2 = 2.0 * acos(-1.0)
    ;

    glfwInit();
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    attribute_coord2d = glGetAttribLocation(program, attribute_name);
    if (attribute_coord2d == -1) {
        fprintf(stderr, "error: attribute_coord2d: %s\n", attribute_name);
        return EXIT_FAILURE;
    }
    height_location = glGetUniformLocation(program, "height");
    periods_x_location = glGetUniformLocation(program, "periods_x");
    periods_y_location = glGetUniformLocation(program, "periods_y");
    pi2_location = glGetUniformLocation(program, "pi2");
    time_location = glGetUniformLocation(program, "time");
    width_location = glGetUniformLocation(program, "width");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glUseProgram(program);
    glViewport(0, 0, WIDTH, HEIGHT);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &ibo_size);

    glUniform1f(pi2_location, pi2);
    glUniform1f(width_location, WIDTH);
    glUniform1f(height_location, HEIGHT);
    glUniform1f(periods_x_location, periods_x);
    glUniform1f(periods_y_location, periods_y);

    common_fps_init();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glEnableVertexAttribArray(attribute_coord2d);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(
            attribute_coord2d,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            0
        );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glUniform1f(time_location, glfwGetTime());
        glDrawElements(GL_TRIANGLES, ibo_size / sizeof(indexes[0]), GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(attribute_coord2d);
        glfwSwapBuffers(window);
        common_fps_print();
    }

    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwTerminate();
    return EXIT_SUCCESS;
}
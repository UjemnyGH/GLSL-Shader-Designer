#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

enum Shape {
    Plane,
    Cube
};

const float gPlaneVertices[] = {
    1.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, -1.0f,
    -1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, -1.0f,
    -1.0f, 0.0f, -1.0f
};

const float gCubeVertices[] = {
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,

    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,

    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f
};

float gProjection[16] = {}, gView[16] = {}, gTransform[16] = {};

int gUsedShape = Plane;
char gVertexShader[1024];
char gFragmentShader[1024];
char gComputeShader[1024];
char gGeometryShader[1024];
char gTessevShader[1024];
char gTessctrlShader[1024];
bool gRefreshPressed = false;

uint32_t LoadShader(const char* path, int type) {
    FILE* f = fopen(path, "rb+");

    fseek(f, 0, SEEK_END);
    uint32_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(len);

    fread(buffer, 1, len, f);

    fclose(f);

    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char* const*)&buffer, nullptr);
    glCompileShader(shader);

    free(buffer);
    
    return shader;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Type --help or -h to get more info!\n");

        return 0;
    }

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf(
                "%s [args...]\n"
                "Available arguments:\n"
                "\t--help                   | -h            -\tShow this prompt\n"
                "\t--shape <shape>          | -s <shape>    -\tChange shape (cube or plane)\n"
                "\t--vertex <path>          | -v <path>     -\tSet used vertex shader\n"
                "\t--fragment <path>        | -f <path>     -\tSet used fragment shader\n"
                "\t--compute <path>         | -c <path>     -\tSet used compute shader\n"
                "\t--tess_evaluation <path> | -te <path>    -\tSet used tesselation evaluation shader\n"
                "\t--tess_control <path>    | -tc <path>    -\tSet used tesselation control shader\n"

                , argv[0]
            );

            return 0;
        }
        else if(strcmp(argv[i], "--shape") == 0 || strcmp(argv[i], "-s") == 0) {
            if(strcmp(argv[i + 1], "cube") == 0) {
                gUsedShape = Cube;
            }
            else {
                gUsedShape = Plane;
            }
        }
        else if(strcmp(argv[i], "--vertex") == 0 || strcmp(argv[i], "-v") == 0) {
            strcpy(gVertexShader, argv[i + 1]);
        }
        else if(strcmp(argv[i], "--fragment") == 0 || strcmp(argv[i], "-f") == 0) {
            strcpy(gFragmentShader, argv[i + 1]);
        }
        else if(strcmp(argv[i], "--compute") == 0 || strcmp(argv[i], "-c") == 0) {
            strcpy(gComputeShader, argv[i + 1]);
        }
        else if(strcmp(argv[i], "--geometry") == 0 || strcmp(argv[i], "-g") == 0) {
            strcpy(gGeometryShader, argv[i + 1]);
        }
        else if(strcmp(argv[i], "--tess_evaluation") == 0 || strcmp(argv[i], "-te") == 0) {
            strcpy(gTessevShader, argv[i + 1]);
        }
        else if(strcmp(argv[i], "--tess_control") == 0 || strcmp(argv[i], "-tc") == 0) {
            strcpy(gTessctrlShader, argv[i + 1]);
        }
        /*else if(strcmp(argv[i], "--texture") == 0 || strcmp(argv[i], "-t") == 0) {

        }*/
    }

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLSL Shader Designer", nullptr, nullptr);

    if(!window) {
        perror("Cannot create window!");

        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        perror("Cannot load OpenGL 4.5 core context!");

        return -2;
    }

    glEnable(GL_DEPTH_TEST);

    uint32_t vs = 0, fs = 0, gs = 0, cs = 0, tes = 0, tcs = 0;
    uint32_t vao, vbo, sh;

    sh = glCreateProgram();

    if(gVertexShader[0] != 0) {
        vs = LoadShader(gVertexShader, GL_VERTEX_SHADER);
        glAttachShader(sh, vs);
    }

    if(gFragmentShader[0] != 0) {
        fs = LoadShader(gFragmentShader, GL_FRAGMENT_SHADER);
        glAttachShader(sh, fs);
    }

    if(gComputeShader[0] != 0) {
        cs = LoadShader(gComputeShader, GL_COMPUTE_SHADER);
        glAttachShader(sh, cs);
    }

    if(gGeometryShader[0] != 0) {
        gs = LoadShader(gGeometryShader, GL_GEOMETRY_SHADER);
        glAttachShader(sh, gs);
    }

    if(gTessevShader[0] != 0) {
        tes = LoadShader(gTessevShader, GL_TESS_EVALUATION_SHADER);
        glAttachShader(sh, tes);
    }

    if(gTessctrlShader[0] != 0) {
        tcs = LoadShader(gTessctrlShader, GL_TESS_CONTROL_SHADER);
        glAttachShader(sh, tcs);
    }

    glLinkProgram(sh);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    if(gUsedShape == Plane) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(gPlaneVertices), gPlaneVertices, GL_DYNAMIC_DRAW);
    }
    else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertices), gCubeVertices, GL_DYNAMIC_DRAW);
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    /*
    Math behind gView
    
    za:
    0.0 0.0 0.0 - 0.0 0.0 1.0 = 
    0.0 0.0 -1.0
    
    xa:
    0.0 1.0 0.0 x 0.0 0.0 -1.0 = 
    -1.0 0.0 0.0
    
    ya:
    -1.0 0.0 0.0 x 0.0 0.0 -1.0 =
    0.0 -1.0 0.0
    */
    gView[0] = -1.0f;
    gView[1] = 0.0f;
    gView[2] = 0.0f;
    gView[3] = 0.0f;
    gView[4] = 0.0f;
    gView[5] = -1.0f;
    gView[6] = 0.0f;
    gView[7] = 0.0f;
    gView[8] = 0.0f;
    gView[9] = 0.0f;
    gView[10] = -1.0f;
    gView[11] = 0.0f;
    gView[12] = 0.0f;
    gView[13] = 0.0f;
    gView[14] = 1.0f;
    gView[15] = 1.0f;

    float c = 0.0f, l = 0.0f, d = 0.0f;
    double mx, my;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        c = glfwGetTime();
        d = c - l;
        l = c;

        int w = 0, h = 0;
        glfwGetWindowSize(window, &w, &h);
        glViewport(0, 0, w, h);
        
        const float fov = 1.0f / tan(((3.1415f / 180.0f) * 90.0f) / 2.0f);
        gProjection[0] = fov * ((float)w / (float)h);
        gProjection[5] = fov;
        gProjection[10] = -(100.0f + 0.001f) / (100.0f - 0.001f);
        gProjection[11] = -1.0f;
        gProjection[14] = -(2.0f * 100.0f * 0.001f) / (100.0f - 0.001f);
        gProjection[15] = 1.0f;

        glUseProgram(sh);
        glBindVertexArray(vao);

        glUniform1f(glGetUniformLocation(sh, "uTime"), c);
        glUniform1f(glGetUniformLocation(sh, "uDeltaTime"), d);
        glUniformMatrix4fv(glGetUniformLocation(sh, "uProjection"), 1, 0, gProjection);
        glUniformMatrix4fv(glGetUniformLocation(sh, "uView"), 1, 0, gView);
        glUniformMatrix4fv(glGetUniformLocation(sh, "uTransform"), 1, 0, gTransform);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);

        glfwPollEvents();

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glfwGetCursorPos(window, &mx, &my);

            /*
            yaw \/
            cos a, -sin a, 0
            sin a, cos a, 0
            0, 0, 1

            pitch >
            cos b, 0, sin b,
            0, 1, 0
            -sin b, 0, cos b

            cosa * cos b, -sin a * cosb, cos b
            sin a, cos a, 0
            -sin b * cos a, -sin a * -sin b + cos b, cos b
            */

            gTransform[0] = cos(mx) * cos(my);
            gTransform[1] = -sin(mx);
            gTransform[2] = sin(mx);
            gTransform[4] = sin(mx) * cos(my);
            gTransform[5] = cos(mx);
            gTransform[6] = -sin(mx);
            gTransform[8] = -sin(my);
            gTransform[9] = sin(mx) * cos(my);
            gTransform[10] = cos(mx) * cos(my);
            gTransform[15] = 1.0f;

        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !gRefreshPressed) {
            gRefreshPressed = true;

            glDeleteProgram(sh);
            sh = glCreateProgram();

            if(gVertexShader[0] != 0) {
                glDeleteShader(vs);
                vs = LoadShader(gVertexShader, GL_VERTEX_SHADER);
                glAttachShader(sh, vs);
            }

            if(gFragmentShader[0] != 0) {
                glDeleteShader(fs);
                fs = LoadShader(gFragmentShader, GL_FRAGMENT_SHADER);
                glAttachShader(sh, fs);
            }

            if(gComputeShader[0] != 0) {
                glDeleteShader(cs);
                cs = LoadShader(gComputeShader, GL_COMPUTE_SHADER);
                glAttachShader(sh, cs);
            }

            if(gGeometryShader[0] != 0) {
                glDeleteShader(gs);
                gs = LoadShader(gGeometryShader, GL_GEOMETRY_SHADER);
                glAttachShader(sh, gs);
            }

            if(gTessevShader[0] != 0) {
                glDeleteShader(tes);
                tes = LoadShader(gTessevShader, GL_TESS_EVALUATION_SHADER);
                glAttachShader(sh, tes);
            }

            if(gTessctrlShader[0] != 0) {
                glDeleteShader(tcs);
                tcs = LoadShader(gTessctrlShader, GL_TESS_CONTROL_SHADER);
                glAttachShader(sh, tcs);
            }

            glLinkProgram(sh);
        }
        else if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && gRefreshPressed) {
            gRefreshPressed = false;
        }

        glfwSwapInterval(0);
    }

    glfwTerminate();

    return 0;
}
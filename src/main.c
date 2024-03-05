/**
 * @file main.c
 * @author Piotr UjemnyGH Plombon
 * @brief GLSL Shader designer
 * @version 0.1
 * @date 2024-03-05
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 * NOTE THAT ENTIRE APP USES C23 STANDARD!!!
 * 
 * For linux install just glfw in your packet manager
 * For windows just run .exe file
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "math3d.h"
#include "meshes.h"

mat4_t gProj, /*gView,*/ gTrans;

// Global variables, cuz why not
int gUsedShape = Plane;
char gVertexShader[1024];
char gFragmentShader[1024];
char gComputeShader[1024];
char gGeometryShader[1024];
char gTessevShader[1024];
char gTessctrlShader[1024];
bool gRefreshPressed = false;

float gScale = 0.1f;
float gMultiplyBy = 1.0f;

/**
 * @brief Loads shaders from file
 * 
 * @param path path to file
 * @param type shader type
 * @return uint32_t 
 */
uint32_t LoadShader(const char* path, int type) {
    // Open folder
    FILE* f = fopen(path, "rb+");

    // Get length
    fseek(f, 0, SEEK_END);
    uint32_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Alloc some memory for source code
    char *buffer = malloc(len + 1);

    // Read file
    fread(buffer, 1, len, f);
    buffer[len] = '\0';

    // Close file
    fclose(f);

    // Make shader
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char* const*)&buffer, nullptr);
    glCompileShader(shader);

    // Check for any errors and display then if they exist
    int isCompiled = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if(!isCompiled) {
        int maxLength = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = (char*)malloc(maxLength);

        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        printf("[INFO]: Shader error <%s>: %s\n", path, infoLog);

        free(infoLog);
        infoLog = nullptr;
    }

    // Free allocated memory, to everyone reading this commenst: DO NOT FORGET TO DO THAT, IT COUSES MANY ERRORS
    free(buffer);
    
    // Return out compiled shader
    return shader;
}

/**
 * @brief Clamp function
 * 
 * @param val clamped value
 * @param max max value
 * @param min min value
 * @return float 
 */
float clamp(float val, float max, float min) {
    return val > max ? max : (val < min ? min : val);
}

/**
 * @brief Framebuffer (Window size) callback used to resize viewport
 * 
 * @param pWnd 
 * @param width 
 * @param height 
 */
void framebufferCallback(GLFWwindow* pWnd, int width, int height) {
    // Error deleter
    pWnd = pWnd;

    // Set new vievport
    glViewport(0, 0, width, height);
    
    // Set perspective
    gProj = MX4PerspectiveFOV((3.14159265359 / 180.0) * 90.0f, (real_t)width, (real_t)height, 0.001f, 30.0f * gMultiplyBy);
}

/**
 * @brief Mouse wheel/scroll callback
 * 
 * @param pWnd 
 * @param x 
 * @param y 
 */
void scrollCallback(GLFWwindow* pWnd, double x, double y) {
    // Error deleter
    pWnd = pWnd;
    // We don`t need x so that is just error deleter
    x = x;

    // Scroll up = 1.0f, Scroll down = -1.0f
    if(y > 0.0f) {
        gScale += 0.005f;
    }
    else if(y < 0.0f) {
        gScale -= 0.005f;
    }

    // Clamp calculated scale for convinience
    gScale = clamp(gScale, 10.0f * gMultiplyBy, 0.025f);
}

int main(int argc, char **argv) {
    // Check for desired amount of arguments, id amount of args don`t satisfy, return 0 and let user know that he must study how to use program
    if(argc < 2) {
        printf("Type --help or -h to get more info!\n");

        return 0;
    }

    // Find desired arguments specified by user
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf(
                "%s [args...]\n"
                "Available arguments:\n"
                "\t--help                   | -h            -\tShow this prompt\n"
                "\t--shape <shape>          | -s <shape>    -\tChange shape (cube, plane or plane10x10)\n"
                "\t--vertex <path>          | -v <path>     -\tSet used vertex shader\n"
                "\t--fragment <path>        | -f <path>     -\tSet used fragment shader\n"
                "\t--compute <path>         | -c <path>     -\tSet used compute shader\n"
                "\t--tess_evaluation <path> | -te <path>    -\tSet used tesselation evaluation shader\n"
                "\t--tess_control <path>    | -tc <path>    -\tSet used tesselation control shader\n"
                "\t--multiply_by <number>   | -mb <number>  -\tMultiply all values by this number\n"

                , argv[0]
            );

            return 0;
        }
        else if(strcmp(argv[i], "--shape") == 0 || strcmp(argv[i], "-s") == 0) {
            if(strcmp(argv[i + 1], "cube") == 0) {
                gUsedShape = Cube;
            }
            else if(strcmp(argv[i + 1], "plane10x10") == 0) {
                gUsedShape = Plane10;
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
        else if(strcmp(argv[i], "--multiply_by") == 0 || strcmp(argv[i], "-mb") == 0) {
            gMultiplyBy = atof(argv[i + 1]);
        }
        // Currently textures are non-existant
        /*else if(strcmp(argv[i], "--texture") == 0 || strcmp(argv[i], "-t") == 0) {

        }*/
    }

    // Multiply every value in vertices by multiplayer set by user (it doesn`t take long so we can just multiply it even if user doesn`t specified multiplayer) 
    for(uint64_t i = 0; i < sizeof(gPlaneVertices) / sizeof(float); i++) {
        gPlaneVertices[i] *= gMultiplyBy;
    }

    for(uint64_t i = 0; i < sizeof(gPlane10Vertices) / sizeof(float); i++) {
        gPlane10Vertices[i] *= gMultiplyBy;
    }

    for(uint64_t i = 0; i < sizeof(gCubeVertices) / sizeof(float); i++) {
        gCubeVertices[i] *= gMultiplyBy;
    }

    // Info user how to use program quicker from window
    printf("R - reaload\n1 - Plane\n2 - Plane 10x10\n3 - Cube\nScroll - Object scale\nMouse button 1 - Rotate object\n");

    // Initialize glfw
    glfwInit();

    // Set context verion of opengl to 4.5 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Set multisampling to 16 samples per pixel
    glfwWindowHint(GLFW_SAMPLES, 16);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLSL Shader Designer", nullptr, nullptr);

    // Check if we have window
    if(!window) {
        perror("Cannot create window!");

        return -1;
    }

    // If window exist make it current context
    glfwMakeContextCurrent(window);

    // Load OpenGL 4.5 core context
    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        perror("Cannot load OpenGL 4.5 core context!");

        return -2;
    }

    // Before setting callback set values to update perspective
    framebufferCallback(window, 800, 600);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    uint32_t vs = 0, fs = 0, gs = 0, cs = 0, tes = 0, tcs = 0;
    uint32_t vao, vbo, sh;

    // Create shader program
    sh = glCreateProgram();

    // Load shader if they are specified and attach them to shader program
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

    // lInk shader program
    glLinkProgram(sh);

    // Gen array and buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Bind desired start data
    if(gUsedShape == Plane) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(gPlaneVertices), gPlaneVertices, GL_DYNAMIC_DRAW);
    }
    else if(gUsedShape == Plane10) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(gPlane10Vertices), gPlane10Vertices, GL_DYNAMIC_DRAW);
    }
    else {
        glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertices), gCubeVertices, GL_DYNAMIC_DRAW);
    }

    // Enable layout attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Basicly don`t work
    //gView = MX4LookAt((vec4_t){0.0f, 0.0f, -4.0f, 0.0f}, (vec4_t){0.0f, 0.0f, 0.0f, 0.0f}, (vec4_t){0.0f, 1.0f, 0.0f, 0.0f});

    // Time and mouse movement
    float c = 0.0f, l = 0.0f, d = 0.0f;
    double mx = 0.0, my = 0.0f;

    // Main loop
    while(!glfwWindowShouldClose(window)) {
        // Clear screen and set bg color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Check if user desire other model
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            gUsedShape = Plane;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(gPlaneVertices), gPlaneVertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
        else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            gUsedShape = Plane10;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(gPlane10Vertices), gPlane10Vertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
        else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            gUsedShape = Cube;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertices), gCubeVertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }

        // Refresh
        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !gRefreshPressed) {
            gRefreshPressed = true;

            // Show previous info
            printf("\nR - reaload\n1 - Plane\n2 - Plane 10x10\n3 - Cube\nScroll - Object scale\nMouse button 1 - Rotate object\n");

            // Delete shader program and create new one
            glDeleteProgram(sh);
            sh = glCreateProgram();

            // Check user specified shaders, compile them, check for errors and return rebuild info 
            if(gVertexShader[0] != 0) {
                glDeleteShader(vs);
                vs = LoadShader(gVertexShader, GL_VERTEX_SHADER);
                glAttachShader(sh, vs);

                printf("[INFO]: Rebuilded %s\n", gVertexShader);
            }

            if(gFragmentShader[0] != 0) {
                glDeleteShader(fs);
                fs = LoadShader(gFragmentShader, GL_FRAGMENT_SHADER);
                glAttachShader(sh, fs);

                printf("[INFO]: Rebuilded %s\n", gFragmentShader);
            }

            if(gComputeShader[0] != 0) {
                glDeleteShader(cs);
                cs = LoadShader(gComputeShader, GL_COMPUTE_SHADER);
                glAttachShader(sh, cs);

                printf("[INFO]: Rebuilded %s\n", gComputeShader);
            }

            if(gGeometryShader[0] != 0) {
                glDeleteShader(gs);
                gs = LoadShader(gGeometryShader, GL_GEOMETRY_SHADER);
                glAttachShader(sh, gs);

                printf("[INFO]: Rebuilded %s\n", gGeometryShader);
            }

            if(gTessevShader[0] != 0) {
                glDeleteShader(tes);
                tes = LoadShader(gTessevShader, GL_TESS_EVALUATION_SHADER);
                glAttachShader(sh, tes);

                printf("[INFO]: Rebuilded %s\n", gTessevShader);
            }

            if(gTessctrlShader[0] != 0) {
                glDeleteShader(tcs);
                tcs = LoadShader(gTessctrlShader, GL_TESS_CONTROL_SHADER);
                glAttachShader(sh, tcs);

                printf("[INFO]: Rebuilded %s\n", gTessctrlShader);
            }

            // Link shader program
            glLinkProgram(sh);

            // Check another time for errors
            int isLinked = 0;

            glGetProgramiv(sh, GL_LINK_STATUS, &isLinked);

            if(!isLinked) {
                int maxLength = 0;

                glGetProgramiv(sh, GL_INFO_LOG_LENGTH, &maxLength);

                char* infoLog = (char*)malloc(maxLength);
                glGetProgramInfoLog(sh, maxLength, &maxLength, &infoLog[0]);

                printf("[INFO]: Link error: %s\n", infoLog);

                free(infoLog);

                infoLog = nullptr;
            }
        }
        else if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && gRefreshPressed) {
            // Set flag
            gRefreshPressed = false;
        }

        // Calculate  delta time
        c = glfwGetTime();
        d = c - l;
        l = c;

        // Set uniforms and draw
        glUseProgram(sh);
        glBindVertexArray(vao);

        glUniform1f(glGetUniformLocation(sh, "uTime"), c);
        glUniform1f(glGetUniformLocation(sh, "uDeltaTime"), d);
        glUniformMatrix4fv(glGetUniformLocation(sh, "uProjection"), 1, 0, gProj.m);
        // Here is mat4(1.0) becouse currently gView doesn`t work 
        glUniformMatrix4fv(glGetUniformLocation(sh, "uView"), 1, 0, /*gView.m*/MX4One().m);
        glUniformMatrix4fv(glGetUniformLocation(sh, "uTransform"), 1, 0, gTrans.m);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(gPlane10Vertices) / 3);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);

        glfwPollEvents();

        // Check for mouse button to rotate object and hide cursor
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glfwGetCursorPos(window, &mx, &my);

            mx /= 800;
            my /= 800;

        }
        else {
            // Show cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // Calculate object transform
        gTrans = MX4MulMX4(MX4MulMX4(MX4MulMX4(MX4RotateX(-my), MX4RotateY(-mx)), MX4RotateZ(0.0f)), MX4Scale((vec4_t){gScale, gScale, gScale, 1.0f}));

        // Disable vertical sync
        glfwSwapInterval(0);
    }

    glfwTerminate();

    return 0;
}
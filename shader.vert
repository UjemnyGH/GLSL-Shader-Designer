#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uTransform;

uniform float uTime;
uniform float uDeltaTime;

layout(location = 0) in vec4 iPos;

void main() {
    gl_Position = uProjection * uView * uTransform * iPos;
}
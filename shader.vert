#version 450 core

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uTransform;

uniform float uTime;
uniform float uDeltaTime;

layout(location = 0) in vec4 iPos;

out vec4 vPos;

void main() {
    vec4 p = iPos + vec4(0.0, (sin(uTime + iPos.x) + sin(uTime + iPos.x * 2.4) + sin(uTime + iPos.z * 1.7) + sin(uTime + iPos.z * 2.3)) / 4.0, 0.0, 0.0);

    vPos = p;

    gl_Position = uProjection * uTransform * p;
}
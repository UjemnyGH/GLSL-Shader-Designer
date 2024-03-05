#version 450 core

out vec4 oCol;
in vec4 vPos;

void main() {
    oCol = vec4(abs(vPos.y) - 0.1, abs(vPos.y) - 0.1, 1.0, 1.0);
}
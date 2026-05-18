#version 300 es
precision mediump float;

in vec2 vTex;
in vec3 vNormal;
in vec4 vColor;

uniform sampler2D uTexture0;
uniform float uAlphaCutoff; // 0.0 - alphaText off
uniform bool uHasTexture;

out vec4 FragColor;

void main() {
    vec4 color = vColor;

    if (uHasTexture) {
        color *= texture(uTexture0, vTex);
    }

    if (uAlphaCutoff > 0.0 && color.a < uAlphaCutoff) {
        discard;
    }

    FragColor = color;
}

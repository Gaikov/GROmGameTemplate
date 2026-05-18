#version 300 es

layout(location = 0) in vec3 aPosition;   // position
layout(location = 1) in vec3 aNormal;     // normal
layout(location = 2) in vec4 aColor;      // color (rgba8 normalized by GL)
layout(location = 3) in vec2 aTexCoord;   // texcoord

uniform mat4 uProjView;   // ProjectView
uniform mat4 uModel;      // Model matrix for correct normal transform
uniform mat4 uTexMat;     // Texture matrix

out highp vec2 vTex;
out mediump vec3 vNormal;
out lowp vec4 vColor;

void main() {
    gl_Position = uProjView * uModel * vec4(aPosition, 1.0);

    // normalize normales with scale
    // mat3 normalMat = transpose(inverse(mat3(uModel)));
    // vNormal = normalize(normalMat * aNormal);

    vNormal = aNormal;
    vTex    = (uTexMat * vec4(aTexCoord, 1, 1)).xy;
    vColor  = aColor;
}
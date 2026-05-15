#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 aPosition [[attribute(0)]];
    float3 aNormal   [[attribute(1)]];
    float4 aColor    [[attribute(2)]];
    float2 aTexCoord [[attribute(3)]];
};

struct VertexOut {
    float4 pos      [[position]];
    float3 vNormal;
    float4 vColor;
    float2 vTexCoord;
};

struct Uniforms {
    float4x4 uProjView;
    float4x4 uModel;
    float4x4 uTexMat;
    float   uAlphaCutoff;
    int     uHasTexture;
};

vertex VertexOut vertexMain(VertexIn in        [[stage_in]],
                            constant Uniforms &u [[buffer(1)]])
{
    VertexOut out;
    out.pos = u.uProjView * u.uModel * float4(in.aPosition, 1.0);
    out.vNormal = in.aNormal;
    out.vColor = in.aColor;
    out.vTexCoord = in.aTexCoord;
    return out;
}

fragment float4 pixelMain(VertexOut in       [[stage_in]],
                           constant Uniforms &u [[buffer(1)]],
                           texture2d<float> texture [[texture(0)]],
                           sampler texSampler  [[sampler(0)]])
{
    float4 baseColor = in.vColor;

    if (u.uHasTexture != 0) {
        float2 texCoord = (u.uTexMat * float4(in.vTexCoord, 0.0, 1.0)).xy;
        baseColor *= texture.sample(texSampler, texCoord);
    }

    if (baseColor.a < u.uAlphaCutoff) {
        discard_fragment();
    }

    return baseColor;
}

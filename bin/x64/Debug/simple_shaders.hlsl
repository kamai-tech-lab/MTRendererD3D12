cbuffer SceneConstantBuffer : register(b0) {
    float4x4 viewMtx;
    float4x4 projMtx;
    float4x4 worldMtx[512];
};

// Vertex shader inputs
struct VSInput {
    float3 Position : POSITION;
    float4 Color    : COLOR;
    uint InstanceID : SV_InstanceID;
};

// Vertex shader outputs
struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

#define PSInput     VSOutput

// Pixel shader outputs
struct PSOutput
{
    float4 color0 : SV_TARGET;
};

// Vertex shader
PSInput VSMain(in VSInput vsInput)
{
    VSOutput vsOut = (VSOutput)0;

    vsOut.position = mul(mul(mul(float4(vsInput.Position, 1.0f), worldMtx[vsInput.InstanceID]), viewMtx), projMtx);
    vsOut.color    = vsInput.Color;

    return vsOut;
}

// Pixel shader
PSOutput PSMain(in PSInput psInput)
{
    PSOutput psOut = (PSOutput)0;

    psOut.color0 = psInput.color;

    return psOut;
}

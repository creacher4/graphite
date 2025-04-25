cbuffer PerFrameData : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
}

cbuffer PerObjectData : register(b1)
{
    matrix worldMatrix;
}

struct VS_INPUT {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    float4 worldPos = mul(worldMatrix, float4(input.position, 1.0f));
    float4 viewPos = mul(viewMatrix, worldPos);
    output.position = mul(projectionMatrix, viewPos);
    output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
    output.texCoord = input.texCoord;
    return output;
}

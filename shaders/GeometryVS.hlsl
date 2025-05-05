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
    float3 tangent  : TANGENT;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 bitangent: BITANGENT;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    float4 worldPos = mul(worldMatrix, float4(input.position, 1.0f));
    float4 viewPos = mul(viewMatrix, worldPos);
    output.position = mul(projectionMatrix, viewPos);

    // i would use the inverse transpose of the world matrix here, but my inverse function doesn't work

    output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
    output.texCoord = input.texCoord;

    float3 T = normalize(mul(input.tangent, (float3x3)worldMatrix));
    output.tangent = T;

    float3 N = normalize(mul(input.normal, (float3x3)worldMatrix));
    output.bitangent = normalize(cross(N, T));
    return output;
}

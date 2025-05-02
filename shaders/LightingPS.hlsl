Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D ormTex : register(t2);
Texture2D depthTex : register(t3);
SamplerState sampl : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 lightDir; 
    float pad0;
    float3 lightColor; 
    float pad1;
};
struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.texCoord;
    float4 albedo = albedoTex.Sample(sampl, uv);
    float3 N = normalTex.Sample(sampl, uv).xyz * 2 - 1; // convert from [0, 1] to [-1, 1]
    N = normalize(N);
    float3 L = normalize(-lightDir);
    float3 NdotL = max(dot(N, L), 0);
    float3 diffuse = albedo.rgb * lightColor * NdotL;
    return float4(diffuse, albedo.a);

}

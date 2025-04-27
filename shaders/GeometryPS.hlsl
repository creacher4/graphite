Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D ormTex    : register(t2);
SamplerState samplerState : register(s0);

struct PS_INPUT {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 bitangent: BITANGENT;
};

struct PS_OUTPUT {
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 ORM    : SV_Target2;
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;

    // build tbn
    float3 nm = normalTex.Sample(samplerState, input.texCoord).xyz * 2.0f - 1.0f;
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);
    float3 wN = normalize(mul(nm, float3x3(T, B, N)));

    output.Albedo = albedoTex.Sample(samplerState, input.texCoord);
    output.Normal = float4(wN * 0.5f + 0.5f, 1.0f);
    output.ORM = ormTex.Sample(samplerState, input.texCoord);
    return output;
}
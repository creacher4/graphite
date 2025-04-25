struct PS_INPUT {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 ORM    : SV_Target2;
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;
    output.Albedo = float4(input.texCoord.x, input.texCoord.y, 1.0f - input.texCoord.x, 1.0f);
    output.Normal = float4(normalize(input.normal) * 0.5f + 0.5f, 1.0f);
    output.ORM = float4(1.0f, 0.7f, 0.1f, 1.0f);
    return output;
}
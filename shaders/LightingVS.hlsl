struct VS_OUTPUT {
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main(uint id : SV_VertexID) {
    // full screen quad
    float2 pos[3] = {
        float2(-1, -1),
        float2(-1, 3),
        float2(3, -1)
    };
    VS_OUTPUT output;
    output.position = float4(pos[id], 0, 1);
    output.texCoord = pos[id] * 0.5 + 0.5; // convert from [-1, 1] to [0, 1]
    return output;
}
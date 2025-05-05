Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D ormTex : register(t2);
Texture2D depthTex : register(t3);
SamplerState sampl : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 lightDir;    float pad0;
    float3 lightColor;  int useAlbedo;
    int useNormals;     int useAO;
    int pad1;           int pad2;
    float3 viewDir;     int useRoughness;
    int useMetallic;  int useFresnel;
    int useRim;       int pad3;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.texCoord;

    float3 albedo = albedoTex.Sample(sampl, uv).rgb;

    // neutral white surface
    if (useAlbedo == 0) albedo = float3(1,1,1);


    // decoding from [0, 1] to [-1, 1]
    // technique to convert normal maps from texture space to world space
    float3 N = normalize(normalTex.Sample(sampl, uv).xyz * 2.0 - 1.0);

    // light and view vectors
    float3 L = normalize(-lightDir);
    float3 V = normalize(viewDir);
    float3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0);

    // disables normals-based lighting; surfaces are lit as if they were flat
    if (useNormals == 0) NdotL = 1;


    // sample orm maps
    float ao = ormTex.Sample(sampl, uv).r;
    float roughness = ormTex.Sample(sampl, uv).g;
    float metallic = ormTex.Sample(sampl, uv).b;

    // ao modulates indirect lighting by darkening crevices and occluded areas
    // it's used as a scalar multiplier for diffuse and ambient terms, so a value of 1 means no occlusion
    // while 0 cancels the diffuse term completely
    // when ao is disabled, we set it to 1 to avoid zeroing out the lighting
    // using 0 here would erase the diffuse term completely, which is not what we want

    if (useAO == 0) ao = 1;

    // roughness controls the spread of specular highlights
    // typically used to interpolate sharpess in lighting models (e.g. shininess or specular blurring)
    // setting roughness to 0 disables the roughness effect by simulating a perfectly smooth surface
    // essentially, acting as a neutral baseline for testing lighting without roughness scattering

    if (useRoughness == 0) roughness = 0;

    // metallic controlsl whether a surface reflects light like a metal or a dielectric
    // a value of 0 here means the surface reflects like plastic, wood, etc.
    // while 1 means it reflects like a metal, with colored reflections and no diffuse component

    if (useMetallic == 0) metallic = 0;
    

    // apply ao to diffuse lighting
    float3 diffuse = albedo * lightColor * NdotL * ao;


    // fresnel specular via Schlick approximation
    float shininess = lerp(4.0f, 64.0f, 1.0f - roughness);

    // where f0 is the reflectace at normal incidence
    // where metal reflects base color, dielectric reflects around 4%
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    // fresnel term using Schlick approximation
    // approximates how reflection increases at glancing angles
    // see: https://learnopengl.com/PBR/Theory
    // see references to other specular models: https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    float VoH = saturate(dot(V, H));
    float3 fresnel = F0 + (1 - F0) * pow(1 - VoH, 5);
    if (useFresnel == 0) fresnel = float3(0.0, 0.0, 0.0); // disables fresnel term

    float NdotH = saturate(dot(N, H));
    float specStrength = pow(NdotH, shininess);
    float3 specular = fresnel * specStrength * NdotL;


    // rimm lighting adds a subtle outline effect around object edges facing away from the light
    // achieved by measuring how perpendicular the normal is to the view direction
    // visually resembles a highlight on the rim of a surface - usually used for stylistic effects
    float rimFactor = pow(1 - saturate(dot(N, V)), 3);
    // rim light can easily be overdone, so it's kept at a low intensity
    // it's also multiplied by the albedo color to match the color of the surface
    float3 rimLight = rimFactor * 0.15f * albedo;
    if (useRim == 0) rimLight = 0; // disables rim light

    
    // final color, combining all lighting terms
    float3 result = diffuse + specular + rimLight;


    // optional tone mapping and what not
    // result = result / (result + 1.0);
    // result = pow(result, 1.0 / 2.2);


    return float4(result, 1.0f);
}

struct VS_OUTPUT
{
  float4 Pos        : POSITION;
  float2 UV         : TEXCOORD0;
  float3 LightDir   : TEXCOORD1;
  float3 HalfVector : TEXCOORD2;
  float3 Normal     : TEXCOORD3;
};

VS_OUTPUT main_vs(
    in float4 Pos : POSITION,
    in float2 Tex : TEXCOORD0,
    in float3 Normal : NORMAL,
    uniform float4x4 worldViewProj_matrix,
    uniform float4 lightPosition,
    uniform float4 eyePosition)
{ 
    VS_OUTPUT Out;

    float3 lightDir = lightPosition - Pos;
    float3 eyeDir = eyePosition - Pos;

    Out.Pos = mul(worldViewProj_matrix, Pos);
    Out.UV = Tex.xy;
    Out.LightDir = lightDir;
    Out.HalfVector = eyeDir + lightDir;
	Out.Normal = Normal;

    return Out;
}

float4 main_ps (
    float2 UV         : TEXCOORD0,
    float3 LightDir   : TEXCOORD1,
    float3 HalfVector : TEXCOORD2,
    float3 Normal     : TEXCOORD3,
	uniform sampler2D diffuseMap,
    uniform sampler2D specMap) : COLOR
{
    float4 texColour = tex2D(diffuseMap, UV);
    float specularLevel = tex2D(specMap, UV).r;

    // The ambient term will always be present
    float4 color = float4(0.2, 0.2, 0.2, 1.0)*texColour;

    // a fragment shader can't write a varying variable, hence we need
    // a new variable to store the normalized interpolated normal
    float3 n = normalize(Normal);

    // compute the dot product between normal and ldir
    float NdotL = max(dot(n, LightDir), 0.0);

    if (NdotL > 0.0)
    {
        color += texColour * float4(0.7, 0.7, 0.7, 1.0) * NdotL;
        float3 halfV = normalize(HalfVector);
        float NdotHV = max(dot(n,halfV),0.0);
        color += float4(0.9, 0.9, 0.9, 1.0)*specularLevel*pow(NdotHV, 42.0);
    }

    return color;
}


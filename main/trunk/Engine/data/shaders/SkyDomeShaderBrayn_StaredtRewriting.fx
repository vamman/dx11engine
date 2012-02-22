struct Light
{
	float3 lightDirection; 
	float4 ambientColor;
	float4 diffuseColor; 
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	// float4x4 WVP;
	// float4x4 worldMatrix;
	matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

Texture2D shaderTexture; 
SamplerState SampleType; 
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	// output.position = mul(inPos, WVP);
	
	output.position = mul(inPos, worldMatrix);
    output.position = mul(inPos, viewMatrix);
    output.position = mul(inPos, projectionMatrix);
	
	// Calculate the normal vector against the world matrix only.
	output.normal = mul(normal, (float3x3)worldMatrix);

	output.tex = inTexCoord;

	return output;
}

SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	// output.position = mul(float4(inPos, 1.0f), WVP).xyww;
	
	output.position = mul(float4(inPos, 1.0f), worldMatrix).xyww;
	output.position = mul(float4(inPos, 1.0f), viewMatrix).xyww;
	output.position = mul(float4(inPos, 1.0f), projectionMatrix).xyww;

	output.tex = inPos;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuseColor = shaderTexture.Sample( SampleType, input.tex );

	float3 finalColor;

	finalColor = diffuseColor * light.ambientColor;
	finalColor += saturate(dot(light.lightDirection, input.normal) * light.diffuseColor * diffuseColor);

	return float4(finalColor, diffuseColor.a);
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(SampleType, input.tex);
}

float4 D2D_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuseColor = shaderTexture.Sample( SampleType, input.tex );

	return diffuseColor;
}
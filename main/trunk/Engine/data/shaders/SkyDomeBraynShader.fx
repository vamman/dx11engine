cbuffer cbPerObject
{
	// matrix WVP;
	matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD;
};

SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set position to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	// output.position = mul(float4(inPos, 1.0f), WVP).xyww;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(inPos, 1.0f), worldMatrix).xyww;
	output.position = mul(float4(inPos, 1.0f), viewMatrix).xyww;
	output.position = mul(float4(inPos, 1.0f), projectionMatrix).xyww;

	output.tex = inPos;
	
	return output;
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_TARGET
{
	return SkyMap.Sample(ObjSamplerState, input.tex);
}
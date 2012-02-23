cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 domePosition : TEXCOORD0;
	float3 tex : TEXCOORD1; // cube map
};

cbuffer GradientBuffer
{
    float4 apexColor;
    float4 centerColor;
	float4 pixelShaderType; // 0 - use gradient; 1 - use cube texture; // 2 - use both
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyMap;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SkyDomeVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Send the unmodified position through to the pixel shader.
    output.domePosition = input.position;
	output.tex = input.position;

    return output;
}
// TODO: Modify pixel shader to support both gradient and cube map shpere mapping
float4 SkyDomePixelShader(PixelInputType input) : SV_TARGET
{
    float height;
    float4 outputColor;

	if (pixelShaderType.x == 0.0f)
	{
		// Determine the position on the sky dome where this pixel is located.
		height = input.domePosition.y;

		// The value ranges from -1.0f to +1.0f so change it to only positive values.
		if(height < 0.0f)
		{
			height = 0.0f;
		}

		// Determine the gradient color by interpolating between the apex and center based on the height of the pixel in the sky dome.
		outputColor = lerp(centerColor, apexColor, height);
	}
	else if (pixelShaderType.x == 1.0f)
	{
		outputColor = SkyMap.Sample(ObjSamplerState, input.tex);
	}
	return outputColor;
}

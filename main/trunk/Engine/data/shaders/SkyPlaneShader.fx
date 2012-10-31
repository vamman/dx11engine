cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

Texture2D cloudTexture : register(t0);
Texture2D perturbTexture : register(t1);
SamplerState SampleType;

cbuffer SkyBuffer
{
    float translation;
    float scale;
    float brightness;
    float padding;
};

PixelInputType SkyPlaneVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    return output;
}

float4 SkyPlanePixelShader(PixelInputType input) : SV_TARGET
{
    float4 perturbValue;
    float4 cloudColor;
	
	// Translate the texture coordinate sampling location by the translation value.
    input.tex.x = input.tex.x + translation;
	
	// Sample the texture value from the perturb texture using the translated texture coordinates.
    perturbValue = perturbTexture.Sample(SampleType, input.tex);
	
	// Multiply the perturb value by the perturb scale.
    perturbValue = perturbValue * scale;
	
	// Add the texture coordinates as well as the translation value to get the perturbed texture coordinate sampling location.
    perturbValue.xy = perturbValue.xy + input.tex.xy + translation;
	
	// Now sample the color from the cloud texture using the perturbed sampling coordinates.
    cloudColor = cloudTexture.Sample(SampleType, perturbValue.xy);
	
	// Reduce the color cloud by the brightness value.
    cloudColor = cloudColor * brightness;

    return cloudColor;
}
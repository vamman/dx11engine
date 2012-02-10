/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	bool isInstanced;
};

cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 instancePosition : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
};

Texture2D shaderTexture;
SamplerState SampleType;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FogVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 cameraPosition;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    if (isInstanced == true)
    {
        input.position.x += input.instancePosition.x;
        input.position.y += input.instancePosition.y;
        input.position.z += input.instancePosition.z;
    }

    // Update the position of the vertices based on the data for this particular instance.
    input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the camera position.
    cameraPosition = mul(input.position, worldMatrix);
    cameraPosition = mul(cameraPosition, viewMatrix);

    // Calculate linear fog.    
    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FogPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 fogColor;
    float4 finalColor;

    // Sample the texture pixel at this location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // Set the color of the fog to grey.
    fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // Calculate the final color using the fog effect equation.
    finalColor = input.fogFactor * textureColor + (1.0 - input.fogFactor) * fogColor;

    return finalColor;
}
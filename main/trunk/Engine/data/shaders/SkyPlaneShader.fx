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

Texture2D cloudTexture1 : register(t0);
Texture2D cloudTexture2 : register(t1);
SamplerState SampleType;

cbuffer SkyBuffer
{
    float firstTranslationX;
    float firstTranslationZ;
    float secondTranslationX;
    float secondTranslationZ;
    float brightness;
    float3 padding;
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
    float2 sampleLocation;
    float4 textureColor1;
    float4 textureColor2;
    float4 finalColor;

    // Translate the position where we sample the pixel from using the first texture translation values.
    sampleLocation.x = input.tex.x + firstTranslationX;
    sampleLocation.y = input.tex.y + firstTranslationZ;

    // Sample the pixel color from the first cloud texture using the sampler at this texture coordinate location.
    textureColor1 = cloudTexture1.Sample(SampleType, sampleLocation);

    // Translate the position where we sample the pixel from using the second texture translation values.
    sampleLocation.x = input.tex.x + secondTranslationX;
    sampleLocation.y = input.tex.y + secondTranslationZ;

    // Sample the pixel color from the second cloud texture using the sampler at this texture coordinate location.
    textureColor2 = cloudTexture2.Sample(SampleType, sampleLocation);

    // Combine the two cloud textures evenly.
    finalColor = lerp(textureColor1, textureColor2, 0.5f);

    // Reduce brightness of the combined cloud textures by the input brightness value.
    finalColor = finalColor * brightness;
    return finalColor;
}
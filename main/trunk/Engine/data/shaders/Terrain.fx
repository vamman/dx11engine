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
    float3 normal : NORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float4 color : COLOR;
};

Texture2D shaderTexture1; // : register(t0);
Texture2D shaderTexture2; // : register(t1);
Texture2D alphaMap; // : register(t2);
//Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float3 padding;
};

cbuffer TextureInfoBuffer
{
    bool useAlplha;
    float3 padding2;
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TerrainVertexShader(VertexInputType input)
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
	
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	// Send the color map color into the pixel shader.	
    output.color = input.color;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float4 textureColor1;
    float4 textureColor2;
    float4 alphaValue;
    float4 blendColor;

    // Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

    if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);
    }

    // Saturate the final light color.
    color = saturate(color);
	
	if(useAlplha)
    {
        // Sample the pixel color from the first texture using the sampler at this texture coordinate location.
        textureColor1 = shaderTexture1.Sample(SampleType, input.tex);

        // Sample the pixel color from the second texture using the sampler at this texture coordinate location.
        textureColor2 = shaderTexture2.Sample(SampleType, input.tex);

        // Sample the alpha blending value.
        alphaValue = alphaMap.Sample(SampleType, input.tex);
		
        // Alpha blend the two colors together based on the alpha value.
        blendColor = (alphaValue * textureColor2) + ((1.0 - alphaValue) * textureColor1);
    }
	else
    {
        // Use the pixel color from the first texture only.
        blendColor = shaderTexture1.Sample(SampleType, input.tex);
		//blendColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
	
	// Multiply the blended texture color and the final light color to get the result.
    color = color *  blendColor;

	// Combine the color map value into the final color.
    color = saturate(color * input.color * 2.0f);

    return color;
}
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
	float4 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float4 color : COLOR;
	float4 depthPosition : TEXCOORD1;
};

Texture2D shaderTexture1; // : register(t0);
Texture2D shaderTexture2; // : register(t1);
Texture2D alphaMap; // : register(t2);
//Texture2D shaderTexture;

Texture2D detailTexture : register(t1);
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
PixelInputType VertexShaderFunction(VertexInputType input)
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
	
	// Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PixelShaderFunction(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float4 textureColor1;
    float4 textureColor2;
    float4 alphaValue;
    float4 blendColor;
	
    float depthValue;
    float detailBrightness;
    float4 detailColor;
	
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;
	
	// Check if the depth value is close to the screen, if so we will apply the detail texture.
    if(depthValue < 0.9f)
    {
        // Sample the pixel color from the detail map texture using the sampler at this texture coordinate location.
        detailColor = detailTexture.Sample(SampleType, input.tex.zw);

        // Set the brightness of the detail texture.
        detailBrightness = 1.8f;
    }

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
		
		// Combine the ground texture 1 and the detail texture.  Also multiply in the detail brightness.
        textureColor1 = textureColor1 * detailColor * detailBrightness;
		

        // Sample the pixel color from the second texture using the sampler at this texture coordinate location.
        textureColor2 = shaderTexture2.Sample(SampleType, input.tex);
		
		// Combine the ground texture 1 and the detail texture.  Also multiply in the detail brightness.
        textureColor2 = textureColor2 * detailColor * detailBrightness;

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
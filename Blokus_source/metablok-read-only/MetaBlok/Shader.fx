
//--------------------------------------------------------------------------------------
//
// File: Shader.fx
// Renders lighted scene geometry
// 
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float4x4 wvp;			// World * View * Projection matrix

float4x4 xLightsWorldViewProjection; // Light's WorldViewProj matrix
float4x4 xWorldViewProjection;		 // Camera's WorldViewProj matrix

float3 xLightPosition;	// Position of the light
float4 xLightPower;		// Sunlight lighting
float4 xAmbient;		// Ambient lighting

Texture xShadowMap;		// Shadow map
Texture xTexture;		// Mesh Texture

//--------------------------------------------------------------------------------------
// Texture sampler
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <xTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;	// vertex position 
    float3 Normal	  : NORMAL;		// vertex normal
	float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

//--------------------------------------------------------------------------------------
// Performs standard vertex transform of input vertices
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS( float4 Position : POSITION,
						 float2 TextureUV : TEXCOORD0, 
						 float3 Normal : NORMAL )
{
    VS_OUTPUT Output;
    
    // Transform the position from object space
    Output.Position = mul( Position, wvp );
    
    // Copy texture uv data
    Output.TextureUV = TextureUV;
    
    // Copy normal data
    Output.Normal = Normal;

    return Output;    
}

//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};

//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texel's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
    
	// Compute dot*normal value for standard directional lighting model
	float dotProd = max( dot( xLightPosition, In.Normal ), 0.0f );
	
	// Modify texture strength to compensate for lighting
    Output.RGBColor = xAmbient + xLightPower*dotProd; //tex2D( MeshTextureSampler, In.TextureUV ) * 1.0f;
    Output.RGBColor = tex2D( MeshTextureSampler, In.TextureUV );
	
    return Output;
}

//--------------------------------------------------------------------------------------
// Renders Game Board with specular highlights
//--------------------------------------------------------------------------------------
technique RenderGameBoard
{
    pass P0
    {          
        VertexShader = compile vs_3_0 RenderSceneVS( );
        PixelShader  = compile ps_3_0 RenderScenePS( );
    }
}
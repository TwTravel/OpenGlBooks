//-----------------------------------------------------------------------------
// File: scene.fx
//
// Desc: Effect file for the Advanced Decals sample. 
// 
// Copyright (c) ElseWhere Entertainment. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
#define APP_TECH_ORIGINAL			0
#define APP_TECH_COMBINED			1
#define APP_TECH_COMBINED_NORMALS	2
#define APP_TECH_COMBINED_DIFFUSE	3
#define APP_TECH_SCENE_NORMALS		4
#define APP_TECH_SCENE_DIFFUSE		5

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

float4x4 g_mWorldViewProj : WORLDVIEWPROJ;
float3   g_vCamera;
float4   g_vGlobalNormalsScale;

float	 g_fAlphaScale;
float	 g_fDiffuseScale;
float	 g_fLightScale;

// Textures
texture g_tGlobalNormals;
texture g_tGlobalDiffuse;

texture g_tNormal : NormalMap
	< 
	string name = "normal_map.dds"; 
	string UIName = "Normal Map";
	>;

texture g_tDiffuse : DiffuseMap
	< 
	string name = "diffuse_map.dds"; 
	string UIName = "Diffuse Map";
	>;

texture g_tSpecular : SpecularMap
	< 
	string name = "specular_map.dds"; 
	string UIName = "Specular Map";
	>;

texture g_tOpacity
	< 
	string name = "opacity_map.dds"; 
	string UIName = "Opacity Map";
	>;

// Lighting parameters
float3 g_vFirstLightDirection = normalize( float3( -1.0f, 0.8f, -0.8f ) );
float3 g_vSecondLightDirection = normalize( float3( 1.0f, 0.8f, 0.8f ) );
float3 g_vLightDiffuse = float3( 1.0f, 0.9f, 0.8f );
float3 g_vLightSpecular = float3( 1.0f, 0.8f, 0.6f ) * 2;

//-----------------------------------------------------------------------------
// Samplers
//-----------------------------------------------------------------------------
sampler2D NormalMapSampler =
sampler_state
{
    Texture = <g_tNormal>;
    MinFilter = Linear;//Anisotropic;
    MagFilter = Linear;
    MipFilter = Linear;
    MaxAnisotropy = 8;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D DiffuseMapSampler =
sampler_state
{
    Texture = <g_tDiffuse>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D SpecularMapSampler =
sampler_state
{
    Texture = <g_tSpecular>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D OpacityMapSampler =
sampler_state
{
    Texture = <g_tOpacity>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D g_samGlobalNormals =
sampler_state
{
    Texture = <g_tGlobalNormals>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

sampler2D g_samGlobalDiffuse =
sampler_state
{
    Texture = <g_tGlobalDiffuse>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

//-----------------------------------------------------------------------------
// Vertex/pixel shader output structures
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float2 Tex : TEXCOORD0;
	float4 tPos : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float3 Tangent : TEXCOORD4;
	float3 Binormal : TEXCOORD5;
	float3 View : TEXCOORD6;
};

struct PS_MRT_OUTPUT
{
    float4 vNormal : COLOR0;
    float4 vDiffuse : COLOR1;
};

//-----------------------------------------------------------------------------
// Name: CalculateLighting
// Type: Helper function                                      
// Desc: Computes lighting term for a single light source
//-----------------------------------------------------------------------------
float3 CalculateLighting(
	float3 light,
	float3 normal,
	float3 view,
	float3 diffuse_map,
	float3 specular_map
	)
{
	float3 output;
    
	output = saturate( dot( normal, light ) ) * diffuse_map * g_vLightDiffuse * g_fLightScale;
	output += pow( saturate( dot( normal, normalize( view + light ) ) ), 32 ) * specular_map * g_vLightSpecular * g_fLightScale;

	return output;
}

//-----------------------------------------------------------------------------
// Name: AdjustAlpha
// Type: Helper function                                      
// Desc: Transforms output alpha for decals
//-----------------------------------------------------------------------------
float AdjustAlpha(
	float alpha
	)
{
	return saturate( alpha * g_fAlphaScale );
}

//-----------------------------------------------------------------------------
// Vertex Shader: SceneVS
// Desc: Process vertex for scene
//-----------------------------------------------------------------------------
VS_OUTPUT SceneVS(
	float3 vPos : POSITION,
	float3 vNormal : NORMAL,
	float3 vTangent : TANGENT,
	float3 vBinormal : BINORMAL,
	float2 tTex : TEXCOORD0
	)
{
	VS_OUTPUT output;

    //
    // Also write view direction to texcoor1 to do per-pixel lighting
    //

    output.View = g_vCamera - vPos;

    //
    // Transform to screen coord
    //
    output.vPos = mul( float4( vPos, 1 ), g_mWorldViewProj );    // position (projected)

    output.tPos = output.vPos;
    output.tPos.y = -output.tPos.y;

    //
    // Transform normal/tangent/binormal and write to texcoords for per-pixel lighting
    //
    output.Normal = vNormal;
    output.Tangent = vTangent;
    output.Binormal = vBinormal;
    
    //
    // Propagate texture coord
    //
    output.Tex = tTex;

	return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ScenePS
// Desc: Process pixel custom lighting composed with decal buffers
//-----------------------------------------------------------------------------
float4 ScenePS(
	float2 Tex : TEXCOORD0,
	float4 Pos : TEXCOORD1,
	float3 Normal : TEXCOORD2,
	float3 Tangent : TEXCOORD4,
	float3 Binormal : TEXCOORD5,
	float3 View : TEXCOORD6,
	uniform int Type
	) : COLOR
{
    float4 output;
	float3 combined_normal;
	float3 combined_diffuse;

	float2 screen_pos = g_vGlobalNormalsScale.zw + ( g_vGlobalNormalsScale.xy * Pos.xy / Pos.w );

	//
	// Sample regular lighting maps
	//
    float3 normal_map = tex2D( NormalMapSampler, Tex ) * 2 - 1;
    float3 diffuse_map = tex2D( DiffuseMapSampler, Tex );
	float3 specular_map = tex2D( SpecularMapSampler, Tex );

	float3 surface_normal = normalize( Tangent * normal_map.x + Binormal * normal_map.y + Normal * normal_map.z );

	if ( Type == APP_TECH_ORIGINAL )
	{
		combined_normal = surface_normal;
		combined_diffuse = diffuse_map;
	}
	else
	{
		//
		// Sample full-screen decal maps
		//
		float3 decal_normal = tex2D( g_samGlobalNormals, screen_pos ) * 2 - 1;
		float4 decal_diffuse = tex2D( g_samGlobalDiffuse, screen_pos );

		//
		// Blend normals 
		//
	    combined_normal = normalize( lerp( surface_normal, decal_normal, tex2D( g_samGlobalNormals, screen_pos ).w ) );
    
		//
		// Blend diffuse maps
		//

	    combined_diffuse = diffuse_map * lerp( 1, decal_diffuse.xyz, decal_diffuse.w );
	}

	//
	// Compute lighting
	//
    float3 view = normalize( View );

	output.xyz  = CalculateLighting( g_vFirstLightDirection, -combined_normal, view, combined_diffuse, specular_map );
	output.xyz += CalculateLighting( g_vSecondLightDirection, -combined_normal, view, combined_diffuse * 0.5f, specular_map * 0.5f ); // backlight
    output.w = 1;

	//
	// Additional output
	//
	if ( Type == APP_TECH_SCENE_NORMALS )
		return float4( surface_normal * 0.5f + 0.5f, 1.0f );

	if ( Type == APP_TECH_SCENE_DIFFUSE )
		return float4( diffuse_map, 1.0f );

	if ( Type == APP_TECH_COMBINED_NORMALS )
		return float4( combined_normal * 0.5f + 0.5f, 1.0f );

	if ( Type == APP_TECH_COMBINED_DIFFUSE )
		return float4( combined_diffuse, 1.0f );

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: NormalDecalsPS
// Desc: Output world-space normals
//-----------------------------------------------------------------------------
float4 NormalDecalsPS(
	float2 Tex : TEXCOORD0,
	float3 Normal : TEXCOORD2,
	float3 Tangent : TEXCOORD4,
	float3 Binormal : TEXCOORD5
    ) : COLOR
{
	float4 output;

	float  alpha = tex2D( OpacityMapSampler, Tex ).r;
    float3 normal = tex2D( NormalMapSampler, Tex ) * 2 - 1;

    output.xyz = normalize( Tangent * normal.x + Binormal * normal.y + Normal * normal.z ) * 0.5 + 0.5;
    output.w = AdjustAlpha( alpha );

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: DiffuseDecalsPS
// Desc: Output diffuse map
//-----------------------------------------------------------------------------
float4 DiffuseDecalsPS(
	float2 Tex : TEXCOORD0
    ) : COLOR
{
	float4 output;

	float  alpha = tex2D( OpacityMapSampler, Tex ).r;
    float4 diffuse = tex2D( DiffuseMapSampler, Tex );

    output.xyz = diffuse * g_fDiffuseScale;
    output.w = AdjustAlpha( alpha );

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: MRTDecalsPS
// Desc: Output diffuse and normal maps
//-----------------------------------------------------------------------------
PS_MRT_OUTPUT MRTDecalsPS(
	float2 Tex : TEXCOORD0,
	float3 Normal : TEXCOORD2,
	float3 Tangent : TEXCOORD4,
	float3 Binormal : TEXCOORD5
	)
{
	PS_MRT_OUTPUT output;

	float  alpha = tex2D( OpacityMapSampler, Tex ).r;
    float3 normal = tex2D( NormalMapSampler, Tex ) * 2 - 1;
    float4 diffuse = tex2D( DiffuseMapSampler, Tex );

	output.vDiffuse.xyz = diffuse * g_fDiffuseScale;
	output.vNormal.xyz = normalize( Tangent * normal.x + Binormal * normal.y + Normal * normal.z ) * 0.5 + 0.5;

    output.vDiffuse.w = AdjustAlpha( alpha );
	output.vNormal.w = output.vDiffuse.w;

	return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: OldschoolDecalsPS
// Desc: Output diffuse decals
//-----------------------------------------------------------------------------
float4 OldschoolDecalsPS(
	float2 Tex : TEXCOORD0,

	float3 Normal : TEXCOORD2,
	float3 Tangent : TEXCOORD4,
	float3 Binormal : TEXCOORD5,
	float3 View : TEXCOORD6
	) : COLOR
{
	float4 output;

	float alpha = tex2D( OpacityMapSampler, Tex ).r;
    float4 diffuse = tex2D( DiffuseMapSampler, Tex );

	output.xyz = lerp( 1, diffuse * g_fDiffuseScale, alpha );
    output.a = 1;
	
    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: BumpMappedDecalsPS
// Desc: Output lit decals
//-----------------------------------------------------------------------------
float4 BumpMappedDecalsPS(
	float2 Tex : TEXCOORD0,

	float3 Normal : TEXCOORD2,
	float3 Tangent : TEXCOORD4,
	float3 Binormal : TEXCOORD5,
	float3 View : TEXCOORD6
	) : COLOR
{
	float4 output;

	float alpha = tex2D( OpacityMapSampler, Tex ).r;
    float4 diffuse = tex2D( DiffuseMapSampler, Tex );
    float3 normal = tex2D( NormalMapSampler, Tex ) * 2 - 1;

	float3 world_normal = normalize( Tangent * normal.x + Binormal * normal.y + Normal * normal.z ) * 0.5 + 0.5;

	//
	// Compute lighting
	//
    float3 view = normalize( View );
	output.xyz  = CalculateLighting( g_vFirstLightDirection, -world_normal, view, diffuse, 0 );
	output.xyz += CalculateLighting( g_vSecondLightDirection, -world_normal, view, diffuse * 0.5f, 0 ); // backlight

	output.xyz = lerp( 1, output.xyz * g_fDiffuseScale, AdjustAlpha( alpha ) );
    output.a = 1;
	
    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: EmptyPS
// Desc: Output 0
//-----------------------------------------------------------------------------
float4 EmptyPS() : COLOR
{
	return 0;
}

//-----------------------------------------------------------------------------
// Technique: showDiffuse, showNormal, ...
// Desc: Techniques and shaders used in 3DS MAX by default
//-----------------------------------------------------------------------------
float4 SceneShowNormalPS(
	float2 Tex : TEXCOORD0
	) : COLOR
{
	return tex2D( NormalMapSampler, Tex );
}

float4 SceneShowDiffusePS(
	float2 Tex : TEXCOORD0
	) : COLOR
{
	return tex2D( DiffuseMapSampler, Tex );
}

float4 SceneShowOpacityPS(
	float2 Tex : TEXCOORD0
	) : COLOR
{
	return tex2D( OpacityMapSampler, Tex );
}

technique ShowDiffuse
{
    pass p0
    {
        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 SceneShowDiffusePS();
    }
}

technique ShowNormal
{
    pass p0
    {
        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 SceneShowNormalPS();
    }
}

technique ShowOpacity
{
    pass p0
    {
        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 SceneShowOpacityPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: Original
// Desc: Render scene objects with original lighting (without decals)
//-----------------------------------------------------------------------------
technique Original
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_ORIGINAL );
    }
}

//-----------------------------------------------------------------------------
// Technique: Combined
// Desc: Render scene objects with applied decals
//-----------------------------------------------------------------------------
technique Combined
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_COMBINED );
    }
}

//-----------------------------------------------------------------------------
// Technique: CombinedNormals
// Desc: Render combined world-space normals
//-----------------------------------------------------------------------------
technique CombinedNormals
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_COMBINED_NORMALS );
    }
}

//-----------------------------------------------------------------------------
// Technique: CombinedDiffuse
// Desc: Render scene with combined diffuse maps
//-----------------------------------------------------------------------------
technique CombinedDiffuse
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_COMBINED_DIFFUSE );
    }
}

//-----------------------------------------------------------------------------
// Technique: SceneNormals
// Desc: Render world-space scene normals
//-----------------------------------------------------------------------------
technique SceneNormals
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_SCENE_NORMALS );
    }
}

//-----------------------------------------------------------------------------
// Technique: SceneDiffuse
// Desc: Render scene with diffuse maps only
//-----------------------------------------------------------------------------
technique SceneDiffuse
{
    pass p0
    {
        ZWriteEnable = FALSE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 ScenePS( APP_TECH_SCENE_DIFFUSE );
    }
}

//-----------------------------------------------------------------------------
// Technique: RenderDepth
// Desc: Render scene objects to depth buffer only
//-----------------------------------------------------------------------------
technique RenderDepth
{
    pass p0
    {
        AlphaBlendEnable = FALSE;
        ZWriteEnable = TRUE;
        ZEnable = TRUE;
        ColorWriteEnable = 0;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 EmptyPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: DecalNormals
// Desc: Render normals of decals
//-----------------------------------------------------------------------------
technique DecalNormals
{
    pass p0
    {
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
		AlphaTestEnable = TRUE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 NormalDecalsPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: DecalDiffuse
// Desc: Render diffuse of decals
//-----------------------------------------------------------------------------
technique DecalDiffuse
{
    pass p0
    {
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
		AlphaTestEnable = TRUE;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 DiffuseDecalsPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: DecalMRT
// Desc: Render diffuse and normals of decals at once
//-----------------------------------------------------------------------------
technique DecalMRT
{
    pass p0
    {
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 MRTDecalsPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: DecalOldschool
// Desc: Render old-school style decals
//-----------------------------------------------------------------------------
technique DecalOldschool
{
    pass p0
    {
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = DESTCOLOR;
        DestBlend = ZERO;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 OldschoolDecalsPS();
    }
}

//-----------------------------------------------------------------------------
// Technique: DecalBumpMapped
// Desc: Render old-school style BUMP MAPPED decals
//-----------------------------------------------------------------------------
technique DecalBumpMapped
{
    pass p0
    {
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = DESTCOLOR;
        DestBlend = ZERO;

        VertexShader = compile vs_2_0 SceneVS();
        PixelShader = compile ps_2_0 BumpMappedDecalsPS();
    }
}

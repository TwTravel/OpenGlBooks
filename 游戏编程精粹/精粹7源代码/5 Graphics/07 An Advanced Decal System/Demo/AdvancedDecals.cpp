//--------------------------------------------------------------------------------------
// File: AdvancedDecals.cpp
//
// Desc: Advanced Decals
//
// Copyright (c) ElseWhere Entertainment. All rights reserved.
//--------------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "resource.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

// Standard stuff
ID3DXFont *             g_pFont = NULL;         // Font for drawing text
ID3DXSprite *           g_pTextSprite = NULL;   // Sprite for batching draw text calls
LPD3DXEFFECT            g_pEffect = NULL;       // D3DX effect interface
CFirstPersonCamera      g_Camera;               // A model viewing camera

bool                    g_bShowHelp = true;
int                     g_nShowGUI = 1;

float                   g_fAlphaScale = 1.5f;
float                   g_fDiffuseScale = 1.0f;
float                   g_fLightScale = 1.0f;
int                     g_nNumDecalDraws = 1;

// UI
CDXUTDialogResourceManager g_DialogResourceManager; // Manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
CDXUTDialog             g_UI;                   // Dialog for standard controls

// Scene Meshes
#define NUM_SCENES      4

int                     g_nCurrentScene = 0;
CDXUTMesh               g_aWorldMesh[ NUM_SCENES ];
CDXUTMesh               g_aDecalMesh[ NUM_SCENES ];
CDXUTMesh *             g_pWorldMesh = NULL;
CDXUTMesh *             g_pDecalMesh = NULL;

// Decall related stuff
int                     g_nRenderTargetWidth;
int                     g_nRenderTargetHeight;
LPDIRECT3DTEXTURE9      g_pNormalsTexture = NULL;
LPDIRECT3DTEXTURE9      g_pDiffuseTexture = NULL;

// Rendering technique
enum APP_TECHNIQUE
{
    APP_TECH_ORIGINAL = 0,
    APP_TECH_COMBINED,
    APP_TECH_COMBINED_NORMALS,
    APP_TECH_COMBINED_DIFFUSE,
    APP_TECH_SCENE_NORMALS,
    APP_TECH_SCENE_DIFFUSE,
    APP_TECH_DECAL_NORMALS,
    APP_TECH_DECAL_DIFFUSE,
    APP_TECH_OLDSCHOOL,
    APP_TECH_OLDSCHOOL_BUMPMAPPED
};

// Vertex format declaration
D3DVERTEXELEMENT9 g_aVertDecl0[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
    { 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
    { 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

    D3DDECL_END()
};

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC              0
#define IDC_RENDER_DECALS       1
#define IDC_TECHNIQUE           2
#define IDC_TOGGLEFULLSCREEN    3
#define IDC_TOGGLEREF           4
#define IDC_CHANGEDEVICE        5
#define IDC_USE_MRT             6
#define IDC_NUM_DECALS          7
#define IDC_NUM_DECALS_STATIC   8
#define IDC_LIGHT_SCALE         9
#define IDC_LIGHT_SCALE_STATIC  10
#define IDC_ALPHA_SCALE         11
#define IDC_ALPHA_SCALE_STATIC  12
#define IDC_DIFFUSE_SCALE       13
#define IDC_DIFFUSE_SCALE_STATIC 14

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

void RenderDecals();
void RenderFullscreenQuad( IDirect3DDevice9 * pd3dDevice );

//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    pDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    pDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_NONE; // Multisampling is not supported in this demo

    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
    int i;

    V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

    for ( i = 0; i < NUM_SCENES; i++ )
    {
        WCHAR fileName[ MAX_PATH ];

        swprintf( fileName, L"data\\scene%d.x", i );
        g_aWorldMesh[ i ].Create( pd3dDevice, fileName );
        g_aWorldMesh[ i ].SetFVF( pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 );
        g_aWorldMesh[ i ].SetVertexDecl( pd3dDevice, g_aVertDecl0, false, true, true );
        g_aWorldMesh[ i ].UseMeshMaterials( false );
        
        swprintf( fileName, L"data\\decals%d.x", i );
        g_aDecalMesh[ i ].Create( pd3dDevice, fileName );
        g_aDecalMesh[ i ].SetFVF( pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 );
        g_aDecalMesh[ i ].SetVertexDecl( pd3dDevice, g_aVertDecl0, false, true, true );
        g_aDecalMesh[ i ].UseMeshMaterials( false );
    }

    g_pWorldMesh = g_aWorldMesh;
    g_pDecalMesh = g_aDecalMesh;

    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, TEXT( "data\\scene.fx" ), NULL, NULL, D3DXFX_NOT_CLONEABLE | D3DXFX_DONOTSAVESTATE,
                                        NULL, &g_pEffect, NULL ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    float fAspectRatio = pBackBufferSurfaceDesc->Width / float( pBackBufferSurfaceDesc->Height );
    g_Camera.SetProjParams( D3DXToRadian( 60.0f ), fAspectRatio, 0.1f, 1500.0f );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

    V_RETURN( g_DialogResourceManager.OnResetDevice() );
    V_RETURN( g_SettingsDlg.OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    g_UI.SetLocation( 0, 0 );
    g_UI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    g_UI.GetControl( IDC_TOGGLEFULLSCREEN )->SetLocation( pBackBufferSurfaceDesc->Width - 130, 10 );
    g_UI.GetControl( IDC_TOGGLEREF )->SetLocation( pBackBufferSurfaceDesc->Width - 130, 34 );
    g_UI.GetControl( IDC_CHANGEDEVICE )->SetLocation( pBackBufferSurfaceDesc->Width - 130, 58 );

    g_nRenderTargetWidth = pBackBufferSurfaceDesc->Width;
    g_nRenderTargetHeight = pBackBufferSurfaceDesc->Height;

    V_RETURN( pd3dDevice->CreateTexture( g_nRenderTargetWidth, g_nRenderTargetHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pNormalsTexture, NULL ) );
    V_RETURN( pd3dDevice->CreateTexture( g_nRenderTargetWidth, g_nRenderTargetHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pDiffuseTexture, NULL ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    if ( GetKeyState( VK_LBUTTON ) & 0x8000 )
        return;

    if ( GetKeyState( VK_SHIFT ) & 0x8000 )
        fElapsedTime *= 5.f;

    if ( GetKeyState( VK_LCONTROL ) & 0x8000 )
        fElapsedTime *= 0.2f;

    g_Camera.FrameMove( fElapsedTime );
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( g_pSprite, strMsg, -1, &rc, DT_NOCLIP, g_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats( true ) );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

    // Draw help
    if( g_bShowHelp )
    {
        const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-15*6 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Controls (F1 to hide):" );

        txtHelper.SetInsertionPos( 40, pd3dsdBackBuffer->Height-15*5 );
        txtHelper.DrawTextLine( L"Look: Any click drag\n"
                                L"Move: A,W,S,D or Arrow Keys\n"
                                L"Toggle HUD: F5\n"
                                L"Next Scene: Space" );
    }
    else
    {
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Press F1 for help" );
    }

    txtHelper.End();
}

//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9 * pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    HRESULT hr;

    APP_TECHNIQUE nTechnique = (APP_TECHNIQUE)PtrToUlong( g_UI.GetComboBox( IDC_TECHNIQUE )->GetSelectedData() );
    bool bRenderDecals = g_UI.GetCheckBox( IDC_RENDER_DECALS )->GetChecked();
    bool bUseMRT = g_UI.GetCheckBox( IDC_USE_MRT )->GetChecked();

    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 32, 64, 128), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        IDirect3DSurface9 * pNormalSurface;
        IDirect3DSurface9 * pDiffuseSurface;
        IDirect3DSurface9 * pBackBufferSurface;
        D3DXMATRIX matWorld;
        D3DXMATRIX matView = *g_Camera.GetViewMatrix();
        D3DXMATRIX matProj = *g_Camera.GetProjMatrix();
        D3DXMATRIX matViewProj = matView * matProj;

        D3DXMatrixIdentity( &matWorld );
        pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
        pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
        pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );                

        pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

        g_pNormalsTexture->GetSurfaceLevel( 0, &pNormalSurface );
        g_pDiffuseTexture->GetSurfaceLevel( 0, &pDiffuseSurface );
        pd3dDevice->GetRenderTarget( 0, &pBackBufferSurface );
        pd3dDevice->SetVertexDeclaration( g_pWorldMesh->m_pDecl );

        // Select main technique
        char * pTechniqueName = "Combined";

        switch ( nTechnique )
        {
        case APP_TECH_ORIGINAL : pTechniqueName = "Original"; break;
        case APP_TECH_COMBINED : pTechniqueName = "Combined"; break;
        case APP_TECH_COMBINED_NORMALS : pTechniqueName = "CombinedNormals"; break;
        case APP_TECH_COMBINED_DIFFUSE : pTechniqueName = "CombinedDiffuse"; break;
        case APP_TECH_SCENE_NORMALS : pTechniqueName = "SceneNormals"; break;
        case APP_TECH_SCENE_DIFFUSE : pTechniqueName = "SceneDiffuse"; break;
        }

        // Setup variables
        V( g_pEffect->SetTexture( "g_tGlobalNormals", g_pNormalsTexture ) );
        V( g_pEffect->SetTexture( "g_tGlobalDiffuse", g_pDiffuseTexture ) );
        V( g_pEffect->SetFloat( "g_fAlphaScale", g_fAlphaScale ) );
        V( g_pEffect->SetFloat( "g_fDiffuseScale", g_fDiffuseScale ) );
        V( g_pEffect->SetFloat( "g_fLightScale", g_fLightScale ) );
        V( g_pEffect->SetVector( "g_vGlobalNormalsScale", &D3DXVECTOR4( 0.5f, 0.5f, 0.5f + 0.5f / float( g_nRenderTargetWidth ), 0.5f + 0.5f / float( g_nRenderTargetHeight ) ) ) );
        V( g_pEffect->SetVector( "g_vCamera", &D3DXVECTOR4( *g_Camera.GetEyePt() ) ) );
        V( g_pEffect->SetMatrix( "g_mWorldViewProj", &matViewProj ) );

        if ( nTechnique == APP_TECH_OLDSCHOOL || nTechnique == APP_TECH_OLDSCHOOL_BUMPMAPPED )
        {
            //
            // Pass 0 : Render scene to depth buffer
            //
            V( g_pEffect->SetTechnique( "RenderDepth" ) );
            g_pWorldMesh->UseShaderTextures( false );
            g_pWorldMesh->Render( g_pEffect );
            g_pWorldMesh->UseShaderTextures( true );

            // ... Render something else ...

            //
            // Pass 1 : Render scene with original lighting
            //
            V( g_pEffect->SetTechnique( "Original" ) );
            g_pWorldMesh->Render( g_pEffect );

            //
            // Pass 2 : Render old-school decals
            //
            if ( nTechnique == APP_TECH_OLDSCHOOL )
            {
                V( g_pEffect->SetTechnique( "DecalOldschool" ) );
            }
            else
            {
                V( g_pEffect->SetTechnique( "DecalBumpMapped" ) );
            }

            if ( bRenderDecals )
                RenderDecals();
        }
        else
        {
            //
            // Pass 0 : Render scene to depth buffer
            //
            V( g_pEffect->SetTechnique( "RenderDepth" ) );
            g_pWorldMesh->UseShaderTextures( false );
            g_pWorldMesh->Render( g_pEffect );
            g_pWorldMesh->UseShaderTextures( true );

            //
            // Pass 1 : Render decals to full-screen render targets
            //
            if ( bUseMRT )
            {
                V( g_pEffect->SetTechnique( "DecalMRT" ) );
                V( pd3dDevice->SetRenderTarget( 0, pNormalSurface ) );
                V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00808080, 1.0f, 0 ) );
                V( pd3dDevice->ColorFill( pDiffuseSurface, NULL, 0x00ffffff ) );
                V( pd3dDevice->SetRenderTarget( 1, pDiffuseSurface ) );

                if ( bRenderDecals )
                    RenderDecals();

                V( pd3dDevice->SetRenderTarget( 1, NULL ) );
            }
            else
            {
                V( g_pEffect->SetTechnique( "DecalNormals" ) );
                V( pd3dDevice->SetRenderTarget( 0, pNormalSurface ) );
                V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00808080, 1.0f, 0 ) );

                if ( bRenderDecals )
                    RenderDecals();

                V( g_pEffect->SetTechnique( "DecalDiffuse" ) );
                V( pd3dDevice->SetRenderTarget( 0, pDiffuseSurface ) );
                V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00ffffff, 1.0f, 0 ) );

                if ( bRenderDecals )
                    RenderDecals();
            }

            V( pd3dDevice->SetRenderTarget( 0, pBackBufferSurface ) );

            //
            // Pass 2 : Render scene combined with decals
            //
            V( g_pEffect->SetTechnique( pTechniqueName ) );
            g_pWorldMesh->Render( g_pEffect );
        }

        SAFE_RELEASE( pDiffuseSurface );
        SAFE_RELEASE( pNormalSurface );
        SAFE_RELEASE( pBackBufferSurface );

        // Draw additional maps
        if ( nTechnique == APP_TECH_DECAL_DIFFUSE || nTechnique == APP_TECH_DECAL_NORMALS )
        {
            if ( nTechnique == APP_TECH_DECAL_DIFFUSE )
                pd3dDevice->SetTexture( 0, g_pDiffuseTexture );
            else
                pd3dDevice->SetTexture( 0, g_pNormalsTexture );

            RenderFullscreenQuad( pd3dDevice );
        }

        // Render UI
        if ( g_nShowGUI > 0 )
        {
            RenderText();

            if ( g_nShowGUI == 1 )
                V( g_UI.OnRender( fElapsedTime ) );
        }

        V( pd3dDevice->EndScene() );
    }
}

//-----------------------------------------------------------------------------
// Name: RenderDecals()
// Desc: Render decals (multiple times)
//-----------------------------------------------------------------------------
void RenderDecals()
{
    for ( int i = 0; i < g_nNumDecalDraws; i++ )
        g_pDecalMesh->Render( g_pEffect );
}

//-----------------------------------------------------------------------------
// Name: RenderFullscreenQuad()
// Desc: Render a quad
//-----------------------------------------------------------------------------
void RenderFullscreenQuad( IDirect3DDevice9 * pd3dDevice )
{
    int color = 0xFFFFFFFF;

    struct ScreenVertex
    {
        float x, y, z, rhw;
        int color;
        float u, v;
    } vertices[ 4 ] =
    {
        { 0, 0, 0, 1, color, 0, 0 },
        { float( g_nRenderTargetWidth ), 0, 0, 1, color, 1, 0 },
        { 0, float( g_nRenderTargetHeight ), 0, 1, color, 0, 1 },
        { float( g_nRenderTargetWidth ), float( g_nRenderTargetHeight ), 0, 1, color, 1, 1 },
    };

    pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof( ScreenVertex ) );
}

//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );

    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_UI.MsgProc( hWnd, uMsg, wParam, lParam );

    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnLostDevice();
    g_SettingsDlg.OnLostDevice();

    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();

    SAFE_RELEASE( g_pTextSprite );
    SAFE_RELEASE( g_pNormalsTexture );
    SAFE_RELEASE( g_pDiffuseTexture );
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    int i;

    g_DialogResourceManager.OnDestroyDevice();
    g_SettingsDlg.OnDestroyDevice();

    SAFE_RELEASE( g_pFont );
    SAFE_RELEASE( g_pEffect );
    SAFE_RELEASE( g_pNormalsTexture );
    SAFE_RELEASE( g_pDiffuseTexture );

    for ( i = 0; i < NUM_SCENES; i++ )
    {
        g_aWorldMesh[ i ].Destroy();
        g_aDecalMesh[ i ].Destroy();
    }

    g_pWorldMesh = NULL;
    g_pDecalMesh = NULL;
}

//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
            case VK_F5: g_nShowGUI = ( g_nShowGUI + 1 ) % 3; break;
            
            case VK_SPACE :
                g_nCurrentScene = ( g_nCurrentScene + 1 ) % NUM_SCENES;
                g_pWorldMesh = &g_aWorldMesh[ g_nCurrentScene ];
                g_pDecalMesh = &g_aDecalMesh[ g_nCurrentScene ];
                break;
        }
    }
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    WCHAR sz[ 100 ];

    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;

        case IDC_NUM_DECALS:
            StringCchPrintf( sz, 100, L"# Decal Draws: %d", g_UI.GetSlider( IDC_NUM_DECALS )->GetValue() ); 
            g_UI.GetStatic( IDC_NUM_DECALS_STATIC )->SetText( sz );

            g_nNumDecalDraws = g_UI.GetSlider( IDC_NUM_DECALS )->GetValue();
            break;

        case IDC_LIGHT_SCALE: 
            g_fLightScale = float( g_UI.GetSlider( IDC_LIGHT_SCALE )->GetValue() * 0.01f );

            StringCchPrintf( sz, 100, L"Light scale: %0.2f", g_fLightScale ); 
            g_UI.GetStatic( IDC_LIGHT_SCALE_STATIC )->SetText( sz );
            break;

        case IDC_ALPHA_SCALE: 
            g_fAlphaScale = float( g_UI.GetSlider( IDC_ALPHA_SCALE )->GetValue() * 0.01f );

            StringCchPrintf( sz, 100, L"Alpha scale: %0.2f", g_fAlphaScale ); 
            g_UI.GetStatic( IDC_ALPHA_SCALE_STATIC )->SetText( sz );
            break;

        case IDC_DIFFUSE_SCALE: 
            g_fDiffuseScale = float( g_UI.GetSlider( IDC_DIFFUSE_SCALE )->GetValue() * 0.01f );

            StringCchPrintf( sz, 100, L"Diffuse scale: %0.2f", g_fDiffuseScale ); 
            g_UI.GetStatic( IDC_DIFFUSE_SCALE_STATIC )->SetText( sz );
            break;
    }
}


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE hInstance, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( KeyboardProc );
   
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_UI.Init( &g_DialogResourceManager );

    g_UI.SetCallback( OnGUIEvent ); int iY = 10;
    g_UI.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_UI.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_UI.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    iY = 32;

    CDXUTComboBox* pComboBox = NULL;

    g_UI.AddStatic( IDC_STATIC, L"Technique (T)", 5, iY += 24, 80, 24 );
    g_UI.AddComboBox( IDC_TECHNIQUE, 5, iY += 24, 150, 24, L'T', false, &pComboBox );

    pComboBox->SetDropHeight( 150 );
    pComboBox->AddItem( L"Original", IntToPtr( APP_TECH_ORIGINAL ) );
    pComboBox->AddItem( L"Combined", IntToPtr( APP_TECH_COMBINED ) );
    pComboBox->AddItem( L"Scene Normals", IntToPtr( APP_TECH_SCENE_NORMALS ) );
    pComboBox->AddItem( L"Combined Normals", IntToPtr( APP_TECH_COMBINED_NORMALS ) );
    pComboBox->AddItem( L"Scene Diffuse", IntToPtr( APP_TECH_SCENE_DIFFUSE ) );
    pComboBox->AddItem( L"Combined Diffuse", IntToPtr( APP_TECH_COMBINED_DIFFUSE ) );
    pComboBox->AddItem( L"Decal Normals", IntToPtr( APP_TECH_DECAL_NORMALS ) );
    pComboBox->AddItem( L"Decal Diffuse", IntToPtr( APP_TECH_DECAL_DIFFUSE ) );
    pComboBox->AddItem( L"Old-school", IntToPtr( APP_TECH_OLDSCHOOL ) );
    pComboBox->AddItem( L"Bump Mapped", IntToPtr( APP_TECH_OLDSCHOOL_BUMPMAPPED ) );
    pComboBox->SetSelectedByIndex( 1 );

    iY += 22;
    g_UI.AddCheckBox( IDC_RENDER_DECALS, L"Render Decals (L)", 5, iY += 22, 150, 16, true, L'L' );
    g_UI.AddCheckBox( IDC_USE_MRT, L"Multiple RT (M)", 5, iY += 22, 150, 16, false, L'M' );

    WCHAR sz[100];

    iY += 10;
    StringCchPrintf( sz, 100, L"Alpha scale: %0.2f", g_fAlphaScale ); 
    g_UI.AddStatic( IDC_ALPHA_SCALE_STATIC, sz, 5, iY += 22, 125, 22 );
    g_UI.AddSlider( IDC_ALPHA_SCALE, 15, iY += 22, 125, 22, 0, 200, int( g_fAlphaScale * 100.0f ) );

    iY += 10;
    StringCchPrintf( sz, 100, L"Diffuse scale: %0.2f", g_fDiffuseScale ); 
    g_UI.AddStatic( IDC_DIFFUSE_SCALE_STATIC, sz, 5, iY += 22, 125, 22 );
    g_UI.AddSlider( IDC_DIFFUSE_SCALE, 15, iY += 22, 125, 22, 0, 300, int( g_fDiffuseScale * 100.0f ) );

    iY += 10;
    StringCchPrintf( sz, 100, L"Light scale: %0.2f", g_fLightScale ); 
    g_UI.AddStatic( IDC_LIGHT_SCALE_STATIC, sz, 5, iY += 22, 125, 22 );
    g_UI.AddSlider( IDC_LIGHT_SCALE, 15, iY += 22, 125, 22, 0, 200, int( g_fLightScale * 100.0f ) );

    iY += 10;
    StringCchPrintf( sz, 100, L"# Decal Draws: %d", g_nNumDecalDraws ); 
    g_UI.AddStatic( IDC_NUM_DECALS_STATIC, sz, 5, iY += 22, 125, 22 );
    g_UI.AddSlider( IDC_NUM_DECALS, 15, iY += 22, 125, 22, 1, 50, g_nNumDecalDraws );

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"ElseWhere Entertainment - Advanced Decals" );//, hInstance, NULL, NULL );//LoadMenu( hInstance, MAKEINTRESOURCE( IDR_MENU1 ) ) );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 800, 600, IsDeviceAcceptable, ModifyDeviceSettings );

    D3DXVECTOR3 vecEye( -23.7f, 6.9f, -6.9f );
    D3DXVECTOR3 vecAt ( -23.0f, 6.6f, -6.3f );

    g_Camera.SetViewParams( &vecEye, &vecAt );

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


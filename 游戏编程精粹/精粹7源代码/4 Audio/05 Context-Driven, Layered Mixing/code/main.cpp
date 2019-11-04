//----------------------------------------------------------------------------
// File: main.cpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include "demo.hpp"
#include "resource.h"
#include "soundfile.hpp"
#include "soundstreamer.hpp"
#include "soundsystem.hpp"
#include "util.hpp"

//----------------------------------------------------------------------------
// Constansts
//----------------------------------------------------------------------------
static const unsigned int SLIDER_RANGE = 100L;
static const int CENTRAL_MIX_SLIDER_IDS[] = 
{ 
    IDC_SLIDER_CM0,
    IDC_SLIDER_CM1,
    IDC_SLIDER_CM2,
    IDC_SLIDER_CM3,
    IDC_SLIDER_CM4,
    IDC_SLIDER_CM5
};
static const int EDIT_SNAPSHOT_SLIDER_IDS[] = 
{ 
    IDC_SLIDER_E0,
    IDC_SLIDER_E1,
    IDC_SLIDER_E2,
    IDC_SLIDER_E3,
    IDC_SLIDER_E4,
    IDC_SLIDER_E5
};
static const int SNAPSHOT_EDIT_IDS[] =
{
    IDC_CHECK_EDIT_PM0,
    IDC_CHECK_EDIT_B0,
    IDC_CHECK_EDIT_B1,
    IDC_CHECK_EDIT_B2,
    IDC_CHECK_EDIT_M0,
    IDC_CHECK_EDIT_M1,
    IDC_CHECK_EDIT_M2,
    IDC_CHECK_EDIT_M3
};
static const int SNAPSHOT_INSTALL_IDS[] =
{
    IDC_CHECK_INSTALL_PM0,
    IDC_CHECK_INSTALL_B0,
    IDC_CHECK_INSTALL_B1,
    IDC_CHECK_INSTALL_B2,
    IDC_CHECK_INSTALL_M0,
    IDC_CHECK_INSTALL_M1,
    IDC_CHECK_INSTALL_M2,
    IDC_CHECK_INSTALL_M3
};

//----------------------------------------------------------------------------
// Local Variables
//----------------------------------------------------------------------------
static Demo* gpDemo = NULL; // The demo
static unsigned int gCurrentEditSnapshot = 0;
static bool gWasInstallCheckboxSet[ Demo::MixingSnapshots_Count ];

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
static void Initialize( HWND hDlg )
{
    ::memset( gWasInstallCheckboxSet, 0, sizeof( gWasInstallCheckboxSet ) );

    // We update 30 times a second using a timer
    SetTimer( hDlg, 0, 33, NULL );

    // The instance
    HINSTANCE hInst = (HINSTANCE) GetWindowLong( hDlg, GWL_HINSTANCE );

    // Set big and small icons
    HICON hIcon = LoadIcon( hInst, MAKEINTRESOURCE( IDI_FAKINGDSP ) );
    PostMessage( hDlg, WM_SETICON, ICON_BIG, ( LPARAM ) hIcon ); 
    PostMessage( hDlg, WM_SETICON, ICON_SMALL, ( LPARAM ) hIcon );

    // Initialize the sound system 
    SoundSystem::Initialize( hDlg );
    
    // Create a new demo
    gpDemo = new Demo( );

    // Prepare the sliders
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        HWND hSlider  = GetDlgItem( hDlg, CENTRAL_MIX_SLIDER_IDS[ i ] );
        PostMessage( hSlider, TBM_SETRANGEMIN, FALSE, 0L );
        PostMessage( hSlider, TBM_SETRANGEMAX, FALSE, SLIDER_RANGE );
        PostMessage( hSlider, TBM_SETPOS, TRUE, 0L );
    }
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        HWND hSlider  = GetDlgItem( hDlg, EDIT_SNAPSHOT_SLIDER_IDS[ i ] );
        PostMessage( hSlider, TBM_SETRANGEMIN, FALSE, 0L );
        PostMessage( hSlider, TBM_SETRANGEMAX, FALSE, SLIDER_RANGE );
        PostMessage( hSlider, TBM_SETPOS, TRUE, 0L );
    }
    MixingSnapshot* pSnapshot = gpDemo->GetMixingSnapshot( ( Demo::MixingSnapshots ) gCurrentEditSnapshot );
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        float volume = pSnapshot->GetVolume( ( MixingCategory ) i );    
        LONG sliderPos = ( LONG )( SLIDER_RANGE * ( 1.0f - volume ) );
        HWND hSlider  = GetDlgItem( hDlg, EDIT_SNAPSHOT_SLIDER_IDS[ i ] );        
        PostMessage( hSlider, TBM_SETPOS, TRUE, sliderPos );
    }
    
    // Update the checkbox displays to reflect the state of the system
    for ( unsigned int i = 0; i < Demo::MixingSnapshots_Count; i++ )
    {
        MixingSnapshot* pSnapshot = gpDemo->GetMixingSnapshot( ( Demo::MixingSnapshots ) i );

        WPARAM checkState = BST_UNCHECKED;
        if ( pSnapshot->GetIsInstalled( ) )
        {
            checkState = BST_CHECKED;
        }

        HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_INSTALL_IDS[ i ] );
        SendMessage( hCheck, BM_SETCHECK, checkState, NULL );
    }        
}

//-----------------------------------------------------------------------------
// Terminate
//-----------------------------------------------------------------------------
static void Terminate( void )
{
    if ( NULL != gpDemo )
    {
        delete gpDemo;
        gpDemo = NULL;
    }
    SoundSystem::Terminate( );
}

//-----------------------------------------------------------------------------
// UpdateCentralMixDisplay
//----------------------------------------------------------------------------
static void UpdateCentralMixDisplay( HWND hDlg )
{
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        float volume = MixingSystem::Get( )->GetCategoryVolume( ( MixingCategory ) i );
        LONG sliderPos = ( LONG )( SLIDER_RANGE * ( 1.0f - volume ) );
        HWND hSlider  = GetDlgItem( hDlg, CENTRAL_MIX_SLIDER_IDS[ i ] );        
        PostMessage( hSlider, TBM_SETPOS, TRUE, sliderPos );
    }
}

//-----------------------------------------------------------------------------
// UpdateEditMixDisplay
//----------------------------------------------------------------------------
static void UpdateEditMixDisplay( HWND hDlg )
{
    unsigned int oldSnapshot = gCurrentEditSnapshot;

    // Read checkbox states
    for ( unsigned int i = 0; i < Demo::MixingSnapshots_Count; i++ )
    {
        if ( i != gCurrentEditSnapshot )
        {
            HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_EDIT_IDS[ i ] );
            LRESULT checkValue = SendMessage( hCheck, BM_GETCHECK, NULL, NULL );
            if ( BST_CHECKED == checkValue )
            {
                gCurrentEditSnapshot = i;
                break;
            }
        }
    }

    // Set new checkbox states
    for ( unsigned int i = 0; i < Demo::MixingSnapshots_Count; i++ )
    {
        WPARAM checkState = BST_UNCHECKED;
        if ( i == gCurrentEditSnapshot )
        {
            checkState = BST_CHECKED;
        }
        
        HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_EDIT_IDS[ i ] );
        SendMessage( hCheck, BM_SETCHECK, checkState, NULL );
    }

    // Handle the snapshot edit sliders
    MixingSnapshot* pSnapshot = gpDemo->GetMixingSnapshot( ( Demo::MixingSnapshots ) gCurrentEditSnapshot );
    if ( oldSnapshot != gCurrentEditSnapshot )
    {
        // Set values if this is a new snapshot
        for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
        {
            float volume = pSnapshot->GetVolume( ( MixingCategory ) i );    
            LONG sliderPos = ( LONG )( SLIDER_RANGE * ( 1.0f - volume ) );
            HWND hSlider  = GetDlgItem( hDlg, EDIT_SNAPSHOT_SLIDER_IDS[ i ] );        
            PostMessage( hSlider, TBM_SETPOS, TRUE, sliderPos );
        }
    }
    else
    {
        // Read the values if this has been around for a while
        for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
        {
            HWND hSlider  = GetDlgItem( hDlg, EDIT_SNAPSHOT_SLIDER_IDS[ i ] );        
            LRESULT position = SendMessage( hSlider, TBM_GETPOS, 0, 0 );
            float volume = 1.0f - ( position / ( float ) SLIDER_RANGE );
            ASSERT( volume <= 1.0f );
            ASSERT( volume >= 0.0f );
            
            pSnapshot->SetVolume( ( MixingCategory ) i, volume );            
        }     
    }        
}

//-----------------------------------------------------------------------------
// UpdateSnapshotInstallDisplay
//----------------------------------------------------------------------------
static void UpdateSnapshotInstallDisplay( HWND hDlg )
{
    // Update the snapshots with the states of the checkboxes
    for ( unsigned int i = 0; i < Demo::MixingSnapshots_Count; i++ )
    {
        HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_INSTALL_IDS[ i ] );
        LRESULT checkValue = SendMessage( hCheck, BM_GETCHECK, NULL, NULL );
        
        MixingSnapshot* pSnapshot = gpDemo->GetMixingSnapshot( ( Demo::MixingSnapshots ) i );
        pSnapshot->SetIsInstalled( BST_CHECKED == checkValue );
    }
    
    // The mixing system needs to clean out the base layer
    MixingSnapshot::CleanBaseLayer( );
    
    // Update the checkbox displays to reflect the state of the system
    for ( unsigned int i = 0; i < Demo::MixingSnapshots_Count; i++ )
    {
        MixingSnapshot* pSnapshot = gpDemo->GetMixingSnapshot( ( Demo::MixingSnapshots ) i );

        bool isInstalled = pSnapshot->GetIsInstalled( );
        WPARAM checkState = BST_UNCHECKED;
        if ( isInstalled )
        {
            checkState = BST_CHECKED;
        }

        // The system is easier to use if we move the edit check box
        // when a new install box is selected.
        if ( isInstalled && !gWasInstallCheckboxSet[ i ] )
        {
            HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_EDIT_IDS[ i ] );
            SendMessage( hCheck, BM_SETCHECK, BST_CHECKED, NULL );        
        }
        gWasInstallCheckboxSet[ i ] = isInstalled;

        HWND hCheck = GetDlgItem( hDlg, SNAPSHOT_INSTALL_IDS[ i ] );
        SendMessage( hCheck, BM_SETCHECK, checkState, NULL );
    }        
}

//-----------------------------------------------------------------------------
// Update
//----------------------------------------------------------------------------
static void Update( HWND hDlg )
{
    // Update the demo.
    if ( NULL != gpDemo )
    {   
        gpDemo->Update( );
    }
    
    // Update edit snapshot display
    UpdateEditMixDisplay( hDlg );
    
    // Update the slider positions of the central mix display
    UpdateCentralMixDisplay( hDlg );
    
    // Update the slider positions of the central mix display
    UpdateSnapshotInstallDisplay( hDlg );
}

//-----------------------------------------------------------------------------
// Handle Windows Messages
//----------------------------------------------------------------------------
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg) 
    {
        case WM_INITDIALOG:
        {
            Initialize( hDlg );
            break;
        }            
        case WM_COMMAND:
        {
            switch ( LOWORD(wParam) )
            {
                case IDCANCEL:
                {
                    PostQuitMessage( 0 );
                    EndDialog( hDlg, IDCANCEL );
                    break;
                }                    
                default:
                {
                    return false;
                }                    
            }
            break;
        }            
        case WM_TIMER:
        {
            Update( hDlg );
            break;
        }        
        case WM_DESTROY:
        {
            Terminate( );        
            break; 
        }
        default:
        {
            return false;
        }            
    }
    return true;
}

//----------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------
INT APIENTRY WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT nCmdShow )
{
    // Initialize common controls because we use sliders
    InitCommonControls( );
    
    // Display the main dialog box.
    DialogBox( hInst, MAKEINTRESOURCE(ID_MAIN), NULL, MainDlgProc );

    return TRUE;
}


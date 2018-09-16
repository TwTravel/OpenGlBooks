// WinRect.c
// OpenGL SuperBible
// Code by Richard S. Wright Jr.
// Chapter 13, demonstrates just drawing a rectangle using the Windows GDI


#include <windows.h>            // Standard Windows include file


// Storeage for the name of the program and window caption
static LPCTSTR lpszAppName = "WinRect";

// Handles to GDI Brushes we will use for drawing
HBRUSH hBlueBrush,hRedBrush;


// Declaration for window procedure, this processes the windows message.
LRESULT CALLBACK WndProc(   HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam);


// Main entry point for the program
int APIENTRY WinMain(   HINSTANCE       hInstance,
						HINSTANCE       hPrevInstance,
						LPSTR           lpCmdLine,
						int             nCmdShow)
	{
	MSG             msg;            // Message Structure
	WNDCLASS        wc;             // Window Class structure
	HWND            hWnd;           // Window handle of main window

	// Create a blue and red brush for drawing and filling
	// Operations.                    
	hBlueBrush = CreateSolidBrush(RGB(0, 0, 255)); 
	hRedBrush = CreateSolidBrush(RGB(255, 0, 0));

	// Register Window style
	wc.style                = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc          = (WNDPROC) WndProc;
	wc.cbClsExtra           = 0;
	wc.cbWndExtra           = 0;
	wc.hInstance            = hInstance;
	wc.hIcon                = NULL;
	wc.hCursor              = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground        = hBlueBrush; // Use blue brush for background
	wc.lpszMenuName         = NULL;
	wc.lpszClassName        = lpszAppName;

	// Register the window class
	if(RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
					lpszAppName,
					lpszAppName,
					WS_OVERLAPPEDWINDOW,
					100, 100,               // Size and dimensions of window
					250, 250,
					NULL,
					NULL,
					hInstance,
					NULL);

	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while(GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}


	// Clean up by deleting old brushes
	DeleteObject(hBlueBrush);
	DeleteObject(hRedBrush);

	return msg.wParam;
	}



// Window procedure, handles all messages for this window
LRESULT CALLBACK WndProc(       HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam)
	{
	switch (message)
		{
		// Application termination 
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			HBRUSH hOldBrush;

			// Start painting 
			BeginPaint(hWnd,&ps);

			// Select and use the red brush
			hOldBrush = SelectObject(ps.hdc,hRedBrush);

			// Draw a rectangle filled with the currently
			// selected brush
			Rectangle(ps.hdc,100,100,150,150);

			// Deselect the brush
			SelectObject(ps.hdc,hOldBrush);
			
			// End painting
			EndPaint(hWnd,&ps);
			}
			break;
      
    	default:          // Passes it on if unproccessed
	        return (DefWindowProc(hWnd, message, wParam, lParam));
	    }

    return (0L);
	}

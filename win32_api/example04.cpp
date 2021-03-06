// This program create a window what pass a grid of right to left in a gamma of colors between blue and green
//need the linker the librearies user32.lib and gdi32.lib
#include<windows.h>
#include<stdint.h>

#define internal static
#define local_persist static
#define global_variable static
//This is global for now
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
global_variable bool Running;
//this will be erase later


global_variable int BitmapWidth;
global_variable int BitmapHeight;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset){
		
		int Width = BitmapWidth; 
		int Height = BitmapHeight;
		int Pitch = Width*BytesPerPixel;
		uint8 *Row = (uint8 *)BitmapMemory;
		for(int Y = 0;
			Y < BitmapHeight;
			++Y){
				uint8 *Pixel = (uint8 *)Row;
				for(int X = 0; 
					X < BitmapWidth;
					++X){
					//Pixel in memory : 0(8bits)
					//little eENDIAN architecture (8 bits color first bite in the first place)
					//BB GG RR xx order from the color
					
					*Pixel = (uint8)(X + XOffset);
					++Pixel;	
				
					*Pixel = (uint8)(Y + YOffset);
					++Pixel;	
				
					*Pixel = 0;
					++Pixel;	
				
					*Pixel = 0;
					++Pixel;	
					}
				Row += Pitch;
			}
}
	 


internal void
Win32ResizeDIBSection(int Width, int Height){
	
	//maybe dont free firts this, then free first if that fails
	if(BitmapMemory){
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	BitmapHeight = Height;
	BitmapWidth = Width;
	
 	BitmapInfo.bmiHeader.biSize  = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	
	
	int BitmapMemorySize = (BitmapHeight*BitmapWidth)*BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
		

}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X,int Y, int Width, int Height){
	
	int WindowWidth = ClientRect->right - ClientRect->left;
	int WindowHeight = ClientRect->bottom - ClientRect->top;   
	StretchDIBits(DeviceContext,
				/*
				  X, Y, Width, Height,
				  X, Y, Width, Height,
				  */
				  0, 0, BitmapWidth, BitmapHeight,
				  0, 0, WindowWidth, WindowHeight,
				  BitmapMemory,
				  &BitmapInfo,
				  DIB_RGB_COLORS, SRCCOPY);
}

//Is like it sounds!
LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window,
						UINT Message,
						WPARAM  WParam,
						LPARAM LParam){
						
	LRESULT Result = 0;
	
	switch(Message){
		
		case WM_SIZE:{
			RECT ClientRect;
			GetClientRect(Window,&ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;
			Win32ResizeDIBSection(Width, Height);
		//	OutputDebugStringA("WM_SIZE\n");			
		}break;
		
		case WM_DESTROY:{
			//handle this with a message to the user?
			Running = false;
			OutputDebugStringA("WM_DESTROY\n");
		}break;
		
		case WM_CLOSE:{
			//handle this as a error?
			Running = false;
			OutputDebugStringA("WM_CLOSE \n");
		}break;
		
		case WM_ACTIVATEAPP:{
			OutputDebugStringA("WM_ACTIVATEAPP \n");
		}break;
		
		case WM_PAINT:{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &Paint);
		}break;
		
		default:{
			OutputDebugStringA("Default \n");
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}		break;
	}

	return(Result);
}
	
	
int CALLBACK
WinMain(HINSTANCE Instance,
		HINSTANCE PrevInstance,
		LPSTR CommandLine,
 		int ShowCode){

		
	//Window Class	
	WNDCLASS WindowClass = {};
	
	//Init Struc Window Class
	// Checar si HRERAW/VREDRAW/OWMDC
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon ;
	WindowClass.lpszClassName = "FinalFartClass";
	
	
	if(RegisterClass(&WindowClass)){
		HWND Window =
			CreateWindowEx(
							0,
							WindowClass.lpszClassName,
							"Final Fart",
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							0,
							0,
							Instance,
							0);
		if(Window){
			
			int XOffset = 0;
			int YOffset = 0;
			
			Running = true;
			while(Running){
				
				
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)){
					
					if(Message.message == WM_QUIT){
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);	
					
				}
				RenderWeirdGradient(XOffset, YOffset);
				
				HDC DeviceContext = GetDC(Window);
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;   
				Win32UpdateWindow( DeviceContext, &ClientRect, 0,0,WindowWidth,WindowHeight);
				ReleaseDC(Window, DeviceContext);
				
				
				++XOffset;
			}
				
		}else{
				//Logging
		}

	}else{
		//Logging
	}
	 
	return (0);
}

// This program create a window what changes between black and white
//need the linker the librearies user32.lib and gdi32.lib
#include<windows.h>

//Is like it sounds!
LRESULT CALLBACK 
MainWindowCallback(HWND Window,
		   UINT Message,
		   WPARAM  WParam,
		   LPARAM LParam){
	
	LRESULT Result = 0;
	
	switch(Message){
		
		case WM_SIZE:{
			OutputDebugStringA("WM_SIZE\n");			
		}break;
		
		case WM_DESTROY:{
			OutputDebugStringA("WM_DESTROY\n");
		}break;
		
		case WM_CLOSE:{
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
			static DWORD Operation = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, Operation);
			if(Operation == WHITENESS){
				Operation = BLACKNESS;
			}else{
				Operation = WHITENESS;
			}
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
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon ;
	WindowClass.lpszClassName = "FinalFartClass";
	
	if(RegisterClass(&WindowClass)){
		HWND WindowHandle =
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
	if(WindowHandle){
		for(;;){
			MSG Message;
			BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
			if(MessageResult > 0){
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}else{
				break;
			 }			
		}		
	}else{
				//Logging
     	}
	}else{
		//Logging
	} 
return (0);
}

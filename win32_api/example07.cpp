//Buffer for sound
//need the linker the librearies user32.lib and gdi32.lib
#include<windows.h>
#include<stdint.h>
#include<xinput.h>
#include<dsound.h>

#define internal static
#define local_persist static
#define global_variable static
//This is global for now
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int bool32;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
//this will be erase later

struct win32_offscreen_buffer{
	//NOTE(ornitorrincco): Pixels are always 32-bits wide, Memory Order BB GG RR XX
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
 };


struct win32_window_dimension{
	int Width;
	int Height;
};

//TODO(ornitorrincco): This is a global for now
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

//NOTE(ornitorrincco): XinputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState) 
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub){
	return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

//NOTE(ornitorrincco): XinputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub){
	return(ERROR_DEVICE_NOT_CONNECTED);
}

global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

internal void
Win32LoadXInput(void){
	//library is so old :V 
	//TODO(ornitorrincco): Test this on windows 8
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(!XInputLibrary){
			XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	
	if(XInputLibrary){
		XInputGetState =(x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		if(!XInputGetState){
			XInputGetState = XInputGetStateStub;
		}
		XInputSetState =(x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
		if(!XInputSetState){
			XInputSetState = XInputSetStateStub;	
		}
		//TODO(ornitorrincco): Diagnostic
	}else{
		//TODO(ornitorrincco): Diagnostic
	}
}



internal void
Win32InitDSound(HWND Window,int32 SamplesPerSecond,int32 BufferSize ){
	//NOTE(ornitorrincco): Load the library
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
	
	if(DSoundLibrary){
		
		//NOTE(ornitorrincco):Get a DirectSound object - cooperative
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)
			GetProcAddress(DSoundLibrary,"DirectSoundCreate");
		//TODO(ornitorrincco): Double-Checl
		LPDIRECTSOUND DirectSound;
		if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))){
			
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = SamplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;
			
			if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))){
				
				DSBUFFERDESC BufferDescription ={};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				
				//TODO(ornitorrincco): DSBCAPS_GLOBALFOCUS
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))){
					if(SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))){
						//NOTE(ornitorrincco): We have finallly set the format!
					}else{
							//TODO:(ornitorrincco): Diagnostic
					}
				}else{
					//TODO:(ornitorrincco): Diagnostic
				}
			}else{
				//TODO:(ornitorrincco): Diagnostic
			}
			
			//TODO(ornitorrincco): 
			DSBUFFERDESC BufferDescription ={};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
			LPDIRECTSOUNDBUFFER SecondaryBuffer;
			if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0))){
			}
		}else{
			
				//TODO(ornitorrincco): Diagnostic		
		}
	}else{
		//TODO(ornitorrincco): Diagnostic
	}
}


internal win32_window_dimension 
Win32GetWindowDimension(HWND Window){
	
	win32_window_dimension Result;
	
 	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;   
	
	return(Result);
}

internal void
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int BlueOffset,int GreenOffset){
		
	
		uint8 *Row = (uint8 *)Buffer->Memory;
		
		for(int Y = 0;Y < Buffer->Height; ++Y){
				
				uint32 *Pixel = (uint32 *)Row;
				
				for(int X = 0;X < Buffer->Width;++X){
					//Pixel in memory : 0(8bits)
					//little eENDIAN architecture (8 bits color first bite in the first place)
					//BB GG RR xx order from the color
					
					uint8 Blue = (X + BlueOffset);
					uint8 Green = (Y + GreenOffset);
					/* 
						Memory:    BB GG RR xx
						Register:  xx RR GG BB
						
						Pixel (32-bits)
					*/
					*Pixel++ = ((Green << 8) | Blue);
					
					}
				Row += Buffer->Pitch;
			}
}
	 


internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer,int Width, int Height){
	
	//maybe dont free firts this, then free first if that fails
	if(Buffer->Memory){
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
	Buffer->Height = Height;
	Buffer->Width = Width;
	
	int BytesPerPixel = 4;
	
	
 	Buffer->Info.bmiHeader.biSize  = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height; //change the orientation
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	
	
	int BitmapMemorySize = (Buffer->Height*Buffer->Width)*BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width*BytesPerPixel;
		
	
	
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
							HDC DeviceContext, int WindowWidth, int WindowHeight){
	
	
	//Aspect ratio correction
	StretchDIBits(DeviceContext,
				/*
				  X, Y, Width, Height,
				  X, Y, Width, Height,
				  */
				  0, 0, WindowWidth, WindowHeight,
				  0, 0, Buffer->Width, Buffer->Height,
				  Buffer->Memory,
				  &Buffer->Info,
				  DIB_RGB_COLORS, SRCCOPY);
}

//Is like it sounds!
internal LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window,
						UINT Message,
						WPARAM  WParam,
						LPARAM LParam){
						
	LRESULT Result = 0;
	
	switch(Message){
		
		case WM_CLOSE:{
			//handle this as a error?
			GlobalRunning = false;
			//OutputDebugStringA("WM_CLOSE \n");
		}break;
		
		case WM_ACTIVATEAPP:{
			OutputDebugStringA("WM_ACTIVATEAPP \n");
		}break;
	
		case WM_DESTROY:{
			//handle this with a message to the user?
			GlobalRunning = false;
			//OutputDebugStringA("WM_DESTROY\n");
		}break;
		
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:{
			
			uint32 VKCode = WParam;
			//if still stroke the same letter
			bool WasDown =((LParam & (1 << 38)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);
			if(WasDown != IsDown){
								
				if(VKCode == 'W'){
				 
				}else if(VKCode == 'A'){
					
				}else if(VKCode == 'S'){
				
				}else if(VKCode == 'D'){
				
				}else if(VKCode == 'Q'){
				
				}else if(VKCode == 'E'){
				
				}else if(VKCode == VK_UP){
				
				}else if(VKCode == VK_LEFT){
				
				}else if(VKCode == VK_DOWN){
				
				}else if(VKCode == VK_RIGHT){
				
				}else if(VKCode == VK_ESCAPE){
					OutputDebugStringA("Escape:");
					if(IsDown){
						OutputDebugStringA("IsDown");
					}
					if(WasDown){
						OutputDebugStringA("WasDown");
					}
					OutputDebugStringA("\n");
				
				
				}else if(VKCode == VK_SPACE){
				
				}	
			}
			
			
			bool32 AltKeyWasDown = (LParam & (1 << 29));
			if((VKCode == VK_F4) && AltKeyWasDown){
				GlobalRunning = false;
			}
		}break;
		
		case WM_PAINT:{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
										Dimension.Width, Dimension.Height);
										
								
			EndPaint(Window, &Paint);
		}break;
		
		default:{
			//OutputDebugStringA("Default \n");
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

	Win32LoadXInput(); 
	//Window Class	
	//take the value of zero, format ANSI (without A is UNICODE)
	WNDCLASSA WindowClass = {};
	
	//Init Struc Window Class
	
	Win32ResizeDIBSection(&GlobalBackbuffer,1280, 720);
	
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
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
			
			//NOTE(ornitorrincco): since we specified CS_OWNDC, we can just get one
			//device context and use it forever because we are not sharing it with anyone.
			
			HDC DeviceContext = GetDC(Window);
			
			int XOffset = 0;
			int YOffset = 0;
			 
			Win32InitDSound(Window, 48000,48000*sizeof(int16)*2);
			GlobalRunning = true;	
			while(GlobalRunning){
				
				MSG Message; 
				
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)){
					
					if(Message.message == WM_QUIT){
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);	
					
				}
				
				for(DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex){
					
					XINPUT_STATE ControllerState;
					if(XInputGetState(ControllerIndex,&ControllerState) == ERROR_SUCCESS){
						//NOTE(ornitorrincco) The controller is plugged
						//NOTE(ornitorrincco) See if controllerState.dwPacketNumber increements
						//to rapid
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
						
						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right  = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start  = (Pad->wButtons & XINPUT_GAMEPAD_START); 
						bool Back  = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder  = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder  = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton  = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton  = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton  = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);
						
						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;
						if(AButton){
							YOffset += 2; 
						}
					}else{
						//NOTE(ornitorrincco) The controller is not available
					}
				
				}
				
				RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
				
				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
											Dimension.Width, Dimension.Height);
											
				
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
  

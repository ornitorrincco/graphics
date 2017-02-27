//TODO(ornitorrincco)update: 
//need the linker the librearies user32.lib and gdi32.lib
/*
TODO(ornitorrincco): + already, - for do
 
	+Create a entry point for windows(a window)
	+Control and Translate Message from windows
	+Buffer for render
	+Buffer for sound
	+Inputs (Keyboard and joystick commands controls)
	+debug tools
	
	- Saved game locations
	- Getting a handle to our own executable file
	- Asset loading path
	- Threading (launch a thread)
	- Raw Input (support for multiple keyboards)
	- Sleep/timebeginPeriod
	- FullScreen support
	- WM_SETCURSOR (control cursor visibility)
	- QueryCancelAutoplay
	- WM_ACTIVATEAPP(for when we are not the active application)
	- Blit speed improvements (BitBlt)
	- Hardware acceleration (OpenGL or Direct3D or BOTH)
	- GetKeyboardLayout (for French keyboards, international WASD support)
	
	Just a partial List
*/
#include<stdint.h>
#include<windows.h>

#include<stdio.h>
#include<xinput.h>
#include<dsound.h>
#include<math.h>

#define internal static
#define local_persist static
#define global_variable static
//This is global for now

#define Pi32 3.14159265359f
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#include"finalfart.h"
#include"FinalFart.cpp"
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
global_variable LPDIRECTSOUNDBUFFER GlobalPrimaryBuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

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

void *
PlatformLoadFile(char *FileName){
	
	//NOTE(ornitorrincco): Implements the win32 file loading
	return(0);
}

internal void
Win32LoadXInput(void){
	//library is so old :V 
	//TODO(ornitorrincco): Test this on windows 8
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(!XInputLibrary){
			XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
	}
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
Win32InitDSound(HWND Window,
				int32 SamplesPerSecond,
				int32 BufferSize ){
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
					HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
					if(SUCCEEDED(Error)){
						OutputDebugStringA("Primary buffer format was set .\n");
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
			BufferDescription.dwFlags = 0; //DSBCAPS_PRIMARYBUFFER
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;
						
			HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer,0);
			if(SUCCEEDED(Error)){
				OutputDebugStringA("Secondary buffer created succesfully.\n");
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
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
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
			
			
			bool32 AltKeyWasDown = (LParam & (1 << 29)); //!= 0  ???
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

struct win32_sound_output{
	//NOTE(ornitorrincco): Sound Test
		int SamplesPerSecond; 
		int ToneHz;
		int16 ToneVolumen;
		uint32 RunningSampleIndex;
		int WavePeriod;  
		int BytesPerSample;
		int SecondaryBufferSize;
		real32 tSine;
		int LatencySampleCount;
		};	
		
		
internal void 
Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite){			

			VOID *Region1;
			DWORD Region1Size;
			VOID  *Region2;
			DWORD Region2Size; 
			if(SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock,BytesToWrite,
													 &Region1,&Region1Size,
													 &Region2,&Region2Size,
													 0))){
							
						//TODO(ornitorrincco): Collapse these two loops
				DWORD Region1SampleCount = Region1Size/SoundOutput->BytesPerSample;
				int16 *SampleOut = (int16 *)Region1;		
				for(DWORD SampleIndex = 0;SampleIndex < Region1SampleCount; ++SampleIndex){
							
					real32 SineValue = sinf(SoundOutput->tSine); 
					int16 SampleValue = (int16)(SineValue * SoundOutput->ToneVolumen);					 
				
					*SampleOut++ = SampleValue;
					*SampleOut++ = SampleValue;
							
					SoundOutput->tSine += 2.0f*Pi32*1.0f/(real32)SoundOutput->WavePeriod; 
					++SoundOutput->RunningSampleIndex;
					}
						
				DWORD Region2SampleCount = Region2Size/SoundOutput->BytesPerSample;
				SampleOut = (int16 *)Region2;
				for(DWORD SampleIndex = 0;SampleIndex < Region2SampleCount; ++SampleIndex){
							
					real32 SineValue = sinf(SoundOutput->tSine); 
					int16 SampleValue = (int16)(SineValue *SoundOutput->ToneVolumen);
					*SampleOut++ = SampleValue;
					*SampleOut++ = SampleValue;
					
					SoundOutput->tSine += 2.0f*Pi32*1.0f/(real32)SoundOutput->WavePeriod; 
					++SoundOutput->RunningSampleIndex;
					}
				GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
			}
}
	
int CALLBACK
WinMain(HINSTANCE Instance,
		HINSTANCE PrevInstance,
		LPSTR CommandLine,
 		int ShowCode){
			
	//is like it sounds		
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
	

	Win32LoadXInput(); 
	//Window Class	
	//take the value of zero, format ANSI (without A is UNICODE)
	WNDCLASSA WindowClass = {}; //WNDCLASSEXA?
	//Init Struc Window Class
	Win32ResizeDIBSection(&GlobalBackbuffer,1280, 720);
	
	WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon ;
	WindowClass.lpszClassName = "FinalFartClass";
	
	if(RegisterClassA(&WindowClass)){
		HWND Window =
			CreateWindowExA(
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
			
			//NOTE(ornitorrincco): Graphic Test
			int XOffset = 0;
			int YOffset = 0; 
			
			win32_sound_output SoundOutput = {};
	
			SoundOutput.SamplesPerSecond = 48000; 
			SoundOutput.ToneHz = 256;
			SoundOutput.ToneVolumen = 900;
			SoundOutput.RunningSampleIndex = 0;
			SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond/SoundOutput.ToneHz;  
			SoundOutput.BytesPerSample = sizeof(int16)*2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond*SoundOutput.BytesPerSample;

			Win32InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
			Win32FillSoundBuffer(&SoundOutput, 0, SoundOutput.SecondaryBufferSize);
			GlobalSecondaryBuffer->Play(0,0, DSBPLAY_LOOPING);
			
			
			GlobalRunning = true;
			//clocks form the processor
			LARGE_INTEGER LastCounter;
			QueryPerformanceCounter(&LastCounter);
			
			uint64 LastCycleCount = __rdtsc();
			
			
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
						
						XOffset += StickX /4096;
						YOffset += StickY /4096;
						
						SoundOutput.ToneHz = 512 + (int)(256.0f*((real32)StickY / 30000.0f));
						SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond/SoundOutput.ToneHz;
 					}else{
						//NOTE(ornitorrincco) The controller is not available
					}
				
				}
				game_offscreen_buffer Buffer = {};
				Buffer.Memory =GlobalBackbuffer.Memory;
				Buffer.Width = GlobalBackbuffer.Width;
				Buffer.Height = GlobalBackbuffer.Height;
				Buffer.Pitch = GlobalBackbuffer.Pitch;
				GameUpdateAndRender(&Buffer, XOffset, YOffset);
	
				//NOTE(ornitorrincco): DirectSound output test
				DWORD PlayCursor;
				DWORD WriteCursor;
				if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor))){
				
					DWORD ByteToLock = ((SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) %
											SoundOutput.SecondaryBufferSize);
					
					DWORD TargetCursor = ((PlayCursor + 
											(SoundOutput.LatencySampleCount*SoundOutput.BytesPerSample)) %
											SoundOutput.SecondaryBufferSize);
					DWORD BytesToWrite;
					//TODO(ornitorrincco): Change this to using a lower latency offset from the playcursor
					// when we actualli having sound effects
					if(ByteToLock > TargetCursor){
						
							BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock);
							BytesToWrite += TargetCursor;
						
					}else{
						
				 		BytesToWrite = TargetCursor - ByteToLock;
						
					}
					
					Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite);
					
				}
				
				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
											Dimension.Width, Dimension.Height);
											

				uint64 EndCycleCount = __rdtsc();
				
				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);

					//TODO(ornitorrincco): Display the value here
				uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
				int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
				
				//we obtain the second of latency in ms
				real64 MSPerFrame = ((1000.0f*(real64)CounterElapsed) / (real64)PerfCountFrequency); 
				real64 FPS = (real64)PerfCountFrequency / (real64)CounterElapsed;
				real64 MCPF = (real64)(CyclesElapsed/(1000.0f*1000.0f));
#if 0
				char Buffer[256];
				sprintf(Buffer,"%.02fms/f, %.02ff/s,%.02fmc/f\n", MSPerFrame,FPS, MCPF);
				OutputDebugStringA(Buffer);
				
#endif
				LastCounter = EndCounter;
				LastCycleCount = EndCycleCount;
			}
				
		}else{
				//Logging
		}

	}else{
		//Logging
	}
	 
	return (0);
}

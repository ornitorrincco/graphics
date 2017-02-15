// This program show a message box(Ansi) with a legend, is necesarry to use user32.lib (linker)


#include<windows.h>
//entry point
int CALLBACK
WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
 		int nCmdShow){

	MessageBoxA(0, "This is underated","Final Fart", 
		MB_OK | MB_ICONINFORMATION);

	return (0);
}

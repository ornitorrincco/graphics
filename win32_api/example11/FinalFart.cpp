#include "finalfart.h"

internal void
RenderWeirdGradient(game_offscreen_buffer *Buffer, int BlueOffset,int GreenOffset){
		
	
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
GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset){
	BlueOffset = 0;
	GreenOffset = 0;
	RenderWeirdGradient(Buffer, BlueOffset, GreenOffset);

}
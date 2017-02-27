#if !defined(FINALFART_H)
/* 
	TODO(ornitorrincco): Services that the platform layer provides to the game
*/
/* 
	Note(ornitorrincco): Services that the game provides to the platform layer.
	(Thus may expand in the future - spund on separate thread, etc.)	
*/
// FOUR THINGS - timing, controller/keyboard input, bitmap buffer yo use, sound buffer to use
struct game_offscreen_buffer{
//NOTE(ornitorrincco): Pixels are always 32-bits wide, Memory Order BB GG RR XX

	void *Memory;
	int Width;
	int Height;
	int Pitch;
	
};

struct game_sound_output_buffer{
	int SamplesPerSecond;
	int SampleCount;
	int16 *Samples;
};

internal void 
GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset,
					game_sound_output_buffer *SoundBuffer);
 
 #define FINALFART_H
 #endif 
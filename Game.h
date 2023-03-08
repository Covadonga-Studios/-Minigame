#pragma once

#include "SDL/include/SDL.h"
#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )


#include "SDL_Mixer/include/SDL_mixer.h"
#pragma comment( lib, "SDL_Mixer/lib_x86/SDL2_mixer.lib")

#include "Entity.h"

#define WINDOW_WIDTH	1520
#define WINDOW_HEIGHT	980
#define MAX_KEYS		256
#define MAX_SHOTS		1
#define MAX_SHOTS2		1
#define MAX_TRACKS		8

class Game
{
public:
	Game();
	~Game();

	bool Init();
	bool LoadAudios();
	bool LoadImages();
	void Release();
	
	bool Input();
	bool Update();
	void Draw();

private:
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	SDL_Texture* img_background, * img_player, * img_shot, * img_player2;

	Entity Player, Shots[MAX_SHOTS], Scene;
	Entity Player2, Shots2[MAX_SHOTS2];
	Entity HP1;
	Entity HP2;
	int idx_shot;
	int idx_shot2;


	enum KEY_STATE { KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP	};
	KEY_STATE keys[MAX_KEYS]; 

	Mix_Music* tracks[MAX_TRACKS];
	int num_tracks;

	Mix_Chunk* sfxs[MAX_SHOTS];
	int num_sfxs;
	
};

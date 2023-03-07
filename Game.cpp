#include "Game.h"
#include <math.h>

Game::Game() {}
Game::~Game(){}

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}
	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	//Init variables
	Player.Init(20, WINDOW_HEIGHT >> 1, 104, 82, 5);
	Player2.Init(1700, WINDOW_HEIGHT >> 1, 104, 82, 5);
	idx_shot = 0;
	idx_shot2 = 0;

	//Entities textures
	IMG_Init;
	SDL_CreateTextureFromSurface;

	return true;
}
void Game::Release()
{
	//Clean up all SDL initialized subsystems
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	//Player 1

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT)	fy = -1;
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT)	fy = 1;
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT)	fx = -1;
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		int x, y, w, h;
		Player.GetRect(&x, &y, &w, &h);
		Shots[idx_shot].Init(x + w - 10, y + (h >> 1) - 3, 56, 20, 10);
		idx_shot++;
		idx_shot %= MAX_SHOTS;
	}
	

	//Logic
	//Player update
	Player.Move(fx, fy);
	//Shots update
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].Move(1, 0);
			if (Shots[i].GetX() > WINDOW_WIDTH)	Shots[i].ShutDown();
		}
	}

	//Player 1

//Process Input
	int fx2 = 0, fy2 = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT)	fy2 = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT)	fy2 = 1;
	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT)	fx2 = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT)	fx2 = 1;
	if (keys[SDL_SCANCODE_RCTRL] == KEY_DOWN)
	{
		int x, y, w, h;
		Player2.GetRect(&x, &y, &w, &h);
		Shots2[idx_shot2].Init(x - w + 70, y + (h >> 1) - 3, 56, 20, 10);
		idx_shot2++;
		idx_shot2 %= MAX_SHOTS;
	}


	//Logic
	//Player update
	Player2.Move(fx2, fy2);
	//Shots update
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].Move(-1, 0);
			if (Shots2[i].GetX() > WINDOW_WIDTH)	Shots2[i].ShutDown();
		}
	}
		
	return false;
}
void Game::Draw()
{
	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	//Draw player
	SDL_Rect rc;
	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_SetRenderDrawColor(Renderer, 0, 192, 0, 255);
	SDL_RenderFillRect(Renderer, &rc);

	//Draw shots
	SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_RenderFillRect(Renderer, &rc);
		}
	}

	
	//Draw player2
	SDL_Rect rc2;
	Player2.GetRect(&rc2.x, &rc2.y, &rc2.w, &rc2.h);
	SDL_SetRenderDrawColor(Renderer, 0, 192, 255, 255);
	SDL_RenderFillRect(Renderer, &rc2);
	
	//Draw shots
	SDL_SetRenderDrawColor(Renderer, 192, 0, 255, 255);
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].GetRect(&rc2.x, &rc2.y, &rc2.w, &rc2.h);
			SDL_RenderFillRect(Renderer, &rc2);
		}
	}

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}

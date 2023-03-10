#include "Game.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>

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

	//Load images
	if (!LoadImages())
		return false;

	// Init SDL_Mixer
	int flags = MIX_INIT_OGG;
	if (Mix_Init(flags) != flags) {
		SDL_Log("Failed to init OGG module for SDL_Mixer!\n");
		SDL_Log("Mix_Init: %s\n", Mix_GetError());
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		SDL_Log("Failed to init SDL_Mixer!\n");
		SDL_Log("Mix_OpenAudio: %s\n", Mix_GetError());
		return false;
	}
	
	if (!LoadAudios())
		return false;


	int w;
	SDL_QueryTexture(img_background, NULL, NULL, &w, NULL);
	Scene.Init(0, 0, w, WINDOW_HEIGHT, 2);
	

	//Init variables
	Player.Init(20, WINDOW_HEIGHT >> 1, 104, 82, 5);
	Player2.Init(1400, WINDOW_HEIGHT >> 1, 104, 82, 5);
	HP1.Init(20, WINDOW_HEIGHT >> 1, 104, 200, 5);
	idx_shot = 0;
	idx_shot2 = 0;
	objects.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	objects2.Init(1000, WINDOW_HEIGHT >> 5, 50, 50, 1);
	objects.setid();
	objects2.setid();


	
	//Entities textures
	IMG_Init;
	SDL_CreateTextureFromSurface;

	return true;
}

bool Game::LoadAudios() {
	num_tracks = 0;
	tracks[num_tracks++] = Mix_LoadMUS("western.ogg");

	Mix_PlayMusic(tracks[0], -1);

	sfxs[num_sfxs++] = Mix_LoadWAV("disparo.wav");

	return true;
}

bool Game::LoadImages()
{
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Log("IMG_Init, failed to init required png support: %s\n", IMG_GetError());
		return false;
	}
	img_background = SDL_CreateTextureFromSurface(Renderer, IMG_Load("paisaje2.png"));
	if (img_background == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_player = SDL_CreateTextureFromSurface(Renderer, IMG_Load("player1.png"));
	if (img_player == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_player2 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("player2.png"));
	if (img_player == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_shot = SDL_CreateTextureFromSurface(Renderer, IMG_Load("shot.png"));
	if (img_shot == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	return true;
}
void Game::Release()
{
	//Release images
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player);
	SDL_DestroyTexture(img_player2);
	SDL_DestroyTexture(img_shot);
	IMG_Quit();
	


	// Free Audios
	for (int i = 0; i < num_tracks; ++i)
		Mix_FreeMusic(tracks[i]);
	for (int i = 0; i < num_sfxs; ++i)
		Mix_FreeChunk(sfxs[i]);

	// Close SDL_Mixer
	Mix_CloseAudio();
	while(Mix_Init(0))
		Mix_Quit();
		
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

	

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT && Player.GetY() > 85 - Player.H())	fy = -1;
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT && Player.GetY() < 980 - Player.H())	fy = 1;
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT && Player.GetX() > 0)	fx = -1;
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT && Player.GetX() < 680 - Player.W())	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		int x, y, w, h;
		Player.GetRect(&x, &y, &w, &h);
		Shots[idx_shot].Init(x + w - 10, y + (h >> 1) - 3, 30, 15, 15);
		idx_shot++;
		idx_shot %= MAX_SHOTS;



		// Play a single Sound
		Mix_PlayChannel(-1, sfxs[0], 0);
	}

	
	//Scene scroll
	Scene.Move(0, -0.5);
	if (Scene.GetY() <= -Scene.H())	Scene.SetY(0);
	//Player update
	Player.Move(fx, fy);
	//Shots update
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].Move(1, 0);
			if (Shots[i].GetX() > WINDOW_WIDTH) 
			{
				Shots[i].ShutDown();
				objects.pickedupp(0);
			}
		}
	}

	//Player 2

//Process Input
	int fx2 = 0, fy2 = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN )	return true;
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT && Player2.GetY() > 85 - Player2.H())	fy2 = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT && Player2.GetY() < 980 - Player2.H())	fy2 = 1;
	if (keys[SDL_SCANCODE_LEFT]  == KEY_REPEAT && Player2.GetX() > 820)	fx2 = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT && Player2.GetX() < 1420)fx2 = 1;
	if (keys[SDL_SCANCODE_RSHIFT] == KEY_DOWN)
	{
		int x, y, w, h;
		Player2.GetRect(&x, &y, &w, &h);
		Shots2[idx_shot2].Init(x - w + 70, y + (h >> 1) - 3, 30, 15, 15);
		idx_shot2++;
		idx_shot2 %= MAX_SHOTS;



		Mix_PlayChannel(-1, sfxs[0], 0);
	}

	//Player2 update
	Player2.Move(fx2, fy2);
	//Shots2 update
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].Move(-1, 0);
			if (Shots2[i].GetX() > WINDOW_WIDTH) 
			{
				Shots2[i].ShutDown();
				objects2.pickedupp(0);

			}
		}
	}
	return false;
}
void Game::Draw()
{
	objects.addtimer();
	objects2.addtimer2();
	SDL_Rect rc;


	//Scene scroll
	Scene.Move(0, -1);
	if (Scene.GetY() <= -Scene.H())	Scene.SetY(0);


	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);


	//Draw scene
	Scene.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);
	rc.y += rc.h;
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);
	
	//Draw player1
	Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_player, NULL, &rc);
	rc.y += rc.h;
	
	
	//Draw shots
	if (objects.ispicked() == 1) 
	{
		switch (objects.getid())
		{
		case 1:
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			break;
		case 2:
			SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
			break;

		default:

			break;
		}
	}
	else 
	{
		SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	}
	


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
	SDL_RenderCopy(Renderer, img_player2, NULL, &rc2);
	rc2.y += rc2.h;

	//Draw shots2

	if (objects2.ispicked() == 1)
	{
		switch (objects2.getid())
		{
		case 1:
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			break;
		case 2:
			SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
			break;

		default:

			break;
		}
	}
	else 
	{
		SDL_SetRenderDrawColor(Renderer, 192, 0, 255, 255);
	}
	 
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].GetRect(&rc2.x, &rc2.y, &rc2.w, &rc2.h);
			SDL_RenderFillRect(Renderer, &rc2);
		}
	}

	//Check collisions


	//check if player1 and player2 collide
	if (Player.GetX() < Player2.GetX() + Player2.W() && Player.GetX() + Player.W() > Player2.GetX() && Player.Y() < Player2.Y() + Player2.H() && Player.Y() + Player.H() > Player2.Y())
	{
		SDL_Quit();
	}

	//check if player1 gets shot
	if (Player.GetX() < Shots2[0].GetX() + Shots2[0].W() && Player.GetX() + Player.W() > Shots2[0].GetX() && Player.Y() < Shots2[0].Y() + Shots2[0].H() && Player.Y() + Player.H() > Shots2[0].Y())
	{

		if (objects2.ispicked() == 1)
		{
			switch (objects2.getid())
			{
			case 1:
				Player.redHP();
				Player.redHP();
				objects2.pickedupp(0);
				objects2.setid();
				break;




			default:
				Player.redHP();
				break;
			}
		}
		else
		{
			Player.redHP();
		}
		
		Shots2[0].Move(0, 1000);
		if (Player.HP() <= 0)
		{
			SDL_Quit();
		}

	}

	//check if player 2 gets shot
	if (Shots[0].GetX() < Player2.GetX() + Player2.W() && Shots[0].GetX() + Shots[0].W() > Player2.GetX() && Shots[0].Y() < Player2.Y() + Player2.H() && Shots[0].Y() + Shots[0].H() > Player2.Y())
	{
		if (objects.ispicked() == 1)
		{
			switch (objects.getid())
			{
			case 1:
				Player2.redHP();
				Player2.redHP();
				objects.pickedupp(0);
				objects.setid();
				break;

			default:
				Player2.redHP();
				break;
			}
		}
		else 
		{
			Player2.redHP();
		}

		Shots[0].Move(0, 1000);
		if (Player2.HP() <= 0)
		{
			SDL_Quit();
		}
	}

	//Power-up/obstacle spawner

	if (objects.timer1() >  objects.randoom(1))
	{
		objects.Move(0, 1);
		SDL_Rect rc5;
		rc5.x = objects.randoom(0);
		objects.GetRect(&rc5.x, &rc5.y, &rc5.w, &rc5.h);
		switch (objects.getid())
		{
		case 1:
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			break;
		case 2:
			SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
			break;

		default:
			
			break;
		}
		
		SDL_RenderFillRect(Renderer, &rc5);
	}

	if (objects2.timmer2() > objects2.randoom2(1))
	{
		objects2.Move(0, 1);
		SDL_Rect rc6;
		rc6.x = objects2.randoom2(0);
		objects2.GetRect(&rc6.x, &rc6.y, &rc6.w, &rc6.h);
		switch (objects2.getid())
		{
		case 1:
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			break;
		case 2:
			SDL_SetRenderDrawColor(Renderer, 255, 255, 0, 255);
			break;

		default:

			break;
		}
		SDL_RenderFillRect(Renderer, &rc6);
	}

	//Power-up/obstacle despawner

	if (objects.GetY() > 1020) 
	{
		objects.resettimer(1);
			

		objects.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	}

	if (objects2.GetY() > 1020) 
	{
		objects2.resettimer(0);
		

		objects2.Init(1000, WINDOW_HEIGHT >> 5, 50, 50, 1);
		
	}

	if (Player.GetX() < objects.GetX() + objects.W() && Player.GetX() + Player.W() > objects.GetX() && Player.Y() < objects.Y() + objects.H() && Player.Y() + Player.H() > objects.Y())
	{
		objects.resettimer(1);

		switch (objects.getid())
		{
		case 1:
			objects.pickedupp(1);
			break;
		case 2:
			Player.addhp();
			objects.pickedupp(0);
			objects.setid();
			break;

		default:

			break;
		}

		objects.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	}
	if (Player2.GetX() < objects2.GetX() + objects2.W() && Player2.GetX() + Player.W() > objects2.GetX() && Player2.Y() < objects2.Y() + objects2.H() && Player2.Y() + Player2.H() > objects2.Y())
	{
		objects2.resettimer(0);

		switch (objects2.getid())
		{
		case 1:
			objects2.pickedupp(1);
			break;
		case 2:
			Player2.addhp();
			objects2.pickedupp(0);
			objects2.setid();
			break;

		default:

			break;
		}

		objects2.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	}
	//HP bars

	SDL_Rect rc3{ 50, 300, 100, -Player.HP() * 30 };


	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &rc3);

	SDL_Rect rc4{ 1370, 300, 100, -Player2.HP() * 30 };


	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &rc4);

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
	

}
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
	Window = SDL_CreateWindow("Wild showdown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
	Scene.Init(0, 0, w, 6000, 3);
	

	//Init variables
	Player.Init(201, WINDOW_HEIGHT >> 1, 60, 180, 5);
	Player2.Init(1200, WINDOW_HEIGHT >> 1, 60, 180, 5);
	HP1.Init(20, WINDOW_HEIGHT >> 1, 104, 200, 5);
	idx_shot = 0;
	idx_shot2 = 0;
	objects.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	objects2.Init(1000, WINDOW_HEIGHT >> 5, 50, 50, 1);
	rounds1.Init(50, 750, 100, 100, 1);
	rounds2.Init(1350, 750, 100, 100, 1);
	objects.setid();
	objects2.setid();
	Player2.settimer(101);
	Player.settimer(101);
	Player.settimer2(360);
	Player2.settimer2(360);


	
	//Entities textures
	IMG_Init;
	SDL_CreateTextureFromSurface;

	return true;
}

bool Game::LoadAudios() {
	num_tracks = 0;
	tracks[num_tracks++] = Mix_LoadMUS("STK_western.ogg");

	Mix_PlayMusic(tracks[0], -1);

	sfxs[num_sfxs++] = Mix_LoadWAV("SFX_shot.wav");

	return true;
}	

bool Game::LoadImages()
{
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Log("IMG_Init, failed to init required png support: %s\n", IMG_GetError());
		return false;
	}
	img_background = SDL_CreateTextureFromSurface(Renderer, IMG_Load("IMG_background.png"));
	if (img_background == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	img_player1_F1 = SDL_CreateTextureFromSurface(Renderer, IMG_Load("spritesheet.png"));
	if (img_player1_F1 == NULL) {
		SDL_Log("CreateTextureFromSurface failed: %s\n", SDL_GetError());
		return false;
	}
	
	return true;
}

void Game::Release()
{
	//Release images
	SDL_DestroyTexture(img_shot);
	SDL_DestroyTexture(img_background);
	SDL_DestroyTexture(img_player1_F1);
	
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

	Player2.addtimer();
	Player.addtimer();

	Player.addtimer2();
	Player2.addtimer2();

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_W] == KEY_REPEAT && Player.GetY() > 85 - Player.H() && Player.isrolling() == 0 && Player.isdead() == 0)	fy = -1;
	if (keys[SDL_SCANCODE_S] == KEY_REPEAT && Player.GetY() < 980 - Player.H() && Player.isrolling() == 0 && Player.isdead() == 0)	fy = 1;
	if (keys[SDL_SCANCODE_A] == KEY_REPEAT && Player.GetX() > 200 && Player.isrolling() == 0 && Player.isdead() == 0)	fx = -1;
	if (keys[SDL_SCANCODE_D] == KEY_REPEAT && Player.GetX() < 580 - Player.W() && Player.isrolling() == 0 && Player.isdead() == 0)	fx = 1;
	if (keys[SDL_SCANCODE_Q] == KEY_DOWN && Player.isrolling() == 0 && Player.timmer2() > 360 && Player.isdead() == 0)
	{
		Player.setroll(1);
		Player.resettimer(2);
	}
	if (keys[SDL_SCANCODE_R] == KEY_DOWN && Player.getbullets() >= 6 && Player.isdead() == 0)
	{
		Player.setreload(1);
	}
		
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN && Player.timer1() > 120 && Player.isrolling() == 0 && Player.getbullets() < 6 && Player.isreloading() == 0 && Player.isdead() == 0)
	{
		int x, y, w, h;
		Player.GetRect(&x, &y, &w, &h);
		Shots[idx_shot].Init(x + w - 10, y + (h >> 1) - 3, 30, 15, 15);
		idx_shot++;
		idx_shot %= MAX_SHOTS;
		Player.addbullets(1);
		Player.settimer(0);

		// Play a single Sound
		Mix_PlayChannel(-1, sfxs[0], 0);
	}

	if (Player.timmer2() > 120 && Player.isrolling() == 1) 
	{
		Player.setroll(0);
	}
	
	//Scene scroll
	Scene.Move(0, 2);
	if (Scene.GetY() == Scene.H())	Scene.SetY(0);
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
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT && Player2.GetY() > 85 - Player2.H() && Player2.isrolling() == 0 && Player2.isdead() == 0)	fy2 = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT && Player2.GetY() < 980 - Player2.H() && Player2.isrolling() == 0 && Player2.isdead() == 0)	fy2 = 1;
	if (keys[SDL_SCANCODE_LEFT]  == KEY_REPEAT && Player2.GetX() > 965 && Player2.isrolling() == 0 && Player2.isdead() == 0)	fx2 = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT && Player2.GetX() < 1280 && Player2.isrolling() == 0 && Player2.isdead() == 0)fx2 = 1;
	if (keys[SDL_SCANCODE_P] == KEY_DOWN && Player2.isrolling() == 0 && Player2.timmer2() > 360 && Player2.isdead() == 0)
	{
		Player2.setroll(1);
		Player2.resettimer(2);
	}
	if (keys[SDL_SCANCODE_O] == KEY_DOWN && Player2.getbullets() >= 6 && Player2.isdead() == 0)
	{
		Player2.setreload(1);
	}

	if (keys[SDL_SCANCODE_RSHIFT] == KEY_DOWN && Player2.timer1() > 120 && Player2.isrolling() == 0 && Player2.getbullets() < 6 && Player2.isreloading() == 0 && Player2.isdead() == 0)
	{
		int x, y, w, h;
		Player2.GetRect(&x, &y, &w, &h);
		Shots2[idx_shot2].Init(x - w + 70, y + (h >> 1) - 3, 30, 15, 15);
		idx_shot2++;
		idx_shot2 %= MAX_SHOTS;
		Player2.addbullets(1);
		Player2.settimer(0);

		Mix_PlayChannel(-1, sfxs[0], 0);
	}

	if (Player2.timmer2() > 120 && Player2.isrolling() == 1)
	{
		Player2.setroll(0);
	}

	//Player2 update
	Player2.Move(fx2, fy2);
	//Shots2 update
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].Move(-1, 0);
			if (Shots2[i].GetX() < 0 )
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
	SDL_Rect rct { 521,995,225, 857 };
	SDL_Rect rct1{ 785,991,257, 869 };
	SDL_Rect rct2{ 1047,979,269, 890 };
	SDL_Rect rct3{ 1339,985,259, 865 };

	SDL_Rect rctroll{ 275,3943,379, 969};
	SDL_Rect rct1roll{ 659,4767,519,993};
	SDL_Rect rct2roll{ 1145,3939,367, 969 };
	SDL_Rect rct3roll{ 1697,4781,377, 957 };


	SDL_Rect rc2t{ 3849,1151,263, 793 };
	SDL_Rect rc2t1{ 4139,1149,266, 795 };
	SDL_Rect rc2t2{ 4410,1159,258, 785 };
	SDL_Rect rc2t3{ 4672,1166,259, 778 };


	SDL_Rect rctroll2{ 4749,4107,379, 971};
	SDL_Rect rct1roll2{ 4271,4927,523,997 };
	SDL_Rect rct2roll2{ 3941,4105,363, 971 };
	SDL_Rect rct3roll2{ 3383,4951,369, 953 };
	//Scene scroll
	Scene.Move(0, -1);
	if (Scene.GetY() <= -Scene.H())	Scene.SetY(900);


	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);


	//Draw scene
	Scene.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);
	rc.y -= rc.h;
	SDL_RenderCopy(Renderer, img_background, NULL, &rc);
	
	//Draw player1

	if (Player.isrolling() == 0 && Player.isdead() == 0)
	{
		switch (Player.animm())
		{
		case 0:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rct, &rc);
			rc.y += rc.h;
			if (Player.extrafram() == 1)
			{
				Player.setanim(1);
				Player.addframe(-5);
			}
			Player.addframe(1);
			break;
		case 1:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rct1, &rc);
			rc.y += rc.h;

			if (Player.extrafram() == 1)
			{
				Player.setanim(2);
				Player.addframe(-5);
			}
			Player.addframe(1);
			break;
		case 2:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rct2, &rc);
			rc.y += rc.h;

			if (Player.extrafram() == 1)
			{
				Player.setanim(3);
				Player.addframe(-5);
			}
			Player.addframe(1);
			break;
		case 3:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rct3, &rc);
			rc.y += rc.h;

			if (Player.extrafram() == 1)
			{
				Player.setanim(0);
				Player.addframe(-5);
			}
			Player.addframe(1);
			break;
		default:

			break;
		}
	}
	else 
	{
		
		if (Player.isdead() == 0) {
			switch (objects.animm())
			{
			case 0:
				Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rctroll, &rc);
				rc.y += rc.h;
				if (objects.extrafram() == 1)
				{
					objects.setanim(1);
					objects.addframe(-30);
				}
				objects.addframe(1);
				break;
			case 1:
				Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct1roll, &rc);
				rc.y += rc.h;

				if (objects.extrafram() == 1)
				{
					objects.setanim(2);
					objects.addframe(-30);
				}
				objects.addframe(1);
				break;
			case 2:
				Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct2roll, &rc);
				rc.y += rc.h;

				if (objects.extrafram() == 1)
				{
					objects.setanim(3);
					objects.addframe(-30);
				}
				objects.addframe(1);
				break;
			case 3:
				Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct3roll, &rc);
				rc.y += rc.h;

				if (objects.extrafram() == 1)
				{
					objects.setanim(0);
					objects.addframe(-30);
				}
				objects.addframe(1);
				break;
			default:

				break;
			}
		}
	}
	
	
	
	//Draw shots

	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Shots[i].IsAlive())
		{
			Shots[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_Rect rct8{3532, 3534,122, 65};
			SDL_Rect rct9{ 3563, 3633,198, 99 };
			if (objects.ispicked() == 1)
			{
				switch (objects.getid())
				{
				case 1:
					SDL_RenderCopy(Renderer, img_player1_F1, &rct9, &rc);
					break;

				default:

					break;
				}
			}
			else
			{
				SDL_RenderCopy(Renderer, img_player1_F1, &rct8, &rc);
			}
		}
	}

	SDL_Rect rc2d{ 607,2051,293, 695};
	SDL_Rect rc2d1{ 903,2017,267, 823 };
	SDL_Rect rc2d2{ 1221,2015,269, 855 };
	SDL_Rect rc2d3{ 613,2877,219, 873 };
	SDL_Rect rc2d4{ 927,2889,209, 875 };
	SDL_Rect rc2d5{ 1249,2889,227, 863 };


	if (Player.isdead() == 1)
	{
		switch (HP1.animm())
		{
		case 0:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d, &rc);
			rc.y += rc.h;
			if (HP1.extrafram() == 1)
			{
				HP1.setanim(1);
				HP1.addframe(-30);
			}
			HP1.addframe(1);
			break;
		case 1:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d1, &rc);
			rc.y += rc.h;

			if (HP1.extrafram() == 1)
			{
				HP1.setanim(2);
				HP1.addframe(-30);
			}
			HP1.addframe(1);
			break;
		case 2:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d2, &rc);
			rc.y += rc.h;

			if (HP1.extrafram() == 1)
			{
				HP1.setanim(3);
				HP1.addframe(-30);
			}
			HP1.addframe(1);
			break;
		case 3:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d3, &rc);
			rc.y += rc.h;

			if (HP1.extrafram() == 1)
			{
				HP1.setanim(4);
				HP1.addframe(-30);
			}
			HP1.addframe(1);
			break;
		case 4:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d4, &rc);
			rc.y += rc.h;

			if (HP1.extrafram() == 1)
			{
				HP1.setanim(5);
				HP1.addframe(-90);
			}
			HP1.addframe(1);
			break;
		case 5:
			Player.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2d5, &rc);
			rc.y += rc.h;

			if (HP1.extrafram() == 1)
			{
				SDL_Quit();
			}
			HP1.addframe(1);
			break;

		default:

			break;
		}
	}



	//Draw player2
	SDL_Rect rc2;
	
	if (Player2.isrolling() == 0 && Player2.isdead() == 0) {
		switch (Player2.animm())
		{
		case 0:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2t, &rc);
			rc.y += rc.h;
			if (Player2.extrafram() == 1)
			{
				Player2.setanim(1);
				Player2.addframe(-5);
			}
			Player2.addframe(1);
			break;
		case 1:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2t1, &rc);
			rc.y += rc.h;

			if (Player2.extrafram() == 1)
			{
				Player2.setanim(2);
				Player2.addframe(-5);
			}
			Player2.addframe(1);
			break;
		case 2:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2t2, &rc);
			rc.y += rc.h;

			if (Player2.extrafram() == 1)
			{
				Player2.setanim(3);
				Player2.addframe(-5);
			}
			Player2.addframe(1);
			break;
		case 3:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rc2t3, &rc);
			rc.y += rc.h;

			if (Player2.extrafram() == 1)
			{
				Player2.setanim(0);
				Player2.addframe(-5);
			}
			Player2.addframe(1);
			break;
		default:

			break;
		}
	}
	else 

	{
		if (Player2.isdead() == 0) 
		{
			switch (objects2.animm())
			{
			case 0:
				Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rctroll2, &rc);
				rc.y += rc.h;
				if (objects2.extrafram() == 1)
				{
					objects2.setanim(1);
					objects2.addframe(-30);
				}
				objects2.addframe(1);
				break;
			case 1:
				Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct1roll2, &rc);
				rc.y += rc.h;

				if (objects2.extrafram() == 1)
				{
					objects2.setanim(2);
					objects2.addframe(-30);
				}
				objects2.addframe(1);
				break;
			case 2:
				Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct2roll2, &rc);
				rc.y += rc.h;

				if (objects2.extrafram() == 1)
				{
					objects2.setanim(3);
					objects2.addframe(-30);
				}
				objects2.addframe(1);
				break;
			case 3:
				Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

				SDL_RenderCopy(Renderer, img_player1_F1, &rct3roll2, &rc);
				rc.y += rc.h;

				if (objects2.extrafram() == 1)
				{
					objects2.setanim(0);
					objects2.addframe(-30);
				}
				objects2.addframe(1);
				break;
			default:

				break;
			}
		}

	}

	SDL_Rect rcd{ 4549,2207,301, 705};
	SDL_Rect rcd1{ 4283,2183,270, 821 };
	SDL_Rect rcd2{ 3965,2181,263, 857 };
	SDL_Rect rcd3{ 4613,3043,227, 877 };
	SDL_Rect rcd4{ 4309,3049,213, 875};
	SDL_Rect rcd5{ 3967,3055,235, 863};
		

	if (Player2.isdead() == 1) 
	{
		switch (HP2.animm())
		{
		case 0:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd, &rc);
			rc.y += rc.h;
			if (HP2.extrafram() == 1)
			{
				HP2.setanim(1);
				HP2.addframe(-30);




			}
			HP2.addframe(1);
			break;
		case 1:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd1, &rc);
			rc.y += rc.h;

			if (HP2.extrafram() == 1)
			{
				HP2.setanim(2);
				HP2.addframe(-30);
			}
			HP2.addframe(1);
			break;
		case 2:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd2, &rc);
			rc.y += rc.h;

			if (HP2.extrafram() == 1)
			{
				HP2.setanim(3);
				HP2.addframe(-30);
			}
			HP2.addframe(1);
			break;
		case 3:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd3, &rc);
			rc.y += rc.h;

			if (HP2.extrafram() == 1)
			{
				HP2.setanim(4);
				HP2.addframe(-30);
			}
			HP2.addframe(1);
			break;
		case 4:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd4, &rc);
			rc.y += rc.h;

			if (HP2.extrafram() == 1)
			{
				HP2.setanim(5);
				HP2.addframe(-90);
			}
			HP2.addframe(1);
			break;
		case 5:
			Player2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

			SDL_RenderCopy(Renderer, img_player1_F1, &rcd5, &rc);
			rc.y += rc.h;

			if (HP2.extrafram() == 1)
			{
				SDL_Quit();
			}
			HP2.addframe(1);
			break;
		
		default:

			break;
		}
	}

	//Draw shots2 
	for (int i = 0; i < MAX_SHOTS2; ++i)
	{
		if (Shots2[i].IsAlive())
		{
			Shots2[i].GetRect(&rc2.x, &rc2.y, &rc2.w, &rc2.h);
			SDL_Rect rct9{ 3349, 3533,126, 65 };
			SDL_Rect rct10{ 3258, 3633,203, 96 };
			if (objects2.ispicked() == 1)
			{
				switch (objects2.getid())
				{
				case 1:
					SDL_RenderCopy(Renderer, img_player1_F1, &rct10, &rc2);
					break;

				case 2:
					SDL_RenderCopy(Renderer, img_player1_F1, &rct10, &rc2);
					break;
				default:

					break;
				}
			}
			else
			{
				SDL_RenderCopy(Renderer, img_player1_F1, &rct9, &rc2);
			}
		}
	}

	//Check collisions

	//check if player1 gets shot
	if (Player.GetX() < Shots2[0].GetX() + Shots2[0].W() && Player.GetX() + Player.W() > Shots2[0].GetX() && Player.Y() < Shots2[0].Y() + Shots2[0].H() && Player.Y() + Player.H() > Shots2[0].Y() && Player.isrolling() == 0)
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
			 Player.setdeath(1);
		}

	}

	//check if player 2 gets shot
	if (Shots[0].GetX() < Player2.GetX() + Player2.W() && Shots[0].GetX() + Shots[0].W() > Player2.GetX() && Shots[0].Y() < Player2.Y() + Player2.H() && Shots[0].Y() + Shots[0].H() > Player2.Y() && Player2.isrolling() == 0)
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
			Player2.setdeath(1);
		}
	}

	//Power-up/obstacle spawner

	if (objects.timer1() >  objects.randoom(1))
	{
		objects.Move(0, 1);
		SDL_Rect rc5;
		rc5.x = objects.randoom(0);
		objects.GetRect(&rc5.x, &rc5.y, &rc5.w, &rc5.h);
		SDL_Rect rc5t{ 3311, 3323, 167,173};
		SDL_Rect rc52t{3529,3331,181,163};
		switch (objects.getid())
		{
		case 1:
			SDL_RenderCopy(Renderer, img_player1_F1, &rc5t, &rc5);
			break;
		case 2:
			SDL_RenderCopy(Renderer, img_player1_F1, &rc52t, &rc5);
			break;

		default:
			
			break;
		}
		
		//SDL_RenderFillRect(Renderer, &rc5);
	}

	if (objects2.timmer2() > objects2.randoom2(1))
	{
		objects2.Move(0, 1);
		SDL_Rect rc6;
		rc6.x = objects2.randoom2(0);
		objects2.GetRect(&rc6.x, &rc6.y, &rc6.w, &rc6.h);
		SDL_Rect rc6t{ 3311, 3323, 167,173 };
		SDL_Rect rc62t{ 3529,3331,181,163 };
		switch (objects2.getid())
		{
		case 1:
			SDL_RenderCopy(Renderer, img_player1_F1, &rc6t, &rc6);
			break;
		case 2:
			SDL_RenderCopy(Renderer, img_player1_F1, &rc62t, &rc6);
			break;

		default:

			break;
		}
		//SDL_RenderFillRect(Renderer, &rc6);
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
			if (!Player.HP() < 4) 
			{
				Player.addhp();
			}
			
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
			if (Player2.HP() < 4) 
			{
				Player2.addhp();
			}
				
			
			objects2.pickedupp(0);
			objects2.setid();
			break;

		default:

			break;
		}

		objects2.Init(300, WINDOW_HEIGHT >> 5, 50, 50, 1);
	}
	//HP bars

	SDL_Rect rc3{ 30, 150, 150, 100 };
	SDL_Rect rc31{ 1831, 1419, 501, 257};
	SDL_Rect rc32{ 1833, 1743, 511, 249 };
	SDL_Rect rc33{ 1833, 2045, 503, 249 };
	SDL_Rect rc34{ 1833, 2367, 505, 249 };
	SDL_Rect rc35{ 1833, 2655, 499, 249 };

	switch (Player.HP())
	{
	case 4:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc31, &rc3);
		break;
	case 3:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc32, &rc3);
		break;
	case 2:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc33, &rc3);
		break;
	case 1:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc34, &rc3);
		break;
	case 0:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc35, &rc3);
		break;
	default:

		break;
	}

	

	SDL_Rect rc4{ 1330, 150, 150, 100};
	SDL_Rect rc41{ 2711, 1419, 501, 287 };
	SDL_Rect rc42{ 2711, 1743, 511, 289 };
	SDL_Rect rc43{ 2711, 2045, 503, 289 };
	SDL_Rect rc44{ 2711, 2367, 505, 289 };
	SDL_Rect rc45{ 2711, 2655, 499, 289 };


	switch (Player2.HP())
	{
	case 4:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc41, &rc4);
		break;
	case 3:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc42, &rc4);
		break;
	case 2:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc43, &rc4);
		break;
	case 1:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc44, &rc4);
		break;
	case 0:
		SDL_RenderCopy(Renderer, img_player1_F1, &rc45, &rc4);
		break;
	default:

		break;
	}


	//Reload animations for player 1

	if (Player.isreloading() == 1) {
		switch (Player.getbullets())
		{
		case 6:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 5:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 4:


			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 3:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 2:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 1:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);
			break;
		case 0:

			if (rounds1.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds1.addframe(-30);
			}
			rounds1.addframe(1);

			Player.setreload(0);
			break;
		default:

			break;
		}
	}

	SDL_Rect rc15;

	SDL_Rect rc151{1977, 3047, 273, 265};
	SDL_Rect rc152{2379, 3047, 269, 271 };
	SDL_Rect rc153{2739, 3047, 275, 271};
	SDL_Rect rc154{1997, 3395, 271, 265};
	SDL_Rect rc155{2395, 3397, 269, 269};
	SDL_Rect rc156{2761, 3395, 271, 267};
	SDL_Rect rc157{1977, 3691, 273, 269};

	rounds1.GetRect(&rc15.x, &rc15.y, &rc15.w, &rc15.h);

	switch (Player.getbullets())
	{
	case 0:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc151, &rc15);
		rc.y += rc.h;
		
		
		break;
	case 1:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc152, &rc15);
		rc.y += rc.h;

		
		break;
	case 2:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc153, &rc15);
		rc.y += rc.h;

		
		break;
	case 3:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc154, &rc15);
		rc.y += rc.h;

		
		break;

	case 4:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc155, &rc15);
		rc.y += rc.h;
		break;

	case 5:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc156, &rc15);
		rc.y += rc.h;
		break;

	case 6:
		rounds1.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc157, &rc15);
		rc.y += rc.h;
		break;
	
	default:

		break;
	}

	//Player 2 reload animations

	if (Player2.isreloading() == 1) {
		switch (Player2.getbullets())
		{
		case 6:

			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 5:

			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 4:


			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 3:

			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 2:

			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 1:

			if (rounds2.extrafram() == 1)
			{
				Player2.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);
			break;
		case 0:

			if (rounds2.extrafram() == 1)
			{
				Player.addbullets(-1);
				rounds2.addframe(-30);
			}
			rounds2.addframe(1);

			Player2.setreload(0);
			break;
		default:

			break;
		}
	}

	

	SDL_Rect rc25;

	rounds2.GetRect(&rc25.x, &rc25.y, &rc25.w, &rc25.h);

	switch (Player2.getbullets())
	{
	case 0:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc151, &rc25);
		rc.y += rc.h;


		break;
	case 1:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc152, &rc25);
		rc.y += rc.h;


		break;
	case 2:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc153, &rc25);
		rc.y += rc.h;


		break;
	case 3:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc154, &rc25);
		rc.y += rc.h;


		break;

	case 4:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc155, &rc25);
		rc.y += rc.h;
		break;

	case 5:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc156, &rc25);
		rc.y += rc.h;
		break;

	case 6:
		rounds2.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);

		SDL_RenderCopy(Renderer, img_player1_F1, &rc157, &rc25);
		rc.y += rc.h;
		break;

	default:

		break;
	}

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
	

}
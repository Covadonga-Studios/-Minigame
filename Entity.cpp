#include "Entity.h"
#include <time.h>
#include <stdlib.h>

Entity::Entity()
{
	is_alive = false;
}
Entity::~Entity()
{

}
void Entity::Init(int posx, int posy, int w, int h, int s) 
{

	x = posx;
	y = posy;
	width = w;
	height = h;
	speed = s;
	is_alive = true;
	hp = 4;
	random1 = rand() % 100 + 10;
	random2 = rand() % 100 + 10;
	subrandom1 = rand() % 450 + 200;
	subrandom2 = rand() % 730 + 720;
	anim = 0;
	extraframe = -3;
	roll = 0;
	bullets = 0;
	reload = 0;
}
void Entity::GetRect(int *posx, int *posy, int *w, int *h)
{
	*posx = x;
	*posy = y;
	*w = width;
	*h = height;
}
int Entity::GetX()
{
	return  x;
}
int Entity::GetY()
{
	return  y;
}

int Entity::Y()
{
	return  y;
}
int Entity::W()
{
	return  width;
}
int Entity::H()
{
	return  height;
}
//function to get the hp value
int Entity::HP()
{
	return hp;
}
//function to reduce hp
void Entity::redHP()
{
	hp--;
}
void Entity::ShutDown()
{
	is_alive = false;
}
bool Entity::IsAlive()
{
	return is_alive;
}
void Entity::Move(int dx, int dy)
{
	x += dx * speed;
	y += dy * speed;
}
void Entity::SetX(int posx)
{
	x = posx;
}
void Entity::SetY(int posy)
{
	y = posy;
}
void Entity::addtimer() 
{
	timer++;
}
int Entity::timer1() 
{
	return timer;
}
void Entity::addtimer2()
{
	timer2++;
}
int Entity::timmer2()
{
	return timer2;
}
int Entity::randoom(int rando) 
{
	if (rando == 1) 
	{
		return random1;
	}
	else 
	{
		x = subrandom1;
		return x;
	}
	
}
int Entity::randoom2(int rando) 
{
	if (rando == 1)
	{
		return random2;
	}
	else
	{
		x = subrandom2;
		return x;
	}
}

void Entity::resettimer(int timerr) 
{
	if (timerr == 1)
	{
		timer = 0;
	}
	else 
	{
		timer2 = 0;
	}
}
void Entity::setid() 
{
	objectid = rand() % 2 + 1;
}

int Entity::getid() 
{
	return objectid;
}

bool Entity::ispicked() 
{
	return pickedup;
}
void Entity::pickedupp(int is) 
{
	if (is == 0) 
	{
		pickedup = 0;
	}
	else 
	{
		pickedup = 1;
	}
}

void Entity::addhp() 
{
	hp++;
}
void Entity::settimer(int timerr) 
{
	timer = timerr;
}
void Entity::settimer2(int timerr) 
{
	timer2 = timerr;
}
void Entity::setanim(int frame) {

	anim = frame;

}
int Entity::animm() {

	return anim;

}
void Entity::addframe( int extra) 
{
	extraframe += extra;
}
int Entity::extrafram() 
{
	return extraframe;
}
bool Entity::isrolling() 
{
	return roll;
}
void Entity::setroll(bool rolls) 
{
	roll = rolls;
}
void Entity::addbullets(int bullet) 
{
	bullets += bullet;
}
int Entity::getbullets()
{
	return bullets;
}
bool Entity::isreloading() 
{
	return reload;
}
void Entity::setreload(bool is) 
{
	reload = is;
}
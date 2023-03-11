 #pragma once
class Entity
{
public:
	Entity();
	~Entity();

	void Init(int posx, int posy, int w, int h, int s);
	void GetRect(int *posx, int *posy, int *w, int *h);
	
	void SetX(int posx);
	int  GetX();

	void SetY(int posy);
	int GetY();

	void ShutDown(); 
	bool IsAlive();
	void Move(int dx, int dy);
	int	 Y();
	int  W();
	int  H();
	int HP();
	void redHP();
	void addtimer();
	int timer1();
	void addtimer2();
	int timmer2();
	int randoom(int rando);
	int randoom2(int rando);
	void resettimer(int timerr);
	void settimer(int timerr);
	void settimer2(int timerr);
	void setid();
	int getid();
	bool ispicked();
	void pickedupp(int is);
	void addhp();
	void setanim(int frame);
	int animm();
	void addframe(int extra);
	int extrafram();
	void setroll(bool rolls);
	bool isrolling();


private:
	int x, y;
	int width, height;
	int speed;
	bool is_alive;
	int sfx_id;
	int hp;
	int timer;
	int timer2;
	int random1;
	int random2;
	int subrandom1;
	int subrandom2;
	int objectid;
	bool pickedup;
	int anim;
	int extraframe;
	bool roll;
};


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

private:
	int x, y;
	int width, height;
	int speed;
	bool is_alive;
	int sfx_id;
	int hp;
};


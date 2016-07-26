typedef struct _BULLET
{
	unsigned int x;
	unsigned int y;
	double dir; //x œ‡∂‘”⁄y
	unsigned int y_step;
	unsigned int type;
	unsigned int power;
}BULLET, *LPBULLET;

#define BULLET_OUT	0
#define BULLET_HIT	1
#define BULLET_MOVE 2
int InitBulletAndFights();
int NewBullet(unsigned int x, unsigned int y);
unsigned int GetBulletsNum();
LPBULLET GetBulletAt(unsigned int i);
LPBULLET CreateBullet(unsigned int x, unsigned int y);
int DestoryAllBulletAndFlights();
void DestoryBullet(LPBULLET lpbullet);
int MoveBullet(LPBULLET lpbullet);
unsigned int GetBulletX(LPBULLET lpbullet);
unsigned int GetBulletY(LPBULLET lpbullet);
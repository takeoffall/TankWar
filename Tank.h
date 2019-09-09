#ifndef _TANK_H_
#define _TANK_H_

#include "cocos2d.h"
//#include "MapLayer.h"
class GameLayer;
#include "Props.h"
class TankController;//new option
class Bullet;

USING_NS_CC;

enum class MOVE_SPEED {
	SLOW = 1,
	MID = 3,
	FAST = 5,
	STOP = 0
};

enum class SHOOT_SPEED {
	SLOW = 1,
	MID = 3,
	FAST = 5,
	LIMIT = 7,
};

enum class DIRECTION {
	LEFT,
	UP,
	DOWN,
	RIGHT,
	DEFAULT,
};

class Tank : public Sprite
{
public:
	virtual bool init();
	Tank(MOVE_SPEED moveSpeed = MOVE_SPEED::MID, SHOOT_SPEED shootSpeed = SHOOT_SPEED::MID);
	static Tank* create(const std::string& filename, MOVE_SPEED moveSpeed = MOVE_SPEED::MID, SHOOT_SPEED shootSpeed = SHOOT_SPEED::MID);
	void update(float dt);

	GameLayer* gameLayer;
	//MapLayer *map;
	int bulletPower;
	int HP;//血量
	int getHP() {
		return HP;
	}
	void addShootSpeed(int i)
	{
		if (shootSpeed + i > (int)SHOOT_SPEED::LIMIT)
		{
			shootSpeed = (int)SHOOT_SPEED::LIMIT;
		}
		else
			shootSpeed += i;
	}
	/*int score;
	int getScore() {
		return score;
	}*/
	void setPlayerName(const std::string& str)
	{
		player = str;
	}
	std::string getPlayerName() {
		return player;
	}
	void addHP(int blood) {
		if (HP + blood > 100)
			HP = 100;
		else
			HP += blood;
	}
	void dropBlood(int blood) {
		if (HP - blood < 0)
			HP = 0;
		else
			HP -= blood;
	}
	//user action
	TankController* tankController;
	void addController(const std::string& xml);
	void removeController();
	//model action
	//Vector<Node *> haveProps;//已有的限时道具
	std::vector<std::string > haveProps;//no use
	Vector <Props* > existProps;//now 
	bool isProtected;//防弹衣
	bool isProtectedUP;//反甲
	Props* currentBuff;

	bool isPause;
	virtual void waitForDie(int damage) {}

	//void dropBlood(int damage);
	bool isCollideMap(float x, float y);
	bool isCollideTank(float x, float y);
	bool isGetBonusByName(const std::string &name);
	bool isGetBonus(float x, float y);//移动检测碰撞时顺便检测了，不用像上式那样单独检测

	Bullet* shootOneBullet();
	Bullet* shootOneBullet(const std::string& filename, int num = 10, int power = 1);
	//unused
	void moving(DIRECTION direction = DIRECTION::DEFAULT);
	void stopMoving(DIRECTION direction);
	bool isCollideOthers(Node *node);
	//flag 
	bool is_moving;
	bool is_moving_left;
	bool is_moving_right;
	bool is_moving_up;
	bool is_moving_down;
	bool left_lock;
	bool right_lock;
	bool up_lock;
	bool down_lock;
	//enum 
	DIRECTION m_direction;
	int moveSpeed;
	int m_defaultSpeed;
	int shootSpeed;

protected:
	void loadBullets(const std::string& filename, int num, int power = 1);//外部接口
	void loadBullets();
	void fixBulletPos();
	Tank *collidedEnemy;

private:
	Vector<Bullet *> m_cartridgeClip;//私有弹夹
	std::string player;
};


#endif
#ifndef _TANK_H_
#define _TANK_H_

#include "cocos2d.h"
//#include "MapLayer.h"
class GameLayer;
#include "Props.h"

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
	int HP;//Ѫ��
	int score;
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
	void addController();
	void removeController();
	//model action
	//Vector<Node *> haveProps;//���е���ʱ����
	std::vector<std::string > haveProps;//no use
	Vector <Props* > existProps;//now 
	bool isProtected;//������
	bool isProtectedUP;//����
	Props* currentBuff;
	std::vector <PROP_TYPE > buffs;
	//void dropBlood(int damage);
	bool isCollideMap(float x, float y);
	bool isCollideTank(float x, float y);
	bool isGetBonusByName(const std::string &name);
	bool isGetBonus(float x, float y);//�ƶ������ײʱ˳�����ˣ���������ʽ�����������

	bool isMeetSomeByType(float x, float y);//������ļ��tank��bonusһ�𣬴�������

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
	MOVE_SPEED m_moveSpeed;
	MOVE_SPEED m_defaultSpeed;
	SHOOT_SPEED m_shootSpeed;

protected:
	void loadBullets(const std::string& filename, int num, int power = 1);//�ⲿ�ӿ�
	void loadBullets();
	void fixBulletPos();
	Tank *collidedEnemy;

private:
	Vector<Bullet *> m_cartridgeClip;//˽�е���
	
};


#endif
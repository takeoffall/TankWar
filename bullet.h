#ifndef _BULLET_H
#define _BULLET_H_

#include "cocos2d.h"
USING_NS_CC;

#include "Tank.h"
//#include "BulletController.h"
//class BulletController;
//class MapLayer;
#include "BulletController.h"

class Bullet : public Sprite//�̳�Sprite�������ܹ���Vectorװ�룬��Ȼ�Լ�д������ģ��
{
public:
	virtual bool init(const std::string& name);

	Bullet(int power = 20);
	static Bullet* create(const std::string& name, int power = 20);
	Bullet(const std::string& name, int power = 20);
	void update(float dt);
	int getPower();
	bool collision();
	bool isCollision(Rect &rect);
	bool ice;//����

	GameLayer* gameLayer;

	void addController();

	DIRECTION m_direction;
	DIRECTION tankDirection;
	int tankShootSpeed;
	void dieEffect();//die ui
	void fly();//normal ui
	Tank *p_tank;
	std::string p_tankName;//��������p_tank->getName()����Ϊ�п����ӵ��������
	//MapLayer* map;

private:
	int power;//��������ӵ��ܴ�שͷ
};

#endif

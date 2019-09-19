#ifndef _BULLET_H
#define _BULLET_H_

#include "cocos2d.h"
USING_NS_CC;

#include "Tank.h"
//#include "BulletController.h"
//class BulletController;
//class MapLayer;
#include "BulletController.h"

class Bullet : public Sprite//继承Sprite以致于能够用Vector装入，不然自己写容器类模板
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
	bool ice;//冰弹

	GameLayer* gameLayer;

	void addController();

	DIRECTION m_direction;
	DIRECTION tankDirection;
	int tankShootSpeed;
	void dieEffect();//die ui
	void fly();//normal ui
	Tank *p_tank;
	std::string p_tankName;//单独保存p_tank->getName()，因为有可能子弹打出来了
	//MapLayer* map;

private:
	int power;//威力大的子弹能打穿砖头
};

#endif

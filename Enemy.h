#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Tank.h"
//#include "MapLayer.h"
#include "GameScene.h"
#include "cocos2d.h"


USING_NS_CC;

class Enemy : public Tank
{
public:
	virtual bool init();
	Enemy(MOVE_SPEED moveSpeed = MOVE_SPEED::MID, SHOOT_SPEED shootSpeed = SHOOT_SPEED::MID);
	static Enemy* create(const std::string& filename, MOVE_SPEED moveSpeed = MOVE_SPEED::MID, SHOOT_SPEED shootSpeed = SHOOT_SPEED::MID);
	void update(float dt);

	bool isCollideObject(float x, float y);
	void autoMove();
	void adjustDirection();
	void autoFire();

	void addHPUI();//添加血量显示，以addChild的形式添加
protected:

private:
	ProgressTimer *bloodBar;
};


#endif
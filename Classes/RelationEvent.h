#ifndef _RELATION_EVENT_H_
#define _RELATION_EVENT_H_

#include "cocos2d.h"
USING_NS_CC;

#include "Tank.h"
#include "MapLayer.h"
#include "Enemy.h"
class Bullet;

class RelateEvent : public Node
{
public:
	CREATE_FUNC(RelateEvent);
	virtual bool init();
	void update(float dt);

	void push(Tank *tank)
	{
		tankSet.pushBack(tank);
	}
	void push(Enemy *enemy)
	{
		enemySet.pushBack(enemy);
	}
	void push(Bullet *bullet)
	{
		bulletSet.pushBack(bullet);
	}
	void addmap(MapLayer *map)
	{
		this->map = map;
	}

	//Interacting events
	void byCollision();
	bool isCollideMap(Point p);
	
	void cTankMap(Tank *tank);
	void cBulletMap(Bullet *bullet);
	void cTankEnemy(Tank *tank, Enemy *enemy);
	void cBulletTank(Bullet *bullet, Tank *tank);

	void bulletDie(Bullet *bullet);
	void tankDie(Tank *tank);
	void enemyDie(Enemy *enemy) {}

private:
	Vector<Tank *> tankSet;
	Vector<Bullet *> bulletSet;
	Vector<Enemy *> enemySet;
	MapLayer *map;
};

#endif
#ifndef _BULLET_CONTROLLER_H_
#define _BULLET_CONTROLLER_H_

#include "cocos2d.h"
USING_NS_CC;
class Bullet;
//#include "bullet.h"
//class Tank;
#include "Tank.h"

class BulletController : public Node
{
public:
	static BulletController* create(Bullet *bullet);
	virtual bool init(Bullet *bullet);

	void update(float dt);

private:
	Bullet *m_bullet;
	Tank *m_hitTank;
	void listenCollideMap();
	void listenCollideTank();
	void listenCollideEnemy();

	void bulletDo();
	void tankDo();
	void enemyDo();
	void tankBoom();
	void tankFlash();
	void enemyBoom();
	void bulletRebound();

	bool isCollideMap(float x, float y);
	bool isCollideByName(const std::string name);

};

#endif
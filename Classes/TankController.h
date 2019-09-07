#ifndef _TANK_CONTROLLER_H_
#define _TANK_CONTROLLER_H_

#include "cocos2d.h"
USING_NS_CC;

#include "Tank.h"
//#include "MapLayer.h"
//class Bullet;

class TankController :public Node
{
public:
	TankController(const std::string& xml_file);
	static TankController* create(Tank *tank, const std::string& xml_file);
	virtual bool init(Tank *tank);
	void update(float dt);
	
private:
	//void propLoseEfficacy(PROP_TYPE type, bool all);
	std::string getPropNameFromType(PROP_TYPE type);
	void clearPropsFromVector(PROP_TYPE type);
	//Buffs Action
	void buffsInAction();
	void getStar();
	void getSpade();
	void getMine();
	void getTimer();
	void getXingXing();
	void getProtected();
	void getProtectedUP();
	void getBlood();

	void tankBorn();
	void listenMove();
	void listenFire();
	void listenGetProps();//unused
	bool isCollideObject(float x, float y);

	EventKeyboard::KeyCode key_direction_left;
	EventKeyboard::KeyCode key_direction_right;
	EventKeyboard::KeyCode key_direction_up;
	EventKeyboard::KeyCode key_direction_down;
	EventKeyboard::KeyCode key_fire;

	void moving(DIRECTION direction);
	void stopMoving(DIRECTION direction);

	Tank *tank;
	Bullet *m_bullet;
	
	EventListenerKeyboard *m_moveListener;
	EventListenerKeyboard *m_fireListener;
};


#endif
#ifndef _TANK_CONTROLLER_H_
#define _TANK_CONTROLLER_H_

#include "cocos2d.h"
USING_NS_CC;

#include "Tank.h"
//#include "MapLayer.h"
//class Bullet;
#include "UIControls.h"//new option

class TankController :public Node
{
public:
	TankController(const std::string& xml_file);
	static TankController* create(Tank *tank, const std::string& xml_file);
	virtual bool init(Tank *tank);
	void update(float dt);

	void addInventory(const std::string& bg, Point pos);
	void addPlayerHP(const std::string& bg, const std::string& source, Point pos);
	
private:
	typedef void (TankController:: *func)();//定义FUNC类型是一个指向函数的指针，该函数参数为void*，返回值为void* 
	typedef void (*FUNC)();//定义FUNC类型是一个指向函数的指针，该函数参数为void*，返回值为void*
	//FUNC callback = (FUNC)&TankController::loseProtected;//强制转换func()的类型 

	//void propLoseEfficacy(PROP_TYPE type, bool all);
	std::string getPropNameFromType(PROP_TYPE type);
	void clearPropsFromVector(PROP_TYPE type);

	void tankBorn();
	void listenMove();
	void listenFire();

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
	Vector <Props *> tankProps;
	
	std::map <PROP_TYPE, int> propTypes = {
		{PROP_TYPE::ADD_BLOOD, 0},
		{PROP_TYPE::PROTECTED, 0},
		{PROP_TYPE::START, 0},
		{PROP_TYPE::SPADE, 0},
		{PROP_TYPE::MINE, 0},
		{PROP_TYPE::TIMER, 0},
		{PROP_TYPE::XINGXING, 0},
	};
	
	EventListenerKeyboard *m_moveListener;
	EventListenerKeyboard *m_fireListener;

	//associated ui controls
	Inventory *inventory;
	PlayerHP *playerHP;
	
};


#endif
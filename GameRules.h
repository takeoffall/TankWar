#ifndef _GAME_RULES_H_
#define _GAME_RULES_H_

#include "cocos2d.h"
USING_NS_CC;
class MapLayer;

const std::string NONE_STRING = "fart";

enum class MYDIRECTION {
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

class MY_OBJECT : public Node
{
public:
	MapLayer* map;
	bool isDynamic;

	int getSpeed();
	int getSpeedForDirection(MYDIRECTION direction);

	MYDIRECTION m_direction;
	int m_moveUpSpeed, m_moveDownSpeed, m_moveLeftSpeed, m_moveRightSpeed;
	
};

class DynamicObject : public MY_OBJECT
{
public:
	bool isCollideObject();
	bool mapBlockAhead();
	bool isLock;

	std::string getObjectInFront();
	MY_OBJECT* getObjectForPosition(float x, float y);

protected:
	MY_OBJECT *m_frontObject;
	
};

class StaticObject : public MY_OBJECT
{
public:
	
protected:
	const int m_moveUpSpeed, m_moveDownSpeed, m_moveLeftSpeed, m_moveRightSpeed;
};

#endif
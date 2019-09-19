#ifndef __PROPS_CONTROLLER_H__
#define __PROPS_CONTROLLER_H__

#include "cocos2d.h"
#include "Props.h"
#include "Tank.h"//newe

USING_NS_CC;

class PropController : public Node
{
public:
	PropController(Props* prop);
	static PropController* create(Props* prop);
	virtual bool init();
	void update(float dt);

private:
	Props* prop;
	void getVajraBody(Tank* tank, const std::string& scheduleName);//限时主动技能放这里， 被动用bool来标示，在别处作用
};

#endif // !__PROPS_CONTROLLER_H__

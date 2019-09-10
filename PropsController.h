#ifndef __PROPS_CONTROLLER_H__
#define __PROPS_CONTROLLER_H__

#include "cocos2d.h"
#include "Props.h"

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
};

#endif // !__PROPS_CONTROLLER_H__

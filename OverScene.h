#ifndef __OVER_SCENE_H__
#define __OVER_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

#include "HelloWorldScene.h"

class OverScene : public Scene
{
public:
	CREATE_FUNC(OverScene);
	virtual bool init();

	HelloWorld* tsm;
};

#endif // !__OVER_SCENE_H__

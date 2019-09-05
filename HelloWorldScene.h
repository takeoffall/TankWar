#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(HelloWorld);
	void display();
	void goLevelScene();
	void goMenuScene();
	void goGameSceneWithMap(int level);
	void goGameOverScene();
public:

};

#endif // __HELLOWORLD_SCENE_H__

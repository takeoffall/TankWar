#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

//const std::string preload_list[] = {
//	"",
//
//
//};

class HelloWorld : public Scene
{
public:
    CREATE_FUNC(HelloWorld);
	virtual bool init();
	virtual void update(float dt);
	
	void goLevelScene();
	void goMenuScene();
	void goGameSceneWithMap(int level);
	void goGameOverScene();
private:
	int count, total;
	//LoadingBar* loadingBar;
	void display();

};

#endif // __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"

class MenuLayer : public cocos2d::Layer
{
public:
	virtual bool init();
	CREATE_FUNC(MenuLayer);
	void display();
	void startGameCallback(Ref* pSender);
	void loadGameCallback(Ref* pSender);
	void settingsCallback(Ref* pSender);
	void exitCallback(Ref* pSender);
public:
	HelloWorld * tsm;
};
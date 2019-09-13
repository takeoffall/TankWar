#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "HelloWorldScene.h"

USING_NS_CC;
using namespace ui;

class LevelLayer : public Scene
{
public:
	virtual bool init() override;

	void pageViewEvent(Ref* pSender, PageViewEventType type);
	void display_backbtn();
	void display_levels();
	CREATE_FUNC(LevelLayer);

public:
	HelloWorld * tsm;
};
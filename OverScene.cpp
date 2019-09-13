#include "OverScene.h"
#include "ui/CocosGUI.h"
using namespace ui;

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

bool OverScene::init()
{
	if (!Scene::init())
		return false;

	auto label = Label::createWithTTF("游戏结束！", "fonts/SIMYOU.TTF", 32);
	auto label_2 = Label::createWithTTF("————没有美术难顶哦", "fonts/SIMYOU.TTF", 24);
	auto label_3 = Label::createWithTTF("————纯代码敲ui难搞哟", "fonts/SIMYOU.TTF", 24);
	auto label_4 = Label::createWithTTF("那我下面就乱入一个按钮...", "fonts/SIMYOU.TTF", 24);
	//auto button = Button::create();
	auto vSize = Director::getInstance()->getVisibleSize();

	label->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
	label_2->setPosition(Vec2(vSize.width / 2, vSize.height / 3));
	label_3->setPosition(Vec2(vSize.width / 2, vSize.height / 4));
	label_4->setPosition(Vec2(vSize.width / 2, vSize.height / 5));

	auto backToLevel = Button::create("back.png", "backe.png");
	//backToLevel->setTitleText("Back");
	//backToLevel->setTitleFontSize(40);
	backToLevel->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
		case Widget::TouchEventType::MOVED:
		case Widget::TouchEventType::CANCELED:
			break;

		case Widget::TouchEventType::ENDED:
			tsm->goMenuScene();
			break;
		}
	});
	backToLevel->setPosition(Vec2(vSize.width - backToLevel->getContentSize().width / 2, backToLevel->getContentSize().height));
	addChild(backToLevel);


	addChild(label);
	addChild(label_2);
	addChild(label_3);
	addChild(label_4);
	return true;
}
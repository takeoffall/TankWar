#include "Menu.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

bool MenuLayer::init()
{
	if (!Scene::init()) { return false; }
	
	display();
	return true;
}

void MenuLayer::display()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto startGame = MenuItemLabel::create(Label::createWithTTF("开始游戏", "fonts/SIMYOU.TTF", 36), CC_CALLBACK_1(MenuLayer::startGameCallback, this));
	auto loadGame = MenuItemLabel::create(Label::createWithTTF("加载游戏", "fonts/SIMYOU.TTF", 36), CC_CALLBACK_1(MenuLayer::loadGameCallback, this));
	auto settings = MenuItemLabel::create(Label::createWithTTF("设置", "fonts/SIMYOU.TTF", 36), CC_CALLBACK_1(MenuLayer::settingsCallback, this));
	auto exit = MenuItemLabel::create(Label::createWithTTF("退出", "fonts/SIMYOU.TTF", 36), CC_CALLBACK_1(MenuLayer::exitCallback, this));
	// create menu, it's an autorelease object
	auto menu = Menu::create(startGame, loadGame, settings, exit, NULL);
	menu->alignItemsVerticallyWithPadding(50);
	menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(menu);
}

void MenuLayer::startGameCallback(Ref* pSender)
{
	tsm->goLevelScene();
}

void MenuLayer::loadGameCallback(Ref* pSender)
{
	log("load game");
}

void MenuLayer::settingsCallback(Ref* pSender)
{
	log("settings");
}

void MenuLayer::exitCallback(Ref* pSender)
{
	log("exit");
	Director::getInstance()->end();
}
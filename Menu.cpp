#include "Menu.h"
USING_NS_CC;

bool MenuLayer::init()
{
	if (!Scene::init()) { return false; }
	
	display();
	return true;
}

void MenuLayer::display()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto startGame = MenuItemLabel::create(Label::createWithTTF("Start Game", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(MenuLayer::startGameCallback, this));
	auto loadGame = MenuItemLabel::create(Label::createWithTTF("Load Game", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(MenuLayer::loadGameCallback, this));
	auto settings = MenuItemLabel::create(Label::createWithTTF("Settings", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(MenuLayer::settingsCallback, this));
	auto exit = MenuItemLabel::create(Label::createWithTTF("Exit", "fonts/Marker Felt.ttf", 36), CC_CALLBACK_1(MenuLayer::exitCallback, this));
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
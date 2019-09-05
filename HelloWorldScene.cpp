#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LevelChoose.h"
#include "Menu.h"
#include "GameScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    log("Error while loading: %s\n", filename);
    log("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()//loadResourceScene，资源加载完后自动跳转至菜单场景
{
    if ( !Scene::init() ){ return false;}

	display();
	scheduleOnce([this](float delta) {
		goMenuScene();
	}, 1.0f, "yaeryo");

    return true;
}

void HelloWorld::display()
{
	auto vSize = Director::getInstance()->getVisibleSize();
	auto label = Label::createWithTTF("loading...", "fonts/Marker Felt.ttf", 36);
	label->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
	addChild(label);
}

void HelloWorld::goLevelScene()
{
	auto scene = Scene::create();
	auto layer = LevelLayer::create();
	layer->tsm = this;
	scene->addChild(layer);
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::goMenuScene()
{
	auto scene = Scene::create();
	auto layer = MenuLayer::create();
	layer->tsm = this;
	scene->addChild(layer);
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::goGameSceneWithMap(const std::string& tmxFile)
{
	auto scene = Scene::create();
	auto layer = GameLayer::createWithMap(tmxFile);
	layer->tsm = this;
	scene->addChild(layer);
	Director::getInstance()->replaceScene(scene);
}
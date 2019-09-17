#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LevelChoose.h"
#include "Menu.h"
#include "GameScene.h"
#include "OverScene.h"

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    log("Error while loading: %s\n", filename);
    log("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool HelloWorld::init()//loadResourceScene，资源加载完后自动跳转至菜单场景
{
    if ( !Scene::init() ){ return false;}
	
	// Create the loading bar
	/*loadingBar = LoadingBar::create("cocosui/slider_bar_active_9patch.png");
	loadingBar->setScale9Enabled(true);
	loadingBar->setCapInsets(Rect(0, 0, 0, 0));
	loadingBar->setContentSize(Size(300, 13));
	auto vSize = Director::getInstance()->getVisibleSize();
	loadingBar->setPosition(vSize.width / 2, vSize.height * 0.25);
	addChild(loadingBar);*/
	display();
	scheduleOnce([&](float dt) {
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
	auto scene = LevelLayer::create();
	scene->tsm = this;
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::goMenuScene()
{
	auto scene = MenuLayer::create();
	scene->tsm = this;
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::goGameSceneWithMap(int level)
{
	auto scene = GameLayer::createWithMap(StringUtils::format("map%d.tmx", level));
	scene->level = level;
	scene->tsm = this;
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::goGameOverScene()
{
	auto scene = OverScene::create();
	scene->tsm = this;
	Director::getInstance()->replaceScene(scene);
}

void HelloWorld::update(float dt)
{
	if (count >= total)
	{
		goMenuScene();
		unscheduleUpdate();
	}
}
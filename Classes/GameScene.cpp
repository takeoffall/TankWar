#include "ui\CocosGUI.h"
#include <vector>
#include "MapLayer.h"
#include "Bullet.h"
#include "Enemy.h"
//#include "GameScene.h"
#include "TankController.h" //new op

#include "AudioEngine.h"
using namespace experimental;

USING_NS_CC;
using namespace ui;

void GameLayer::testPlist(const std::string& file)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");
	auto s = Sprite::createWithSpriteFrameName(file);
	s->setPosition(vSize.width / 2, vSize.height / 2);
	addChild(s, 3);
}

bool GameLayer::init(const std::string& tmxFile)
{
	if (!Scene::init()) { return false; }

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("images.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");//use ice
	this->tmxFile = tmxFile;
	vSize = Director::getInstance()->getVisibleSize();
	initActionSet();
	
	addBackButton();
	addMap();
	addPlayer1("tk1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST);
	addPlayer2("tk1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST);
	
	addEnemy("en1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en1");
	addEnemy("en2.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en2");
	addEnemy("en3.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en3");

	//scheduleUpdate();//一进来就检测游戏结果，map都还没初始化，太快不行
	listenControlMoving();
	listenControlScaling();
	checkGameResult();

	//testPlist("crystal.png");
	return true;
}

GameLayer* GameLayer::createWithMap(const std::string& tmxFile)
{
	GameLayer *pRet = new(std::nothrow) GameLayer;
	if (pRet && pRet->init(tmxFile))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

void GameLayer::listenControlScaling()
{
	auto mouseEventListener = EventListenerMouse::create();
	mouseEventListener->onMouseScroll = [this](EventMouse* event) {
		for (auto &i : mycontrols) {
			if (i->getBoundingBox().containsPoint(event->getLocationInView())) {
				if (event->getScrollY() > 0) {
					if (i->getScale() * 0.8 >= 0.25f) {
						i->setScale(i->getScale() * 0.8f);
					}
				}
				else {
					i->setScale(i->getScale() * 1.25f);
				}
				break;
			}
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEventListener, this);
}

void GameLayer::listenControlMoving()
{
	touchedControls = nullptr;
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event) {
		auto touchLocation = touch->getLocation();
		for (auto &i : mycontrols)
		{
			if (i->getBoundingBox().containsPoint(touchLocation))
			{
				//i->isTouched = true;
				touchedControls = i;
				break;
			}
			
		}
		return true;
	};
	listener->onTouchMoved = [this](Touch* touch, Event* event) {
		if (touchedControls)
			touchedControls->setPosition(touch->getLocation());
	};
	listener->onTouchEnded = [this](Touch* touch, Event* event) {
		
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}


void GameLayer::addBackButton()
{
	auto backToLevel = Button::create();
	backToLevel->setTitleText("BackToLevel");
	backToLevel->setTitleFontSize(40);
	backToLevel->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
		case Widget::TouchEventType::MOVED:
		case Widget::TouchEventType::CANCELED:
			break;


		case Widget::TouchEventType::ENDED:
			Director::getInstance()->pause();
			auto bg = Sprite::create("control_bg.png");
			bg->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
			this->addChild(bg, 1);

			auto btn1 = Button::create();
			btn1->setTitleText("sure");
			btn1->setTitleFontSize(30);
			btn1->setColor(Color3B(50, 200, 30));
			btn1->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type)
			{
				if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->resume();
					tsm->goLevelScene();
				}
			});
			btn1->setPosition(Vec2(40, 20));
			bg->addChild(btn1);

			auto btn2 = Button::create();
			btn2->setTitleText("cancel");
			btn2->setTitleFontSize(30);
			btn2->setColor(Color3B(50, 200, 30));
			btn2->addTouchEventListener([this, bg](Ref* pSender, Widget::TouchEventType type) {
				if (type == Widget::TouchEventType::ENDED)
				{
					bg->removeFromParentAndCleanup(true);
					Director::getInstance()->resume();
				}
			});
			btn2->setPosition(Vec2(160, 20));
			bg->addChild(btn2);

			break;

		}
	});
	backToLevel->setPosition(Vec2(vSize.width - backToLevel->getContentSize().width / 2, backToLevel->getContentSize().height));
	addChild(backToLevel);
}

void GameLayer::addMap()
{
	//m_map = new MapLayer(tmxFile);
	m_map = MapLayer::create(tmxFile);
	m_map->setPosition(Vec2(vSize.width / 2, vSize.height / 2));
	addChild(m_map);
	AudioEngine::play2d("sounds/levelstarting.mp3");
}

void GameLayer::addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	auto pos = m_map->getObjPos("objects", "pl1");
	auto objSize = m_map->getObjSize("objects", "pl1");

	auto tank = Tank::create(name, moveSpeed, shootSpeed);
	tank->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	m_map->addChild(tank, 0, "tank");
	m_map->tankSet.pushBack(tank);//remember
	
	tank->gameLayer = this;
	tank->addController("playercontroller_2.xml");
}

void GameLayer::addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName)
{
	auto pos = m_map->getObjPos("objects", posName);
	auto objSize = m_map->getObjSize("objects", posName);

	auto animation = AnimationCache::getInstance()->getAnimation("enemyborn");
	auto action = Animate::create(animation);
	auto born = Sprite::create();
	born->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	m_map->addChild(born);
	born->runAction(Sequence::create(action, RemoveSelf::create(), CallFunc::create([=]() {
		auto enemy = Enemy::create(name, moveSpeed, shootSpeed);
		enemy->setScale(0.99f);
		enemy->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
		m_map->addChild(enemy, 0, "enemy");
		m_map->tankSet.pushBack(enemy);
		enemy->gameLayer = this;
	}), NULL));

	//auto enemy = Enemy::create(name, moveSpeed, shootSpeed);
	//enemy->setScale(0.99f);
	//enemy->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	//m_map->addChild(enemy, 0, "enemy");
	//m_map->tankSet.pushBack(enemy);
	////m_map->enemySet.pushBack(enemy);
	////enemy->map = m_map;
	//enemy->gameLayer = this;
}

void GameLayer::genEnemyRandom()
{
	auto message = Dictionary::createWithContentsOfFile("enemy.xml");    //读取xml文件，文件在Resources目录下
	auto dic = (__Dictionary *)message->randomObject();
	auto filename = dic->valueForKey("file_name")->getCString();
	auto randomPos = (__String *)((__Dictionary *)dic->objectForKey("pos"))->randomObject();
	auto pos = randomPos->getCString();

	int n[3][2] = { {(int)MOVE_SPEED::MID, (int)SHOOT_SPEED::FAST},
				{(int)MOVE_SPEED::SLOW, (int)SHOOT_SPEED::MID},
				{(int)MOVE_SPEED::SLOW, (int)SHOOT_SPEED::FAST}
	};
	int s = real_rand_0_1() * 3;
	
	addEnemy(filename, (MOVE_SPEED)n[s][0], (SHOOT_SPEED)n[s][1], pos);
}

void GameLayer::checkGameResult()
{
	totalScore = 0;
	schedule([&](float dt) {
		bool haveTank = false;
		for (auto &i : m_map->tankSet)
		{
			if (i->getName() == "tank")
			{
				haveTank = true;
				//totalScore = i->score;
			}
		}
		if (!haveTank)
		{
			tsm->goMenuScene();//后面再改ui及效果
			unschedule("CheckGameResult");
		}
		if (totalScore >= 100)
		{
			goNextLevel();
			unschedule("CheckGameResult");
		}
	}, 0.0f, kRepeatForever, 0.0f, "CheckGameResult");
}

void GameLayer::update(float dt)
{
	
}

void GameLayer::initActionSet()
{
	auto frameCache = SpriteFrameCache::getInstance();
	SpriteFrame* frame = NULL;
	//3.0中改用vector 而不是用Array
	Vector<SpriteFrame *> frameVector;

	for (int i = 1; i <= 2; i++) {
		//从缓存池中加载精灵到Vector
		frame = frameCache->getSpriteFrameByName(StringUtils::format("explode%d.png", i));
		frameVector.pushBack(frame);
	}
	//用vector里面的SpriteFrame列表创建Animation  以及设置一些参数
	auto tankboom = Animation::createWithSpriteFrames(frameVector, 0.1f);
	AnimationCache::getInstance()->addAnimation(tankboom, "tankboom");


	frameVector.clear();
	for (int i = 1; i <= 3; i++) {
		frame = frameCache->getSpriteFrameByName(StringUtils::format("explode-%d.png", i));
		frameVector.pushBack(frame);
	}
	auto enemyboom = Animation::createWithSpriteFrames(frameVector, 0.1f);//不设置无限循环
	AnimationCache::getInstance()->addAnimation(enemyboom, "enemyboom");

	//enemy born
	frameVector.clear();
	for (int i = 1; i <= 4; i++) {
		frame = frameCache->getSpriteFrameByName(StringUtils::format("xing%d.png", i));
		frameVector.pushBack(frame);
	}
	auto enemyborn = Animation::createWithSpriteFrames(frameVector, 0.1f);//不设置无限循环
	AnimationCache::getInstance()->addAnimation(enemyborn, "enemyborn");

	//tank born
	frameVector.clear();
	for (int i = 1; i <= 2; i++) {
		frame = frameCache->getSpriteFrameByName(StringUtils::format("shield%d.png", i));
		frameVector.pushBack(frame);
	}
	auto tankborn = Animation::createWithSpriteFrames(frameVector, 0.1f);//不设置无限循环
	AnimationCache::getInstance()->addAnimation(tankborn, "tankborn");
}

void GameLayer::goNextLevel()
{
	if (++level > 20)
	{
		tsm->goGameOverScene();//通关，后面再改改效果
	}
	else
		tsm->goGameSceneWithMap(level);//下一关，后面改效果
}

void GameLayer::addPlayer1(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	auto pos = m_map->getObjPos("objects", "pl1");
	auto objSize = m_map->getObjSize("objects", "pl1");

	player1 = Tank::create(file, moveSpeed, shootSpeed);
	player1->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	m_map->addChild(player1, 0, "tank");
	m_map->tankSet.pushBack(player1);//remember
	
	player1->gameLayer = this;

	player1->addController("playercontroller_1.xml");

	player1->tankController->addPlayerHP("PlayerHP.png", "tk.png", Point(vSize.width / 5, vSize.height * 3 / 4));
	player1->tankController->addInventory("Inventory.png", Point(vSize.width / 5, vSize.height * 1 / 4));
}

void GameLayer::addPlayer2(const std::string &file, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	auto pos = m_map->getObjPos("objects", "pl2");
	auto objSize = m_map->getObjSize("objects", "pl2");

	player2 = Tank::create(file, moveSpeed, shootSpeed);
	player2->setPosition(pos.x + objSize.width / 2, pos.y + objSize.height / 2);
	m_map->addChild(player2, 0, "tank");
	m_map->tankSet.pushBack(player2);//remember

	player2->gameLayer = this;

	player2->addController("playercontroller_2.xml");

	player2->tankController->addPlayerHP("PlayerHP.png", "tk.png", Point(vSize.width * 4 / 5, vSize.height * 3 / 4));
	player2->tankController->addInventory("Inventory.png", Point(vSize.width * 4 / 5, vSize.height * 1 / 4));
}
#include "ui\CocosGUI.h"
#include <vector>
#include "MapLayer.h"
#include "Bullet.h"
#include "Enemy.h"
//#include "GameScene.h"

#include "AudioEngine.h"
using namespace experimental;

USING_NS_CC;
using namespace ui;

void GameLayer::test(const std::string& file)
{
	auto s = Sprite::createWithSpriteFrameName(file);
	s->setPosition(vSize.width / 2, vSize.height / 2);
	addChild(s, 3);
}

bool GameLayer::init(const std::string& tmxFile)
{
	if (!Layer::init()) { return false; }

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("images.plist");
	this->tmxFile = tmxFile;
	vSize = Director::getInstance()->getVisibleSize();
	initActionSet();
	
	addMap();
	addPlayer1("tk1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST);
	addPlayer1_HP("PlayerHP.png", "tk.png", Point(100, 100));
	addPlayer2("tk1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST);
	addPlayer2_HP("PlayerHP.png", "tk.png", Point(700, 100));
	//addTank("tk1.png", MOVE_SPEED::FAST, SHOOT_SPEED::FAST);
	addEnemy("p2-c.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en1");
	addEnemy("p2.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en2");
	addEnemy("shield1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en3");

	addBackButton();
	addInventory();
	//addPlayerHP();
	//addPlayerHP("PlayerHP.png", "tk.png");
	//scheduleUpdate();//一进来就检测游戏结果，map都还没初始化，太快不行
	listenControlMoving();
	listenControlScaling();
	checkGameResult();

	//test("shield1.png");
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

void GameLayer::addPlayerHP()
{
	player1 = nullptr;
	auto bg = Sprite::create("tk.png");
	bg->setOpacity(20);
	bg->setPosition(Point(vSize.width - (vSize.width - m_map->getBoundingBox().getMaxX()) / 2, vSize.height / 2));
	addChild(bg);

	auto blood = Sprite::create("tk.png");
	blood->setColor(Color3B::RED);
	progress = ProgressTimer::create(blood);
	progress->setType(ProgressTimer::Type::BAR);//设置进程条的类型
	progress->setBarChangeRate(Point(0, 1));//
	progress->setMidpoint(Point(0, 0));//设置进度的运动方向
	progress->setPosition(Point(vSize.width - (vSize.width - m_map->getBoundingBox().getMaxX()) / 2, vSize.height / 2));
	addChild(progress);

	for (auto &i : m_map->tankSet)
	{
		if (i->getName() == "tank") 
		{
			player1 = i;
			break;
		}
	}
	schedule([&](float dt) {
		if (player1 != nullptr && player1->HP >= 0 && player1->HP <= 100)
			progress->setPercentage(player1->HP);
	}, 0.0f, kRepeatForever, 0.0f, "HP");
}

void GameLayer::addPlayerHP(const std::string& name, const std::string& source)
{
	php = PlayerHP::create(name, source);
	php->setPosition(Vec2(100, 100));
	addChild(php);

	schedule([=](float dt) {
		if (player1->isRunning() && player1->HP >= 0 && player1->HP <= 100)
			progress->setPercentage(player1->HP);
	}, 0.0f, kRepeatForever, 0.0f, "HP");
}

void GameLayer::listenControlScaling()
{
	auto mouseEventListener = EventListenerMouse::create();
	mouseEventListener->onMouseScroll = [this](EventMouse* event) {
	
		if (playerHP_2->getBoundingBox().containsPoint(event->getLocationInView())) {
			
			if (event->getScrollY() > 0) {
				if (playerHP_2->getScale() * 0.8 >= 0.25f) {
					playerHP_2->setScale(playerHP_2->getScale() * 0.8f);
				}
			}else {
				playerHP_2->setScale(playerHP_2->getScale() * 1.25f);
			}
		}
		else if (playerHP_1->getBoundingBox().containsPoint(event->getLocationInView())) {

			if (event->getScrollY() > 0) {
				if (playerHP_1->getScale() * 0.8 >= 0.25f) {
					playerHP_1->setScale(playerHP_1->getScale() * 0.8f);
				}
			}
			else {
				playerHP_1->setScale(playerHP_1->getScale() * 1.25f);
			}
		}
		else if (inventory->getBoundingBox().containsPoint(event->getLocationInView())) {
			if (event->getScrollY() > 0) {
				if (inventory->getScale() * 0.8 >= 0.25f) {
					inventory->setScale(inventory->getScale() * 0.8f);
				}
			}else {
				inventory->setScale(inventory->getScale() * 1.25f);
			}
		}
		
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEventListener, this);
}

void GameLayer::listenControlMoving()
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event) {
		auto touchLocation = touch->getLocation();
		if (playerHP_2->getBoundingBox().containsPoint(touchLocation)) {
			playerHP_2->isTouched = true;
		}
		else if (playerHP_1->getBoundingBox().containsPoint(touchLocation)) {
			playerHP_1->isTouched = true;
		}
		else if (inventory->getBoundingBox().containsPoint(touchLocation)) {
			inventory->isTouched = true;
		}

		return true;
	};
	listener->onTouchMoved = [this](Touch* touch, Event* event) {
		if (playerHP_2->isTouched) {
			playerHP_2->setPosition(touch->getLocation());
		}
		if (playerHP_1->isTouched) {
			playerHP_1->setPosition(touch->getLocation());
		}
		else if (inventory->isTouched) {
			inventory->setPosition(touch->getLocation());
		}
	};
	listener->onTouchEnded = [this](Touch* touch, Event* event) {
		playerHP_2->isTouched = false;
		playerHP_1->isTouched = false;
		inventory->isTouched = false;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameLayer::addInventory()
{
	inventory = Inventory::create("Inventory.png");
	inventory->setPosition(vSize.width / 2, vSize.height / 2);
	addChild(inventory);
}

void GameLayer::EX_AddProp(Props* p)
{
	inventory->addItem(p);
}

void GameLayer::EX_RemoveProp(const std::string& name, bool removeAll)
{
	inventory->removeItem(name, removeAll);
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
	auto randomPos = (String *)((__Dictionary *)dic->objectForKey("pos"))->randomObject();
	auto pos = randomPos->getCString();

	addEnemy(filename, MOVE_SPEED::MID, SHOOT_SPEED::FAST, pos);
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

void GameLayer::update(float delta)
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
}

void GameLayer::addPlayer1_HP(const std::string& bg, const std::string& source, Point pos)
{
	playerHP_1 = PlayerHP::create(bg, source);
	playerHP_1->setPosition(pos);
	addChild(playerHP_1);

	schedule([=](float dt) {
		if (player1 != nullptr && player1->HP >= 0 && player1->HP <= 100)
			playerHP_1->getProgressBar()->setPercentage(player1->HP);
	}, 0.0f, kRepeatForever, 0.0f, "playerHP_1");
}
void GameLayer::addPlayer2_HP(const std::string& bg, const std::string& source, Point pos)
{
	playerHP_2 = PlayerHP::create(bg, source);
	playerHP_2->setPosition(pos);
	addChild(playerHP_2);

	schedule([=](float dt) {
		if (player2 != nullptr && player2->HP >= 0 && player2->HP <= 100)
			playerHP_2->getProgressBar()->setPercentage(player2->HP);
	}, 0.0f, kRepeatForever, 0.0f, "playerHP_2");
}
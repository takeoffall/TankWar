
#include "ui\CocosGUI.h"
#include <vector>
//#include "TankController.h"
#include "MapLayer.h"
#include "Bullet.h"
#include "Enemy.h"
//#include "GameScene.h"

USING_NS_CC;
using namespace ui;

bool GameLayer::init(const std::string& tmxFile)
{
	if (!Layer::init()) { return false; }
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("images.plist");
	this->tmxFile = tmxFile;
	vSize = Director::getInstance()->getVisibleSize();
	initActionSet();
	
	addMap();
	addBackButton();
	//addTank();
	addTank("tk1.png", MOVE_SPEED::FAST, SHOOT_SPEED::FAST);
	addEnemy("en1.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en1");
	addEnemy("en2.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en2");
	addEnemy("en3.png", MOVE_SPEED::MID, SHOOT_SPEED::FAST, "en3");

	/*addProps("props-tank.png", "t3");
	addProps("props-protect.png", "t1");
	addProps("props-protect.png", "en2");*/
	addPlayerHP();
	addInventory();

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
	tank = nullptr;
	auto bg = Sprite::create("tk.png");
	bg->setOpacity(20);
	bg->setPosition(Point(vSize.width - (vSize.width - m_map->getBoundingBox().getMaxX()) / 2, vSize.height / 2));
	addChild(bg);

	auto blood = Sprite::create("tk.png");
	blood->setColor(Color3B(255, 0, 0));
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
			tank = i;
			break;
		}
	}
	schedule([&](float dt) {
		if (tank != nullptr && tank->HP >= 0 && tank->HP <= 100)
			progress->setPercentage(tank->HP);
	}, 0.0f, kRepeatForever, 0.0f, "HP");
}

void GameLayer::addInventory()
{
	inventory = Inventory::create("Inventory.png");
	inventory->setPosition(vSize.width / 2, vSize.height / 2);
	addChild(inventory);
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event) {
		auto touchLocation = touch->getLocation();
		if (inventory->getBoundingBox().containsPoint(touchLocation)) {
			//currentPoint = touchLocation;
			inventory->isTouched = true;
		}
		else {
			inventory->isTouched = false;
		}
		return true;
	};
	listener->onTouchMoved = [this](Touch* touch, Event* event) {
		if (inventory->isTouched) {
			inventory->setPosition(touch->getLocation());
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	/*inventory->addItem("props-protect.png");
	inventory->addItem("props-protect.png");
	inventory->addItem("props-protect.png");
	inventory->addItem("props-protect.png");*/
}

void GameLayer::EX_AddProp(Sprite* s)
{
	inventory->addItem(s);
}

void GameLayer::EX_AddProp(const std::string& name)
{
	inventory->addItem(name);
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
}

void GameLayer::addTank(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	auto pos = m_map->getObjPos("objects", "pl1");
	auto objSize = m_map->getObjSize("objects", "pl1");

	auto tank = Tank::create(name, moveSpeed, shootSpeed);
	tank->setPosition(Point(pos.x + objSize.width / 2, pos.y + objSize.height / 2));

	m_map->addChild(tank, 0, "tank");
	m_map->tankSet.pushBack(tank);//remember
	
	//tank->map = m_map;
	tank->gameLayer = this;
	tank->addController();
}

void GameLayer::addEnemy(const std::string &name, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed, const std::string &posName)
{
	auto pos = m_map->getObjPos("objects", posName);
	auto objSize = m_map->getObjSize("objects", posName);

	auto enemy = Enemy::create(name, moveSpeed, shootSpeed);
	enemy->setScale(0.99f);
	enemy->setPosition(Point(pos.x + objSize.width / 2, pos.y + objSize.height / 2));
	m_map->addChild(enemy, 0, "enemy");
	m_map->tankSet.pushBack(enemy);

	//enemy->map = m_map;
	enemy->gameLayer = this;
}

void GameLayer::addProps(const std::string &name, const std::string &posName)
{
	auto pos = m_map->getObjPos("objects", posName);
	auto objSize = m_map->getObjSize("objects", posName);

	auto s = Sprite::createWithSpriteFrameName(name);
	s->setPosition(Point(pos.x + objSize.width / 2, pos.y + objSize.height / 2));
	m_map->addChild(s, 0, name);

	s->scheduleOnce([s, this](float dt) {
		auto blink = Blink::create(1.0f, 5);
		s->runAction(Sequence::create(blink, RemoveSelf::create(), [s, this]() {
			s->removeFromParentAndCleanup(true); }, NULL));
	}, 5.0f, "time_flash");//5s后开始闪烁
}

void GameLayer::update(float delta)
{
	//检测游戏结果

}

void GameLayer::initActionSet()
{
	auto frameCache = SpriteFrameCache::getInstance();
	SpriteFrame* frame = NULL;
	//3.0中改用vector 而不是用Array
	Vector<SpriteFrame *> frameVector;

	for (int i = 1; i <= 2; i++) {
		//从缓存池中加载精灵到Vector
		frame = frameCache->spriteFrameByName(StringUtils::format("explode%d.png", i));
		frameVector.pushBack(frame);
	}
	//用vector里面的SpriteFrame列表创建Animation  以及设置一些参数
	auto tankboom = Animation::createWithSpriteFrames(frameVector, 0.1f, 2);
	AnimationCache::getInstance()->addAnimation(tankboom, "tankboom");


	frameVector.clear();
	for (int i = 1; i <= 3; i++) {
		frame = frameCache->spriteFrameByName(StringUtils::format("explode-%d.png", i));
		frameVector.pushBack(frame);
	}
	auto enemyboom = Animation::createWithSpriteFrames(frameVector, 0.1f);//不设置无限循环
	AnimationCache::getInstance()->addAnimation(enemyboom, "enemyboom");
}

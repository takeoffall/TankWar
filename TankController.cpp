#include "TankController.h"
#include "Bullet.h"
#include "GameScene.h"
#include "MapLayer.h"
#include "ModalLayer.h" // new oppo
#include "AudioEngine.h"

#include "ui/CocosGUI.h"
using namespace ui;
using namespace experimental;

TankController::TankController(const std::string& xml_file)
{
	auto message = Dictionary::createWithContentsOfFile(xml_file.c_str());
	auto firekey = ((__Dictionary *)message->objectForKey("key"))->valueForKey("fire_key")->intValue();
	auto directionkey = (__Dictionary *)((__Dictionary *)message->objectForKey("key"))->objectForKey("direction_key");
	auto leftkey = directionkey->valueForKey("LEFT")->intValue();
	auto rightkey = directionkey->valueForKey("RIGHT")->intValue();
	auto upkey = directionkey->valueForKey("UP")->intValue();
	auto downkey = directionkey->valueForKey("DOWN")->intValue();
	auto buildkey = ((__Dictionary *)message->objectForKey("key"))->valueForKey("build_key")->intValue();

	key_fire = (EventKeyboard::KeyCode)firekey;
	key_direction_down = (EventKeyboard::KeyCode)downkey;
	key_direction_left = (EventKeyboard::KeyCode)leftkey;
	key_direction_right = (EventKeyboard::KeyCode)rightkey;
	key_direction_up = (EventKeyboard::KeyCode)upkey;
	key_build = (EventKeyboard::KeyCode)buildkey;

	inventory = nullptr;
	playerHP = nullptr;
}

TankController* TankController::create(Tank *tank, const std::string& xml_file)
{
	TankController *tkcnter = new (std::nothrow) TankController(xml_file);
	if (tkcnter && tkcnter->init(tank))
	{
		tkcnter->autorelease();
		return tkcnter;
	}
	CC_SAFE_DELETE(tkcnter);
	return nullptr;
}

bool TankController::init(Tank *tank)
{
	if (!Node::init())
		return false;
	this->tank = tank;
	tank->bulletIce = false;
	
	m_moveListener = EventListenerKeyboard::create();
	m_fireListener = EventListenerKeyboard::create();
	m_buildListener = EventListenerKeyboard::create();

	tankBorn();
	listenMove();//Include collide
	listenFire();
	listenBuild();
	scheduleUpdate();
	return true;
}

void TankController::update(float dt)
{
	if (tank->HP <= 0)
	{
		AudioEngine::play2d("sounds/explosion-player.mp3");

		tank->gameLayer->m_map->tankSet.eraseObject(tank);

		auto animation = AnimationCache::getInstance()->getAnimation("tankboom");
		auto action = Animate::create(animation);
		tank->runAction(Sequence::create(action, CCRemoveSelf::create(), NULL));
		unscheduleUpdate();
	}
	
	if (playerHP != nullptr && tank->HP >= 0 && tank->HP <= 100)
		playerHP->getProgressBar()->setPercentage(tank->HP);
	
}

void TankController::tankBorn()
{
	auto s = Sprite::create();
	s->setPosition(tank->getContentSize().width / 2, tank->getContentSize().height / 2);
	tank->addChild(s);

	tank->isProtected = true;//Invincible Time(无敌时间)

	auto animation = AnimationCache::getInstance()->getAnimation("tankborn");
	animation->setLoops(9);
	auto action = Animate::create(animation);
	s->runAction(Sequence::create(action, RemoveSelf::create(), CallFunc::create([&]() {tank->isProtected = false; }), NULL));
}

void TankController::building()
{
	schedule([&](float dt) {

		struct timeval psv;
		gettimeofday(&psv, NULL);    // 计算时间种子   
		unsigned int tsrans = psv.tv_sec * 1000 + psv.tv_usec / 1000;    // 初始化随机数   
		srand(tsrans);
		int a = tsrans % 4;

		int g1, g2, g3, g4;//每一排的起始gid值
		if (tank->buildType == "block.png")
		{
			g1 = 1, g2 = 29, g3 = 57, g4 = 85;
		}
		else if (tank->buildType == "steel.png")
		{
			g1 = 5, g2 = 33, g3 = 61, g4 = 89;
		}
		else if (tank->buildType == "grass.png")
		{
			g1 = 9, g2 = 37, g3 = 65, g4 = 93;
		}
		else if (tank->buildType == "snow.png")
		{
			g1 = 13, g2 = 41, g3 = 69, g4 = 97;
		}
		//选行
		int row;
		switch (a)
		{
			case 0:
				row = g1;
			case 1:
				row = g2;
			case 2:
				row = g3;
			case 3:
				row = g4;
		}

		auto p1 = Point(tank->getBoundingBox().getMinX() + 0.000003, tank->getBoundingBox().getMaxY());//左上
		auto p2 = Point(tank->getBoundingBox().getMaxX(), tank->getBoundingBox().getMaxY());//右上
		auto p3 = Point(tank->getBoundingBox().getMinX() + 0.000003, tank->getBoundingBox().getMinY());//左下
		auto p4 = Point(tank->getBoundingBox().getMaxX(), tank->getBoundingBox().getMinY());//右下
		//参数0.000003是为了防止左边界超出地图，移动的时候也加了这个参数(bug)
		

		if (tank->m_direction == DIRECTION::UP)
		{
			auto position1 = tank->gameLayer->m_map->tileCoordForPosition(p3);
			auto position2 = tank->gameLayer->m_map->tileCoordForPosition(p4);
			
			for (auto i = position1.x; i <= position2.x; i++)
			{
				tank->gameLayer->m_map->layer1->setTileGID(row + a, Vec2(i, position1.y));
			}
		}

		else if (tank->m_direction == DIRECTION::DOWN)
		{
			auto position1 = tank->gameLayer->m_map->tileCoordForPosition(p1);
			auto position2 = tank->gameLayer->m_map->tileCoordForPosition(p2);

			for (auto i = position1.x; i <= position2.x; i++)
			{
				tank->gameLayer->m_map->layer1->setTileGID(row + a, Vec2(i, position1.y));
			}
		}

		else if (tank->m_direction == DIRECTION::LEFT)
		{
			auto position1 = tank->gameLayer->m_map->tileCoordForPosition(p4);
			auto position2 = tank->gameLayer->m_map->tileCoordForPosition(p2);
			
			for (auto i = position2.y; i <= position1.y; i++)
			{
				tank->gameLayer->m_map->layer1->setTileGID(row + a, Vec2(position1.x, i));
			}
		}

		else if (tank->m_direction == DIRECTION::RIGHT)
		{
			auto position1 = tank->gameLayer->m_map->tileCoordForPosition(p3);
			auto position2 = tank->gameLayer->m_map->tileCoordForPosition(p1);

			for (auto i = position2.y; i <= position1.y; i++)
			{
				tank->gameLayer->m_map->layer1->setTileGID(row + a, Point(position1.x, i));
			}
		}

	}, 0.0f, kRepeatForever, 0.0f, "build");
}

void TankController::tileSelect()
{
	auto selectTile = ModalLayer::create("modal.png");
	auto listview = ListView::create();
	listview->setTouchEnabled(true);
	listview->setBounceEnabled(true);
	listview->setDirection(ui::ListView::Direction::HORIZONTAL);
	listview->setSize(Size(selectTile->bgSprite->getContentSize().width, selectTile->bgSprite->getContentSize().height / 2));
	listview->setPosition(Vec2(0, listview->getSize().height));
	selectTile->bgSprite->addChild(listview);
	listview->setItemsMargin(20);

	listview->addEventListener([this](Ref* pSender, ListView::EventType type) {

		auto listView = (ListView*)pSender;
		if (type == ListView::EventType::ON_SELECTED_ITEM_START)
		{
			auto idnex = listView->getCurSelectedIndex();
			switch (idnex) {
			case 0:
				tank->buildType = "block.png";
				break;
			case 1:
				tank->buildType = "steel.png";
				break;
			case 2:
				tank->buildType = "grass.png";
				break;
			case 3:
				tank->buildType = "snow.png";
				break;
			case 4:
				tank->buildType = "river.png";
				break;
			}
		}

		if (type == ListView::EventType::ON_SELECTED_ITEM_END)
		{
			//selectTile->removeFromParentAndCleanup(true);
		}

	});

	auto block = Button::create("block.png", "block_select.png");
	auto steel = Button::create("steel.png", "steel.png");
	auto grass = Button::create("grass.png", "grass.png");
	auto snow = Button::create("snow.png", "snow.png");
	auto river = Button::create("river.png", "river.png");
	listview->pushBackCustomItem(block);
	listview->pushBackCustomItem(steel);
	listview->pushBackCustomItem(grass);
	listview->pushBackCustomItem(snow);
	listview->pushBackCustomItem(river);

	tank->gameLayer->m_map->addChild(selectTile);
}

void TankController::listenBuild()
{
	keyShift = false;
	m_buildListener->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_build)
		{
			if (tank->enableBuildFunction)
			{
				if (tank->buildType != "")
				{
					log("build type: %s", tank->buildType.c_str());
					building();	
				}
				else
				{
					tileSelect();
				}
				/*auto listenr = EventListenerKeyboard::create();
				listenr->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {*/
				if (keyShift)
				{
					tileSelect();
				}

			}
			else
			{
				log("not get");
				//弹幕弹出未获得的效果
			}
		}

		if (keycode == EventKeyboard::KeyCode::KEY_SHIFT)
		{
			keyShift = true;
		}

	};

	m_buildListener->onKeyReleased = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_build)
		{
			unschedule("build");
		}
		if (keycode == EventKeyboard::KeyCode::KEY_SHIFT)
		{
			keyShift = false;
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_buildListener, this);
}

void TankController::listenMove()
{
	if (tank == nullptr)
		return;

	m_moveListener->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {
		
		if (keycode == key_direction_left)
		{
			this->moving(DIRECTION::LEFT);
		}

		if (keycode == key_direction_right)
		{
			this->moving(DIRECTION::RIGHT);
		}

		if (keycode == key_direction_up)
		{
			this->moving(DIRECTION::UP);
		}

		if (keycode == key_direction_down)
		{

			this->moving(DIRECTION::DOWN);
		}

	};

	m_moveListener->onKeyReleased = [&](EventKeyboard::KeyCode keycode, Event *event) {

		if (keycode == key_direction_left)
		{
			unschedule("move_left");
		}
		if (keycode == key_direction_right)
		{
			unschedule("move_right");
		}
		if (keycode == key_direction_up)
		{
			unschedule("move_up");
		}
		if (keycode == key_direction_down)
		{
			unschedule("move_down");
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_moveListener, this);
}

void TankController::listenFire()
{
	/*if (tank == nullptr)
		return;*/
	m_fireListener->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_fire)
		{
			//tank->fire();	//经典的一按一发
			//tank->fire(); //改进：按键连发
			//先声夺人
			/*if (tank->shootSpeed < 7){
				AudioEngine::play2d("sounds/shoot.mp3");
			}
			else {
				AudioEngine::play2d("sounds/fire-powerful.mp3");
			}*/
			
			m_bullet = tank->shootOneBullet();//打出来的子弹
			tank->gameLayer->m_map->addChild(m_bullet, -1, "bullet");
			m_bullet->setPosition(tank->getPosition());//其实可以这样粗略处理子弹位置
			m_bullet->gameLayer = tank->gameLayer;

			m_bullet->m_direction = tank->m_direction;//获取按下键时的方向
			m_bullet->tankShootSpeed = tank->shootSpeed;
			m_bullet->ice = tank->bulletIce;//ice 
			m_bullet->addController();
		}

	};

	m_fireListener->onKeyReleased = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_fire)
		{
			//log("space release");
			//tank->stopFire();    //松开熄火
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_fireListener, this);
}

bool TankController::isCollideObject(float x, float y)
{
	if (tank->isCollideMap(x, y) || tank->isCollideTank(x, y)) {
		return true;
	}
	return false;
}

void TankController::moving(DIRECTION direction)
{
	tank->m_direction = direction;
	if (direction == DIRECTION::UP)
	{
		//tank->m_direction = direction;
		tank->setRotation(0.0f);
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			bool flag = false;
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i, rect.getMaxY() + tank->moveSpeed))
				{
					while (!isCollideObject(i, rect.getMaxY() + 1))
					{
						tank->setPositionY(tank->getPositionY() + 1);
						rect = tank->getBoundingBox();
					}
					flag = true;
					break;
				}
			}
			if (!flag)
				tank->setPositionY(tank->getPositionY() + tank->moveSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "move_up");
	}

	if (direction == DIRECTION::DOWN)
	{
		//tank->m_direction = direction;
		tank->setRotation(180.0f);
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			bool flag = false;
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideObject(i + 0.000003, rect.getMinY() - tank->moveSpeed))
				{
					while (!isCollideObject(i, rect.getMinY() - 1))
					{
						tank->setPositionY(tank->getPositionY() - 1);
						rect = tank->getBoundingBox();
					}
					flag = true;
					break;
				}
			}
			if (!flag)
				tank->setPositionY(tank->getPositionY() - tank->moveSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "move_down");
	}

	if (direction == DIRECTION::LEFT)
	{
		//tank->m_direction = direction;
		tank->setRotation(-90.0f);
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			bool flag = false;
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMinX() - tank->moveSpeed, i))
				{
					while (!isCollideObject(rect.getMinX() - 1, i))
					{
						tank->setPositionX(tank->getPositionX() - 1);
						rect = tank->getBoundingBox();
					}
					flag = true;
					break;
				}
			}
			if (!flag)
				tank->setPositionX(tank->getPositionX() - tank->moveSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "move_left");
	}

	if (direction == DIRECTION::RIGHT)
	{
		//tank->m_direction = direction;
		tank->setRotation(90.0f);
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			bool flag = false;
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideObject(rect.getMaxX() + tank->moveSpeed, i))
				{
					while (!isCollideObject(rect.getMaxX() + 1, i))
					{
						tank->setPositionX(tank->getPositionX() + 1);
						rect = tank->getBoundingBox();
					}
					flag = true;
					break;
				}
			}
			if (!flag)
				tank->setPositionX(tank->getPositionX() + tank->moveSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "move_right");
	}

}

void TankController::stopMoving(DIRECTION direction)
{
	if (direction == DIRECTION::LEFT)
	{
		unschedule("move_left");
	}
	if (direction == DIRECTION::RIGHT)
	{
		unschedule("move_right");
	}
	if (direction == DIRECTION::UP)
	{
		unschedule("move_up");
	}
	if (direction == DIRECTION::DOWN)
	{
		unschedule("move_down");
	}
}

void TankController::addInventory(const std::string& bg, Point pos)
{
	/*inventory = Inventory::create(bg);
	inventory->setPosition(pos);
	tank->gameLayer->addChild(inventory);
	tank->gameLayer->mycontrols.pushBack(inventory);*/

	tank->inventory = Inventory::create(bg);
	tank->inventory->setPosition(pos);
	tank->gameLayer->addChild(tank->inventory);
	tank->gameLayer->mycontrols.pushBack(tank->inventory);
}

void TankController::addPlayerHP(const std::string& bg, const std::string& source, Point pos)
{
	playerHP = PlayerHP::create(bg, source);
	playerHP->setPosition(pos);
	tank->gameLayer->addChild(playerHP);
	tank->gameLayer->mycontrols.pushBack(playerHP);

	/*schedule([=](float dt) {
		if (playerHP != nullptr && tank->HP >= 0 && tank->HP <= 100)
			playerHP->getProgressBar()->setPercentage(tank->HP);
	}, 0.0f, kRepeatForever, 0.0f, "player_hp");*/

}

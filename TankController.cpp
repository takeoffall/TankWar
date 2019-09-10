#include "TankController.h"
#include "Bullet.h"
#include "GameScene.h"
#include "MapLayer.h"

#include "AudioEngine.h"
using namespace experimental;

TankController::TankController(const std::string& xml_file)
{
	auto message = Dictionary::createWithContentsOfFile(xml_file.c_str());
	auto firekey = ((__Dictionary *)message->objectForKey("controller"))->valueForKey("fire_key")->intValue();
	auto directionkey = (__Dictionary *)((__Dictionary *)message->objectForKey("controller"))->objectForKey("direction_key");
	auto leftkey = directionkey->valueForKey("LEFT")->intValue();
	auto rightkey = directionkey->valueForKey("RIGHT")->intValue();
	auto upkey = directionkey->valueForKey("UP")->intValue();
	auto downkey = directionkey->valueForKey("DOWN")->intValue();

	key_fire = (EventKeyboard::KeyCode)firekey;
	key_direction_down = (EventKeyboard::KeyCode)downkey;
	key_direction_left = (EventKeyboard::KeyCode)leftkey;
	key_direction_right = (EventKeyboard::KeyCode)rightkey;
	key_direction_up = (EventKeyboard::KeyCode)upkey;

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
	
	m_moveListener = EventListenerKeyboard::create();
	m_fireListener = EventListenerKeyboard::create();

	tankBorn();
	listenMove();//Include collide
	listenFire();
	//listenGetProps();
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

void TankController::clearPropsFromVector(PROP_TYPE type)
{
	for (auto it = tank->existProps.begin(); it != tank->existProps.end(); ) {
		if ((*it)->getType() == type){
			it = tank->existProps.erase(it);
		}
		else {
			it++;
		}
	}
}

std::string TankController::getPropNameFromType(PROP_TYPE type)//其实也可以用map容器来映射
{
	for (auto &i : tank->existProps)
	{
		if (i->getType() == type)
		{
			return i->getName();
		}
	}
	return "NONE";
}


void TankController::listenGetProps()//重用
{
	schedule([&](float dt) {
		for (auto &i : tank->gameLayer->m_map->propSet)
		{
			if (i->getParent() == nullptr)//未加到自动消失的道具
			{
				//c.eraseObject(i);
				continue;
			}
			if (tank->getBoundingBox().intersectsRect(i->getBoundingBox()))
			{
				//i->isObtained = true;
				tank->currentBuff = i;
				propTypes[i->getType()]++;//单走一个爽字，哈哈哈哈舒服了
				i->changeParent();
				
				inventory->addItem((Sprite *)i);
				
				/*bool haveSameType = false;
				for (auto &j : tankProps)
				{
					if (j->getType() == i->getType())
					{
						haveSameType = true;
						break;
					}
				}
				if (!haveSameType) {
					tankProps.pushBack(i);
				}*/
			}
		}
	}, 0.0f, kRepeatForever, 0.0f, "listenGetProps");
	
}

void TankController::clearTankProps(float ctime, const std::string& name)
{
	scheduleOnce([&](float dt) {
		propTypes[tank->currentBuff->getType()]--;
		tank->currentBuff->removeFromParent();
	}, ctime, name);
}

void TankController::clearTankProps(Props* prop, float ctime, const std::string& name, CallFunc* f)
{
	prop->scheduleOnce([&, prop](float dt) {
		propTypes[prop->getType()]--;
		prop->removeFromParent();
	}, ctime, name);
}

void TankController::clearTankProps(Props* prop, float ctime, const std::string& name, void(TankController:: *func)())
{
	prop->scheduleOnce([&, prop](float dt) {
		func;
		propTypes[prop->getType()]--;
		prop->removeFromParent();
	}, ctime, name);
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

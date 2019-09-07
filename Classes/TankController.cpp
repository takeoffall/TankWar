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

TankController*  TankController::create(Tank *tank)
{
	TankController *tkcnter = new (std::nothrow) TankController();
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
	left_arrow = right_arrow = up_arrow = down_arrow = false;
	m_moveListener = EventListenerKeyboard::create();
	m_fireListener = EventListenerKeyboard::create();

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
		tank->gameLayer->m_map->tankSet.eraseObject(tank);

		AudioEngine::play2d("sounds/fexplosion.mp3");
		auto animation = AnimationCache::getInstance()->animationByName("tankboom");
		auto action = Animate::create(animation);
		tank->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {tank->removeFromParentAndCleanup(true); }, NULL));
		unscheduleUpdate();
	}
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

void TankController::getBlood()
{
	tank->addHP(40);
	scheduleOnce([&](float dt) {
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::ADD_BLOOD), false);
		clearPropsFromVector(PROP_TYPE::ADD_BLOOD);
	}, tank->currentBuff->getContinuousTime(), "add_blood_lose_efficacy");
}

void TankController::getProtected()
{
	tank->isProtected = true;
	tank->initWithFile("tk1_p.png");

	scheduleOnce([&](float dt) {
		tank->isProtected = false;
		tank->initWithFile("tk1.png");
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::PROTECTED), false);
		clearPropsFromVector(PROP_TYPE::PROTECTED);
	}, tank->currentBuff->getContinuousTime(), "protected_lose_efficacy");
}

void TankController::getProtectedUP()
{
	tank->initWithFile("tk1_f.png");
	if (tank->isProtectedUP == true)
		unschedule("protectedUP_lose_efficacy");
	else
		unschedule("protected_lose_efficacy");

	tank->isProtectedUP = true;
	scheduleOnce([&](float dt) {
		tank->isProtected = false;
		tank->isProtectedUP = false;
		tank->initWithFile("tk1.png");
		//删除物品栏中所有同类的物品
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::PROTECTED), true);
		clearPropsFromVector(PROP_TYPE::PROTECTED);
	}, tank->currentBuff->getContinuousTime() * 1.5, "protectedUP_lose_efficacy");
}

void TankController::getStar()
{
	tank->bulletPower *= 2;
	scheduleOnce([&](float dt) {
		tank->bulletPower = 20;
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::START), false);
		clearPropsFromVector(PROP_TYPE::START);
	}, tank->currentBuff->getContinuousTime(), "start_lose_efficacy");
}

void TankController::getSpade()
{
	tank->gameLayer->m_map->getLayer("bg2")->setVisible(true);
	scheduleOnce([&](float dt) {

		tank->gameLayer->m_map->getLayer("bg2")->setVisible(false);
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::SPADE), false);
		clearPropsFromVector(PROP_TYPE::SPADE);

	}, tank->currentBuff->getContinuousTime(), "spade_lose_efficacy");
}

void TankController::getMine()
{
	for (auto &i : tank->gameLayer->m_map->tankSet)
	{
		if (i->getName() == "enemy")
		{
			i->HP = 0;
			break;
		}
	}
	scheduleOnce([&](float dt) {
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::MINE), false);
		clearPropsFromVector(PROP_TYPE::MINE);

	}, tank->currentBuff->getContinuousTime(), "mine_lose_efficacy");
}

void TankController::getTimer()
{
	for (auto &i : tank->gameLayer->m_map->tankSet)
	{
		if (i->getName() == "enemy")
		{
			i->pause();
			i->isPause = true;
		}
	}
	scheduleOnce([&](float dt) {
		for (auto &i : tank->gameLayer->m_map->tankSet)
		{
			if (i->getName() == "enemy")
			{
				i->resume();
				i->isPause = false;
			}
		}
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::TIMER), false);
		clearPropsFromVector(PROP_TYPE::TIMER);

	}, tank->currentBuff->getContinuousTime(), "timer_lose_efficacy");
}

void TankController::getXingXing()
{
	scheduleOnce([&](float dt) {
		
		//物品栏删除单个
		tank->gameLayer->EX_RemoveProp(getPropNameFromType(PROP_TYPE::XINGXING), false);
		clearPropsFromVector(PROP_TYPE::XINGXING);

	}, tank->currentBuff->getContinuousTime(), "xingxing_lose_efficacy");
}

void TankController::listenGetProps()
{
	schedule([&](float dt) {
		if (tank->isGetBonusByName("props-tank.png"))
		{
			if (tank->HP + 40 > 100)
				tank->HP = 100;
			else
				tank->HP += 40;
			log("tank->hp: %d", tank->HP);
			
			for (auto &i : tank->gameLayer->m_map->getChildren())
			{
				if (i->getName() == "props-tank.png")
				{
					tank->haveProps.erase(std::remove(tank->haveProps.begin(), tank->haveProps.end(), "props-tank.png"),
						tank->haveProps.end());
					i->removeFromParentAndCleanup(true);
					break;
				}
			}
		}

		if (tank->isGetBonusByName("props-protect.png"))
		{
			tank->initWithFile("tk1_p.png");
			tank->isProtected = true;
			int num = 0;
			for (auto &i : tank->haveProps)
			{
				if (i == "props-protect.png")
				{
					num++;
				}
			}
			if (num >= 2)
			{
				tank->isProtectedUP = true;
				tank->initWithFile("tk1_f.png");
			}
			for (auto &i : tank->gameLayer->m_map->getChildren())//移花接木
			{
				if (i->getName() == "props-protect.png")
				{
					i->removeFromParentAndCleanup(true);
					auto i = Sprite::createWithSpriteFrameName("props-protect.png");
					tank->gameLayer->addChild(i, 0, "protect");
					auto vSize = Director::getInstance()->getVisibleSize();
					i->setPosition(Point(vSize.width - (vSize.width - tank->gameLayer->m_map->getBoundingBox().getMaxX()) / 2, vSize.height / 2 + 50));
					break;
				}
			}
			if (tank->isProtectedUP)
			{
				unschedule("protected");
				scheduleOnce([&, num](float dt) {
					tank->isProtected = false;
					tank->isProtectedUP = false;
					for (int i = 0; i < num; i++)
					{
						tank->haveProps.erase(std::remove(tank->haveProps.begin(), tank->haveProps.end(), "props-protect.png"),
							tank->haveProps.end());
						tank->gameLayer->removeChildByName("protect");
					}
					tank->initWithFile("tk1.png");
				}, 30.0f, "protectedUP");
			}
			else
			{
				scheduleOnce([&](float dt) {
					tank->isProtected = false;
					tank->haveProps.erase(std::remove(tank->haveProps.begin(), tank->haveProps.end(), "props-protect.png"),
						tank->haveProps.end());
					tank->initWithFile("tk1.png");
					tank->gameLayer->removeChildByName("protect");
				}, 20.0f, "protected");
			}
		}

	}, 0.0f, kRepeatForever, 0.0f, "get_prop");
}

void TankController::listenMove()
{
	if (tank == nullptr)
		return;

	m_moveListener->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {
		
		if (keycode == key_direction_left)
		{
			left_arrow = true;
			this->moving(DIRECTION::LEFT);
		}

		if (keycode == key_direction_right)
		{
			right_arrow = true;
			this->moving(DIRECTION::RIGHT);
		}

		if (keycode == key_direction_up)
		{
			up_arrow = true;
			this->moving(DIRECTION::UP);
		}

		if (keycode == key_direction_down)
		{
			down_arrow = true;
			this->moving(DIRECTION::DOWN);
		}

	};

	m_moveListener->onKeyReleased = [&](EventKeyboard::KeyCode keycode, Event *event) {

		if (keycode == key_direction_left)
		{
			left_arrow = false;
			unschedule("move_left");
		}
		if (keycode == key_direction_right)
		{
			right_arrow = false;
			unschedule("move_right");
		}
		if (keycode == key_direction_up)
		{
			up_arrow = false;
			unschedule("move_up");
		}
		if (keycode == key_direction_down)
		{
			down_arrow = false;
			unschedule("move_down");
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_moveListener, this);
}

void TankController::listenFire()
{
	if (tank == nullptr)
		return;

	m_fireListener->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_fire)
		{
			//tank->fire();	//经典的一按一发
			//tank->fire(); //改进：按键连发
			//先声夺人
			AudioEngine::play2d("sounds/shoot.mp3");
			m_bullet = tank->shootOneBullet();//打出来的子弹
			//m_bullet->enemy = enemy;
			tank->gameLayer->m_map->addChild(m_bullet, -1, "bullet");
			m_bullet->setPosition(tank->getPosition());//其实可以这样粗略处理子弹位置

			m_bullet->m_direction = tank->m_direction;//获取按下键时的方向
			//m_bullet->map = tank->map;
			m_bullet->gameLayer = tank->gameLayer;
			m_bullet->tankShootSpeed = (int)tank->m_shootSpeed;
			m_bullet->addController();
			//m_bullet->scheduleUpdate();
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

void TankController::buffsInAction()
{
	if (tank->currentBuff->getType() == PROP_TYPE::ADD_BLOOD) {
		getBlood();
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::PROTECTED) {
		int count = 0;
		for (auto &i : tank->existProps)
		{
			if (i->getType() == PROP_TYPE::PROTECTED) {
				count++;
			}
		}
		if (count >= 2) {
			getProtectedUP();
		}
		else {
			getProtected();
		}
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::START) {
		getStar();
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::SPADE) {
		getSpade();
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::MINE) {
		getMine();
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::TIMER) {
		getTimer();
	}
	else if (tank->currentBuff->getType() == PROP_TYPE::XINGXING) {
		getXingXing();
	}
}

bool TankController::isCollideObject(float x, float y)
{
	if (tank->isGetBonus(x, y)) {
		//tank->gameLayer->EX_AddProp(tank->currentBuff->getName());//添加ui到主界面
		//tank->currentBuff->removeFromParentAndCleanup(true);//也可以让buff自己完成
		tank->currentBuff->removeFromParent();
		tank->gameLayer->EX_AddProp(tank->currentBuff);
		//创建飞入物品栏的动态ui替换上面直接remove
		//贝塞尔曲线运动
		buffsInAction();//包含了失效后的处理
		tank->gameLayer->m_map->propSet.eraseObject(tank->currentBuff);
		return false;//
	}


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
				if (isCollideObject(i, rect.getMaxY() + (int)tank->m_moveSpeed))
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
				tank->setPositionY(tank->getPositionY() + (int)tank->m_moveSpeed);
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
				if (isCollideObject(i + 0.000003, rect.getMinY() - (int)tank->m_moveSpeed))
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
				tank->setPositionY(tank->getPositionY() - (int)tank->m_moveSpeed);
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
				if (isCollideObject(rect.getMinX() - (int)tank->m_moveSpeed, i))
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
				tank->setPositionX(tank->getPositionX() - (int)tank->m_moveSpeed);
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
				if (isCollideObject(rect.getMaxX() + (int)tank->m_moveSpeed, i))
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
				tank->setPositionX(tank->getPositionX() + (int)tank->m_moveSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "move_right");
	}

	/*if (!tank->isLock)
	{
		switch (tank->m_direction)
		{
			case DIRECTION::UP:
				tank->setPositionY(tank->getPositionY() + (int)tank->m_moveSpeed);
				break;
			case DIRECTION::DOWN:
				tank->setPositionY(tank->getPositionY() - (int)tank->m_moveSpeed);
				break;
			case DIRECTION::LEFT:
				tank->setPositionX(tank->getPositionX() - (int)tank->m_moveSpeed);
				break;
			case DIRECTION::RIGHT:
				tank->setPositionX(tank->getPositionX() + (int)tank->m_moveSpeed);
				break;
			default:
				log("Direction type error! code: %d", (int)tank->m_moveSpeed);
				break;
		}
	}*/
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

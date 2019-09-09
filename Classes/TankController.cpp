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

	tank->isProtected = true;//Invincible Time(�޵�ʱ��)

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

std::string TankController::getPropNameFromType(PROP_TYPE type)//��ʵҲ������map������ӳ��
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
	log("HP after add blood: %d", tank->HP);
	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		inventory->removeItem(getPropNameFromType(PROP_TYPE::ADD_BLOOD), false);
		clearPropsFromVector(PROP_TYPE::ADD_BLOOD);
	}), NULL));
}

void TankController::getProtected()
{
	tank->isProtected = true;
	tank->initWithFile("tk1_p.png");

	scheduleOnce([&](float dt) {
		tank->isProtected = false;
		tank->initWithFile("tk1.png");
		//��Ʒ��ɾ������
		inventory->removeItem(getPropNameFromType(PROP_TYPE::PROTECTED), false);
		//tank->gameLayer->EX_RemoveProp(tank->getPlayerName(), getPropNameFromType(PROP_TYPE::PROTECTED), false);
		clearPropsFromVector(PROP_TYPE::PROTECTED);
	}, tank->currentBuff->getContinuousTime(), "protected_lose_efficacy");

	/*runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		tank->isProtected = false;
		tank->initWithFile("tk1.png");
		inventory->removeItem(getPropNameFromType(PROP_TYPE::PROTECTED), false);
		clearPropsFromVector(PROP_TYPE::PROTECTED);
	}), NULL));*/
}

void TankController::getProtectedUP()
{
	tank->initWithFile("tk1.png");
	if (tank->isProtectedUP == true) {
		//unschedule("protectedUP_lose_efficacy");
		tank->HP += 20;
		return;
	}
	else {
		unschedule("protected_lose_efficacy");
	}

	auto s = Sprite::create();
	s->setPosition(tank->getContentSize().width / 2, tank->getContentSize().height / 2);
	tank->addChild(s, 0, "shield");
	auto animation = AnimationCache::getInstance()->getAnimation("tankborn");
	auto action = Animate::create(animation);
	s->runAction(RepeatForever::create(action));

	tank->isProtectedUP = true;

	scheduleOnce([&](float dt) {
		tank->isProtected = false;
		tank->isProtectedUP = false;
		tank->removeChildByName("shield");
		//ɾ����Ʒ��������ͬ�����Ʒ
		inventory->removeItem(getPropNameFromType(PROP_TYPE::PROTECTED), true);
		//tank->gameLayer->EX_RemoveProp(tank->getPlayerName(), getPropNameFromType(PROP_TYPE::PROTECTED), true);
		clearPropsFromVector(PROP_TYPE::PROTECTED);
	}, tank->currentBuff->getContinuousTime() * 1.5, "protectedUP_lose_efficacy");
}

void TankController::getStar()
{
	if (tank->shootSpeed < (int)SHOOT_SPEED::LIMIT)
		tank->shootSpeed += 1;//����ӵ������β��ܴ�ש���м�һ�Σ��߼����ӾͿ��Դ�ש

	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		inventory->removeItem(getPropNameFromType(PROP_TYPE::START), false);
		clearPropsFromVector(PROP_TYPE::START);
	}), NULL));
}

void TankController::getSpade()//���Ѳ�����·�����ϼ�20����
{
	tank->gameLayer->m_map->getLayer("bg2")->setVisible(true);

	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		tank->gameLayer->m_map->getLayer("bg2")->setVisible(false);
		inventory->removeItem(getPropNameFromType(PROP_TYPE::SPADE), false);
		clearPropsFromVector(PROP_TYPE::SPADE);
	}), NULL));
}

void TankController::getMine()//�������߱����£�����+20hp
{
	for (auto &i : tank->gameLayer->m_map->tankSet)
	{
		if (i->getName() == "enemy")
		{
			i->HP = 0;
			//break;//ȫըע��break
		}
	}
	//scheduleOnce([&](float dt) {
	//	inventory->removeItem(getPropNameFromType(PROP_TYPE::MINE), false);
	//	//tank->gameLayer->EX_RemoveProp(tank->getPlayerName(), getPropNameFromType(PROP_TYPE::MINE), false);//��Ʒ��ɾ������
	//	clearPropsFromVector(PROP_TYPE::MINE);//��������

	//}, tank->currentBuff->getContinuousTime(), "mine_lose_efficacy");

	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		//tank->gameLayer->m_map->getLayer("bg2")->setVisible(false);
		inventory->removeItem(getPropNameFromType(PROP_TYPE::MINE), false);
		clearPropsFromVector(PROP_TYPE::MINE);
	}), NULL));
}

void TankController::getTimer()//����+������Ч������+20HP
{
	for (auto &i : tank->gameLayer->m_map->tankSet)
	{
		if (i->getName() == "enemy")
		{
			i->pause();
			i->isPause = true;
		}
	}
	//scheduleOnce([&](float dt) {
	//	for (auto &i : tank->gameLayer->m_map->tankSet)
	//	{
	//		if (i->getName() == "enemy")
	//		{
	//			i->resume();
	//			i->isPause = false;
	//		}
	//	}
	//	inventory->removeItem(getPropNameFromType(PROP_TYPE::TIMER), false);
	//	//tank->gameLayer->EX_RemoveProp(tank->getPlayerName(), getPropNameFromType(PROP_TYPE::TIMER), false);//��Ʒ��ɾ������
	//	clearPropsFromVector(PROP_TYPE::TIMER);//��������

	//}, tank->currentBuff->getContinuousTime(), "timer_lose_efficacy");

	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		for (auto &i : tank->gameLayer->m_map->tankSet)
		{
			if (i->getName() == "enemy")
			{
				i->resume();
				i->isPause = false;
			}
		}
		inventory->removeItem(getPropNameFromType(PROP_TYPE::TIMER), false);
		clearPropsFromVector(PROP_TYPE::TIMER);
	}), NULL));
}

void TankController::getXingXing()//���õ͸���
{
	//�������ǹؿ�//��������//����������ͼ//��������
	//scheduleOnce([&](float dt) {
	//	inventory->removeItem(getPropNameFromType(PROP_TYPE::XINGXING), false);
	//	//tank->gameLayer->EX_RemoveProp(tank->getPlayerName(), getPropNameFromType(PROP_TYPE::XINGXING), false);//��Ʒ��ɾ������
	//	clearPropsFromVector(PROP_TYPE::XINGXING);//��������

	//}, tank->currentBuff->getContinuousTime(), "xingxing_lose_efficacy");

	runAction(Sequence::create(DelayTime::create(tank->currentBuff->getContinuousTime()), CallFunc::create([this]() {
		
		inventory->removeItem(getPropNameFromType(PROP_TYPE::XINGXING), false);
		clearPropsFromVector(PROP_TYPE::XINGXING);
	}), NULL));
}

void TankController::listenGetProps()//����
{
	for (auto &i : tank->gameLayer->m_map->propSet)
	{
		if (i->getParent() == nullptr)//δ�ӵ��Զ���ʧ�ĵ���
		{
			//c.eraseObject(i);
			continue;
		}
		if (tank->getBoundingBox().intersectsRect(i->getBoundingBox()))
		{
			i->numOfReference++;//get + 1
			tankProps.pushBack(i);
		}
	}
	
}

void TankController::checkTankProps()
{
	for (auto &i : tankProps)
	{
		if (i->getType() == PROP_TYPE::ADD_BLOOD) {
			tank->addHP(40);
		}
		else if (tank->currentBuff->getType() == PROP_TYPE::START) {
			tank->addShootSpeed(1);
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
			//tank->fire();	//�����һ��һ��
			//tank->fire(); //�Ľ�����������
			//��������
			/*if (tank->shootSpeed < 7){
				AudioEngine::play2d("sounds/shoot.mp3");
			}
			else {
				AudioEngine::play2d("sounds/fire-powerful.mp3");
			}*/
			
			m_bullet = tank->shootOneBullet();//��������ӵ�
			tank->gameLayer->m_map->addChild(m_bullet, -1, "bullet");
			m_bullet->setPosition(tank->getPosition());//��ʵ�����������Դ����ӵ�λ��
			m_bullet->gameLayer = tank->gameLayer;

			m_bullet->m_direction = tank->m_direction;//��ȡ���¼�ʱ�ķ���
			m_bullet->tankShootSpeed = tank->shootSpeed;

			m_bullet->addController();
		}

	};

	m_fireListener->onKeyReleased = [&](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == key_fire)
		{
			//log("space release");
			//tank->stopFire();    //�ɿ�Ϩ��
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
		if (tank->isProtected)
			getProtectedUP();
		else
			getProtected();
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
		//AudioEngine::play2d("sounds/bonus-get.mp3");
		tank->currentBuff->removeFromParent();
		
		inventory->addItem(tank->currentBuff);
		log("current type: %d", (int)tank->currentBuff->getType());
		
		//����������Ʒ���Ķ�̬ui�滻����ֱ��remove
		//�����������˶�
		buffsInAction();//������ʧЧ��Ĵ���
		//tank->gameLayer->m_map->propSet.eraseObject(tank->currentBuff);
		return false;//������
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
	inventory = Inventory::create(bg);
	inventory->setPosition(pos);
	tank->gameLayer->addChild(inventory);
	tank->gameLayer->mycontrols.pushBack(inventory);
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

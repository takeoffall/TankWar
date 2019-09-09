#include "Tank.h"
#include "Bullet.h"
#include "TankController.h"
#include "GameScene.h"
#include "MapLayer.h"
//#include "Props.h"

bool Tank::init()
{
	
	collidedEnemy = nullptr;
	isProtected = false;
	isProtectedUP = false;
	return true;
}

Tank* Tank::create(const std::string& filename, MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	Tank *tank = new (std::nothrow) Tank(moveSpeed, shootSpeed);
	if (tank && tank->initWithFile(filename) && tank->init())
	{
		tank->autorelease();
		return tank;
	}
	CC_SAFE_DELETE(tank);
	return nullptr;
}

Tank::Tank(MOVE_SPEED moveSpeed, SHOOT_SPEED shootSpeed)
{
	m_defaultSpeed = (int)moveSpeed;
	this->moveSpeed = (int)moveSpeed;
	this->shootSpeed = (int)shootSpeed;
	this->m_direction = DIRECTION::UP;
	HP = 100;
	//score = 0;
	bulletPower = 20;
}

void Tank::fixBulletPos()//固定子弹位置为坦克枪口位置
{
	for (auto &bullet : m_cartridgeClip)
	{
		bullet->setPosition(this->getContentSize().width / 2, this->getContentSize().height);
	}
}

void Tank::update(float dt)//把每帧都要检测的放入，比如坦克死亡
{
	
}

void Tank::loadBullets(const std::string& filename, int num, int power)//外部接口图片文件名
{
	for (int i = 0; i < num; i++)
	{
		Bullet *bullet = Bullet::create(filename, power);
		this->addChild(bullet);
		m_cartridgeClip.pushBack(bullet);
	}
	fixBulletPos();
}

void Tank::loadBullets()//默认子弹
{
	for (int i = 0; i < 10; i++)
	{
		Bullet* bullet = Bullet::create("bullet.png", bulletPower);
		this->addChild(bullet);
		bullet->p_tank = this;
		bullet->p_tankName = this->getName();
		m_cartridgeClip.pushBack(bullet);
	}
	fixBulletPos();
}

bool Tank::isGetBonus(float x, float y)
{
	//auto c = gameLayer->m_map->propSet;
	for (auto &i : gameLayer->m_map->propSet)
	{
		if (i->getParent() == nullptr)//未加到自动消失的道具
		{
			//c.eraseObject(i);
			continue;
		}
		auto rect = i->getBoundingBox();
		if (x >= rect.getMinX() && x <= rect.getMaxX() && y >= rect.getMinY() && y <= rect.getMaxY())
		{
			currentBuff = i;
			existProps.pushBack(currentBuff);
			//buffs.push_back(i->getType());
			return true;
		}
	}
	return false;
}

bool Tank::isGetBonusByName(const std::string &name)
{
	auto c = this->getParent()->getChildren();
	c.eraseObject(this);
	for (auto &i : c)
	{
		if (i->getName() != name)
			continue;
		if (isCollideOthers(i))
		{
			haveProps.push_back(name);
			return true;
		}
	}
	return false;
}

bool Tank::isCollideMap(float x, float y)
{
	return gameLayer->m_map->isCollision(gameLayer->m_map->tileCoordForPosition(Point(x, y)));
}

bool Tank::isCollideTank(float x, float y)
{
	auto c = gameLayer->m_map->tankSet;
	c.eraseObject(this);
	for (auto &i : c)
	{
		if (i->getName() != "tank" && i->getName() != "enemy")
			continue;
		auto rect = i->getBoundingBox();
		if (x >= rect.getMinX() && x <= rect.getMaxX() && y >= rect.getMinY() && y <= rect.getMaxY())
		{
			if (i->getName() != this->getName())
			{
				collidedEnemy = i;
			}
			
			return true;
		}
	}
	return false;
}

Bullet* Tank::shootOneBullet()
{
	//"自动步枪"，自动装弹，外界不用担心打完
	if (!m_cartridgeClip.empty())
	{
		m_cartridgeClip.back()->removeFromParent();//子弹脱离坦克但不删除
		return m_cartridgeClip._popBack();//手改源码，自己的popBack
		//这里先不忙popBack,即删除这颗子弹,而是射向外界，所以修改了源码
	}
	loadBullets();
	m_cartridgeClip.back()->removeFromParent();
	return m_cartridgeClip._popBack();
}

Bullet* Tank::shootOneBullet(const std::string& filename, int num, int power)
{
	//打完过后自主选择装哪种子弹，装多少
	if (!m_cartridgeClip.empty())
	{
		m_cartridgeClip.back()->removeFromParent();
		return m_cartridgeClip._popBack();//手改源码，自己的popBack
		//这里先不忙popBack,即删除这颗子弹,而是射向外界
	};
	loadBullets(filename, num, power);
	m_cartridgeClip.back()->removeFromParent();
	return m_cartridgeClip._popBack();
}

void Tank::moving(DIRECTION direction)
{
	//is_moving = true;
	m_direction = direction;
	if (direction == DIRECTION::UP)
	{
		//is_moving_up = true;
		this->setRotation(0.0f);
		this->setPositionY(this->getPositionY() + moveSpeed);
	}

	if (direction == DIRECTION::DOWN)
	{
		//is_moving_down = true;
		this->setRotation(180.0f);
		this->setPositionY(this->getPositionY() - moveSpeed);
	}

	if (direction == DIRECTION::LEFT)
	{
		//is_moving_left = true;
		this->setRotation(-90.0f);
		this->setPositionX(this->getPositionX() - moveSpeed);
	}

	if (direction == DIRECTION::RIGHT)
	{
		//is_moving_right = true;
		this->setRotation(90.0f);
		this->setPositionX(this->getPositionX() + moveSpeed);
	}
}

void Tank::stopMoving(DIRECTION direction)
{
	//is_moving = false;
	if (direction == DIRECTION::UP)
	{
		//is_moving_up = false;
		unschedule("moving_up");
	}
	if (direction == DIRECTION::DOWN)
	{
		//is_moving_down = false;
		unschedule("moving_down");
	}
	if (direction == DIRECTION::LEFT)
	{
		//is_moving_left = false;
		unschedule("moving_left");
	}
	if (direction == DIRECTION::RIGHT)
	{
		//is_moving_right = false;
		unschedule("moving_right");
	}
}

void Tank::addController(const std::string& xml)
{
	tankController = TankController::create(this, xml);
	addChild(tankController, -1, "controller");
}

void Tank::removeController()
{
	removeChildByName("controller");
}

bool Tank::isCollideOthers(Node *node)
{
	auto rect = this->getBoundingBox();
	auto rect1 = node->getBoundingBox();

	auto minRight = rect.getMaxX() < rect1.getMaxX() ? rect.getMaxX() : rect1.getMaxX();
	auto maxLeft = rect.getMinX() > rect1.getMinX() ? rect.getMinX() : rect1.getMinX();
	auto minTop = rect.getMaxY() < rect1.getMaxY() ? rect.getMaxY() : rect1.getMaxY();
	auto maxBottom = rect.getMinY() > rect1.getMinY() ? rect.getMinY() : rect1.getMinY();
	if (maxBottom <= minTop && maxLeft <= minRight)
	{
		return true;
	}
	return false;
}


#include "Bullet.h"
#include "MapLayer.h"
#include "GameScene.h"

//#include "ui/CocosGUI.h"
//using namespace ui;

bool Bullet::init(const std::string& name)
{
	this->initWithSpriteFrameName(name);
	/*tankShootSpeed = (int)p_tank->m_shootSpeed;
	map = p_tank->map;
	m_direction = p_tank->m_direction;*/
	return true;
}

//Sprite::createWithSpriteFrameName()

Bullet* Bullet::create(const std::string& name, int power)
{
	Bullet *bullet = new (std::nothrow) Bullet(power);
	if (bullet && bullet->init(name))
	{
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

Bullet::Bullet(const std::string& name, int power)
{
	this->power = power;
	this->initWithSpriteFrameName(name);
	this->autorelease();
}

Bullet::Bullet(int power)
{
	this->power = power;
}

int Bullet::getPower()
{
	return power;
}


void Bullet::update(float dt)
{
	
}

bool Bullet::collision()
{
	auto v = gameLayer->m_map->getChildren();
	v.eraseObject(this->p_tank);
	for (auto &i : v)
	{
		if (isCollision(i->getBoundingBox()) && (i->getName() == "enemy" || i->getName() == "tank"))
		{
			log("bullet hit something: %s", i->getName().c_str());
			//CC_SAFE_DELETE(i);
			i->removeFromParentAndCleanup(true);
			return true;
		}
	}
	return false;
}

bool Bullet::isCollision(Rect &rect)
{
	auto bulletRect = this->getBoundingBox();
	auto minRight = rect.getMaxX() < bulletRect.getMaxX() ? rect.getMaxX() : bulletRect.getMaxX();
	auto maxLeft = rect.getMinX() > bulletRect.getMinX() ? rect.getMinX() : bulletRect.getMinX();

	auto minTop = rect.getMaxY() < bulletRect.getMaxY() ? rect.getMaxY() : bulletRect.getMaxY();
	auto maxBottom = rect.getMinY() > bulletRect.getMinY() ? rect.getMinY() : bulletRect.getMinY();
	if (maxBottom <= minTop && maxLeft <= minRight)
		return true;

	return false;

}

void Bullet::addController()
{
	auto bulletController = BulletController::create(this);
	addChild(bulletController);
}

void Bullet::dieEffect()
{

}

void Bullet::fly()
{
	if (m_direction == DIRECTION::UP)
	{
		schedule([&](float dt) {
			this->setPositionY(this->getPositionY() + tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_up");
	}
	else if (m_direction == DIRECTION::DOWN)
	{
		schedule([&](float dt) {
			this->setPositionY(this->getPositionY() - tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_down");
	}
	else if (m_direction == DIRECTION::LEFT)
	{
		schedule([&](float dt) {
			this->setPositionX(this->getPositionX() - tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_left");
	}
	else if (m_direction == DIRECTION::RIGHT)
	{
		schedule([&](float dt) {
			this->setPositionX(this->getPositionX() + tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_right");
	}
}

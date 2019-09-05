#include "Bullet.h"
#include "MapLayer.h"
#include "GameScene.h"
//#include "BulletController.h"

#include "Mmsystem.h"//playsound头文件
#pragma comment(lib,"winmm.lib")
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
	auto map = gameLayer->m_map;
	auto position = map->tileCoordForPosition(this->getPosition());
	
	if (collision())
	{
		//SimpleAudioEngine::getInstance()->playEffect("sounds/eexplosion.wav");
		//PlaySound("SystemStart", NULL, SND_ALIAS | SND_ASYNC);
		PlaySoundA("F:\\cocos_cpp\\comeback1\\Resources\\sounds\\eexplosion.wav", NULL, SND_FILENAME | SND_ASYNC);
		auto animation = AnimationCache::getInstance()->animationByName("enemyboom");
		auto action = Animate::create(animation);
		this->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {this->removeFromParentAndCleanup(true); }, NULL));
		unscheduleUpdate();
	}

	if (map->isCollision(position))
	{
		auto animation = AnimationCache::getInstance()->animationByName("enemyboom");
		auto action = Animate::create(animation);
		this->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {this->removeFromParentAndCleanup(true); }, NULL));
		if (!map->isSteel(position))
		{
			//让一定范围内砖块消失
			map->layer1->removeTileAt(position);
			switch (tankDirection)
			{
			case DIRECTION::UP:
			case DIRECTION::DEFAULT:
			case DIRECTION::DOWN:
				if (!map->isSteel(Point(position.x - 1, position.y)))
					map->layer1->removeTileAt(Point(position.x - 1, position.y));
				if (!map->isSteel(Point(position.x + 1, position.y)))
					map->layer1->removeTileAt(Point(position.x + 1, position.y));
				break;
			case DIRECTION::LEFT:
			case DIRECTION::RIGHT:
				if (!map->isSteel(Point(position.x, position.y - 1)))
					map->layer1->removeTileAt(Point(position.x, position.y - 1));
				if (!map->isSteel(Point(position.x, position.y + 1)))
					map->layer1->removeTileAt(Point(position.x, position.y + 1));
				break;
			default:
				log("error");
				break;
			}
		}
		unscheduleUpdate();
	}
	
	else
	{
		switch (tankDirection)
		{
		case DIRECTION::UP:
		case DIRECTION::DEFAULT:
			this->setPositionY(this->getPositionY() + tankShootSpeed);
			break;
		case DIRECTION::LEFT:
			this->setPositionX(this->getPositionX() - tankShootSpeed);
			break;
		case DIRECTION::RIGHT:
			this->setPositionX(this->getPositionX() + tankShootSpeed);
			break;
		case DIRECTION::DOWN:
			this->setPositionY(this->getPositionY() - tankShootSpeed);
			break;
		default:
			log("error");
			break;
		}
	}
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

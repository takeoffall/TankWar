#include "RelationEvent.h"
#include "Bullet.h"

bool RelateEvent::init()
{
	if (!Node::init())
		return false;

	scheduleUpdate();
	return true;
}

void RelateEvent::update(float dt)
{
	byCollision();
}

void RelateEvent::byCollision()
{
	for (auto &i : tankSet)
	{
		cTankMap(i);
		for (auto &j : enemySet)
		{
			cTankEnemy(i, j);
			cTankMap(j);
		}
	}

	for (auto &i : bulletSet)
	{
		for (auto &j : enemySet)//bullet-enemy
		{
			cBulletTank(i, j);
			//cBulletEnemy(i, j);
		}
		for (auto &k : tankSet)//bullet-tank
		{
			cBulletTank(i, k);
		}
		for (auto &other : bulletSet)//bullet-bullet
		{
			if (i != other && i->getPosition() == other->getPosition())
			{
				bulletDie(i);
				bulletDie(other);
			}
		}
		
		cBulletMap(i);//bullet-map
	}
}



bool RelateEvent::isCollideMap(Point p)
{
	auto a = map->layer1->getTileGIDAt(p);
	if (a == 0 || (a >= 8 && a <= 11) || (a >= 36 && a <= 39) || (a >= 64 && a <= 67) || (a >= 92 && a <= 95) ||
		(a >= 12 && a <= 19) || (a >= 40 && a <= 47) || (a >= 68 && a <= 75) || (a >= 96 && a <= 103))
		return false;
	return true;
}

void RelateEvent::cTankMap(Tank *tank)
{
	if (tank->m_direction == DIRECTION::UP)
	{
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideMap(map->tileCoordForPosition(Point(i, rect.getMaxY() + (int)tank->m_moveSpeed))))
				{
					tank->stopMoving(tank->m_direction);
					while (!isCollideMap(map->tileCoordForPosition(Point(i, rect.getMaxY() + 1))))
					{
						tank->setPositionY(tank->getPositionY() + 1);
						rect = tank->getBoundingBox();
					}
					break;
				}
			}
			
		}, 0.0f, kRepeatForever, 0.0f, "move_up");
	}

	else if (tank->m_direction == DIRECTION::DOWN)
	{
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			for (float i = rect.getMinX(); i <= rect.getMaxX(); i = i + 1)
			{
				if (isCollideMap(map->tileCoordForPosition(Point(i + 0.000003, rect.getMinY() - (int)tank->m_moveSpeed))))
				{
					tank->stopMoving(tank->m_direction);
					while (!isCollideMap(map->tileCoordForPosition(Point(i, rect.getMinY() - 1))))
					{
						tank->setPositionY(tank->getPositionY() - 1);
						rect = tank->getBoundingBox();
					}
					break;
				}
			}

		}, 0.0f, kRepeatForever, 0.0f, "move_down");
	}

	else if (tank->m_direction == DIRECTION::LEFT)
	{
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideMap(map->tileCoordForPosition(Point(rect.getMinX() - (int)tank->m_moveSpeed, i))))
				{
					tank->stopMoving(tank->m_direction);
					while (!isCollideMap(map->tileCoordForPosition(Point(rect.getMinX() - 1, i))))
					{
						tank->setPositionX(tank->getPositionX() - 1);
						rect = tank->getBoundingBox();
					}
					break;
				}
			}
			

		}, 0.0f, kRepeatForever, 0.0f, "move_left");
	}

	else if (tank->m_direction == DIRECTION::RIGHT)
	{
		schedule([&](float dt) {
			auto rect = tank->getBoundingBox();
			for (float i = rect.getMinY(); i <= rect.getMaxY(); i = i + 1)
			{
				if (isCollideMap(map->tileCoordForPosition(Point(rect.getMaxX() + (int)tank->m_moveSpeed, i))))
				{
					tank->stopMoving(tank->m_direction);
					while (!isCollideMap(map->tileCoordForPosition(Point(rect.getMaxX() + 1, i))))
					{
						tank->setPositionX(tank->getPositionX() + 1);
						rect = tank->getBoundingBox();
					}
					break;
				}
			}
			
		}, 0.0f, kRepeatForever, 0.0f, "move_right");
	}
}

void RelateEvent::cTankEnemy(Tank *tank, Enemy *enemy)
{
	auto rect = enemy->getBoundingBox();
	auto rect1 = tank->getBoundingBox();

	auto minRight = rect.getMaxX() < rect1.getMaxX() ? rect.getMaxX() : rect1.getMaxX();
	auto maxLeft = rect.getMinX() > rect1.getMinX() ? rect.getMinX() : rect1.getMinX();
	auto minTop = rect.getMaxY() < rect1.getMaxY() ? rect.getMaxY() : rect1.getMaxY();
	auto maxBottom = rect.getMinY() > rect1.getMinY() ? rect.getMinY() : rect1.getMinY();
	if (maxBottom <= minTop && maxLeft <= minRight)
	{
		tank->stopMoving(tank->m_direction);
		enemy->stopMoving(enemy->m_direction);
	}

}

void RelateEvent::cBulletTank(Bullet *bullet, Tank *tank)
{
	auto pos = bullet->getPosition();
	auto rect = tank->getBoundingBox();
	if (pos.x >= rect.getMinX() && pos.x <= rect.getMaxX() &&
		pos.y >= rect.getMinY() && pos.y <= rect.getMaxY())
	{
		tankDie(tank);
		bulletDie(bullet);
	}
}

void RelateEvent::cBulletMap(Bullet *bullet)
{
	auto position = map->tileCoordForPosition(bullet->getPosition());

	if (isCollideMap(position))
	{
		if (!map->isSteel(position))
		{
			//让一定范围内砖块消失
			map->layer1->removeTileAt(position);
			switch (bullet->tankDirection)
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
				log("bullet->tankDirection error");
				break;
			}
		}
		//bullet->stopFly();
		bulletDie(bullet);
	}
}

void RelateEvent::bulletDie(Bullet *bullet)
{
	//animation and die
	auto animation = AnimationCache::getInstance()->animationByName("bulletboom");
	auto action = Animate::create(animation);
	this->runAction(Sequence::create(
		action,
		CCRemoveSelf::create(),
		[&]() {bullet->removeFromParentAndCleanup(true); },
		NULL));
}

void RelateEvent::tankDie(Tank *tank)
{
	//boom animation and remove
	auto animation = AnimationCache::getInstance()->animationByName("tankboom");
	auto action = Animate::create(animation);
	this->runAction(Sequence::create(
		action, 
		CCRemoveSelf::create(), 
		[&]() {tank->removeFromParentAndCleanup(true); }, 
		NULL));
}
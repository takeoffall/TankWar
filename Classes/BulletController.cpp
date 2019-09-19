#include "BulletController.h"
#include "Bullet.h"
#include "GameScene.h"
#include "MapLayer.h"

#include "AudioEngine.h"
using namespace experimental;

BulletController* BulletController::create(Bullet *bullet)
{
	BulletController *btcnter = new (std::nothrow) BulletController();
	if (btcnter && btcnter->init(bullet))
	{
		btcnter->autorelease();
		return btcnter;
	}
	CC_SAFE_DELETE(btcnter);
	return nullptr;
}

bool BulletController::init(Bullet *bullet)
{
	if (!Node::init())
		return false;
	m_bullet = bullet;

	m_bullet->fly();
	listenCollideMap();
	listenCollideTank();
	listenCollideEnemy();

	return true;
}

void BulletController::update(float dt)
{

}

void BulletController::bulletDo()
{
	//do some action
	m_bullet->removeFromParentAndCleanup(true);//die
}

void BulletController::tankDo()
{
	//do some action
	tankFlash();
	m_hitTank->dropBlood(m_bullet->getPower());//掉的血量等于子弹威力
	log("tank blood: %d", m_hitTank->HP);
}

void BulletController::enemyDo()
{
	//do some action
	if (!m_hitTank->isPause){
		m_hitTank->dropBlood(m_bullet->getPower());//掉的血量等于子弹威力
	}
	else {
		m_hitTank->waitForDie(m_bullet->getPower());
	}
	if (m_bullet->ice)
	{
		if (!m_hitTank->isPause)
		{
			auto ice = Sprite::createWithSpriteFrameName("crystal.png");
			ice->setPosition(m_hitTank->getContentSize().width / 2, m_hitTank->getContentSize().height / 2);
			m_hitTank->addChild(ice, 2, "ice");
			m_hitTank->pause();
			m_hitTank->isPause = true;
		}
		else
		{
			m_hitTank->removeChildByName("ice");
			m_hitTank->resume();
			m_hitTank->isPause = false;
		}
	}
	
	//log("enemy blood: %d", m_hitTank->HP);
	//enemyBoom();
}

void BulletController::tankFlash()
{
	m_hitTank->runAction(Sequence::create(Blink::create(0.5f, 5), Show::create(), NULL));
}

void BulletController::tankBoom()
{
	AudioEngine::play2d("sounds/fexplosion.mp3");
	auto animation = AnimationCache::getInstance()->getAnimation("tankboom");
	auto action = Animate::create(animation);
	m_bullet->gameLayer->m_map->tankSet.eraseObject(m_hitTank);
	m_hitTank->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {m_hitTank->removeFromParentAndCleanup(true); }, NULL));
}

void BulletController::enemyBoom()
{
	AudioEngine::play2d("sounds/fexplosion.mp3");
	auto animation = AnimationCache::getInstance()->getAnimation("enemyboom");
	auto action = Animate::create(animation);
	m_bullet->gameLayer->m_map->tankSet.eraseObject(m_hitTank);
	m_hitTank->runAction(Sequence::create(action, CCRemoveSelf::create(), [&]() {m_hitTank->removeFromParentAndCleanup(true); }, NULL));
}

void BulletController::bulletRebound()
{
	if (m_bullet->m_direction == DIRECTION::UP)
	{
		m_bullet->unschedule("fly_up");
		m_bullet->schedule([&](float dt) {
			m_bullet->setPositionY(m_bullet->getPositionY() - m_bullet->tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_down");
	}
	else if (m_bullet->m_direction == DIRECTION::DOWN)
	{
		m_bullet->unschedule("fly_down");
		m_bullet->schedule([&](float dt) {
			m_bullet->setPositionY(m_bullet->getPositionY() + m_bullet->tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_up");
	}
	else if (m_bullet->m_direction == DIRECTION::LEFT)
	{
		m_bullet->unschedule("fly_left");
		m_bullet->schedule([&](float dt) {
			m_bullet->setPositionX(m_bullet->getPositionX() + m_bullet->tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_right");
	}
	else if (m_bullet->m_direction == DIRECTION::RIGHT)
	{
		m_bullet->unschedule("fly_right");
		m_bullet->schedule([&](float dt) {
			m_bullet->setPositionX(m_bullet->getPositionX() - m_bullet->tankShootSpeed);
		}, 0.0f, kRepeatForever, 0.0f, "fly_left");
	}
}

void BulletController::listenCollideMap()
{
	schedule([&](float dt) {
		auto x = m_bullet->getPositionX();
		auto y = m_bullet->getPositionY();
		if (isCollideMap(x, y))
		{
			m_bullet->dieEffect();//bullet ui
			auto position = m_bullet->gameLayer->m_map->tileCoordForPosition(m_bullet->getPosition());
			if (!m_bullet->gameLayer->m_map->isSteel(position))//map ui
			{
				//让一定范围内砖块消失
				m_bullet->gameLayer->m_map->layer1->removeTileAt(position);
				switch (m_bullet->m_direction)
				{
				case DIRECTION::UP:
				case DIRECTION::DEFAULT:
				case DIRECTION::DOWN:
					if (!m_bullet->gameLayer->m_map->isSteel(Point(position.x - 1, position.y)))
						m_bullet->gameLayer->m_map->layer1->removeTileAt(Point(position.x - 1, position.y));
					if (!m_bullet->gameLayer->m_map->isSteel(Point(position.x + 1, position.y)))
						m_bullet->gameLayer->m_map->layer1->removeTileAt(Point(position.x + 1, position.y));
					break;
				case DIRECTION::LEFT:
				case DIRECTION::RIGHT:
					if (!m_bullet->gameLayer->m_map->isSteel(Point(position.x, position.y - 1)))
						m_bullet->gameLayer->m_map->layer1->removeTileAt(Point(position.x, position.y - 1));
					if (!m_bullet->gameLayer->m_map->isSteel(Point(position.x, position.y + 1)))
						m_bullet->gameLayer->m_map->layer1->removeTileAt(Point(position.x, position.y + 1));
					break;
				default:
					log("Direction type error!");
					break;
				}
			}
			m_bullet->removeFromParentAndCleanup(true);
			unschedule("collide_map");
		}
	}, 0.0f, kRepeatForever, 0.0f, "collide_map");
}

void BulletController::listenCollideTank()
{
	schedule([&](float dt) {
		if (isCollideByName("tank"))
		{
			if (m_bullet->p_tankName == "enemy")
			{		
				if (m_hitTank->isProtectedUP)
				{
					//子弹反弹
					//m_bullet->m_direction = DIRECTION(3 - (int)m_bullet->m_direction);//巧妙转换
					bulletRebound();
					//子弹变为tank的
					m_bullet->p_tank = m_hitTank;
					m_bullet->p_tankName = "tank";
				}
				if (!m_hitTank->isProtected)
				{
					tankDo();//tank action
					bulletDo();//bullet action
				}
				unschedule("collide_tank");
			}
		}
	}, 0.0f, kRepeatForever, 0.0f, "collide_tank");
}

void BulletController::listenCollideEnemy()
{
	schedule([&](float dt) {
		if (isCollideByName("enemy"))
		{
			if (m_bullet->p_tankName == "tank")
			{
				enemyDo();
				
				bulletDo();
				unschedule("collide_enemy");
			}
		}
	}, 0.0f, kRepeatForever, 0.0f, "collide_enemy");
}

bool BulletController::isCollideMap(float x, float y)
{
	return m_bullet->gameLayer->m_map->isCollision(m_bullet->gameLayer->m_map->tileCoordForPosition(Point(x, y)));
}

bool BulletController::isCollideByName(const std::string name)
{
	auto c = m_bullet->gameLayer->m_map->tankSet;
	c.eraseObject(m_bullet->p_tank);//自己的子弹长眼睛
	for (auto &i : c)
	{
		if (i->getName() != name)
			continue;
		auto rect = i->getBoundingBox();
		auto x = m_bullet->getPositionX();
		auto y = m_bullet->getPositionY();
		if (x >= rect.getMinX() && x <= rect.getMaxX() && y >= rect.getMinY() && y <= rect.getMaxY())
		{
			this->m_hitTank = i;
			return true;
		}
	}
	return false;
}

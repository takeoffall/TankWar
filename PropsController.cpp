#include "PropsController.h"
#include "MapLayer.h"

PropController* PropController::create(Props* prop)
{
	PropController *cnter = new (std::nothrow) PropController(prop);
	if (cnter && cnter->init())
	{
		cnter->autorelease();
		return cnter;
	}
	CC_SAFE_DELETE(cnter);
	return nullptr;
}

PropController::PropController(Props* prop)
{
	this->prop = prop;
}

bool PropController::init()
{
	//根据道具自身的属性去控制它
	//检测是否被碰到
	scheduleUpdate();
	return true;
}

void PropController::getVajraBody(Tank* tank, const std::string& scheduleName)
{
	tank->schedule([tank, this](float dt) {
		auto p1 = Point(tank->getBoundingBox().getMinX() + 0.000003, tank->getBoundingBox().getMaxY());//左上
		auto p2 = Point(tank->getBoundingBox().getMaxX(), tank->getBoundingBox().getMaxY());//右上
		auto p3 = Point(tank->getBoundingBox().getMinX() + 0.000003, tank->getBoundingBox().getMinY());//左下
		auto p4 = Point(tank->getBoundingBox().getMaxX(), tank->getBoundingBox().getMinY());//右下
		if (tank->m_direction == DIRECTION::UP)
		{
			auto position1 = prop->map->tileCoordForPosition(p1);
			auto position2 = prop->map->tileCoordForPosition(p2);
			if (position1.y - 1 > 0)
			{
				for (auto i = position1.x; i <= position2.x; i++)
				{
					prop->map->layer1->removeTileAt(Point(i, position1.y - 1));
				}
			}
		}

		else if (tank->m_direction == DIRECTION::DOWN)
		{
			auto position1 = prop->map->tileCoordForPosition(p3);
			auto position2 = prop->map->tileCoordForPosition(p4);
			if (position1.y + 1 < prop->map->heightTiles - 1)
			{
				for (auto i = position1.x; i <= position2.x; i++)
				{
					prop->map->layer1->removeTileAt(Point(i, position1.y + 1));
				}
			}
		}

		else if (tank->m_direction == DIRECTION::LEFT)
		{
			auto position1 = prop->map->tileCoordForPosition(p3);
			auto position2 = prop->map->tileCoordForPosition(p1);
			if (position1.x - 1 > 0) 
			{
				for (auto i = position2.y; i <= position1.y; i++)
				{
					prop->map->layer1->removeTileAt(Point(position1.x - 1, i));
				}
			}
		}

		else if (tank->m_direction == DIRECTION::RIGHT)
		{
			auto position1 = prop->map->tileCoordForPosition(p4);
			auto position2 = prop->map->tileCoordForPosition(p2);
			if (position1.x + 1 < prop->map->widthTiles - 1)
			{
				for (auto i = position2.y; i <= position1.y; i++)
				{
					prop->map->layer1->removeTileAt(Point(position1.x + 1, i));
				}
			}
		}

	}, 0.0f, kRepeatForever, 0.0f, scheduleName);
}

void PropController::update(float dt)
{
	for (auto &i : prop->map->tankSet)
	{
		if (prop->getBoundingBox().intersectsRect(i->getBoundingBox()) && i->getName() == "tank")
		{
			i->propTypes[prop->getType()]++;
			prop->changeParent();
			//prop->unschedule("no_use");
			/*auto s = (Sprite *) prop;
			log("s: %p, prop: %p", s, prop);
			i->inventory->addItem(&s);*/
			i->inventory->addItem(prop);

			if (prop->getType() == PROP_TYPE::ADD_BLOOD)
			{
				i->addHP(40);
				log("HP after add blood: %d", i->HP);
				i->scheduleOnce([&](float dt) {
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_blood");
			}

			else if (prop->getType() == PROP_TYPE::PROTECTED)
			{
				if (i->propTypes[PROP_TYPE::PROTECTED] == 1)
				{
					i->isProtected = true;
					i->initWithFile("tk1_p.png");
					i->scheduleOnce([&](float dt) {
						i->isProtected = false;
						i->initWithFile("tk1.png");
						prop->removeFromParentAndCleanup(true);
						prop->isDelete = true;

						//log("prop p: %p", prop);
						////CC_SAFE_DELETE(prop);
						//CC_SAFE_RELEASE_NULL(prop);
						//log("delete prop: %p", prop);
						//i->inventory->print();
					}, prop->getContinuousTime(), "lose_protected");
				}
				else if (i->propTypes[PROP_TYPE::PROTECTED] == 2)
				{
					auto a = Sprite::create("tidao.png");
					auto b = Sprite::create("tidao.png");
					auto c = Sprite::create("tidao.png");
					auto d = Sprite::create("tidao.png");
					auto rect = i->getContentSize();
					a->setAnchorPoint(Vec2(0, 1));
					a->setPosition(0, rect.height);
					b->setAnchorPoint(Vec2(1, 1));
					b->setPosition(rect.width, rect.height);
					c->setAnchorPoint(Vec2(0, 0));
					c->setPosition(0, 0);
					d->setAnchorPoint(Vec2(1, 0));
					d->setPosition(rect.width, 0);

					a->setAnchorPoint(Vec2(0.5, 0.5));
					b->setAnchorPoint(Vec2(0.5, 0.5));
					c->setAnchorPoint(Vec2(0.5, 0.5));
					d->setAnchorPoint(Vec2(0.5, 0.5));
					a->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
					b->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
					c->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
					d->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
					i->addChild(a, 1, 1);
					i->addChild(b, 1, 2);
					i->addChild(c, 1, 3);
					i->addChild(d, 1, 4);

					i->isProtected = true;
					i->isProtectedUP = true;
					i->initWithFile("tk1_p.png");
					i->scheduleOnce([&](float dt) {
						i->isProtected = false;
						i->isProtectedUP = false;
						i->removeChildByTag(1);
						i->removeChildByTag(2);
						i->removeChildByTag(3);
						i->removeChildByTag(4);
						i->initWithFile("tk1.png");
						prop->removeFromParentAndCleanup(true);
						prop->isDelete = true;
					}, prop->getContinuousTime()*1.5, "lose_protectedup");
				}
				else if (i->propTypes[PROP_TYPE::PROTECTED] >= 3)
				{
					//所到之处，寸草不生
					i->isProtected = true;
					i->isProtectedUP = true;
					getVajraBody(i, "getVajraBody");
					auto sprite = Sprite::create();
					sprite->setPosition(i->getContentSize().width / 2, i->getContentSize().height / 2);
					i->addChild(sprite, 0, "vajraBody");
					auto animation = AnimationCache::getInstance()->getAnimation("tankborn");
					auto action = Animate::create(animation);
					sprite->runAction(RepeatForever::create(action));
					log("protect_3");
					i->scheduleOnce([&](float dt) {
						i->isProtected = false;
						i->isProtectedUP = false;
						i->unschedule("getVajraBody");
						i->removeChildByName("vajraBody");
						i->propTypes[PROP_TYPE::PROTECTED] = 0;//置0
						prop->removeFromParentAndCleanup(true);
						prop->isDelete = true;
					}, prop->getContinuousTime(), "lose_protectedupp");
				}
			}

			else if (prop->getType() == PROP_TYPE::START)
			{
				i->addShootSpeed(1);
				i->scheduleOnce([&](float dt) {
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_star");
			}

			else if (prop->getType() == PROP_TYPE::SPADE)
			{
				prop->map->layer2->setVisible(true);
				prop->map->isKingProtected = true;

				i->scheduleOnce([&](float dt) {
					prop->map->layer2->setVisible(false);
					prop->map->isKingProtected = false;//
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_spade");
				if (i->propTypes[PROP_TYPE::SPADE] >= 2)
				{
					log("set up zaolu");
					i->enableBuildFunction = true;
				}
			}

			else if (prop->getType() == PROP_TYPE::MINE)
			{
				for (auto &j : prop->map->tankSet)
				{
					if (j->getName() == "enemy")
					{
						//j->waitForDie(j->HP);//error
						if (j->isPause)
							j->resume();
						j->HP = 0;
						//break;//全炸注释break
					}
				}
				i->scheduleOnce([&](float dt) {
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_mine");
				if (i->propTypes[PROP_TYPE::MINE] >= 2)
				{
					log("set up zhadan");
					i->enableBombFunction = true;
				}
			}

			else if (prop->getType() == PROP_TYPE::TIMER)
			{
				if (i->propTypes[PROP_TYPE::TIMER] >= 2)
				{
					//log("bing tian xue di");//特效
					//ParticleSystem* ps = ParticleSnow::create();
					//ps->setTexture(Director::getInstance()->getTextureCache()->addImage("particle-snow.png"));
					//ps->setPosition(prop->map->mapSize.width / 2, prop->map->mapSize.height);//生成的雪花从这个坐标往下落
					//ps->setSpeed(50);
					//ps->setRadialAccel(50);
					//ps->setTangentialAccel(10);
					////ps->setEmissionRate(100);
					//prop->map->addChild(ps, 10, "snow");
					////log("speed : %f", ps->getSpeed());//5
					auto ps = ParticleSystemQuad::create("particle_texture.plist");
					prop->map->addChild(ps, 10, "snow");
					ps->setPosition(prop->map->mapSize.width / 2, prop->map->mapSize.height);
				}
				for (auto &j : prop->map->tankSet)
				{
					if (j->getName() == "enemy")
					{
						j->pause();
						j->isPause = true;
					}
				}
				i->scheduleOnce([&](float dt) {
					for (auto &k : prop->map->tankSet)
					{
						if (k->getName() == "enemy" && k->isPause)
						{
							k->resume();
							k->isPause = false;
						}
					}
					prop->map->removeChildByName("snow");
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_timer");
				
			}

			else if (prop->getType() == PROP_TYPE::XINGXING)
			{
				log("xingxing");
				i->scheduleOnce([&](float dt) {
					prop->removeFromParentAndCleanup(true);
					prop->isDelete = true;
				}, prop->getContinuousTime(), "lose_xingxing");
			}

		}
	}
}
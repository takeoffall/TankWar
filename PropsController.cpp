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

void PropController::update(float dt)
{
	for (auto &i : prop->map->tankSet)
	{
		if (prop->getBoundingBox().intersectsRect(i->getBoundingBox()) && i->getName() == "tank")
		{
			i->propTypes[prop->getType()]++;
			prop->changeParent();

			i->inventory->addItem((Sprite *)prop);

			if (prop->getType() == PROP_TYPE::ADD_BLOOD)
			{
				i->addHP(40);
				log("HP after add blood: %d", i->HP);
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_blood");
			}

			else if (prop->getType() == PROP_TYPE::PROTECTED)
			{
				if (i->propTypes[PROP_TYPE::PROTECTED] == 1)
				{
					i->isProtected = true;
					i->initWithFile("tk1_p.png");
					scheduleOnce([&](float dt) {
						//i->propTypes[PROP_TYPE::PROTECTED]--;
						i->isProtected = false;
						i->initWithFile("tk1.png");
						prop->removeFromParentAndCleanup(true);
					}, prop->getContinuousTime(), "lose_protected");

				}
				else if (i->propTypes[PROP_TYPE::PROTECTED] == 2)
				{
					i->isProtectedUP = true;
					i->initWithFile("tk1_f.png");
					scheduleOnce([&](float dt) {
						//i->propTypes[PROP_TYPE::PROTECTED]--;
						i->isProtectedUP = false;
						i->initWithFile("tk1.png");
						prop->removeFromParent();
					}, prop->getContinuousTime()*1.5, "lose_protectedup");
				}
				else if (i->propTypes[PROP_TYPE::PROTECTED] >= 3)
				{
					//所到之处，寸草不生
					scheduleOnce([&](float dt) {
						i->propTypes[PROP_TYPE::PROTECTED] = 0;//置0
						prop->removeFromParent();
					}, prop->getContinuousTime()*1.5, "lose_protectedupp");
				}
			}

			else if (prop->getType() == PROP_TYPE::START)
			{
				i->addShootSpeed(1);
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_star");
			}

			else if (prop->getType() == PROP_TYPE::SPADE)
			{
				prop->map->getLayer("bg2")->setVisible(false);
				scheduleOnce([&](float dt) {
					prop->map->getLayer("bg2")->setVisible(true);
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_spade");
				if (i->propTypes[PROP_TYPE::SPADE] >= 2)
				{
					log("set up zaolu");
				}
			}

			else if (prop->getType() == PROP_TYPE::MINE)
			{
				for (auto &j : prop->map->tankSet)
				{
					if (j->getName() == "enemy")
					{
						j->HP = 0;
						//break;//全炸注释break
					}
				}
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_mine");
				if (i->propTypes[PROP_TYPE::MINE] >= 2)
				{
					log("set up zhadan");
				}
			}

			else if (prop->getType() == PROP_TYPE::TIMER)
			{
				for (auto &j : prop->map->tankSet)
				{
					if (j->getName() == "enemy")
					{
						j->pause();
						j->isPause = true;
					}
				}
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_timer");
				if (i->propTypes[PROP_TYPE::TIMER] >= 2)
				{
					log("bing tian xue di");
				}
			}

			else if (prop->getType() == PROP_TYPE::TIMER)
			{
				for (auto &j : prop->map->tankSet)
				{
					if (j->getName() == "enemy")
					{
						j->pause();
						j->isPause = true;
					}
				}
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_timer");
				if (i->propTypes[PROP_TYPE::TIMER] >= 2)
				{
					log("bing tian xue di");
				}
			}

			else if (prop->getType() == PROP_TYPE::XINGXING)
			{
				log("xingxing");
				scheduleOnce([&](float dt) {
					prop->removeFromParent();
				}, prop->getContinuousTime(), "lose_xingxing");
			}

		}
	}
}
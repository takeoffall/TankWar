﻿#include "Props.h"
//#include "MapLayer.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

Props* Props::createWithPropName(const std::string &sourceName, PROP_TYPE type, float ctime, float wtime)
{
	Props *prop = new (std::nothrow) Props();
	//自定义
	{
		prop->setName(sourceName);
		prop->m_ctime = ctime;
		prop->m_wtime = wtime;
	}
	if (prop && prop->initWithSpriteFrameName(sourceName) && prop->init(type))
	{
		prop->autorelease();
		return prop;
	}
	CC_SAFE_DELETE(prop);
	return nullptr;
}

bool Props::init(PROP_TYPE type)
{
	m_type = type;
	if (type == PROP_TYPE::ADD_BLOOD)
	{
		m_description = "加40滴血";
	}

	else if (type == PROP_TYPE::PROTECTED)
	{
		m_description = "一件保护物品免疫子弹伤害，两件可以反弹子弹并产生“日炎”效果";
	}

	checkTime();
	return true;
}

void Props::checkTime()
{
	scheduleOnce([this](float dt) {
		auto blink = Blink::create(1.0f, 5);
		this->runAction(Sequence::create(blink, RemoveSelf::create(), [this]() {
			//map->propSet.eraseObject(this);
			this->removeFromParentAndCleanup(true);
		}, NULL));
	}, m_wtime, "no_use");
}

void Props::update(float dt)
{

}


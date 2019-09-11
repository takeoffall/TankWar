#include "Props.h"
#include "PropsController.h"
//#include "MapLayer.h"

//#if defined(_MSC_VER) && (_MSC_VER >= 1900)
//#pragma execution_character_set("utf-8")
//#endif

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
	isDelete = false;
	m_type = type;

	auto message = Dictionary::createWithContentsOfFile("props.xml");    //读取xml文件，文件在Resources目录下
	auto key = (__String *)message->objectForKey(this->getName());    //根据key，获取value
	m_description = key->getCString();
	this->setUserData((std::string *)key->getCString());
	

	checkTime();
	//scheduleUpdate();
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
	if (isObtained)
	{
		unschedule("no_use");

		this->runAction(Sequence::create(CallFunc::create([&]() {
			this->removeFromParentAndCleanup(false);
		}), DelayTime::create(m_ctime), RemoveSelf::create(), NULL));
		unscheduleUpdate();
	}
}

void Props::disappear()
{
	this->runAction(Sequence::create(DelayTime::create(m_ctime), RemoveSelf::create(), NULL));
}

void Props::changeParent()
{
	unschedule("no_use");
	this->removeFromParentAndCleanup(false);
}

void Props::addController()
{
	controller = PropController::create(this);
	addChild(controller);
}

